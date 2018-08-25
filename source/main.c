#include <string.h>
#include <stdio.h>
#include <switch.h>

/**
 * Text colors
 */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define MAX_CELLS 8

/**
 * Default game settings, probably dont need to change these
 */
char* player2 = "O";
char* player1 = "X";
char* defaultCell = "-";
bool gameRunning = true;

char* cells[9] = {
	"-", "-", "-", 
	"-", "-", "-", 
	"-", "-", "-"
};

int currentPosition = 0;
char* currentPlayer = "X";
int player1Score = 0;
int player2Score = 0;

/**
 * Draw the frame buffer to the screen
 */
void drawGame(){
    int x = 0;
    int y = 0;

    for(unsigned int i = 0; i <= MAX_CELLS; i++ ){

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

/**
 * Reset all cells and player position
 */
void resetGame(){
    currentPosition = 0;
    for(unsigned int i = 0; i <= MAX_CELLS; i++ ){
		cells[i] = defaultCell;
    }
}

/**
 * Check if the player has won
 */
void checkWinner(){

    bool gameWon = false;
    bool gameDraw = false;

    // horizonal
    if((cells[0] == currentPlayer && cells[1] == currentPlayer && cells[2] == currentPlayer)
    || (cells[3] == currentPlayer && cells[4] == currentPlayer && cells[5] == currentPlayer) 
    || (cells[6] == currentPlayer && cells[7] == currentPlayer && cells[8] == currentPlayer) 

    // vertical
    || (cells[0] == currentPlayer && cells[3] == currentPlayer && cells[6] == currentPlayer) 
    || (cells[1] == currentPlayer && cells[4] == currentPlayer && cells[7] == currentPlayer) 
    || (cells[2] == currentPlayer && cells[5] == currentPlayer && cells[8] == currentPlayer) 

    // diagonal
    || (cells[0] == currentPlayer && cells[4] == currentPlayer && cells[8] == currentPlayer) 
    || (cells[2] == currentPlayer && cells[3] == currentPlayer && cells[6] == currentPlayer)) gameWon = true;

    // check if the game is a draw
    for(unsigned int i = 0; i <= MAX_CELLS; i++ ){
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
        if(currentPlayer == player1){
            player1Score++;
        } else {
            player2Score++;
        }
        printf(ANSI_COLOR_GREEN "\x1b[6;2H%s is the winner!" ANSI_COLOR_RESET, currentPlayer);
        resetGame();
    }
}

/**
 * End turn and swap to the other player
 */
void changePlayer()
{
	if(currentPlayer == player1){
	    currentPlayer = player2;
	} else {
	    currentPlayer = player1;
	}
}

void gameUpdate()
{
	u64 key = hidKeysDown(CONTROLLER_P1_AUTO);

    // quit the game
    if (key & KEY_PLUS) gameRunning = false; 

    // move the player position
    if (key & KEY_RIGHT) currentPosition++;
    if (key & KEY_LEFT) currentPosition--;
    if (key & KEY_UP) currentPosition-=3;
    if (key & KEY_DOWN) currentPosition+=3;

    // ensure current positon is within bounds
    if(currentPosition < 0) currentPosition = 0;
    if (currentPosition > MAX_CELLS) currentPosition = MAX_CELLS;

    drawGame();

    // save cell and swap player
    if (key & KEY_A){
        // check if the cell is blank, otherwise warn the player it is already taken
        if(cells[currentPosition] == defaultCell){
            // place the current players symbol in the blank space
            cells[currentPosition] = currentPlayer;

            // check if the player won
            checkWinner();
            changePlayer();
        } else {
            printf(ANSI_COLOR_RED "\x1b[44;2HSpace is already taken! Please choose another." ANSI_COLOR_RESET);  
        }     
    }
}

/**
 * Main app loop
 * 
 * @param  argc 
 * @param  argv 
 * @return
 */
int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);

    // Main loop
    while(appletMainLoop())
    {
        // draw high scores
        printf("\x1b[2;60HPress PLUS to exit.");
        printf("\x1b[2;2HPlayer X Score: %i", player1Score);
        printf("\x1b[4;2HPlayer O Score: %i", player2Score);

        hidScanInput();
        gameUpdate();

        if(!gameRunning) break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}