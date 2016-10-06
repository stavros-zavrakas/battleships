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

typedef struct battlecell {
  // Possible values for statusA, statusB
  //  1: hit
  //  0: available
  // -1: missed
  int statusA;
  int statusB;

  // Possible values for hasShipA, hasShipB
  // 1: there is a piece of the ship in this cell
  // 0: no ship in this cell
  int shipA;
  int shipB;
} cell0;

struct bmap {
  cell0 cell[MAP_SIZE][MAP_SIZE];
  int progressA;
  int progressB;
};

typedef struct bmap * battlemap;

void execute_server(int shmid);
void *execute_playerA(void *shmid);
void *execute_playerB(void *shmid);
