#include <iostream>
#include<stdio.h>
#include <cstdlib>
#define X 40

int board[X][X] = { 0 };
static int k = 0;

bool checkBoard(int board[X][X], int row, int col, int N)
{
	int i, j;

	/* Check this row on left side */
	for (i = 0; i < col; i++)
		if (board[row][i])
			return false;

	/* Check upper diagonal on left side */
	for (i = row, j = col; i >= 0 && j >= 0; i--, j--)
		if (board[i][j])
			return false;

	/* Check lower diagonal on left side */
	for (i = row, j = col; j >= 0 && i<N; i++, j--)
		if (board[i][j])
			return false;

	return true;
}

//recursion
bool solveNQRecurse(int board[X][X], int col, int N)
{
	/* base case: If all queens are placed
	then return true */
	if (col == N)
	{
		k++;
		return true;
	}
	
	if (col == 0 && N % 2 == 0) { //you are in the first row
		for(int i = 0; i < (N)/2; i++) {
			if (checkBoard(board, i, col, N)){
				board[i][col] = 1;
				solveNQRecurse(board, col + 1, N);
				board[i][col] = 0;		
			}
		}
		return false;
	}

/*	if (N % 2 != 0 && col == 1 && board[((N+1)/2)][0] == 1) {
		for(int i = 0; i < ((N + 1)/2) - 1; i++) {
			if (checkBoard(board, i, col, N)) {
				board[i][col] = 1;
				solveNQRecurse(board, col + 1, N);
				board[i][col] = 0;
			}
		}
		return false;
	} */


	//try all rows in column
	for (int i = 0; i < N; i++)
	{
		//can queen be placed?
		if (checkBoard(board, i, col, N)){
			// Place this queen in board[i][col]
			board[i][col] = 1;

			// recur to next 
			solveNQRecurse(board, col + 1, N);

			board[i][col] = 0; // go back one, last one was not optimal
		}
	}

	
	return false;
}


void solveNQ(int N)
{

	if (solveNQRecurse(board, 0, N))
	{
		std::cout << ("Solution does not exist");
		return;
	}

	return;
}

int main(int argc, char *argv[])
{
		if (argc != 2) {
			std::cout << "Usage: bad1 <input size>\n";
			return 0;
		}

		int N = atoi(argv[1]);  // get first command-line argument
		if (N<1 || N>100000) {
			std::cout << "Invalid input size!\n";
			return 0;
		}

	solveNQ(N);
	
	if (N % 2 == 0)
		std::cout << k * 2;
	else
		std::cout << k;

	return 0;
}
