#define MAP_SIZE 5

// The name of the pipe file
#define PIPE_FILE_NAME "/tmp/battle"

void server_start(int shmid);
void init_map(battlemap map);
void print_map(battlemap map, int player);
void set_ships(battlemap map);
int hasColision(int posXA, int posYA, int angleA, int posXB, int posYB, int angleB);
int checkForWinner(battlemap map);