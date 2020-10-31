/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define LOG_FILE     "LOG/log.txt"  
FILE* log_file;

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )

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
void array_push(void* ptr, int size, size_t unit_size, void* item);
//static void consumer2(void* pvParameters);

/*
 * The callback function executed when the software timer expires.
 */

/*-----------------------------------------------------------*/

static  UBaseType_t N=5;
static int buffer = 0;

 
/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

static SemaphoreHandle_t sem_empty;
static SemaphoreHandle_t sem_full;
static SemaphoreHandle_t mutex;
static int count = 0;

static int empty = 5;
static int full = 0;
static int mu = 0;

static int work_num_consumer=10;
static int work_num_producer = 10;

static int pro_count;
static int con_count;
/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_blinky( void )
{



	/* Start the two tasks as described in the comments at the top of this
	file. */
	xTaskCreate( producer,			/* The function that implements the task. */
				"pro1", 							/* The text name assigned to the task - for debug only as it is not used by the kernel. */
				configMINIMAL_STACK_SIZE, 		/* The size of the stack to allocate to the task. */
				NULL, 							/* The parameter passed to the task - not used in this simple case. */
				mainQUEUE_RECEIVE_TASK_PRIORITY,/* The priority assigned to the task. */
				NULL );							/* The task handle is not required, so NULL is passed. */

	xTaskCreate(consumer, "con1", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );
	//xTaskCreate(consumer2, "con2", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);

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
	
	
	for(pro_count=0; pro_count <work_num_producer; pro_count++)
	{
		if (sem_empty != NULL) {
			if (xSemaphoreTake(sem_empty, NULL) == pdTRUE) {
				empty--;
				if (mutex != NULL) {
					if (xSemaphoreTake(mutex, NULL) == pdTRUE){
						//insert item
						 buffer++;
						 mu++;
						 printf("pr2-buffer:%d empty:%d full:%d mutex:%d \n", buffer, empty, full, mu);
						xSemaphoreGive(mutex);
						mu--;
					}
					else {
						xSemaphoreGive(sem_empty);
						empty--;
					}

				}
				xSemaphoreGive(sem_full);
				full++;
			}
			else {
			}
		}
	
		
		// add a semaphore
		printf("pro-buffer:%d empty:%d full:%d mutex:%d \n", buffer,empty,full,mu);
		//LOG_PRINT(buffer);
		vTaskDelay(10);
	}
}


static void consumer( void *pvParameters )
{
	( void ) pvParameters;
	for (con_count = 0; con_count < work_num_consumer; con_count++)
	{

		if (sem_full != NULL) {
			if (xSemaphoreTake(sem_full, NULL) == pdTRUE) { //(TickType_t)0) == pdTRUE
				full--;
				if (mutex != NULL) {
					if (xSemaphoreTake(mutex, NULL) == pdTRUE) {
						buffer--;
						mu++;
						printf("co2-buffer:%d empty:%d full:%d mutex:%d \n", buffer, empty, full, mu);
						xSemaphoreGive(mutex);
						mu--;
					}
					else {
						xSemaphoreGive(sem_full);
						full--;
					}


				}
				xSemaphoreGive(sem_empty);
				empty++;
			}
			else {
				
			}

		}
		printf("con-buffer:%d empty:%d full:%d mutex:%d \n", buffer, empty, full, mu);
		//LOG_PRINT(buffer);
		vTaskDelay(10);
	}
}



/*static void consumer2(void* pvParameters)
{
	(void)pvParameters;
	while(1)
	{

		if (sem_full != NULL) {
			if (xSemaphoreTake(sem_full, NULL) == pdTRUE) {
				if (mutex != NULL) {
					if (xSemaphoreTake(mutex, NULL) == pdTRUE) {
						//remove item
						printf("consumer2\n");
						xSemaphoreGive(mutex);
					}
					else {
						printf("consumer2 dont get mutex\n");
					}

				}
				xSemaphoreGive(sem_empty);
			}

		}
		else {
			printf("don't get semaphore\n");
		}
		
		vTaskDelay(1);

	}
}
*/

/*

void* array_push(void* ptr, int size, size_t unit_size, void* item) {
	void* adr = malloc(unit_size * (size + 1));
	memcpy(adr, ptr, unit_size * size);
	memcpy((char*)adr + (unit_size * size), item, unit_size);
	return(adr);
}
*/