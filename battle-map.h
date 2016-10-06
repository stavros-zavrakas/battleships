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

// The size of the battle map (height and width).
#define MAP_SIZE 5

// The size of the ship in cells
#define SHIP_SIZE 3

// The CPU player speed
#define PLAYER_SPEED 3

typedef struct battle_cell {
  // Possible values for status[PLAYER_A], status[PLAYER_B]
  //  0: available
  // -1: miss
  //  1: hit
  int status[2];

  // Possible values for ship[PLAYER_A], ship[PLAYER_B]
  // 0: no ship in this cell
  // 1: there is a piece of the ship in this cell
  int ship[2];
} battle_cell;

struct battlemap {
  battle_cell cell[MAP_SIZE][MAP_SIZE];
  int progress[2];
};

typedef struct battlemap *battlemap;

void execute_server(int shmid);
void *execute_playerA(void *shmid);
void *execute_playerB(void *shmid);
