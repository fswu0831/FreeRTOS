/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

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

/*
 * The callback function executed when the software timer expires.
 */

/*-----------------------------------------------------------*/

static  UBaseType_t N=100;
static  UBaseType_t zero = 0;
 
/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

static SemaphoreHandle_t sem_empty;
static SemaphoreHandle_t sem_full;
static SemaphoreHandle_t mutex;
/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_blinky( void )
{



	/* Start the two tasks as described in the comments at the top of this
	file. */
	xTaskCreate( producer,			/* The function that implements the task. */
				"Rx", 							/* The text name assigned to the task - for debug only as it is not used by the kernel. */
				configMINIMAL_STACK_SIZE, 		/* The size of the stack to allocate to the task. */
				NULL, 							/* The parameter passed to the task - not used in this simple case. */
				mainQUEUE_RECEIVE_TASK_PRIORITY,/* The priority assigned to the task. */
				NULL );							/* The task handle is not required, so NULL is passed. */

	xTaskCreate(consumer, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

	//creating resouses
	sem_empty = xSemaphoreCreateCounting(N, N);
	sem_full = xSemaphoreCreateCounting(N,0);
	mutex = xSemaphoreCreateMutex();


	/* Start the tasks and timer running. */
	vTaskStartScheduler();

}
/*-----------------------------------------------------------*/

static void producer( void *pvParameters )
{
	( void ) pvParameters;

	while(1)
	{
		if (sem_empty != NULL) {
			if (xSemaphoreTake(sem_empty, (TickType_t)10) == pdTRUE) {
				if (mutex != NULL) {
					if (xSemaphoreTake(mutex, (TickType_t)10) == pdTRUE){
						//insert item
						printf("producer %lf\n",sem_empty);
						xSemaphoreGive(mutex);
					}

				}
				xSemaphoreGive(sem_empty);
			}
			
		}
		else {
			printf("don't get semaphore\n");
		}
		
		vTaskDelay(100);
		
		// add a semaphore

		
	}
}


static void consumer( void *pvParameters )
{
	( void ) pvParameters;

	while (1)
	{

		if (sem_full != NULL) {
			if (xSemaphoreTake(sem_full, (TickType_t)10) == pdTRUE) {
				if (mutex != NULL) {
					if (xSemaphoreTake(mutex, (TickType_t)10) == pdTRUE) {
						//remove item
						printf("consumer\n");
						xSemaphoreGive(mutex);
					}

				}
				xSemaphoreGive(sem_full);
			}

		}
		else {
			printf("don't get semaphore\n");
		}
	
	}
}
/*-----------------------------------------------------------*/


