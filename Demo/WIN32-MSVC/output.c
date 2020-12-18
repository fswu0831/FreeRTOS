#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "FreeRTOS.h"


#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "message_buffer.h"
#include "list.h"
#include "output.h"

#define SEND_EVENT "send"
#define RECV_EVENT "recv"

#define MAX_NUM (1000)

static char FILE_NAME_1[30];
static char FILE_NAME_2[30];

typedef struct{
	char THREAD[5];
	int PORT;
	char EVENT[5];
	int INDEX;
}SEQUENCE;

static SEQUENCE Qr[MAX_NUM];
static SEQUENCE Qs[MAX_NUM];

static int r_count = 0;
static int s_count = 0;

//static struct SEQUENCE* Qr=NULL;
//static struct SEQUENCE* Qs=NULL;

SEQUENCE Substitution(char thread[5], int port, char event1[5], int index) {
	SEQUENCE s;
	strcpy(s.THREAD, thread);
	s.PORT = port;
	strcpy(s.EVENT, event1);
	s.INDEX = index;
	return s;
}



extern void append_row(StreamBufferHandle_t xStreamBuffer,char event_type[6], TaskHandle_t xTaskHandle) {
	uint8_t index = atoi(pcTaskGetName(xTaskHandle));
	

	incrementCOUNT(index);

	if (strcmp(event_type, SEND_EVENT) == 0) {
		Qr[r_count] = Substitution(pcTaskGetName(xTaskHandle), xStreamBuffer, event_type, getCOUNT(index));
		r_count++;

	}
	else if (strcmp(event_type, RECV_EVENT) == 0) {
		Qs[s_count] = Substitution(pcTaskGetName(xTaskHandle), xStreamBuffer, event_type, getCOUNT(index));
		s_count++;
	}
}


extern void export_csv(int SEND_TASK_NUM, int RECEIVE_TASK_NUM, int NUMVER) {

	FILE* fr;
	FILE* fs;
	uint8_t i, j;


	sprintf(FILE_NAME_1, "LOG/recv-task-%d-loop-%d.csv", SEND_TASK_NUM,NUMVER);
	sprintf(FILE_NAME_2, "LOG/send-task-%d-loop-%d.csv", RECEIVE_TASK_NUM, NUMVER);

	if ((fr = fopen(FILE_NAME_1, "w")) == NULL){
		printf("receiveイベントのログの出力に失敗しました");
	}
	else {
		printf("RECEIVE\n");
		printf("No.| THREAD |  PORT  | EVENT | INDEX |\n");
		printf("---|--------|--------|-------|-------|\n");
		for (i = 0; i < MAX_NUM; i++) {
			if (Qr[i].PORT == 0) {
				break;
			}
			printf("%3d|%8s|%6d|%7s|%7d|\n",i+1, Qr[i].THREAD, Qr[i].PORT, Qr[i].EVENT, Qr[i].INDEX);
			fprintf(fr, "%s,%d,%s,%d\n",Qr[i].THREAD, Qr[i].PORT,Qr[i].EVENT,Qr[i].INDEX);
		}
		printf("---|--------|--------|-------|-------|\n");
		printf("%s に結果が出力されました\n\n", FILE_NAME_1);
		fclose(fr);
	}
	
	if ((fs = fopen(FILE_NAME_2, "w")) == NULL) {
	printf("sendイベントのログの出力に失敗しました");
	}
	else {
		printf("SEND\n");
		printf("No.| THREAD |  PORT  | EVENT | INDEX |\n");
		printf("---|--------|--------|-------|-------|\n");
		for (j = 0; j < MAX_NUM; j++) {
			if (Qs[j].PORT == 0) {
				break;
			}
			printf("%3d|%8s|%6d|%7s|%7d|\n",j+1, Qs[j].THREAD, Qs[j].PORT, Qs[j].EVENT, Qs[j].INDEX);
			fprintf(fs, "%s,%d,%s,%d\n", Qs[j].THREAD, Qs[j].PORT, Qs[j].EVENT, Qs[j].INDEX);
		}
		printf("---|--------|--------|-------|-------|\n");
		printf("%s に結果が出力されました\n\n", FILE_NAME_2);
		fclose(fs);
	}

}


