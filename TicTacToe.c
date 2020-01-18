/*
Project: TicTacToe
Name: Ernst Mach and David Mehic
Date: February 22, 2019
Description: A simple TicTacToe game using serial communication so that players can play on separate computers.
Class: TEJ4M1-01
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#define TURN_COLOR 1                                    //Value for the colour of the board when it is the player's turn
#define NOT_TURN_COLOR 2                                //Value for the colour of the board when it is the opponent's turn

#define WIN 1                                           //Value for when the results is a win
#define LOSE 2                                          //Value for when the result is a loss
#define TIE 3                                           //Value for when the result is a tie

#define INITIAL_TURN 0                                  //Initial turn for the player

void assignInput(int grid[], int input, int turn);      //Function to assign characters to the grid
void drawGrid(int grid[], int turn);                    //Function to draw the grid on the screen
char charForTurn(int i);                                //Function to indicate is symbol placed in a spot is an X or an O
int isPositionOccupied(int grid[], int pos);            //Function to check if a space has been used
int checkWinner(int grid[]);                            //Function to check if there is a winner
int isGridFull(int grid[]);                             //Function to check if the grid is full

int main() {

 //Initialize curses window
  initscr();
  start_color();
  init_pair(TURN_COLOR, COLOR_GREEN, COLOR_BLACK);
  init_pair(NOT_TURN_COLOR, COLOR_RED, COLOR_BLACK);
  timeout(1);
  cbreak();
  noecho();

  system("stty -F /dev/ttyS0 -icrnl -icanon clocal -echo min 0 time 1");

  int gameGrid[9];                                        //Sets all the spaces on the game grid to -1 / blank
  for (int i = 0; i < 9; ++i){
    gameGrid[i] = -1;
  }

  FILE * myfileptr;
  myfileptr = fopen("/dev/ttyS0", "r+");

  int turn = INITIAL_TURN;                                //Set the player's turn
  int result;                                             //Variable for the end reults of the match

  while (1) {
    drawGrid(gameGrid, turn);                             //Draw the grid
    if (isGridFull(gameGrid)) {                           //Check if the grid is filled
      result = TIE;
      break;                                              //Jump out of the while loop
    }

    if (turn == 1) {                                      //If it is players turn
 	    char charInput = getch();                           //Get a key input to select a location
 	    int intRepresentation = charInput - '0';
 	    assignInput(gameGrid, intRepresentation, turn);     //Assigns that location to a character
	
 	    fprintf(myfileptr, "%c", charInput);                //Sends the input to the other computer
 	    fflush(myfileptr);

 	    int winnerCheck = checkWinner(gameGrid);            //Checks who has won the game
 	    if (winnerCheck == 1) {                      
 		    result = WIN;
		    mvprintw(8,0, "YOU WIN");
 		    break;                                            //Jump out of the while loop
	    } else if (winnerCheck != -1) {                     //Checks who has lost the game
 		    result = LOSE;
		    mvprintw(8,0, "YOU LOSE");
 		    break;                                            //Jump out of the while loop
	    }

	    if (charInput != -1) {
		    turn = 0;
	    }	
    } else {                                            	//If it is opponents turn
      char in = fgetc(myfileptr);                        	//Get input from opponent
      if (in != -1) {                                   	//If value isnt blank
        mvprintw(5, 5, "%c", in); 	
        assignInput(gameGrid, (in - '0'), turn);          //Assign that location to with a character
	      int winnerCheck = checkWinner(gameGrid);          //Checks if someone has won the game
 	      if (winnerCheck == 1) {                      
 		      result = WIN;
		      mvprintw(8,0, "YOU WIN");
 		      break;                                          //Jump out of the while loop
	      } else if (winnerCheck != -1) {                   //Checks who has lost the game
 		      result = LOSE;
		      mvprintw(8,0, "YOU LOSE");
 		      break;                                          //Jump out of the while loop
	      }
 	      turn = 1;                                         //Change the turn
      }
    }
  }
 
  drawGrid(gameGrid, turn);                               //Draw the game grid
  fclose(myfileptr);                                      
  sleep(5);                                               //Wait 5 seconds
  endwin();                                               //Close the window
  return 0;
}

//Function to check if the grid is full by checking for if there is an open space
int isGridFull(int grid[]) {
 for (int i = 0; i < 9; ++i) {
    if (grid[i] == -1) {
      return 0;
    }
  }
 return 1;
}

//Function to check if a player has won
int checkWinner(int grid[]) {
  int grid_2d[3][3];
  for (int i = 0; i < 9; ++i){        //Assign the grid array into a 2D array
    int row = 2 - i / 3;
    int col = i - row * 3;
    grid_2d[row][col] = grid[i];
  }

 //Checks for a horizontal win
 for (int i = 0; i < 3; ++i){
    if ((grid_2d[i][0] == grid_2d[i][1] && grid_2d[i][0] == grid_2d[i][2]) && grid_2d[i][0] != -1) {
    return grid_2d[i][0];             //Returns the character at that location
    }
  }

 //Checks for a vertical win
 for (int i = 0; i < 3; ++i){
   if ((grid_2d[0][i] == grid_2d[1][i] && grid_2d[0][i] == grid_2d[2][i]) && grid_2d[0][i] != -1) {
     return grid_2d[0][i];            //Returns the character at that location
    }
  }

 //Checks for a diagonal win
  if ((grid_2d[0][0] == grid_2d[1][1] && grid_2d[0][0] == grid_2d[2][2]) && grid_2d[0][0] != -1) {
    return grid_2d[0][0];
  } else if ((grid_2d[2][0] == grid_2d[1][1] && grid_2d[2][0] == grid_2d[0][2]) && grid_2d[2][0] != -1) {
    return grid_2d[2][0];
  }

  return -1;                          //Returns a blank
}

//Function that places a character into the specified location
void assignInput(int grid[], int input, int turn) {
  if (isPositionOccupied(grid, input - 1) != 0 && isPositionOccupied(grid, input - 1) != 1) {
    grid[input - 1] = turn;
  }
}

//Function that draws the grid onto the screen
void drawGrid(int grid[], int turn) {
  int color;
  
  if (turn) {                         //Will change colour based on which player's turn it currently is
    color = TURN_COLOR;
  } else {
    color = NOT_TURN_COLOR;
  }
 
  attron(COLOR_PAIR(color));
  mvprintw(0, 0, " ------------- ");
  mvprintw(1, 0, "| %c | %c | %c |", charForTurn(grid[6]), charForTurn(grid[7]), charForTurn(grid[8]));
  mvprintw(2, 0, " ------------- ");
  mvprintw(3, 0, "| %c | %c | %c |", charForTurn(grid[3]), charForTurn(grid[4]), charForTurn(grid[5]));
  mvprintw(4, 0, " ------------- ");
  mvprintw(5, 0, "| %c | %c | %c |", charForTurn(grid[0]), charForTurn(grid[1]), charForTurn(grid[2]));
  mvprintw(6, 0, " ------------- ");
  refresh();
}

//Function that checks if the current posiution is occupied
int isPositionOccupied(int grid[], int pos) {
  return grid[pos];
}

//Function to determine the current symbol that will be placed
char charForTurn(int i) {
  if (i == 1) {
    return 'O';
  } else if (i == 0) {
    return 'X';
  }
 return ' ';
}