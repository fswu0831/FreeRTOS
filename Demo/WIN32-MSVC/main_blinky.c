/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "message_buffer.h"

#define LOG_FILE     "LOG/log.txt"  
FILE* log_file;

/* Priorities at which the tasks are created. */
#define main_TASK_PRIORITY_HIGH		( tskIDLE_PRIORITY + 2 )
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


#define SEND_TASK_NUM (1)  //送信タスクの数
#define RECEIVE_TASK_NUM (1)  //受信タスクの数


/*
 * The tasks as described in the comments at the top of this file.
 */
static void SEND_TASK( void *pvParameters );
static void RECEIVE_TASK( void *pvParameters );

static UBaseType_t N=5;




/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

//static MessageBufferHandle_t xMessageBuffer;
static MessageBufferHandle_t xMessageBuffer;
static const size_t xMessageBufferSizeBytes = 100;

static int i = 1;//ループ用の関数
static int j = 1;

static char T[10];


static void create_object(void) {
	/* Start the two tasks as described in the comments at the top of this
	file. */

	for (i; i <= SEND_TASK_NUM; i++) {
		sprintf(T, "T%d", i);
		xTaskCreate(RECEIVE_TASK, T , configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);
		
	}

	for (j; j <= RECEIVE_TASK_NUM; j++) {
		sprintf(T, "T%d", j+SEND_TASK_NUM);
		xTaskCreate(SEND_TASK, T , configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_HIGH, NULL);
	}

	xMessageBuffer = xMessageBufferCreate(xMessageBufferSizeBytes);
}


void main_blinky( void )
{
		create_object();
		vTaskStartScheduler();
		printf("finish this kernel\n");
}
/*-----------------------------------------------------------*/

/*メッセージバッファのための変数*/

uint8_t ucArrayToSend[] = { 0,1,2,3 };
size_t xBytesSent;
static const TickType_t x100ms = pdMS_TO_TICKS(100);


uint8_t ucRxData[20];
size_t xReceivedBytes;
const TickType_t xBlockTime = pdMS_TO_TICKS(20);


static void SEND_TASK( void *pvParameters )
{
	( void ) pvParameters;

	while(1)
	{
		xBytesSent = xMessageBufferSend(xMessageBuffer, (void *)ucArrayToSend, sizeof(ucArrayToSend), x100ms);

		if (xBytesSent != sizeof(ucArrayToSend)) {
			//dont have enough space
			printf("not enough send spase\n");
		}
		else {
			printf("send:%d\n",xBytesSent);
		}

		vTaskDelay(10);
	}
	//vTaskEndScheduler();
}


static void RECEIVE_TASK( void *pvParameters )
{
	( void ) pvParameters;



	while (1)
	{
		xReceivedBytes = xMessageBufferReceive(xMessageBuffer, (void*)ucRxData, sizeof(ucRxData), xBlockTime);

		if (xReceivedBytes>0) {
			//dont have enough space
			printf("rece:%d\n", ucRxData[1]);
		}
		else {
			printf("ERROR\n");
		}

		vTaskDelay(10);
	}
	//vTaskEndScheduler();
}