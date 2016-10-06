#include "utils.h"
#include "battle-map.h"
#include "server.h"

void server_start(int shmid) {
  battlemap map;

  int create_pipe_status;
  int pipe;
  int winner_status;
  int do_exit = 0;

  if ((map = shmat(shmid, NULL, 0)) == (battlemap) - 1) {
    perror("shmat");
    exit(1);
  }

  // Create the pipe to enable communicaton between the client and the server
  create_pipe_status = mkfifo(PIPE_FILE_NAME, 0666);

  if ((create_pipe_status == -1) && (errno != EEXIST)) {
    perror("Error creating the named pipe");
    exit (1);
  }

  pipe = open(PIPE_FILE_NAME, 0666);

  // Create the map and set the ships on the map
  init_map(map);
  set_ships(map);

  // Send a signal that the server is ready
  sem_post(server_response);

  // Wait for the signal of the client
  sem_wait(client_response);

  do {
    sem_init(server_response, 1, 0);
    sem_wait(server_response);

    if (do_exit == 0) {
      winner_status = check_for_winner(map);
    }

    write(pipe, &winner_status, sizeof(int));

    sem_post(client_response);

    if (winner_status != 0) {
      do_exit++;
    }
  } while (do_exit < 2);

  if (winner_status == 1) {
    print_map(map, 1);
  } else {
    print_map(map, 2);
  }

  // Detach the shared memory
  if (shmdt(map) == -1) {
    perror("shmdt");
    exit(1);
  }

  close(pipe);

  sem_close(server_response);
  sem_close(client_response);

  unlink(PIPE_FILE_NAME);
}

void init_map(battlemap map) {
  int i, j;

  for (i = 0; i < MAP_SIZE; i++) {
    for (j = 0; j < MAP_SIZE; j++) {
      map->cell[i][j].statusA = 0;
      map->cell[i][j].statusB = 0;
      map->cell[i][j].shipA = 0;
      map->cell[i][j].shipB = 0;
    }
  }

  map->progressA = 0;
  map->progressB = 0;
}

// The orientations are represented as:
// 0 - horizontal
// 1 - vertical
void set_ships(battlemap map) {
  srand(time(NULL));

  // Ship A
  int posXA;
  int posYA;

  int orientationA;

  int done = 0;

  // Iterate until the ship will be placed
  // succesfully on the map
  do {
    // Pick random point on the X axis
    posXA = rand() % MAP_SIZE;

    // Pick random point on the Y axis
    posYA = rand() % MAP_SIZE;

    // Pick the orientation
    orientationA = rand() % 2;

    if (orientationA == 0) {
      if (posXA <= MAP_SIZE - SHIP_SIZE) {
        int i;

        for (i = posXA; i < posXA + SHIP_SIZE; i++) {
          map->cell[posYA][i].shipA = 1;
        }

        done = 1;
      }
    } else {
      if (posYA <= MAP_SIZE - SHIP_SIZE) {
        int i;

        for (i = posYA; i < posYA + SHIP_SIZE; i++) {
          map->cell[i][posXA].shipA = 1;
        }

        done = 1;
      }
    }
  } while (done == 0);

  // Ship B
  int posXB;
  int posYB;

  int orientationB;

  done = 0;

  do {
    posXB = rand() % MAP_SIZE;
    posYB = rand() % MAP_SIZE;

    orientationB = rand() % 2;

    if (has_colision(posXA, posYA, orientationA, posXB, posYB, orientationB) == 0) {
      if (orientationB == 0) {
        if (posXB <= MAP_SIZE - SHIP_SIZE) {
          int i;
          for (i = posXB; i < posXB + SHIP_SIZE; i++) {
            map->cell[posYB][i].shipB = 1;
          }
          done = 1;
        }
      } else {
        if (posYB <= MAP_SIZE - SHIP_SIZE) {
          int i;
          for (i = posYB; i < posYB + SHIP_SIZE; i++) {
            map->cell[i][posXB].shipB = 1;
          }
          done = 1;
        }
      }
    }

  } while (done == 0);
}

int is_smaller(int pos_a, int pos_b, int size) {
  int smaller = 0;
  if (abs(pos_a - pos_b) < size) {
    smaller = 1;
  }

  return smaller;
}

int same_orientation_colision (int posXA, int posYA, int posXB, int posYB, int orientationB, int ship_size) {
  int colision = 0;
  int pos_a = posXA;
  int pos_b = posXB;

  if (orientationB == 1) {
    pos_a = posYA;
    pos_b = posYB;
  }

  int smaller = is_smaller(pos_a, pos_b, ship_size);
  if (smaller > 0) {
    colision = 1;
  }

  return colision;
}

int different_orientation_colision (int posXA, int posYA, int orientationA, int posXB, int posYB, int orientationB, int ship_size) {
  int colision = 0;
  int i, j;
  int cA, vA, cB, vB;

  if (orientationA == 0) {
    cA = posYA;
    vA = posXA;
    cB = posXB;
    vB = posYB;
  } else {
    cA = posXA;
    vA = posYA;
    cB = posYB;
    vB = posXB;
  }

  for (i = vA; i < vA + ship_size; i++) {
    for (j = vB; j < vB + ship_size; j++) {
      if (i == cB && cA == j) {
        colision = 1;

        break;
      }
    }
  }

  return colision;
}

int has_colision(int posXA, int posYA, int orientationA, int posXB, int posYB, int orientationB) {
  int colision = 0;

  if (orientationA == orientationB) {
    colision = same_orientation_colision(posXA, posYA, posXB, posYB, orientationB, SHIP_SIZE);
  } else {
    colision = different_orientation_colision (posXA, posYA, orientationA, posXB, posYB, orientationB, SHIP_SIZE);
  }

  return colision;
}

int check_for_winner(battlemap map) {
  if (map->progressA == SHIP_SIZE) {
    return 1;
  }

  if (map->progressB == SHIP_SIZE) {
    return 2;
  }

  return 0;
}

void print_map(battlemap map, int player) {
  int i, j;

  printf("\n  ========= BATTLE WINNER :");
  if (player == 1) {
    puts(" Player A =========");
  } else {
    puts(" Player B =========");
  }

  printf(",");
  for (i = 0; i <= MAP_SIZE * 3 - 1; i++) {
    printf("-");
  }

  puts(".");

  for (i = 0; i < MAP_SIZE; i++) {
    printf("|");
    for (j = 0; j < MAP_SIZE; j++) {
      if (player == 1) {
        if (map->cell[i][j].shipA == 1) {
          printf(" A ");
        } else {
          if ( map->cell[i][j].statusA == 0) {
            printf("   ");
          } else if (map->cell[i][j].statusA == 1) {
            printf(" X ");
          } else {
            printf(" * ");
          }
        }
      } else {
        if (map->cell[i][j].shipB == 1) {
          printf(" B ");
        } else {
          if ( map->cell[i][j].statusB == 0) {
            printf("   ");
          } else if (map->cell[i][j].statusB == 1) {
            printf(" X ");
          } else {
            printf(" * ");
          }
        }
      }
    }

    printf("|");
    puts("");
  }

  printf("\'");

  for (i = 0; i <= MAP_SIZE * 3 - 1; i++) {
    printf("-");
  }

  puts("\'");
}