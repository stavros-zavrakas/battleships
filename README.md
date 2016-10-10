# Battleships!

**Compile by typing:**
`make all`

**Clean the itermediate files and the gemerated binary by typing:**
`make clean`

**Start the game by typing:**
`./game`

## Description

The program is creating 2 processes, the parent process is playing the role of the server and the child process is spawning 2 threads that are playing the role of the 2 players (clients).

When the program starts we are creating a shared memory space and 2 POSIX semaphores that will be used to synchronize the server with the client. After the fork that creates the server and the client process, the client process is waiting a signal from the server that the battle map is ready with the two ships in place. After receiving the signal, the client send its signal to inform the server that the game will start and has to wait for the player turns. The client process (child) is creating the 2 POSIX threads that are executing the execute_playerA function and the execute_playerB respectively. These threads have access to the shared memory that we' ve created when the program started. After the creation of the first thread that will be used from the first player, this thread is waiting until the second thread (second player) will be ready. When the second thread is ready, it is sending a signal to the first thread to start the battle. A named pipe is used to pass messages between the client and the server, a message can be if there is a winner for the game.

### Game engine:
The two threads are waiting random amount of time and then they are trying to hit by using a POSIX semaphore. When a player is assigned to hit, it is picking randonly a point on the map. After the hit, notifies the server with a semaphore to check if there is a winner and then is waiting for the response through the named pipe. When the thread will receive the response that can be:
0 - no winner yet
1 - player 1 wins
2 - player 2 wins

If the player that just played receive a response that won, the thread stops executing the function and returns. At the same time the server enters the do_exit state and executes all the steps that informs the other player through the named pipe that there is a winner and that has to stop the execution. At the very end the server prints the map of the player that won. The ship is represented with A or B depending on the winner, the unsucessful hits with * and the successfull hits with X.

## Configurations:
- You can configure the size of the maps, the size of the ships and the speed of the game