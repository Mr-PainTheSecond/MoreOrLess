#ifndef SERVER_H_
#define SERVER_H_
#include "commons.h"
#include "globals.h"
zsock_t* establishConnection();
void getYtData(zsock_t* connection, Queue* queue);
#endif
