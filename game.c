#include "common.h"
#include "battle-map.h"
#include "utils.h"

int main(void) {
  // Thread vars
  pid_t pid;
  pthread_t playerA, playerB;
  int thread_status;

  // Shared Memory vars
  key_t key = SHM_KEY;
  int shmid;
  int shmsize = sizeof(battlemap);

  // Try to create the shared memory
  if ((shmid = shmget(key, shmsize, IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
  }

  // Initialize the semaphores
  init_channel();

  // Split the game to the server and the clients
  // The parent Process is the server
  // The child Process (0) are the clients (with two threads)
  switch (pid = fork()) {
    case -1:
      // Error forking the process
      perror("fork");
      exit(-1);
    case 0:
      // Clients case

      // Waiting for the server to be ready
      sem_wait(server_response);
      sem_post(client_response);

      // Create the thread for the first player
      thread_status = pthread_create(&playerA, NULL, execute_playerA, (void *)(intptr_t)shmid);
      if (thread_status) {
        perror("pthread_create");
        exit(-1);
      }

      // Create the thread for the second player
      thread_status = pthread_create(&playerB, NULL, execute_playerB, (void *)(intptr_t)shmid);
      if (thread_status) {
        perror("pthread_create");
        exit(-1);
      }

      pthread_join(playerA, (void *)&thread_status);
      pthread_join(playerB, (void *)&thread_status);

      pthread_exit(NULL);
      exit(1);
    default:
      // Server case
      server_start(shmid);
  }

  sem_close(server_response);
  sem_close(client_response);
  sem_unlink(SEM_SERVER);
  sem_unlink(SEM_CLIENT);

  puts("");
  return 0;
}
