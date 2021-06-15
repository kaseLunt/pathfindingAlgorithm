#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <stack>

using namespace std;

void initializer(int** grid, int rows, int cols, int percentImpassable);

int randomizer();
void pathNeighbors(int** grid, int rows, int cols, struct XYpair target);
void PrintEnvironment(int** array, char** charArray, int width, int height);
void pathPlanning(int* p, struct XYpair target, int newRow, int newCol);
void greedyPath(int** grid, int rows, int cols, XYpair* min, int goalRow, int goalCol);
char** toCharacterGrid(int**, int, int);

// struct for holding XYpairs
struct XYpair {
  int rowY;
  int colX;
  int value;
};

//global queue and stack which will hold pathing targets
queue<XYpair> pathQ;
stack<XYpair> pathStack;

int main(){

  // declare variables which will be used to store user's input specifications
  char choice; // will be used to save user's choice (y/n)
  int numRows, numCols, percentImpassable, i;
  int goalX, goalY, startX, startY;
  char **characterGrid;
  int **grid; 
  bool validInput = true;
  struct XYpair goal;
  struct XYpair start;
  cout << endl << "Welcome to CSE240 WaveFront Pather" << endl;
  cout << endl << "Please tell me about the grid you want to generate." << endl << endl;

  // prompt user for number of columns, reprompt while under 10
  do {
    cout << "Enter the width or number or columns (at least 10): " << endl;
    cin >> numCols;
    // if numCols < 10, validInput is set to false
    (numCols > 9)? validInput = true : validInput = false;
  } while (!validInput);  

  // prompt user for number of columns, reprompt while under 10
  do {
    cout << "Enter the height or number of rows (at least 10): " << endl;
    cin >> numRows;
    // if numRows < 10, validInput is set to false
    (numRows < 10) ? validInput = false : validInput = true;
  } while (!validInput);
  
  // prompt user for percent impassible terrain
  do {
    validInput = true;
    cout << endl <<  "Enter the percent of impassible terrain (0 – 100): " << endl;
    cin >> percentImpassable;
    
    // if percent is invalid, set validInput to false
    if(percentImpassable < 0 || percentImpassable > 100) {
      validInput = false;
      cout << "Invalid percentage. Please try again " << endl;
    }
    // if percent is valid but over 40, warn the user of poor results and ask if they want to continue
    if(validInput == true && percentImpassable > 40) {
      cout << "Choosing a value greater than 40% might lead to poor results, do you wish to continue? (y/n): ";
      cin >> choice;
      
      // if 'y', validInput is true and program will continue
      (choice == 'y') ? validInput = true : validInput = false;
    }  
  } while(!validInput);  // while validInput is false, repeat the loop

  // create a dynamically sized grid and store its address in grid
  // first allocate space for a 1D array of integer pointers of length numRows
  grid = (int**) malloc(numRows * sizeof(int*));

  // for each row, allocate space for an array of integers of length numCols
  for(i = 0; i < numRows; i++) {
    grid[i] = (int*) malloc(numCols * sizeof(int));
  }

  // initialize the new 2D grid array with 0's and -1's according to chosen percentageImpassable
  initializer(grid, numRows, numCols, percentImpassable);

  // print the grid
  PrintEnvironment(grid, NULL, numCols, numRows);

  // prompt for goal position
  do {
    validInput = true;
    
    cout << "please enter the goal position X: " << endl;
    cin >> goalX;
    cout << "please enter the goal position Y: " << endl;
    cin >> goalY;

    // make sure goalX and goalY are in bounds
    if(goalX < 0 || goalX >= numCols || goalY < 0 || goalY >= numRows) {
      validInput = false;
      cout << "Sorry, that position is out of bounds " << endl;
    }

    // if inbounds, dereference location in the grid and check value
    if(validInput == true) {
      // if value at starting location is -1, set validInput to false
      if(grid[goalY][goalX] == -1) {
	validInput = false;
	cout << "Sorry, that position is inside an obstacle" << endl;
	// else set the position to 1
      } else { grid[goalY][goalX] = 1;

	// create a pair and push it into the queue
	goal.rowY = goalY;
	goal.colX = goalX;
	goal.value = 1;
	pathQ.push(goal);
      }
    }     
  } while (validInput == false); // loop while validInput is false

  // create a characterGrid and assign it to characterGrid
  characterGrid = toCharacterGrid(grid, numRows, numCols);
  PrintEnvironment(grid, characterGrid, numCols, numRows);
  // prompt for start position
  do {
    validInput = true;
    
    cout << "please enter the start position X: " << endl;
    cin >> startX;
    
    cout << "please enter the start position Y: " << endl;
    cin >> startY;
  

    // make sure startX and startY are in bounds
    if((startX < 0) || (startX >= numCols) || (startY < 0) || (startY >= numRows)) {
      validInput = false;
      cout << "Sorry, that position is out of bounds " << endl;
    }

    // if inbounds, dereference location in the grid and check value
    if(validInput == true) {
      // if value at starting location is -1, set validInput to false
      if(grid[startY][startX] == -1) {
	validInput = false;
	cout << "Sorry, that position is inside an obstacle" << endl;
	// else set the position to 1
      } else {
	// create a pair and push it onto the pathStack
	start.rowY = startY;
	start.colX = startX;
	pathStack.push(start);
      }
    }  
  } while (validInput == false); // loop while validInput is false

  // set the goal and start positions in characterGrid
  characterGrid[startY][startX] = '@';
  characterGrid[goalY][goalX] = '$'; 

  //begin pathing through the items in the queue
  while(!pathQ.empty()) {
    XYpair temp = pathQ.front(); pathQ.pop();
    pathNeighbors(grid, numRows, numCols, temp);
  }
  
  PrintEnvironment(grid, characterGrid, numCols, numRows);
  // XYpair and pointer to keep track of minimum
  XYpair minimum;
  XYpair* min = &minimum;
  (*min).rowY = startY;
  (*min).colX = startX;
  (*min).value = grid[startY][startX];
  
  bool found = false;
  while(!found) {
    if((*min).value == 1) {
      printf("\n\nTarget reached!\n\n Here is your path: \n\n");
      found = true;
    } else {
      greedyPath(grid, numRows, numCols, min, goalY, goalX);  
      if(characterGrid[(*min).rowY][(*min).colX] != '$') {
	characterGrid[(*min).rowY][(*min).colX] = '*';
      } 
    }  
  }
  PrintEnvironment(grid, characterGrid, numCols, numRows);
  return 0;
}

// initializes the grid with 0 and -1 according to percentImpassable
void initializer(int** grid, int rows, int cols, int percentImpassable) {
  int i, j;
  for(i = 0; i < rows; i++) {
    for(j = 0; j < cols; j++) {  
      // randomizer returns a value from 0 - 100. If this value is less than percentImpassable,
      // set the grid position to -1. Else set to 0.
      if(randomizer() < percentImpassable) {
	grid[i][j] = -1;
      } else { grid[i][j] = 0; }
    }  
  }
}

// return a random integer between 1 and 100
int randomizer() {
  int i;
  i = rand() % 100 + 1;
  return i;
}  

// print the array, orint char array in parallel if it is not NULL
 void PrintEnvironment(int** array, char** charArray, int width, int height) {
  int i, j;
  for(i = 0; i < height; i++) {
    for(j = 0; j < width; j++) {
      printf("%4d", array[i][j]);
    }
    if(charArray != NULL) {
      printf("\t");
      for(j = 0; j < width; j++) {
        printf("%3c", charArray[i][j]);
      }
    }
    cout << endl; // add a newLine after the inner for loop executes
  }
}

void pathNeighbors(int** grid, int rows, int cols, struct XYpair target) {
  int newRow, newCol, rowIndex, colIndex;
  int* p;
  // for loop which will scan the 8 surrounding positions using array math
  for(rowIndex = -1; rowIndex <= 1; rowIndex++) {
    // set target row
    newRow = target.rowY + rowIndex;
    for(colIndex = -1; colIndex <= 1; colIndex++) {
      // set target col. if out of bounds, or if indices are both 0,  break loop
      newCol = target.colX + colIndex;
      if(newRow < 0 || newRow > rows - 1 || newCol < 0 || newCol > cols - 1) {continue;}

      // set pointer p to the target address
      p = &grid[newRow][newCol];
      pathPlanning(p, target, newRow, newCol);
     
    }
  }
}

void pathPlanning(int* p, struct XYpair target, int newRow, int newCol) {
  if (*p == 0) {
    *p = target.value + 1;
    XYpair temp;
    temp.rowY = newRow;
    temp.colX = newCol;
    temp.value = target.value + 1;
    pathQ.push(temp);
  }
}  

char** toCharacterGrid(int** inputGrid, int rows, int cols) {
  int i, j;
  char** newGrid;
  newGrid = (char**) malloc(rows * sizeof(char*));
  // for each row, allocate space for an array of integers of length numCols                                                                                             
  for(i = 0; i < rows; i++) {
    newGrid[i] = (char*) malloc(cols * sizeof(char));
  }
  for(i = 0; i < rows; i++) {
    for(j = 0; j < cols; j++) {
      if(inputGrid[i][j] == -1) {
	newGrid[i][j] = '#';
      } else { newGrid[i][j] = ' '; }
    }  
  }
  return newGrid;
}  
 
//path through the array, choosing lowest value neighbor to move to
void greedyPath(int** grid, int rows, int cols, XYpair* min, int goalRow, int goalCol) {
  int rowIndex, colIndex, newRow, newCol;
  for(rowIndex = -1; rowIndex <= 1; rowIndex++) {
    newRow = (*min).rowY + rowIndex;
    for(colIndex = -1; colIndex <= 1; colIndex++) {
      newCol = (*min).colX + colIndex;
      if(newCol < 0 || newCol > cols - 1 || newRow < 0 || newRow > rows - 1) {continue;}
      if((grid[newRow][newCol] <= (*min).value) && (grid[newRow][newCol] > 0)) {
      
	(*min).rowY = newRow;
	(*min).colX = newCol;
	(*min).value = grid[newRow][newCol];
      }
    }  
  }
}

  
