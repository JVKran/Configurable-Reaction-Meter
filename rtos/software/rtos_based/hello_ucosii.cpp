#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/alt_irq.h>
#include <stdbool.h>
#include <io.h>

#include "includes.h"
#include <altera_avalon_pio_regs.h>

#define	  MAX_RAND_DELAY	   2000
#define   MIN_RAND_DELAY	   1000
#define   RESP_TIMEOUT		   2000
#define   LED_AMT			   10

#define   START_KEY			   1
#define   RESP_KEY			   2

#define   TASK_STACKSIZE       1024
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];

#define TASK1_PRIORITY      1		// Period of 1ms (highest priority)
#define TASK2_PRIORITY      2		// Period of 1 second
#define TASK3_PRIORITY      3		// Worst case period of +infinity (lowest priority)

OS_EVENT *running_sem, *start_sem, *stop_sem;
uint16_t highscore = UINT16_MAX;
uint8_t tries = 0;
volatile int edge_capture;
enum states {IDLE, START, DELAY, COUNT, STOP};

void show_score(unsigned int score){
	int data = score % 10;
	data |= (score / 10 % 10) << 4;
	data |= (score / 100 % 10) << 8;
	data |= (score / 1000 % 10) << 12;
	IOWR_32DIRECT(REG32_AVALON_INTERFACE_0_BASE, 4, data);
}

void show_tries(unsigned int tries){
	int data = tries % 10;
	data |= (tries / 10 % 10) << 4;
	IOWR_32DIRECT(REG32_AVALON_INTERFACE_0_BASE, 0, data);
}

void button_isr(void* isr_context){
	volatile int* edge_capture_ptr = (volatile int*) isr_context;
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTONS_BASE);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTONS_BASE, 0x0);

	if(*edge_capture_ptr == START_KEY){
		OSSemPost(start_sem);
	} else if(*edge_capture_ptr == 2){
		OSSemPost(stop_sem);
	}
}

unsigned int rotate_left(unsigned int value, int shift) {
	if ((shift &= (LED_AMT - 1)) == 0){
		return value;
	}
	return (value << shift) | (value >> (10 - LED_AMT));
}

// Task 1
void reaction_meter(void* pdata){
	INT8U ret = OS_NO_ERR;
	enum states state = IDLE;
	uint16_t response_time;
	uint16_t leds = 0;

	while (1){
		switch(state){
		case IDLE: {
			show_score(highscore);
			leds = 0;
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
			OSSemPend(start_sem, 0, &ret);
			show_tries(++tries);
			state = START;
			break;
		}
		case START: {
			show_score(0);
			printf("Waiting for button to be released!\n");
			while((IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE) & START_KEY) == 0){
				OSTimeDlyHMSM(0, 0, 0, 1);
			}
			printf("Button has been released!\n");
			state = DELAY;
			break;
		}
		case DELAY: {
			int32_t sys_time = OSTimeGet();
			srand(sys_time);
			int delay = (rand() % MAX_RAND_DELAY) + MIN_RAND_DELAY;
			printf("Starting random delay of %ims.\n", delay);
			OSTimeDly(delay);
			while((IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE) & RESP_KEY) == 0){
				printf("Cheat prevention kicking in...\n");
				OSSemSet(stop_sem, 0, &ret);		// Reset semaphore.
				OSTimeDlyHMSM(0, 0, 0, delay);
			}
			state = COUNT;
			break;
		}
		case COUNT: {
			enum states next_state = IDLE;
			for(uint16_t time = 0; time < RESP_TIMEOUT; time++){
				show_score(time);
				if(time % (RESP_TIMEOUT / LED_AMT) == 0){
					leds |= 1UL << time / (RESP_TIMEOUT / LED_AMT);
					IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
				}
				OSSemPend(stop_sem, 1, &ret); 	// Wait 1ms at max.
				if(ret != OS_TIMEOUT){
					if(time < 80){
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
			if(response_time < highscore){
				highscore = response_time;
				IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, 0x3FF);
			}
			printf("Waiting for button to be released!\n");
			while((IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE) & RESP_KEY) == 0){
				OSTimeDlyHMSM(0, 0, 0, 100);
				leds = rotate_left(leds, 1);
				IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
			}
			printf("Button has been released!\n");
			state = IDLE;
			break;
		}
		}
	}
}

int main(void){

	running_sem = OSSemCreate(0);
	start_sem = OSSemCreate(0);
	stop_sem = OSSemCreate(0);

	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTONS_BASE, 0xF);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTONS_BASE, 0x0);
	alt_ic_isr_register(BUTTONS_IRQ_INTERRUPT_CONTROLLER_ID, BUTTONS_IRQ, button_isr, (void*)&edge_capture, 0x0);

	OSTaskCreateExt(reaction_meter,
                  NULL,
                  (OS_STK*)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

	OSStart();
	return 0;
}