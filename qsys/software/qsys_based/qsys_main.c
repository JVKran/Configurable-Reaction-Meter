#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <io.h>
#include <sys/alt_irq.h>

#include "includes.h"
#include "system.h"
#include <altera_avalon_pio_regs.h>

// Application logic definitions.
#define	  FIXED_DELAY		   2000		// Must be between 0 and 5000ms.
#define   MIN_RESP_TIME		   80
#define   MAX_MSG_AMT		   2

// Hardware definitions.
#define   LED_AMT			   10
#define   START_KEY			   1
#define   RESP_KEY			   2

// States
#define   SYS_RESTART	0
#define	  SYS_START		1
#define   SYS_COUNTING 	3
#define   SYS_STOP 		4

// Task configuration.
#define   TASK_STACKSIZE       1024
OS_STK    task1_stk[TASK_STACKSIZE];
#define   TASK1_PRIORITY      1		// Period of 1ms (highest priority)
OS_STK    task2_stk[TASK_STACKSIZE];
#define TASK2_PRIORITY      2

// IRQ-enabled custom platform designer component
#define	  IRQ_ID			   1
#define	  IRQ_CONTROLLER	   0

// Synchronisation mechanisms.
OS_EVENT *running_sem, *start_sem, *stop_sem, *start_counting_sem, *restart_sem, *player_queue;
static void *player_queue_buffer[MAX_MSG_AMT];
char player_name[50];

void show_score(unsigned int score){
	// Can be simplified. Directly write score?
	int data = score % 10;
	data |= (score / 10 % 10) << 4;
	data |= (score / 100 % 10) << 8;
	data |= (score / 1000 % 10) << 12;
	IOWR_32DIRECT(SSD_CONTROLLER_BASE, 4, data);
}

void show_tries(unsigned int tries){
	int data = tries % 10;
	data |= (tries / 10 % 10) << 4;
	IOWR_32DIRECT(SSD_CONTROLLER_BASE, 0, data);
}

int rotate_left(int num, int shift){
    return (num << shift) | (num >> (9 - shift));
}

static void user_register(){
	INT8U ret;
	static uint8_t rounds = 3;
	void* name;
	if(rounds++ >= 3){
		printf("Register your name with the 'name' command.\n");
		name = OSQPend(player_queue, 0, &ret);
		strcpy(player_name, (char*)name);
		printf("Registered %s.\n", player_name);
		rounds = 0;
	}
}

void reaction_meter(void* pdata){
	INT8U ret = OS_NO_ERR;
	uint16_t highscore = UINT16_MAX;
	uint16_t response_time, leds = 0;
	uint8_t tries = 0;

	enum states {IDLE, START, DELAY, COUNT, STOP};
	enum states state = IDLE;

	while (1){
		switch(state){
		case IDLE: {
			IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
			if(highscore != UINT16_MAX){
				show_score(highscore);
			} else {
				show_score(0);
			}
			show_tries(tries++);
			leds = 0;
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
			user_register();
			printf("Press KEY_0 whenever you're ready %s.\n", player_name);
			OSSemPend(start_sem, 0, &ret);
			state = START;
			break;
		}
		case START: {
			IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
			show_score(0);
			OSSemPend(start_counting_sem, 0, &ret);
			state = COUNT;
			break;
		}
		case COUNT: {
			IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
			enum states next_state = IDLE;
			for(uint16_t time = 0; time < 20; time++){
				leds <<= 1;
				leds |= 1UL;
				IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
				OSSemPend(stop_sem, 100, &ret); 	// Wait 100ms at max.
				if(ret != OS_TIMEOUT){
					response_time = IORD_16DIRECT(RESPONSE_TIME_METER_0_BASE, 2);
					if(response_time >= MIN_RESP_TIME){
						printf("Responded within %ims!\n", response_time);
						next_state = STOP;
					}
					break;
				}
			}
			state = next_state;
			break;
		}
		case STOP: {
			IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
			show_score(response_time);
			bool new_highscore = false;
			if(response_time < highscore){
				highscore = response_time;
				new_highscore = true;
			}
			while(true){
				OSSemPend(restart_sem, 100, &ret);
				if(ret != OS_TIMEOUT){
					break;
				}
				if(new_highscore){
					leds = rotate_left(leds, 1);
					IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
				}
			}
			state = IDLE;
			break;
		}
		}
	}
}

char* receive(){
	static char payload[10];
	int i = 0;
	char data;

	do {
		data = getchar();
		payload[i++] = data;
	} while(data != '\n');

	return payload;
}

// Task 3
void console(void* pdata){
	char* command;
	INT8U ret = OS_NO_ERR;

	while(1){
		command = receive();
		if(strstr(command, "name") != NULL){
			char *name = (command + 5);
			strtok(name, "\n");
			OSQPost(player_queue, (void*)name);
		} else {
			printf("Unkown command \'%s\'.", command);
		}
	}
}

static void isr(void * isr_context){
	uint16_t state = IORD_16DIRECT(RESPONSE_TIME_METER_0_BASE, 0);
	if(state == SYS_COUNTING){
		OSSemPost(start_counting_sem);
	} else if(state == SYS_START){
		OSSemPost(start_sem);
	} else if(state == SYS_STOP) {
		OSSemPost(stop_sem);
	} else if (state == SYS_RESTART){
		OSSemPost(restart_sem);
	}
	IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
}

static void init_response_meter(){
	alt_ic_isr_register(IRQ_CONTROLLER, IRQ_ID, isr, 0, 0x0);
#ifdef FIXED_DELAY
	IOWR_16DIRECT(RESPONSE_TIME_METER_0_BASE, 0, FIXED_DELAY);
#else
	IOWR_16DIRECT(RESPONSE_TIME_METER_0_BASE, 0, 0);
#endif
	uint16_t state = IORD_16DIRECT(RESPONSE_TIME_METER_0_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
}

int main(void){
	start_sem = OSSemCreate(0);
	start_counting_sem = OSSemCreate(0);
	running_sem = OSSemCreate(0);
	stop_sem = OSSemCreate(0);
	restart_sem = OSSemCreate(0);
	player_queue = OSQCreate((void**)&player_queue_buffer, MAX_MSG_AMT);

	init_response_meter();

	OSTaskCreateExt(reaction_meter,
                  NULL,
                  (OS_STK*)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

	OSTaskCreateExt(console,
		                  NULL,
						  (OS_STK*)&task2_stk[TASK_STACKSIZE-1],
		                  TASK2_PRIORITY,
		                  TASK2_PRIORITY,
		                  task2_stk,
		                  TASK_STACKSIZE,
		                  NULL,
		                  0);

	OSStart();
	return 0;
}
