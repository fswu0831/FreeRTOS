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


static char FILE_NAME_1[30];
static char FILE_NAME_2[30];

typedef struct{
	char THREAD[5];
	int PORT;
	char EVENT[5];
	int INDEX;

}SEQUENCE;

static SEQUENCE Qr[1000];
static SEQUENCE Qs[1000];




extern void append_row(StreamBufferHandle_t xStreamBuffer,char event_type[6], TaskHandle_t xTaskHandle) {
	uint8_t index = atoi(pcTaskGetName(xTaskHandle));
	

	incrementCOUNT(index);

	if (strcmp(event_type, SEND_EVENT) == 0) {
		struct 
	}
	
	/*

	if (strcmp(event_type, SEND_EVENT) == 0) {
		uint8_t new_index = (sizeof Qs) / (sizeof(SEQUENCE));
		printf("%d,%d\n", (sizeof Qs), (sizeof(SEQUENCE)));
		strcpy(Qs[new_index].THREAD, pcTaskGetName(xTaskHandle));
		Qs[new_index].PORT = xStreamBuffer;
		strcpy(Qs[new_index].EVENT, event_type);
		Qs[new_index].INDEX = getCOUNT(index);
		printf("%4d->%s:%s,%d,%s,%d\n",new_index ,Qs[new_index].EVENT, Qs[new_index].THREAD, Qs[new_index].PORT, Qs[new_index].EVENT, Qs[new_index].INDEX);

	}
	else if (strcmp(event_type, RECV_EVENT) == 0) {
		uint8_t new_index = (sizeof Qr) / (sizeof(SEQUENCE));
		strcpy(Qr[new_index].THREAD, pcTaskGetName(xTaskHandle));
		Qr[new_index].PORT = xStreamBuffer;
		strcpy(Qr[new_index].EVENT, event_type);
		Qr[new_index].INDEX = getCOUNT(index);
		printf("%4d->%s:%s,%d,%s,%d\n", new_index,Qr[new_index].EVENT, Qr[new_index].THREAD, Qr[new_index].PORT, Qr[new_index].EVENT, Qr[new_index].INDEX);
	}
	else {
		printf("ERROR\n");
	}
	*/
}


extern void export_csv(void) {

	FILE* fr;
	FILE* fs;
	uint8_t i, j;

	

	uint8_t LEN_RECV=(sizeof Qr) / (sizeof(SEQUENCE)); 
	uint8_t LEN_SEND=(sizeof Qs) / (sizeof(SEQUENCE)); 

	sprintf(FILE_NAME_1, "LOG/SYN-sequence-receive.csv");
	sprintf(FILE_NAME_2, "LOG/SYN-sequence-send.csv");

	if ((fr = fopen(FILE_NAME_1, "w")) == NULL){
		printf("receiveイベントのログの出力に失敗しました");
	}
	else {
		printf("RECEIVE\n");
		for (i = 0; i < LEN_RECV; i++) {
			printf("%5d->%5s:%5s,%5d,%5s,%5d\n",i, Qr[i].EVENT, Qr[i].THREAD, Qr[i].PORT, Qr[i].EVENT, Qr[i].INDEX);
			fprintf(fr, "%s,%d,%s,%d\n",Qr[i].THREAD, Qr[i].PORT,Qr[i].EVENT,Qr[i].INDEX);
		}
		printf("%s に結果が出力されました\n", FILE_NAME_1);
		fclose(fr);
	}
	
	if ((fs = fopen(FILE_NAME_2, "w")) == NULL) {
	printf("sendイベントのログの出力に失敗しました");
	}
	else {
		printf("SEND\n");
		for (j = 0; j < LEN_SEND; j++) {
			printf("%5d->%5s:%5s,%5d,%5s,%5d\n",j, Qs[j].EVENT, Qs[j].THREAD, Qs[j].PORT, Qs[j].EVENT, Qs[j].INDEX);
			fprintf(fs, "%s,%d,%s,%d\n", Qs[j].THREAD, Qs[j].PORT, Qs[j].EVENT, Qs[j].INDEX);
		}
		printf("%s に結果が出力されました\n", FILE_NAME_2);
		fclose(fs);
	}

}


