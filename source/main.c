#include <string.h>
#include <stdio.h>
#include <switch.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char* defaultCell = "-";

char* cells[9] = {
	"-", "-", "-", 
	"-", "-", "-", 
	"-", "-", "-"
};

int currentPosition = 0;
char* currentPlayer = "X";
char* player1 = "X";
char* player2 = "O";

void drawGame(){
    int x = 0;
    int y = 0;

    for(unsigned int i = 0; i <= 8; i++ ){

        // Draw the current value of the cell
        char* currentCell = cells[i];

        // move the row down if a multiple of 3 (reset x)
        if(i % 3 == 0){
            y++;
            x = 0;
        }

        x++;

        // Place the player in the selected cell
        // Draw the cell to the screen
        if(i == currentPosition){
            currentCell = currentPlayer;
            printf(ANSI_COLOR_YELLOW "\x1b[%d;%dH%s" ANSI_COLOR_RESET, y + 20, x + 38, currentCell);  
        } else {
            printf("\x1b[%d;%dH%s", y + 20, x + 38, currentCell); 
        }
    }
}

void resetGame(){
    currentPosition = 0;
    cells[0] = defaultCell;
    cells[1] = defaultCell;
    cells[2] = defaultCell;
    cells[3] = defaultCell;
    cells[4] = defaultCell;
    cells[5] = defaultCell;
    cells[6] = defaultCell;
    cells[7] = defaultCell;
    cells[8] = defaultCell;
}

void checkWinner(){

    bool gameWon = false;
    bool gameDraw = false;

    // horizonal
    if(cells[0] == currentPlayer && cells[1] == currentPlayer && cells[2] == currentPlayer) gameWon = true;
    if(cells[3] == currentPlayer && cells[4] == currentPlayer && cells[5] == currentPlayer) gameWon = true;
    if(cells[6] == currentPlayer && cells[7] == currentPlayer && cells[8] == currentPlayer) gameWon = true;

    // vertical
    if(cells[0] == currentPlayer && cells[3] == currentPlayer && cells[6] == currentPlayer) gameWon = true;
    if(cells[1] == currentPlayer && cells[4] == currentPlayer && cells[7] == currentPlayer) gameWon = true;
    if(cells[2] == currentPlayer && cells[5] == currentPlayer && cells[8] == currentPlayer) gameWon = true;

    // diagonal
    if(cells[0] == currentPlayer && cells[4] == currentPlayer && cells[8] == currentPlayer) gameWon = true;
    if(cells[2] == currentPlayer && cells[3] == currentPlayer && cells[6] == currentPlayer) gameWon = true;

    // check if the game is a draw
    for(unsigned int i = 0; i <= 8; i++ ){
        if(cells[i] == defaultCell){
            gameDraw = false;
            break;
        } else {
            gameDraw = true;
        }
    }

    if(gameDraw){
        printf(ANSI_COLOR_RED "\x1b[4;2HThe game is a draw!" ANSI_COLOR_RESET);
        resetGame();
    }

    if(gameWon){
        printf(ANSI_COLOR_GREEN "\x1b[4;2H%s is the winner!" ANSI_COLOR_RESET, currentPlayer);
        resetGame();
    }
}

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);

    printf("\x1b[2;2HPress PLUS to exit.");

    // Main loop
    while(appletMainLoop())
    {
        hidScanInput();
        u64 isKeyDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // break in order to return to hbmenu
        if (isKeyDown & KEY_PLUS) break; 

        // move the player position
        if (isKeyDown & KEY_RIGHT) currentPosition++;
        if (isKeyDown & KEY_LEFT) currentPosition--;
        if (isKeyDown & KEY_UP) currentPosition-=3;
        if (isKeyDown & KEY_DOWN) currentPosition+=3;

        // ensure current positon is within bounds
        if(currentPosition < 0) currentPosition = 0;
        if (currentPosition > 8) currentPosition = 8;

        drawGame();

        // save cell and swap player
        if (isKeyDown & KEY_A){
            // check if the cell is blank, otherwise warn the player it is already taken
            if(cells[currentPosition] == defaultCell){
                // place the current players symbol in the blank space
                cells[currentPosition] = currentPlayer;

                // check if the player won
                checkWinner();

                // end turn and swap to the other player
                if(currentPlayer == player1){
                    currentPlayer = player2;
                } else {
                    currentPlayer = player1;
                }
            } else {
                printf(ANSI_COLOR_RED "\x1b[100;2HSpace is already taken! Please choose another." ANSI_COLOR_RESET);  
            }     
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}