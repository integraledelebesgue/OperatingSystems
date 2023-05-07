#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>


#ifndef GRID_SIZE
#define GRID_SIZE 10
#endif

#ifndef FRAME_DELAY
#define FRAME_DELAY 500
#endif

#ifndef THREAD_START_DELAY
#define THREAD_START_DELAY 100
#endif

#ifndef THREADPOOL_SIZE
#define THREADPOOL_SIZE 8
#endif

#define cell_no(i, j) (i) * GRID_SIZE + j

#define out_of_bounds(i) (i < 0) || (i >= GRID_SIZE)

#define swap(grid1, grid2, tmp) {\
    tmp = grid1;\
    grid1 = grid2;\
    grid2 = tmp;\
}\


typedef struct {
    const size_t data[GRID_SIZE * GRID_SIZE];
    size_t start;
    pthread_mutex_t read_lock;
} queue_t;


typedef struct {
    queue_t* tasks;
    short* foreground;
    short* background;
    pthread_cond_t* start;
    pthread_barrier_t* barrier;
} map_t;


void queue_init(queue_t* queue) {
    for (size_t i = 0; i < GRID_SIZE * GRID_SIZE; i++)
        *((size_t*)(queue->data) + i) = i;

    queue->start = 0;
    pthread_mutex_init(&queue->read_lock, NULL);
}


void queue_destroy(queue_t* queue) {
    pthread_mutex_destroy(&queue->read_lock);
}


void reset(queue_t* queue) {
    queue->start = 0;
}


#define empty(queue) queue->start == GRID_SIZE * GRID_SIZE

#define SUCCESS 1
#define FAILURE 0

short get(queue_t* queue, size_t* result) {
    pthread_mutex_lock(&queue->read_lock);
    
    if (empty(queue)) { 
        pthread_mutex_unlock(&queue->read_lock);
        return FAILURE;
    }
    
    *result = queue->data[queue->start++];
    
    pthread_mutex_unlock(&queue->read_lock);

    return SUCCESS;
}


short running;

void stop() {
    running = 0;
}


void init(short* foreground) {
    for (size_t i = 0; i < GRID_SIZE * GRID_SIZE; i++)
        foreground[i] = rand() % 2;
}


void draw(short* grid) {
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (grid[cell_no(i, j)]) {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }

    refresh();
}


short new_state(short* grid, const size_t cell) {
    short alive_neighbours = 0;
    const size_t i = cell / GRID_SIZE;
    const size_t j = cell % GRID_SIZE;

    for (int i_offset = -1; i_offset <= 1; i_offset++) {
        if (out_of_bounds(i + i_offset)) continue;

        for (int j_offset = -1; j_offset <= 1; j_offset++) {
            if (out_of_bounds(j + j_offset)) continue;
            if (i_offset == 0 && j_offset == 0) continue;

            alive_neighbours += grid[cell_no(i + i_offset, j + j_offset)];
        }
    }

    return (grid[cell] && alive_neighbours == 2) || (alive_neighbours == 3);
}


void compute(map_t* const map) {
    queue_t* tasks = map->tasks;
    short* foreground = map->foreground;
    short* background = map->background;
    short* tmp;
    size_t cell;
    pthread_barrier_t* barrier = map->barrier;
    pthread_cond_t* start = map->start;

    free(map);

    pthread_mutex_t start_wait_ex;
    pthread_mutex_init(&start_wait_ex, NULL);

    pthread_barrier_wait(barrier);
    pthread_cond_wait(start, &start_wait_ex);

    while (running) {
        while (get(tasks, &cell))
            background[cell] = new_state(foreground, cell);

        pthread_barrier_wait(barrier);
        swap(foreground, background, tmp);
    }

    pthread_mutex_destroy(&start_wait_ex);
}


void spawn(queue_t* tasks, short* foreground, short* background, pthread_barrier_t* barrier, pthread_cond_t* start) {
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

    pthread_t ignore;

    for (size_t i = 0; i < THREADPOOL_SIZE; i++) {
        map_t* map = malloc(sizeof(map_t));
        *map = (map_t) {
            .tasks = tasks,
            .foreground = foreground,
            .background = background,
            .barrier = barrier,
            .start = start,
        };

        pthread_create(
            &ignore, 
            &attributes, 
            (void* (*)(void*))compute, 
            (void*)map
        );
    }

    pthread_attr_destroy(&attributes);
}


int main(const int __attribute__ ((unused)) argc, const char** __attribute__ ((unused)) argv) {
    srand(time(NULL));
    setlocale(LC_CTYPE, "");
	initscr();

    signal(SIGINT, (void (*)(const int))stop);
    running = 1;

    short* foreground = malloc(GRID_SIZE * GRID_SIZE * sizeof(short));
    short* background = malloc(GRID_SIZE * GRID_SIZE * sizeof(short));
    short* tmp;

    init(foreground);

    queue_t tasks;
    queue_init(&tasks);

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, THREADPOOL_SIZE + 1);

    pthread_cond_t start;
    pthread_cond_init(&start, NULL);

    spawn(&tasks, foreground, background, &barrier, &start);

    pthread_barrier_wait(&barrier);
    usleep(THREAD_START_DELAY * THREADPOOL_SIZE);
    pthread_cond_broadcast(&start);

    while (running) {
        draw(foreground);
        usleep(1000 * FRAME_DELAY);
        pthread_barrier_wait(&barrier);
        reset(&tasks);
        swap(foreground, background, tmp);
    }

    endwin();

    queue_destroy(&tasks);

    pthread_barrier_destroy(&barrier);
    pthread_cond_destroy(&start);
    
    free(foreground);
    free(background);
    
    return 0;
}