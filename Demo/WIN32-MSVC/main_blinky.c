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

#define BUFFER_SIZE ( ( unsigned portSHORT ) 512 ) 
static signed portCHAR pcWriteBuffer1[BUFFER_SIZE] = { 10 };

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


static int SEND_TASK_NUM;// = 2;  //送信タスクの数
static int RECEIVE_TASK_NUM;// = 2;  //受信タスクの数
static char YES_NO[1];

static int NUMVER=5;

/*
 * The tasks as described in the comments at the top of this file.
 */
static void SEND_TASK( void *pvParameters );
static void RECEIVE_TASK( void *pvParameters );


/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

//static MessageBufferHandle_t xMessageBuffer;
static MessageBufferHandle_t xMessageBuffer;
static const size_t xMessageBufferSizeBytes = 1000;



static int COUNT[100];

int getCOUNT(int i) {
	return COUNT[i];
}

void incrementCOUNT(int i) {
	COUNT[i] += 1;

}

void setCOUNT(int i,int j) {
	COUNT[i] = j;
}


static int k;
static char T[200];


static void create_object(void) {
	/* Start the two tasks as described in the comments at the top of this
	file. */
	//static int index_send[i];
	//static int index_receive[j];
	uint8_t i;
	uint8_t j;

	for (i=1; i <= SEND_TASK_NUM; i++) {
		//T= (char*)i;
		printf(".");
		k = sprintf(T, "%d", i);
		xTaskCreate(RECEIVE_TASK, T, configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);
		
	}

	for (j= SEND_TASK_NUM+1; j <= RECEIVE_TASK_NUM+ SEND_TASK_NUM; j++) {
		printf(".");
		k = sprintf(T, "%d", j);
		xTaskCreate(SEND_TASK, T , configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);
	}

	xMessageBuffer = xMessageBufferCreate(xMessageBufferSizeBytes);

	for (i = 0; i <= SEND_TASK_NUM + RECEIVE_TASK_NUM + 1; i++) {
		COUNT[i] = 0;
	}
}


void main_blinky( void )
{		
	printf("送信タスクの数:");
	scanf("%d", &SEND_TASK_NUM);
	printf("受信タスクの数:");
	scanf("%d", &RECEIVE_TASK_NUM);
	printf("kernelを起動します(y/n):");
	//scanf("%s", &YES_NO);
	YES_NO[0]="y";
	if (strcmp("y",YES_NO)==0 || strcmp("Y", YES_NO) == 0) {
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
/*-----------------------------------------------------------*/

/*メッセージバッファのための変数*/

uint8_t ucArrayToSend[] = { 0,1,2 };
size_t xBytesSent;
static const TickType_t x100ms = pdMS_TO_TICKS(100);


uint8_t ucRxData[2000];
size_t xReceivedBytes;
const TickType_t xBlockTime = pdMS_TO_TICKS(20);

static void check_task_num() {
	//printf("Task Number:%d\n", uxTaskGetNumberOfTasks());
	if (uxTaskGetNumberOfTasks()-2 == 1) {
		printf("OK\n");
		printf("kernel停止\n");
		export_csv(SEND_TASK_NUM, RECEIVE_TASK_NUM,NUMVER);//シーケンスの出力
		vTaskEndScheduler();
	}
}

static void SEND_TASK( void *pvParameters )
{
	( void ) pvParameters;
	

	while(1)
	{
		TaskHandle_t tskHand_send, tskHand_res;
		tskHand_send = xTaskGetCurrentTaskHandle();
		

		xBytesSent = xMessageBufferSend(xMessageBuffer, (void *)ucArrayToSend, sizeof(ucArrayToSend), x100ms, tskHand_send);

		if (xBytesSent != sizeof(ucArrayToSend)) {
			//dont have enough space
			printf("not enough send spase\n");
		}
		else {
			//printf("send:%d\n", getCOUNT(atoi(pcTaskGetName(tskHand_send))));
		}


		if (getCOUNT(atoi(pcTaskGetName(tskHand_send)))>= NUMVER) {
			check_task_num();
			vTaskDelete(tskHand_send);
		}
		printf(".");
		vTaskDelay(0);

		tskHand_res = xTaskGetCurrentTaskHandle();
		xReceivedBytes = xMessageBufferReceive(xMessageBuffer, (void*)ucRxData, sizeof(ucRxData), xBlockTime, tskHand_res);

		if (xReceivedBytes > 0) {
			//printf("send:%d\n",getCOUNT(atoi(pcTaskGetName(tskHand_res))));
		}
		else {
			printf("ERROR\n");
		}

		if (getCOUNT(atoi(pcTaskGetName(tskHand_res))) >= NUMVER) {
			check_task_num();
			vTaskDelete(tskHand_res);
		}
		printf(".");
		vTaskDelay(0);
	}
	
	//vTaskEndScheduler();
}


static void RECEIVE_TASK( void *pvParameters )
{
	( void ) pvParameters;



	while (1)
	{
		TaskHandle_t tskHand_send, tskHand_res;
		
		
		tskHand_res = xTaskGetCurrentTaskHandle();
		xReceivedBytes = xMessageBufferReceive(xMessageBuffer, (void*)ucRxData, sizeof(ucRxData), xBlockTime, tskHand_res);

		if (xReceivedBytes > 0) {
			//printf("send:%d\n",getCOUNT(atoi(pcTaskGetName(tskHand_res))));
		}
		else {
			printf("ERROR\n");
		}

		if (getCOUNT(atoi(pcTaskGetName(tskHand_res))) >= NUMVER) {
			check_task_num();
			vTaskDelete(tskHand_res);
		}
		printf(".");
		vTaskDelay(0);

		tskHand_send = xTaskGetCurrentTaskHandle();
		xBytesSent = xMessageBufferSend(xMessageBuffer, (void*)ucArrayToSend, sizeof(ucArrayToSend), x100ms, tskHand_send);

		if (xBytesSent != sizeof(ucArrayToSend)) {
			//dont have enough space
			printf("not enough send spase\n");
		}
		else {
			//printf("send:%d\n", getCOUNT(atoi(pcTaskGetName(tskHand_send))));
		}


		if (getCOUNT(atoi(pcTaskGetName(tskHand_send))) >= NUMVER) {
			check_task_num();
			vTaskDelete(tskHand_send);
		}
		printf(".");
		vTaskDelay(0);


	}
	
	//vTaskEndScheduler();
}
//回数取得用
//動いているバッファを取得

