#include<errno.h>
#include "safe_functions.h"


key_t safe_ftok(const char* path, const int id) {
    int ret = ftok(path, id);
    if (ret == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    return ret;
}


int safe_shmget(const key_t key, const size_t size, const int flag) {
    int ret = shmget(key, size, flag);
    if (ret == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    return ret;
}


int shm_create(const key_t key, const size_t size, const int flag) {
    return safe_shmget(key, size, flag | IPC_CREAT);
}


void* shm_attach(const int id, const int flag) {
    void* seg = shmat(id, NULL, flag);
    if (seg == (void*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    return seg;
}


void shm_detach(const void* seg) {
    if (shmdt(seg) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}


void shm_control(const int id, const int cmd, struct shmid_ds* buf) {
    if (shmctl(id, cmd, buf) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}


void shm_remove(const int id) {
    shm_control(id, IPC_RMID, NULL);
}


int safe_semget(const key_t key, const int n_sems, const int flag) {
    int ret = semget(key, n_sems, flag);
    if (ret == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    return ret;
}


void safe_semop(const int id, struct sembuf* ops, unsigned n_ops) {
    if (semop(id, ops, n_ops) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}


void sem_wait(const int id, const int num) {
    safe_semop(
        id, 
        &(struct sembuf) {.sem_num = num, .sem_op = -1, .sem_flg = 0},
        1        
    );
}


int sem_trywait(const int id, const int num) {
    semop(
        id, 
        &(struct sembuf) {.sem_num = num, .sem_op = -1, .sem_flg = SEM_UNDO | IPC_NOWAIT},
        1        
    );

    return errno == EAGAIN ? -1 : 0;
}


void sem_post(const int id, const int num) {
    safe_semop(
        id, 
        &(struct sembuf) {.sem_num = num, .sem_op = 1, .sem_flg = SEM_UNDO},
        1        
    );
}


int safe_semctl(const int id, const int num, const int cmd, union semun arg) {
    int ret = semctl(id, num, cmd, arg);
    if (ret == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return ret;
}


void sem_init(const int id, const int num, const int val) {
    safe_semctl(
        id,
        num,
        SETVAL,
        (union semun) {.val = val}
    );
}


void semset_remove(const int id) {
    safe_semctl(id, 0, IPC_RMID, (union semun) {});
}
