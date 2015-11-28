#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int sudoku[9][9];
//Flags checking the specific checks are correct in format row, column, box.
int isCorrect[3] = {0, 0, 0};

typedef struct {
	int column,row;
} box_coordinates;

//Read the file
void readPuzzle(FILE *file) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			fscanf(file, "%d", &sudoku[i][j]);
		}
	}

}


//confirms that a specified number is not already used in the given row, column, and 3x3 section
//The following code was referenced to assist in checking a given box: http://codereview.stackexchange.com/questions/37430/sudoku-solver-in-c
int checkNum(int row, int column, int num){
	//Determines the row and column start of the entry's box, setting them to a multiple of 3
	int rStart = (row/3) * 3;
	int cStart = (column/3)*3;

	//Flag determining if the given value works
	int flag = 1;

	for (int i = 0; i < 9; i++) {

		//If number appears in the row, number is unavailable
		if (sudoku[row][i] == num) {
			flag = 0;
		}
		//If number appears in the column, number is unavailable
		if (sudoku[i][column] == num) {
			flag = 0;
		}


		//Iterate using rStart and cStart to check the specific box the number appears in
		for (int j = rStart; j< rStart+3; j++){
			for (int k = cStart; k < 3; k++) {
				if (sudoku[j][k] == num ) {
					flag = 0;

				}
			}
		}
	}
	return flag;

}
//The following code was referenced to assist in recursion / backtracking: http://codereview.stackexchange.com/questions/37430/sudoku-solver-in-c
//Solves the sudoku problem
int solver(int row, int col) {
		if (sudoku[row][col] == 0) {
			//Entry is 0; determine values to place using a for loop
			for (int i=1; i<=9; i++){
				//Confirm the number is available
				if (checkNum(row, col, i) == 1){
					//If number is available, place in position
					sudoku[row][col] = i;
					//increment to the next row if possible
					if ((row+1) < 9) {
						//Ensure that the next position can be solved
						if (solver(row + 1, col) == 1) {
							return 1;
						}

						else {
							//Reset value to 0. This allows for backtracking
							sudoku[row][col] = 0;
						}

					//Increment to the next column if possible
					} else if ((col+1)<9) {
						if (solver(0, col + 1) == 1) {
							return 1;
						}
						else {
							//Again, if the next position does not solve, do not change this position so that backtracking can occur.
							sudoku[row][col] = 0;
						}
					}
					else { //Completed
						return 1;
					}
				}
			}
		}
		else { //current number not 0, increment where possible
			if((row+1) < 9) {
				return solver(row +1, col);
			} else if((col+1) < 9) {
				return solver(0, col+1);
			} else {
				return 1;
			}
		} return 0;

}



void *rowChecker() {
	int passfail = 1;
	//goes through each row
	for (int a = 0; a < 9; a++) {
		//Sets flags for each value in a row
		int flags[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		//goes through each column
		for (int i = 0; i < 9; i++) {
			//If the number has not been used in the row, set the flag for that value to 1.
			if (flags[sudoku[a][i] - 1] == 0)
				flags[sudoku[a][i] - 1] = 1;
			else {
			//If the number has been used, break out of the loop
				passfail = 0;
				break;
			}
		}
	}
	if (passfail == 1) {
		isCorrect[0] = 1;
	}
	return NULL;
}

void *columnChecker() {
	int incorrect = 0;
	//Iterate through columns, one at a time
	for (int column = 0; column < 9; column++) {
		//Set array of flags
		int flag[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		//Iterate through rows, one at a time
		for (int row = 0; row < 9; row++) {
			//Set the current value into toCheck, and confirm that value does not occur already
			int toCheck = sudoku[row][column];
			if (flag[toCheck - 1] == 0) {
				flag[toCheck - 1] = 1;
			} else {
				//If the value already occurs, break out of the loop.
				incorrect = 1;
				break;
			}
			if (incorrect == 1)
				break;
		}
	}
	if (incorrect == 0) {
		isCorrect[1] = 1;
	}
	return NULL;
}

/*check the box that is sent into it.
	it used the box coordinates to offset the validator when checking the values
*/
void *check_box(void * boxInfo) {
	int value;
	box_coordinates *coordinates = boxInfo;
	int box_column = coordinates->column;
	int box_row = coordinates->row;

	int error = 0;
	
	//flags to check if the value was already seen in the box
	int flags[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	//go through all the elements in the box
	for (int column = 0; column < 3; column++) {
		for (int row = 0; row < 3; row++) {
			value = sudoku[row + box_row][column + box_column];


			//if the value has been found before, there is an error in the box
			if (flags[value - 1] == 1) {
				error = 1;
				break;

			} else {
				flags[value - 1] = 1;
			}
		}
		if (error == 1) {
			break;
		}
		if (error == 0) {
			isCorrect[2] = 1;

		}


	}

	return NULL;
}

void *validate(void) {
		pthread_t pth;
		pthread_create(&pth, 0, rowChecker, (void *) "Row Checker");

		pthread_t pth2;
		pthread_create(&pth2, 0, columnChecker, (void *) "Column");

		pthread_t boxThread[9];
		//start coordinates for the seprate boxes
		box_coordinates boxes[9];

		//makes threads for each box in sudoku by offsetting each box by the coordinates
		int i = 0;
		for (int countcolumn = 0; countcolumn <= 6; countcolumn += 3) {
			for (int countrow = 0; countrow <= 6; countrow += 3) {
				//Create a thread for each box to be checked
				boxes[i].column = countcolumn;
				boxes[i].row = countrow;
				pthread_create(&boxThread[i], 0, check_box, (void *) &boxes[i]);
				i++;
			}
		}
		//////JOINS/////

		pthread_join(pth, 0);
		pthread_join(pth2, 0);
		for(int i = 0; i < 9; i++){
			pthread_join(boxThread[i], 0);
		}

		return NULL;
}


int main() {
	//Open the puzzle file and store the puzzle into a variable
	FILE *puzzle = fopen("puzzle.txt", "r");
	readPuzzle(puzzle);
	FILE *solution = fopen("solution.txt", "w");

	//Run the solver. If it returns 1, then the solution is found. Otherwise, it is not found
	if (solver(0, 0)){
		printf("Solution found\n");
	}
	else {
		printf("No Solution\n");
	}
	for (int i = 0; i < 9; i++){
		for (int j = 0; j < 9; j++) {
			fprintf(solution, "%d ", sudoku[i][j]);
			if (j == 8) {
				fprintf(solution, "\n");
			}
		}
	}


	validate();
	if (isCorrect[0] == 1 && isCorrect[1] == 1 && isCorrect[2] == 1){
		printf("Sudoku is correct");
	} else {
		printf("Sudoku is incorrect");
	}

	return 0;

}


