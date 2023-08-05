/*
 * Luke Shankin
 * Tower game checker to see if input is correct
 * works for 4x4 test and 5x5
 * needs solver function
 * could structure for individual cells in board
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct puzzle_t{
	int size; //length of the board 1-9
	int *left_hint;
	int *right_hint;
	int *top_hint;
	int *low_hint;
	int **board;
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
int *select_col(int column, struct puzzle_t *puzzle);
int see_check(int hint, int *array, int length);
int *reverse(int *array, int length);




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
	check_all(puzzle);
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
	puzzle->board = calloc(sizeof(int*), puzzle->size);
	for(int i = 0; i < puzzle->size; i++)
	{
		puzzle->top_hint[i] = give(input);
	}
	//for loop left&right_hints,
	for(int line = 0; line < puzzle->size; line++)
	{
		puzzle->left_hint[line] = give(input);
		puzzle->board[line] = calloc(sizeof(int), puzzle->size);
		for(int i = 0; i < puzzle->size; i++)
			puzzle->board[line][i] = give(input);
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
	int feedback = 1;
	if(feedback)
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
		int *col = select_col(line,puzzle);
		int *rev = reverse(col, puzzle->size);
		if(!see_check(puzzle->low_hint[line],rev, puzzle->size))
		{
			free(col);
			free(rev);
			return 0;
		}
		if(!see_check(puzzle->top_hint[line], col, puzzle->size))
		{
			free(col);
			free(rev);
			return 0;
		}
		free(col);
		free(rev);
	}
	//horizontal hints
	for(int line = 0; line < puzzle->size; line++)
	{
		if(!see_check(puzzle->left_hint[line], puzzle->board[line], puzzle->size))
			return 0;
		int *rev = reverse(puzzle->board[line], puzzle->size);
		if(!see_check(puzzle->right_hint[line], rev, puzzle->size))
		{
			free(rev);
			return 0;
		}
		free(rev);
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
				if(puzzle->board[line][i] == target)
					found++;
			if(found-1)
				return 0;
		}
		//check target is in each column
		for(int i = 0; i < puzzle->size; i++)
		{
			int found = 0;
			for(int line = 0; line < puzzle->size; line++)
				if(puzzle->board[line][i] == target)
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
			if(!puzzle->board[line][i])
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
			printf("%d ", puzzle->board[line][i]);
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
int *select_col(int column, struct puzzle_t *puzzle)
{
	int *returnee = calloc(sizeof(int), puzzle->size);
	for(int line = 0; line < puzzle->size; line++)
		returnee[line] = puzzle->board[line][column];
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
	return hint == seen ? 1 : 0;
}
int *reverse(int *array, int length)
{
	int *returnee = calloc(sizeof(int), length);
	for(int i = 0; i < length; i++)
		returnee[i] = array[length-i-1];
	return returnee;
}
