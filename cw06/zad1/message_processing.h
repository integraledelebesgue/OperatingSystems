#ifndef MESSAGE_PROCESSING
#define MESSAGE_PROCESSING

#define _GNU_SOURCE
#include<string.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<semaphore.h>

#include "safe_functions.h"
#include "messages.h"
#include "client_storage.h"
#include "logs.h"

extern sem_t mem_lock;

void process(client_storage* storage);
void listen(int msqid);

#endif