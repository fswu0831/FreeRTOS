/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdint.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "message_buffer.h"
#include "list.h"
#include "output.h"

#define BUFFER_SIZE ( ( unsigned portSHORT ) 2048 ) 
static signed portCHAR pcWriteBuffer1[BUFFER_SIZE] = { 2048 };

#define LOG_FILE     "LOG/log.txt"  
FILE* log_file;

/* Priorities at which the tasks are created. */
#define main_TASK_PRIORITY_HIGH		( tskIDLE_PRIORITY )
#define	main_TASK_PRIORITY_LOW		( tskIDLE_PRIORITY )

/* The rate at which data is sent to the queue.  The times are converted from
milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 200UL )
#define mainTIMER_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 2000UL )

/* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH					( 2 )

/* The values sent to the queue receive task from the queue send task and the
queue send software timer respectively. */
#define mainVALUE_SENT_FROM_TASK			( 100UL )
#define mainVALUE_SENT_FROM_TIMER			( 200UL )

#define NUM_TIMERS 5


static int SEND_TASK_NUM;// = 2;  //送信タスクの数
static int RECEIVE_TASK_NUM;// = 2;  //受信タスクの数
static int BUFFER_NUM;
static char YES_NO[1];
static int LOOP_TASK;
static int LOOP_BUFFER;
static int MODE;

static int NUMVER = 5;

/*
 * The tasks as described in the comments at the top of this file.
 */
static void SEND_TASK(void* pvParameters);
static void RECEIVE_TASK(void* pvParameters);
static void restart(void);


/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

//static MessageBufferHandle_t xMessageBuffer;
static MessageBufferHandle_t xMessageBuffer[10];
static const size_t xMessageBufferSizeBytes = 16384;


/* Timerに関する関数など */

TimerHandle_t xTimers[NUM_TIMERS];

void vTimerCallback(TimerHandle_t xTimer)
{
	const uint32_t ulMaxExpiryCountBeforeStopping = 1;
	uint32_t ulCount;
	ulCount = (uint32_t)pvTimerGetTimerID(xTimer);
	ulCount++;
	if (ulCount >= ulMaxExpiryCountBeforeStopping)
	{
		xTimerStop(xTimer, 0);
		printf("kernel停止\n");
		export_csv(SEND_TASK_NUM, RECEIVE_TASK_NUM, BUFFER_NUM);//シーケンスの出力
		if (MODE == 1) {
			restart();
			xTimerStart(xTimer, 0);
		}
		else {
			vTaskEndScheduler();
		}
		
	}
	else
	{
		vTimerSetTimerID(xTimer, (void*)ulCount);
	}
}

static int COUNT[100];

int getCOUNT(int i) {
	return COUNT[i];
}

void incrementCOUNT(int i) {
	COUNT[i] += 1;

}

void setCOUNT(int i, int j) {
	COUNT[i] = j;
}


static int k;
static char T[200];

static void restart(void) {
	uint8_t i;

	printf("タスク数:%d\n", uxTaskGetNumberOfTasks());
	
	reset_data();
	for (i = 0; i < 100; i++) {
		setCOUNT(i, 0);
	}
	for (i = 0; i < BUFFER_NUM; i++) {
		xMessageBufferReset(xMessageBuffer[i]);
	}

	k = sprintf(T, "%d", RECEIVE_TASK_NUM+SEND_TASK_NUM+1);
	xTaskCreate(RECEIVE_TASK, T, configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);
	k = sprintf(T, "%d", RECEIVE_TASK_NUM + SEND_TASK_NUM + 2);
	xTaskCreate(SEND_TASK, T, configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);
	RECEIVE_TASK_NUM++;
	SEND_TASK_NUM++;
	if (SEND_TASK_NUM >LOOP_TASK || RECEIVE_TASK_NUM > LOOP_TASK) {
		vTaskEndScheduler();
	}
	vTaskSuspendAll();
	xTaskResumeAll();
	printf("start");
}


static void create_object(void) {
	/* Start the two tasks as described in the comments at the top of this
	file. */
	//static int index_send[i];
	//static int index_receive[j];
	uint8_t i;
	uint8_t j;

	for (i = 1; i <= SEND_TASK_NUM; i++) {
		//T= (char*)i;
		printf(".");
		k = sprintf(T, "%d", i);

		xTaskCreate(RECEIVE_TASK, T, configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);


	}

	for (j = SEND_TASK_NUM + 1; j <= RECEIVE_TASK_NUM + SEND_TASK_NUM; j++) {
		printf(".");
		k = sprintf(T, "%d", j);

		xTaskCreate(SEND_TASK, T, configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);


	}

	for (i = 0; i < BUFFER_NUM; i++) {
		xMessageBuffer[i] = xMessageBufferCreate(xMessageBufferSizeBytes);
	}

	//printf("%d", uxTaskGetNumberOfTasks());
	for (i = 0; i <= SEND_TASK_NUM + RECEIVE_TASK_NUM + 1; i++) {
		COUNT[i] = 0;
	}

	xTimer = xTimerCreate("Timer", 40, pdTRUE, (void*)0, vTimerCallback);
	xTimerStart(xTimer, 0);
}


void main_blinky(void)
{
	
	printf("ループ(1/0):");
	scanf("%d", &MODE);
	if (MODE == 1) {
		printf("バッファーの数:");
		scanf("%d", &LOOP_BUFFER);
		printf("タスクの数:");
		scanf("%d", &LOOP_TASK);
		BUFFER_NUM = LOOP_BUFFER;
		RECEIVE_TASK_NUM = 1;
		SEND_TASK_NUM = 1;
		create_object();
		vTaskStartScheduler();
	}
	else {
		printf("バッファーの数:");
		scanf("%d", &BUFFER_NUM);
		printf("タスクの数:");
		scanf("%d", &SEND_TASK_NUM);
		RECEIVE_TASK_NUM = SEND_TASK_NUM;
		printf("kernelを起動します(y/n):");
		//scanf("%s", &YES_NO);
		YES_NO[0] = "y";
		if (strcmp("y", YES_NO) == 0 || strcmp("Y", YES_NO) == 0) {
			printf("オブジェクト作成");
			create_object();
			printf("OK\n");
			//vTaskStartTrace(pcWriteBuffer1, BUFFER_SIZE);
			printf("kernel起動\n");
			printf("実行中");
			vTaskStartScheduler();
		}
		else if (strcmp("n", YES_NO) == 0 || strcmp("N", YES_NO) == 0) {
			printf("中断しました\n");
		}
		else {
			printf("オブジェクト作成");
			create_object();
			printf("\nkernel起動\n");
			printf("実行中");
			vTaskStartScheduler();
		}
	}

}
/*-----------------------------------------------------------*/

/*メッセージバッファのための変数*/

uint8_t ucArrayToSend[] = { 0,1 };
size_t xBytesSent;
static const TickType_t x100ms = pdMS_TO_TICKS(100);


uint8_t ucRxData[2000];
size_t xReceivedBytes;
const TickType_t xBlockTime = pdMS_TO_TICKS(20);

static void SEND_TASK(void* pvParameters)
{
	(void)pvParameters;


	while (1)
	{
		TaskHandle_t tskHand_send;
		uint8_t i;

		for (i = 0; i < BUFFER_NUM; i++) {
			tskHand_send = xTaskGetCurrentTaskHandle();
			xBytesSent = xMessageBufferSend(xMessageBuffer[i], (void*)ucArrayToSend, sizeof(ucArrayToSend), x100ms, tskHand_send);

			if (xBytesSent != sizeof(ucArrayToSend)) {
				//dont have enough space
				printf("not enough send spase\n");
			}
			else {
				//printf("send:%d\n", getCOUNT(atoi(pcTaskGetName(tskHand_send))));
			}

		}
		printf(".");
		vTaskDelay(10);

	}

	//vTaskEndScheduler();
}


static void RECEIVE_TASK(void* pvParameters)
{
	(void)pvParameters;

	while (1)
	{
		TaskHandle_t tskHand_res;
		uint8_t i;

		for (i = 0; i < BUFFER_NUM; i++) {
			tskHand_res = xTaskGetCurrentTaskHandle();
			xReceivedBytes = xMessageBufferReceive(xMessageBuffer[i], (void*)ucRxData, sizeof(ucRxData), xBlockTime, tskHand_res);

			if (xReceivedBytes > 0) {
				//printf("send:%d\n",getCOUNT(atoi(pcTaskGetName(tskHand_res))));
			}
			else {
				printf("ERROR\n");
			}

		}

		printf(".");
		vTaskDelay(10);
	}

	//vTaskEndScheduler();
}





