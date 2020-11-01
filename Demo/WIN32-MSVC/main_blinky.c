/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"

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

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void producer( void *pvParameters );
static void consumer( void *pvParameters );
//void array_push(void* ptr, int size, size_t unit_size, void* item);
//static void consumer2(void* pvParameters);

/*
 * The callback function executed when the software timer expires.
 */

/*-----------------------------------------------------------*/

static UBaseType_t N=5;


 
/* The queue used by both tasks. */
static QueueHandle_t myQueue;

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

static SemaphoreHandle_t sem_empty;
static SemaphoreHandle_t sem_full;
static SemaphoreHandle_t mutex;

static int empty = 5; //3
static int full = 0; //2
static int mu = 0;


static int buffer = 0; //資源の初期値

static int work_num=100; //消費者のループ回数
//static int work_num_producer = 100; //生産者のループ回数

static int count=1;

static char myTxBuff[5] ;
static char myRxBuff[5] ;


/*---------CSVに保存するための関数など------------*/

typedef struct {
	double x, y, z;
}Vector;

static void save_csv(data) {
	FILE* fp;
	if ((fp = fopen("data.csv", "w")) != NULL) {
		fprintf(fp, "%s", data);
	}
	fclose(fp);
}

/*---------CSVに保存するための関数など------------*/


static void create_object(void) {
	/* Start the two tasks as described in the comments at the top of this
	file. */
	xTaskCreate(producer,			/* The function that implements the task. */
		"pro1", 							/* The text name assigned to the task - for debug only as it is not used by the kernel. */
		configMINIMAL_STACK_SIZE, 		/* The size of the stack to allocate to the task. */
		NULL, 							/* The parameter passed to the task - not used in this simple case. */
		main_TASK_PRIORITY_HIGH,/* The priority assigned to the task. */
		NULL);							/* The task handle is not required, so NULL is passed. */

	xTaskCreate(consumer, "con1", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY_LOW, NULL);
	//xTaskCreate(consumer2, "con2", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);

	//creating resouses
	sem_empty = xSemaphoreCreateCounting(N, N); //N
	sem_full = xSemaphoreCreateCounting(N, 0);  //0
	mutex = xSemaphoreCreateMutex();
}


void main_blinky( void )
{
		create_object();
		vTaskStartScheduler();
		printf("finish this kernel\n");
}
/*-----------------------------------------------------------*/



static void producer( void *pvParameters )
{
	( void ) pvParameters;
	myQueue = xQueueCreate(N, sizeof(myTxBuff));
	//xQueCreate(5,sizeof())//

	for(count; count <=work_num; count++)
	{

		if (sem_empty != NULL) {
			if (xSemaphoreTake(sem_empty, (TickType_t)0xFFFFFFFF) == pdTRUE) {
				empty--;
				if (mutex != NULL) {
					if (xSemaphoreTake(mutex, (TickType_t)0xFFFFFFFF) == pdTRUE){
						//insert item
						sprintf(myTxBuff, "item");
						xQueueSend(myQueue, (void*)myTxBuff, (TickType_t)0);
						printf("==================================\n");
						printf("producer:%d\n", count);
						printf("waiting to be read:%d\n", uxQueueMessagesWaiting(myQueue));
						printf("available spases  :%d\n", uxQueueSpacesAvailable(myQueue));
						printf("==================================\n\n");
						xSemaphoreGive(mutex);
					}
					else {
						xSemaphoreGive(sem_empty);
						//mutex取れなかった時→基本的にここにはこない
					}

				}
				else {
					//mutexが取られている
				}
				xSemaphoreGive(sem_full);
			}
			else {
				//セマフォ取れなかった→基本的にここにはこない
			}
		}
		else {
			//セマフォがない
		}
		vTaskDelay(10);
	}
	vTaskEndScheduler();
}


static void consumer( void *pvParameters )
{
	( void ) pvParameters;
	for (count; count <= work_num; count++)
	{
		
		if (sem_full != NULL) {
			if (xSemaphoreTake(sem_full, (TickType_t)0xFFFFFFFF) == pdTRUE) { //(TickType_t)0) == pdTRUE
				full--;
				if (mutex != NULL) {
					if (xSemaphoreTake(mutex, (TickType_t)0xFFFFFFFF) == pdTRUE) {
						xQueueReceive(myQueue, (void*)myRxBuff,(TickType_t) 5 );
						printf("==================================\n");
						printf("consumer:%d\n", count);
						printf("waiting to be read:%d\n", uxQueueMessagesWaiting(myQueue));
						printf("available spases  :%d\n", uxQueueSpacesAvailable(myQueue));
						printf("==================================\n\n");
						xSemaphoreGive(mutex);
					}
					else {
						xSemaphoreGive(sem_full);
						//mutex取れなかった時→基本的にここにはこない
					}


				}
				else {
					//mutexが取られている
				}
				xSemaphoreGive(sem_empty);
			}
			else {
				//セマフォ取れなかった→基本的にここにはこない
			}

		}
		else {
			//セマフォがない
		}

		vTaskDelay(10);
	}
	vTaskEndScheduler();
}