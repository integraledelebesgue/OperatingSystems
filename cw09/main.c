#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#ifndef N_REINDEERS
#define N_REINDEERS 9
#endif

#define MAX_REINDEERS_WAITING N_REINDEERS

#define REINDEER_DELAY_HIGH 5
#define REINDEER_DELAY_LOW 2

#ifndef N_ELVES
#define N_ELVES 10
#endif

#define MAX_ELVES_WAITING 3

#define ELF_DELAY_HIGH 2
#define ELF_DELAY_LOW 1

#ifndef N_ACTIONS
#define N_ACTIONS 3
#endif

#define ACTION_DELAY_HIGH 5
#define ACTION_DELAY_LOW 2


#define random(low, high) low + rand() % (high - low)

#define set_async_cancel() pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)


typedef struct {
    sem_t* reindeers;
    sem_t* elves;
    pthread_cond_t* reindeer_sleep;
    pthread_cond_t* elf_sleep;
    pthread_mutex_t* sleep_lock;
    pthread_cond_t* santa_sleep;
    pthread_barrier_t* action_wait;
} arg_t;


#define unpack(args)\
    sem_t*  __attribute__ ((unused)) reindeers = args->reindeers;\
    sem_t*  __attribute__ ((unused)) elves = args->elves;\
    pthread_mutex_t*  __attribute__ ((unused)) sleep_lock = args->sleep_lock;\
    pthread_cond_t*  __attribute__ ((unused)) santa_sleep = args->santa_sleep;\
    pthread_cond_t*  __attribute__ ((unused)) reindeer_sleep = args->reindeer_sleep;\
    pthread_cond_t*  __attribute__ ((unused)) elf_sleep = args->elf_sleep;\
    pthread_barrier_t*  __attribute__ ((unused)) action_wait = args->action_wait;\


void action(sem_t* reindeers, sem_t* elves) {
    int reindeers_count;
    sem_getvalue(reindeers, &reindeers_count);
    
    int elves_count;
    sem_getvalue(elves, &elves_count);

    if (elves_count == 0)
        puts("[Mikołaj]: Rozwiązuję problemy elfów");

    else if (reindeers_count == 0)
        puts("[Mikołaj]: Dostarczam zabawki");

    else
        puts("[Mikołaj]: Co się dzieje???");

    fflush(stdout);
}


void reset(sem_t* sem, const int value) {
    for (int i = 0; i < value; i++)
        sem_post(sem);
}


void santa(arg_t* args) {
    unpack(args);

    int i = N_ACTIONS;

    while (i--) {
        pthread_mutex_lock(sleep_lock);
        pthread_cond_wait(santa_sleep, sleep_lock);

        puts("[Mikołaj]: Budzę się");
        fflush(stdout);

        action(reindeers, elves);

        reset(reindeers, N_REINDEERS);
        reset(elves, N_ELVES);

        sleep(random(ACTION_DELAY_LOW, ACTION_DELAY_HIGH));

        pthread_mutex_unlock(sleep_lock);

        pthread_cond_broadcast(reindeer_sleep);
        pthread_cond_broadcast(elf_sleep);

        puts("[Mikołaj]: Zasypiam");
        fflush(stdout);
    }

    puts("[Mikołaj]: Odchodzę na zasłużoną emeryturę");
    fflush(stdout);
}


void spawn_santa(arg_t* args, pthread_t* tids) {
    pthread_create(tids, NULL, (void* (*)(void*))santa, (void*)args);
}


void reindeer(arg_t* args) {
    unpack(args);

    set_async_cancel();

    int n;
    static pthread_mutex_t sleep_mx;

    while (1) {
        sleep(random(REINDEER_DELAY_LOW, REINDEER_DELAY_HIGH));
        
        if (sem_trywait(reindeers) == -1) {
            printf("[Renifer %ld]: Wracam na wakacje\n", pthread_self());
            fflush(stdout);
            continue;
        }

        printf("[Renifer %ld]: Czekam na Mikołaja\n", pthread_self());
        fflush(stdout);

        sem_getvalue(reindeers, &n);

        if (n != 0) goto wait;

        if (pthread_mutex_trylock(sleep_lock) != -1) {
            printf("[Renifer %ld]: Budzę Mikołaja\n", pthread_self());
            fflush(stdout);

            pthread_cond_signal(santa_sleep);
            pthread_mutex_unlock(sleep_lock);
        }

        wait:
        pthread_cond_wait(reindeer_sleep, &sleep_mx);
    }
}


void spawn_reindeers(arg_t* args, pthread_t* tids) {
    for (int i = 0; i < N_REINDEERS; i++)
        pthread_create(tids + i, NULL, (void* (*)(void*))reindeer, (void*)args);
}


void elf(arg_t* args) {
    unpack(args);

    set_async_cancel();

    int n;
    static pthread_mutex_t sleep_mx;

    while (1) {
        sleep(random(ELF_DELAY_LOW, ELF_DELAY_HIGH));
        
        if (sem_trywait(elves) == -1) {
            printf("[Elf %ld]: Rozwiązuję swój problem samodzielnie\n", pthread_self());
            fflush(stdout);
            continue;
        }

        printf("[Elf %ld]: czekam na Mikołaja\n", pthread_self());
        fflush(stdout);

        sem_getvalue(elves, &n);

        if (n != 0) goto wait;

        if (pthread_mutex_trylock(sleep_lock) != -1) {
            printf("[Elf %ld]: budzę Mikołaja\n", pthread_self());

            pthread_cond_signal(santa_sleep);
            pthread_mutex_unlock(sleep_lock);
        }

        wait:
        pthread_cond_wait(elf_sleep, &sleep_mx);
    }
}


void spawn_elves(arg_t* args, pthread_t* tids) {
    for (int i = 0; i < N_ELVES; i++)
        pthread_create(tids + i, NULL, (void* (*)(void*))elf, (void*)args);
}


void cancel_all(pthread_t* tids) {
    for (int i = 0; i < N_ELVES + N_REINDEERS; i++)
        pthread_cancel(tids[i]);
}


int main() {
    sem_t reindeers;
    sem_init(&reindeers, 0, MAX_REINDEERS_WAITING);

    sem_t elves;
    sem_init(&elves, 0, MAX_ELVES_WAITING);

    pthread_mutex_t sleep_lock;
    pthread_mutex_init(&sleep_lock, NULL); 

    pthread_cond_t reindeer_sleep;
    pthread_cond_init(&reindeer_sleep, NULL);
 
    pthread_cond_t elf_sleep;
    pthread_cond_init(&elf_sleep, NULL);

    pthread_cond_t santa_sleep;
    pthread_cond_init(&santa_sleep, NULL);
 
    arg_t args = (arg_t) {
        .reindeers = &reindeers,
        .elves = &elves,
        .reindeer_sleep = &reindeer_sleep,
        .elf_sleep = &elf_sleep,
        .sleep_lock = &sleep_lock,
        .santa_sleep = &santa_sleep
    };

    pthread_t tids[N_ELVES + N_REINDEERS + 1];

    spawn_santa(&args, tids);
    spawn_reindeers(&args, tids + 1);
    spawn_elves(&args, tids + N_REINDEERS + 1);

    pthread_join(tids[0], NULL);
    cancel_all(tids + 1);

    sem_destroy(&reindeers);
    sem_destroy(&elves);
    pthread_cond_destroy(&reindeer_sleep);
    pthread_cond_destroy(&elf_sleep);
    pthread_cond_destroy(&santa_sleep);
    pthread_mutex_destroy(&sleep_lock);

    return 0;
}