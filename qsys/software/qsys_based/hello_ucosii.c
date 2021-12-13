#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <io.h>
#include <sys/alt_irq.h>

#include "includes.h"
#include <altera_avalon_pio_regs.h>

// Application logic definitions.
#define	  MAX_RAND_DELAY	   2000
#define   MIN_RAND_DELAY	   1000
#define   RESP_TIMEOUT		   1000
#define   MIN_RESP_TIME		   80

// Hardware definitions.
#define   LED_AMT			   10
#define   START_KEY			   1
#define   RESP_KEY			   2

// Task configuration.
#define   TASK_STACKSIZE       1024
OS_STK    task1_stk[TASK_STACKSIZE];
#define   TASK1_PRIORITY      1		// Period of 1ms (highest priority)

// Synchronisation mechanisms.
OS_EVENT *running_sem, *start_sem, *stop_sem;

void show_score(unsigned int score){
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

void reaction_meter(void* pdata){
	uint16_t data, leds;
	leds = 1;

	while (1){
		data = IORD_16DIRECT(RESPONSE_TIME_METER_0_BASE, 0);
		printf("Register 0 contains \'%i\'.\n", data);
		OSTimeDlyHMSM(0, 0, 1, 0);
//
//		data = IORD_16DIRECT(RESPONSE_TIME_METER_0_BASE, 2);
//		printf("Register 1 contains \'%i\'.\n", data);
//		OSTimeDlyHMSM(0, 0, 1, 0);

		leds = rotate_left(leds, 1);
		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, leds);
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
}

static void isr(void * isr_context){
	uint16_t time = IORD_16DIRECT(RESPONSE_TIME_METER_0_BASE, 2);
	show_score(time);
}

int main(void){
	running_sem = OSSemCreate(0);
	start_sem = OSSemCreate(0);
	stop_sem = OSSemCreate(0);

	alt_ic_isr_register(RESPONSE_TIME_METER_0_IRQ_INTERRUPT_CONTROLLER_ID, RESPONSE_TIME_METER_0_IRQ, isr, 0, 0x0);

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
