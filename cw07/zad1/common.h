#ifndef COMMON
#define COMMON

#include<stdlib.h>
#include<pthread.h>

#ifndef MAX_WAITING
#define MAX_WAITING 15
#endif

#ifndef N_CUSTOMERS
#define N_CUSTOMERS 10
#endif

#ifndef N_CHAIRS
#define N_CHAIRS 5
#endif

#ifndef N_BARBERS
#define N_BARBERS 10
#endif

#if (N_BARBERS < N_CHAIRS)
#error "A number of barbers should be equal or greater than the number of chairs"
#endif 

#ifndef SIGDONE
#define SIGDONE SIGUSR1
#endif


typedef enum {
    customer_wait,
    customer_get,
    chair
} sems;


typedef struct {
    size_t hairstyle;
    int pid;
} hairstyle_t;


typedef struct {
    pthread_spinlock_t put_lock;
    pthread_spinlock_t get_lock;
    size_t start;
    size_t stop;
    hairstyle_t data[MAX_WAITING];
} queue_t;


void queue_init(queue_t* queue) {
    queue->start = 0;
    queue->stop = 0;

    pthread_spin_init(&queue->get_lock, 1);
    pthread_spin_init(&queue->put_lock, 1);
}


void queue_put(queue_t* queue, hairstyle_t element) {
    size_t new_stop;

    pthread_spin_lock(&queue->put_lock);

    new_stop = (queue->stop + 1) % MAX_WAITING;
    queue->data[queue->stop] = element;
    queue->stop = new_stop;

    pthread_spin_unlock(&queue->put_lock);
}


void queue_get(queue_t* queue, hairstyle_t* element) {
    size_t new_start;

    pthread_spin_lock(&queue->get_lock);
    
    new_start = (queue->start + 1) % MAX_WAITING;
    element->hairstyle = queue->data[queue->start].hairstyle;
    element->pid = queue->data[queue->start].pid;
    queue->start = new_start;

    pthread_spin_unlock(&queue->get_lock);
}


#endif