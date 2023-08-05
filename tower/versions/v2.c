/*	Vesrion 2
 * Luke Shankin
 * Tower game checker to see if input is correct
 * works for size=3-5, too slow for 5 on solver
 * slow for 4 solver too.
 * has solver function
 * has implemented speedup on raw brute force,
 * allows for loading in known values to cut down
 * on brute force
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //for sleep

#define FEEDBACK 0

struct spot_t{
	int val;
	int correct; //0 means val is subject to change
		     //1 means this spot is correct
};

struct puzzle_t{
	int size; //length of the board 1-9
	int *left_hint;
	int *right_hint;
	int *top_hint;
	int *low_hint;
	struct spot_t **board;
};

//check_all calls other 3 checks and returns & results.
//return 1 means it passed the test, 0 means it failed
//and something is incorrect with the board
struct puzzle_t *make_board(FILE *input);
int check_all(struct puzzle_t *puzzle);
int check_hints(struct puzzle_t *puzzle);
int check_sudoku(struct puzzle_t *puzzle); //not 3x3, but uniqueness in lines
int check_filled(struct puzzle_t *puzzle);
void print_puzzle(struct puzzle_t *puzzle);
void free_puzzle(struct puzzle_t *puzzle);
int give(FILE *input); //only works for one digit, to enable higher numbers,
		       //This function must be adapted/upgraded
 
//sight calls select_col and see_check in order to calculate whether a hint is abided by
int *select_row(int row, struct puzzle_t *puzzle);
int *select_col(int column, struct puzzle_t *puzzle);
int see_check(int hint, int *array, int length);
int *reverse(int *array, int length);
void solve_brute_force(struct puzzle_t *puzzle);
int increment_board(struct puzzle_t *puzzle, int y, int x);



int main(int argc, char **argv)   
{
	if(argc != 2)
	{
		printf("Error. Usage: ./tower [input]\n");
		return 0;
	}
	char *filename = argv[1];
	FILE *input = fopen(filename, "r");
	struct puzzle_t *puzzle = make_board(input);
	
	print_puzzle(puzzle);
	//check_all(puzzle);
	//print_puzzle(puzzle);
	solve_brute_force(puzzle);
	print_puzzle(puzzle);

	free_puzzle(puzzle);
	fclose(input);
	return 0;
}

struct puzzle_t *make_board(FILE *input)
{
	//read in file, or use preset
	//preset here
	if(input == NULL)
	{
		printf("no input specified\n");
		//exit();
		return NULL;
	}
	struct puzzle_t *puzzle = calloc(sizeof(struct puzzle_t), 1);
	puzzle->size = give(input);
	//top_hints
	puzzle->top_hint = calloc(sizeof(int), puzzle->size);
	puzzle->left_hint = calloc(sizeof(int), puzzle->size);
	puzzle->right_hint = calloc(sizeof(int), puzzle->size);
	puzzle->low_hint = calloc(sizeof(int), puzzle->size);
	puzzle->board = calloc(sizeof(struct spot_t*), puzzle->size);
	for(int i = 0; i < puzzle->size; i++)
	{
		puzzle->top_hint[i] = give(input);
	}
	//for loop left&right_hints,
	for(int line = 0; line < puzzle->size; line++)
	{
		puzzle->left_hint[line] = give(input);
		puzzle->board[line] = calloc(sizeof(struct spot_t), puzzle->size);
		for(int i = 0; i < puzzle->size; i++)
		{
			puzzle->board[line][i].val = give(input);
			if(puzzle->board[line][i].val)
				puzzle->board[line][i].correct = 1;
		}
		puzzle->right_hint[line] = give(input);
	}
	//bottom_hints
	for(int i = 0; i < puzzle->size; i++)
	{
		puzzle->low_hint[i] = give(input);
	}
	return puzzle;
}

int check_all(struct puzzle_t *puzzle)
{
	if(FEEDBACK)
	{
		if(check_filled(puzzle))
		{
			printf("Check:puzzle is full\n");
			if(check_sudoku(puzzle))
			{
				printf("Check:no duplicates\n");
				if(check_hints(puzzle))
				{
					printf("Check:follows hints\nSuccess!\n");
					return 1;
				}else{
					printf("Fail:Doesn't Abide by hints\n");
					return 0;
				}
			}else{
				printf("Fail:Duplicates in Line/Column\n");
				return 0;
			}
		}else{
			printf("Fail:Not full\n");
			return 0;
		}
		return 0; //false
	}
	return check_filled(puzzle) && check_sudoku(puzzle) && check_hints(puzzle);
}

int check_hints(struct puzzle_t *puzzle)
{
	//vertical hints
	for(int line = 0; line < puzzle->size; line++)
	{
		if(!see_check(puzzle->low_hint[line],reverse(select_col(line,puzzle), puzzle->size), puzzle->size))
			return 0;
		if(!see_check(puzzle->top_hint[line], select_col(line,puzzle), puzzle->size))
			return 0;
	}
	//horizontal hints
	for(int line = 0; line < puzzle->size; line++)
	{
		if(!see_check(puzzle->left_hint[line], select_row(line, puzzle), puzzle->size))
			return 0;
		if(!see_check(puzzle->right_hint[line], reverse(select_row(line, puzzle), puzzle->size), puzzle->size))
			return 0;
	}
	return 1;
}

int check_sudoku(struct puzzle_t *puzzle)
{
	for(int target = 1; target <= puzzle->size; target++)
	{
		//check target is in each row
		for(int line = 0; line < puzzle->size; line++)
		{
			int found = 0;
			for(int i = 0; i < puzzle->size; i++)
				if(puzzle->board[line][i].val == target)
					found++;
			if(found-1)
				return 0;
		}
		//check target is in each column
		for(int i = 0; i < puzzle->size; i++)
		{
			int found = 0;
			for(int line = 0; line < puzzle->size; line++)
				if(puzzle->board[line][i].val == target)
					found++;
			if(found-1)
				return 0;
		}
	}
	return 1;
}

int check_filled(struct puzzle_t *puzzle)
{
	for(int line = 0; line < puzzle->size; line++)
		for(int i = 0; i < puzzle->size; i++)
			if(!puzzle->board[line][i].val)
				return 0;
	return 1;
}
void print_puzzle(struct puzzle_t *puzzle)
{
	printf("size = %d\n", puzzle->size);
	printf("  ");
	for(int i = 0; i < puzzle->size; i++)
		printf("%d ", puzzle->top_hint[i]);
	printf("\n");
	for(int line = 0; line < puzzle->size; line++)
	{
		printf("%d ", puzzle->left_hint[line]);
		for(int i = 0; i < puzzle->size; i++)
			printf("%d ", puzzle->board[line][i].val);
		printf("%d\n", puzzle->right_hint[line]);
	}
	printf("  ");
	for(int i = 0; i < puzzle->size; i++)
		printf("%d ", puzzle->low_hint[i]);
	printf("\n");
}
void free_puzzle(struct puzzle_t *puzzle)
{
	free(puzzle->top_hint);
	free(puzzle->left_hint);
	free(puzzle->right_hint);
	free(puzzle->low_hint);
	for(int i = 0; i < puzzle->size; i++)
		free(puzzle->board[i]);
	free(puzzle->board);
	free(puzzle);
}
int give(FILE *input)
{
	int returnee = fgetc(input);
	while(returnee < 48 || returnee > 57)
		returnee = fgetc(input);
	return returnee - 48;
}
int *select_row(int row, struct puzzle_t *puzzle)
{
	int *returnee = calloc(sizeof(int), puzzle->size);
	for(int i = 0; i < puzzle->size; i++)
		returnee[i] = puzzle->board[row][i].val;
	return returnee;
}
int *select_col(int column, struct puzzle_t *puzzle)
{
	int *returnee = calloc(sizeof(int), puzzle->size);
	for(int line = 0; line < puzzle->size; line++)
		returnee[line] = puzzle->board[line][column].val;
	return returnee;
}
int see_check(int hint, int *array, int length)
{
	int high = 0;
	int seen = 0;
	for(int i = 0; i < length; i++)
		if(array[i] > high)
		{
			high = array[i];
			seen++;
		}
	free(array); //to be used with select_col and select_row
	return hint == seen ? 1 : 0;
}
int *reverse(int *array, int length)
{
	int *returnee = calloc(sizeof(int), length);
	for(int i = 0; i < length; i++)
		returnee[i] = array[length-i-1];
	int *tmp = array;
	array = returnee;
	free(tmp);
	return array;
}
void solve_brute_force(struct puzzle_t *puzzle)
{
	int i = 0;
	for(; !check_all(puzzle); i++)
	{
		if(increment_board(puzzle, 0, 0))
		{
			printf("\nNo solutions found\nTried: %d different solutions\n", i);
			return;
		}
		if((i + 1) < i)
			printf("\tFlipping bits\n");
		//if(i % 10000 == 0)
		//	printf(".");
		//printf("\t___Testing___\n");
		//print_puzzle(puzzle);	
		//sleep(1);
	}
	printf("\nfound a solution!\nOnly needed to try %d solutions\n", i);
}

//returns (1) true if board cannot be incremented further,
//returns (0) false if board can be incremented again
int increment_board(struct puzzle_t *puzzle, int y, int x)
{
	if(x == puzzle->size)
		return increment_board(puzzle, y+1, 0);
	if(y == puzzle->size)
		//printf("Error:Cannot increment any more\n");
		return 1;
	if(puzzle->board[y][x].correct) //this spot is already correct
		return increment_board(puzzle, y, x+1);
	if(puzzle->board[y][x].val == puzzle->size)
	{
		puzzle->board[y][x].val = 1;
		return increment_board(puzzle, y, x+1);
	}
	puzzle->board[y][x].val++;
	return 0;
}
