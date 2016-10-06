#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

// Semaphore names definition
#define SEM_SERVER "sem0"
#define SEM_CLIENT "sem1"

// Shared memory key
#define SHM_KEY 8888

sem_t player_turn;
sem_t players_ready;
sem_t *server_response;
sem_t *client_response;

void init_channel(void);
