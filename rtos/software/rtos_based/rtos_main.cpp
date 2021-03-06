#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <io.h>
#include <sys/alt_irq.h>
#include <altera_avalon_pio_regs.h>

#include "includes.h"
#include "src/button.hpp"

// Application logic definitions.
#define	  MAX_RAND_DELAY	   2000
#define   MIN_RAND_DELAY	   1000
#define   RESP_TIMEOUT		   1000
#define   MIN_RESP_TIME		   80
#define   MAX_MSG_AMT		   2

// Hardware definitions.
#define   LED_AMT			   10
#define   START_KEY			   1
#define   RESP_KEY			   2

// Task configuration.
#define   TASK_STACKSIZE       1024
OS_STK    task1_stk[TASK_STACKSIZE];
#define   TASK1_PRIORITY      1		// Period of 1ms (highest priority)
OS_STK    task2_stk[TASK_STACKSIZE];
#define TASK2_PRIORITY      2

// Synchronisation mechanisms.
OS_EVENT *running_sem, *start_sem, *stop_sem, *player_queue;
static void *player_queue_buffer[MAX_MSG_AMT];
char player_name[50];

buttons_c buttons(BUTTONS_BASE, BUTTONS_IRQ_INTERRUPT_CONTROLLER_ID, BUTTONS_IRQ);

void show_score(unsigned int score){
	int data = score % 10;
	data |= (score / 10 % 10) << 4;
	data |= (score / 100 % 10) << 8;
	data |= (score / 1000 % 10) << 12;
	IOWR_32DIRECT(SEVEN_SEGMENT_CONTROLLER_0_BASE, 4, data);
}

void show_tries(unsigned int tries){
	int data = tries % 10;
	data |= (tries / 10 % 10) << 4;
	IOWR_32DIRECT(SEVEN_SEGMENT_CONTROLLER_0_BASE, 0, data);
}

void button_isr(uint8_t pin){
	if(pin == START_KEY){
		OSSemPost(start_sem);
	} else if(pin == RESP_KEY){
		OSSemPost(stop_sem);
	}
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
			user_register();
			printf("Press KEY_0 whenever you're ready %s.\n", player_name);
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
			OSSemPend(start_sem, 0, &ret);
			state = START;
			break;
		}
		case START: {
			IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
			show_score(0);
			printf("Waiting for button to be released!\n");
			while(buttons.pressed(START_KEY)){
				OSTimeDlyHMSM(0, 0, 0, 1);
			}
			printf("Button has been released!\n");
			state = DELAY;
			break;
		}
		case DELAY: {
			IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
			int32_t sys_time = OSTimeGet();
			srand(sys_time);
			int16_t delay = (rand() % MAX_RAND_DELAY) + MIN_RAND_DELAY;
			printf("Starting random delay of %ims.\n", delay);
			OSTimeDly(delay);
			while(buttons.pressed(RESP_KEY)){
				printf("Cheat prevention kicking in...\n");
				OSSemSet(stop_sem, 0, &ret);		// Reset semaphore.
				OSTimeDly(delay);
			}
			state = COUNT;
			break;
		}
		case COUNT: {
			IOWR_ALTERA_AVALON_PIO_DATA(MEASUREMENT_BASE, 1UL << state);
			enum states next_state = IDLE;
			for(uint16_t time = 0; time < RESP_TIMEOUT; time++){
				if(time % (RESP_TIMEOUT / LED_AMT) == 0){
					leds |= 1UL << time / (RESP_TIMEOUT / LED_AMT);
					IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
				}
				OSSemPend(stop_sem, 1, &ret); 	// Wait 1ms at max.
				if(ret != OS_TIMEOUT){
					if(time < MIN_RESP_TIME){
						printf("Impossible response-time of %ims detected; invalid!\n", time);
					} else {
						printf("Responded within %ims!\n", time);
						response_time = time;
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
			printf("Waiting for button to be released!\n");
			while(buttons.pressed(RESP_KEY)){
				OSTimeDlyHMSM(0, 0, 0, 100);
				if(new_highscore){
					leds = rotate_left(leds, 1);
					IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
				}
			}
			printf("Button has been released!\n");
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

int main(void){
	running_sem = OSSemCreate(0);
	start_sem = OSSemCreate(0);
	stop_sem = OSSemCreate(0);
	player_queue = OSQCreate((void**)&player_queue_buffer, MAX_MSG_AMT);

	buttons.init(button_isr);

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
