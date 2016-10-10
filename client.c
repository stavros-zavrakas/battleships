#include "common.h"
#include "utils.h"
#include "battle-map.h"
#include "client.h"

void *execute_playerA(void *shmid) {
  battlemap map;

  int pi;
  int reqPlayer = 1;
  int winner_status = 0;

  if ((map = shmat((long)shmid, NULL, 0)) == (battlemap) - 1) {
    perror("shmat");
    exit(1);
  }

  pi = open(PIPE_FILE_NAME, 0666);

  // Waiting for the ready signal of the second player
  sem_wait(&players_ready);

  srand(time(NULL));

  do {
    sleep((rand() % 3) / PLAYER_SPEED);

    sem_wait(&player_turn);

    sem_init(client_response, 1, 0);

    hit(map, reqPlayer);

    sem_post(server_response);
    sem_wait(client_response);

    read(pi, &winner_status, sizeof(int));

    sem_post(&player_turn);
  } while (winner_status == 0);

  // Detach the shared memory
  if (shmdt(map) == -1) {
    perror("shmdt");
    exit(1);
  }

  close(pi);
}

void *execute_playerB(void *shmid) {
  battlemap map;
  int pi;
  int reqPlayer = 2;
  int winner_status = 0;

  if ((map = shmat((long)shmid, NULL, 0)) == (battlemap) - 1) {
    perror("shmat");
    exit(1);
  }

  pi = open(PIPE_FILE_NAME, 0666);

  // Announce that the second player is ready
  sem_post(&players_ready);

  srand(time(NULL));

  do {
    sleep((rand() % 3) / PLAYER_SPEED);

    sem_wait(&player_turn);

    sem_init(client_response, 1, 0);

    hit(map, reqPlayer);

    sem_post(server_response);
    sem_wait(client_response);

    read(pi, &winner_status, sizeof(int));

    sem_post(&player_turn);
  } while (winner_status == 0);

  // Detach the shared memory
  if (shmdt(map) == -1) {
    perror("shmdt");
    exit(1);
  }

  close(pi);
}

void hit(battlemap map, int reqPlayer) {
  int attemps = 0;
  int req_status, reqX, reqY;

  do {
    // Attempt 3 times to find an available squre.
    // if the random choice will fail after three times, it will scan
    // the grid and it will pick the first available.
    if (attemps < 3) {
      reqX = rand() % MAP_SIZE;
      reqY = (rand() + 112) % MAP_SIZE;
    } else {
      // Pass the reference for the reqX and the reqY and just return
      // the status (succesfull = 1, unsuccesful = 0)
      if (get_standard_position(map, &reqX, &reqY, reqPlayer) == 0) {
        puts("ERROR");
      }
    }

    req_status = request_hit(map, reqX, reqY, reqPlayer);

    attemps++;

  } while (req_status == 0);
}

int request_hit(battlemap map, int posX, int posY, int player) {
  int ship;
  int ownship;
  
  int *status;
  int *progress;

  if (player == 1) {
    status = &map->cell[posY][posX].status[PLAYER_A];
    ship = map->cell[posY][posX].ship[PLAYER_B];
    ownship = map->cell[posY][posX].ship[PLAYER_A];
    progress = &map->progress[PLAYER_A];
  } else {
    status = &map->cell[posY][posX].status[PLAYER_B];
    ship = map->cell[posY][posX].ship[PLAYER_A];
    ownship = map->cell[posY][posX].ship[PLAYER_B];
    progress = &map->progress[PLAYER_B];
  }

  // Ask the player to choose another position
  if (*status != 0 || ownship == 1) {
    return 0;
  }

  if (ship == 1) {
    *status = 1;
    (*progress)++;
  } else {
    *status = -1;
  }

  return 1;
}

int get_standard_position(battlemap map, int *posX, int *posY, int player) {
  int i, j;
  for (i = 0; i < MAP_SIZE; i++) {
    for (j = 0; j < MAP_SIZE; j++) {
      if (player == 1) {
        if ( map->cell[i][j].status[PLAYER_A] == 0 && map->cell[i][j].ship[PLAYER_A] == 0) {
          *posX = j;
          *posY = i;
          return 1;
        }
      } else {
        if ( map->cell[i][j].status[PLAYER_B] == 0 && map->cell[i][j].ship[PLAYER_B] == 0) {
          *posX = j;
          *posY = i;
          return 1;
        }
      }
    }
  }
  return 0;
}
