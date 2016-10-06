// The name of the pipe file
#define PIPE_FILE_NAME "/tmp/battle"

void *execute_playerA(void *shmid);
int request_hit(battlemap map, int posX, int posY, int player);
void hit(battlemap map, int reqPlayer);
int get_standard_position(battlemap map, int *posX, int *posY, int player);
