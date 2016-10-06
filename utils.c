#include "utils.h"

void init_channel (void) {
  // The semaphores below are shared between processes
  // @see: man sem_init

  // Initialize the player_turn semaphore. The state is unlocked.
  sem_init(&player_turn, 1, 1);

  // Initialize the players_ready semaphore. The state is locked.
  sem_init(&players_ready, 1, 0);

  // Create the semaphore for the server response
  server_response = sem_open(SEM_SERVER, O_CREAT, 0666, 0);
  if (server_response == SEM_FAILED) {
    perror("ERROR CREATE SEMAPHORE");
  }

  sem_init(server_response, 1, 0);

  // Create the semaphore for the client response
  client_response = sem_open(SEM_CLIENT, O_CREAT, 0666, 0);
  if (client_response == SEM_FAILED) {
    perror("ERROR CREATE SEMAPHORE");
  }

  sem_init(client_response, 1, 0);
}
