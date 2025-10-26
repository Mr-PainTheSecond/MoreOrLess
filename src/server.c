#include "server.h"

zsock_t* establishConnection() {
	zsock_t* requester = zsock_new(ZMQ_REQ);
	zsock_connect(requester, "tcp://localhost:5555");

	zstr_send(requester, "Roger");
	return requester;
}

int convertToInt(char* sInt) {
	int sIntSize = strlen(sInt);
	int finalInt = 0;
	for (int a = 0; a < sIntSize; a++) {
		int digit = (int)(sInt[a]) - 48;
		int addedDigit = pow(10, sIntSize - a - 1) * digit;
		finalInt += addedDigit;
	}


	return finalInt;
}

void storeYTData(Queue* queue,char* sData, int data, char* file_name) {
	YTNode* dataNode = malloc(sizeof(YTNode));
	if (dataNode == NULL) {
		exit(1);
	}

	if (queue->front == NULL) {
		queue->front = dataNode;
		queue->back = dataNode;
	}
	else {
		queue->back->next = dataNode;
		queue->back = dataNode;
	}

	dataNode->views = data;
	dataNode->filePath = malloc(sizeof(char) * strlen(file_name) + 1);
	if (dataNode->filePath == NULL) {
		exit(1);
	}

	strcpy(dataNode->filePath, file_name);

	dataNode->sViews = malloc(sizeof(char) * strlen(sData) + 1);
	if (dataNode->sViews == NULL) {
		exit(1);
	}

	strcpy(dataNode->sViews, sData);

	SDL_Surface* surf = IMG_Load(dataNode->filePath);
	dataNode->img = SDL_CreateTextureFromSurface(renderer, surf);
	dataNode->next = NULL;
}

void getYtData(zsock_t* connection, Queue* queue) {
	char* data = zstr_recv(connection);
	while (strcmp(data, "-1") != 0) {
		int intData = convertToInt(data);
		zstr_send(connection, "Roger");
		char* str = zstr_recv(connection);
		storeYTData(queue, data, intData, str);
		zstr_send(connection, "Roger");
		zstr_free(&str);
		data = zstr_recv(connection);
	}
}