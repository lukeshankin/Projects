/* Versoin 3.1
 * Luke Shankin
 * Tower game checker to see if input is correct
 * works for size=3-5, too slow for 5 on solver
 * slow for 4 solver too.
 * has solver function
 * has implemented speedup on raw brute force,
 * allows for loading in known values to cut down
 * on brute force currently in rework.
 * changed solution checking mechanism.
 * hopefully will allow reuse with logic solver
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
	int *wrong; //length is puzzle_t size
		    //0 means that the value at that index + 1 
		    //is a possiblility at that spot
		    //1 means that the spot cannot hold a value 
		    //of that index + 1
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
int check_all_cells(struct puzzle_t *puzzle);
int check_cell_all(struct puzzle_t *puzzle, int x, int y);
int check_cell_hints(struct puzzle_t *puzzle, int x, int y);
int gizmo(int size, struct spot_t *array, int hone, int htwo);
int check_cell_sudoku(struct puzzle_t *puzzle, int x, int y); //not 3x3, but uniqueness in lines
int check_cell_filled(struct puzzle_t *puzzle, int x, int y); //possible misnomer, checks row and column to be clear
void print_puzzle(struct puzzle_t *puzzle);
void free_puzzle(struct puzzle_t *puzzle);
int give(FILE *input); //only works for one digit, to enable higher numbers,
		       //This function must be adapted/upgraded
 
//sight calls select_col and see_check in order to calculate whether a hint is abided by
int *select_row(int row, struct puzzle_t *puzzle);
int *select_col(int column, struct puzzle_t *puzzle);
int see_check(int *array, int length);
int *reverse(int *array, int length);
void solve_brute_force(struct puzzle_t *puzzle);
int increment_board(struct puzzle_t *puzzle, int y, int x);
int min(int x, int y) {return x < y ? x : y;}


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
	if(!check_all_cells(puzzle))
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
		puzzle->top_hint[i] = give(input);
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
		puzzle->low_hint[i] = give(input);
	//initialize possibilities
	for(int line = 0; line < puzzle->size; line++)
		for(int i = 0; i < puzzle->size; i++)
		{
			puzzle->board[line][i].wrong = calloc(sizeof(int), puzzle->size);
			if(puzzle->board[line][i].val)
				for(int j = 0; j < puzzle->size; j++)
					if(j + 1 != puzzle->board[line][i].val)
						puzzle->board[line][i].wrong[j] = 1;
		}
	//now incorporate hints
	//for(int i = 0;  i < puzzle->size; i++)
	return puzzle;
}

int check_all_cells(struct puzzle_t *puzzle)
{
	for(int x = 0; x < puzzle->size; x++)
		for(int y = 0; y < puzzle->size; y++)
			if(check_cell_all(puzzle, x, y) != 2)
				return 0;
	return 1;
}
int check_cell_all(struct puzzle_t *puzzle, int x, int y)
{
	//should return [-1, 2]
	//-1)wrong outside of spot
	//0)not correct
	//1)possible
	//2)correct
	int fill = check_cell_filled(puzzle, x, y); //cant be -1
	int sudo = check_cell_sudoku(puzzle, x, y);
	int hint = check_cell_hints(puzzle, x, y);
	if(FEEDBACK)
		printf("Fill:%d\tSudo:%d\tHint:%d\n", fill, sudo, hint);
	if(sudo < 0 || hint < 0) //fill cant be < 0
		return -1; //error
	if(!fill)
		return 0;
	if(!(fill-1)) //check to see if fill is one, in which case its false
		return 0;
	if(fill == 2 && sudo == 2 && hint == 2)
		return 2;
	if(!(sudo && hint))
		return 0;
	//if(sudo > 0 && hint > 0)
	return 1;

}

int check_cell_hints(struct puzzle_t *puzzle, int x, int y)
{
	int possible = 1;
	//puzzle->low_hint[y] vs puzzle->board[][-y]
	//puzzle->top_hint[y] vs puzzle->board[][y]
	//puzzle->left_hint[x] vs puzzle->board[x]
	//puzzle->right_hint[x] vs puzzle->board[-x]	
	
	//if(puzzle->low_hint[y] == //TODO seen(select_col(puzzle->board[y]))?

	
	/*int high = 0;
	int seen = 0;
	int *array = select_col(y, puzzle);
	int i;
	for(i = 0; i < puzzle->size; i++)
		if(array[i] > high)
		{
			high = array[i];
			seen++;
		}
	free(array); //to be used with select_col and select_row
	if(seen != puzzle->top_hint[y])
		possible = 0;
	high = 0;
	seen = 0;
	array = reverse(select_col(y, puzzle), puzzle->size);
	for(i = 0; i < puzzle->size; i++)
		if(array[i] > high)
		{
			high = array[i];
			seen++;
		}
	free(array);
	if(seen != puzzle->low_hint[y])
		possible = 0;
	
	high = 0;
	seen = 0;
	array = select_row(x, puzzle);
	for(i = 0; i < puzzle->size; i++)
		if(array[i] > high)
		{
			high = array[i];
			seen++;
		}
	free(array);
	if(seen != puzzle->left_hint[x])
		possible = 0;

	high = 0;
	seen = 0;
	array = reverse(select_row(x, puzzle), puzzle->size);
	for(i = 0; i < puzzle->size; i++)
		if(array[i] > high)
		{
			high = array[i];
			seen++;
		}
	free(array);
	if(seen != puzzle->right_hint[x])
		possible = 0;



	if(possible)
		return 1;
	return 0;*/
	struct spot_t *array = calloc(sizeof(struct spot_t), puzzle->size);
	for(int i = 0; i < puzzle->size; i++)
		array[i] = puzzle->board[x][i];
	gizmo(puzzle->size, array, puzzle->top_hint[y], puzzle->low_hint[y]);
	free(array); //to be used with select_col and select_row
	gizmo(puzzle->size, puzzle->board[x], puzzle->left_hint[x], puzzle->right_hint[x]);



	if(possible)
		return 1;
	return 0;
}
int gizmo(int size, struct spot_t *array, int hone, int htwo)
{
	//hone = hint one, htwo = hint two
	//done together because i believe it provides more
	//information (and hopefully faster)
	printf("%d| ", hone);
	for(int i = 0; i < size; i++)
		printf("%d ", array[i].val);
	printf("|%d\n", htwo);
	return 0;
}

int check_cell_sudoku(struct puzzle_t *puzzle, int x, int y)
{
	//check target is in each row
	int hfound = 0, vfound = 0;
	for(int i = 0; i < puzzle->size; i++)
	{
		if(puzzle->board[x][i].val == puzzle->board[x][y].val)
			hfound++;
	//check target is in each column
		if(puzzle->board[i][y].val == puzzle->board[x][y].val)
			vfound++;
	}
	if(!((hfound-1) || (vfound-1))) //check to see that each occur only once
		return 2;
	if(hfound == 2 || vfound == 2)
		return 0;
	if(hfound > 2 || vfound > 2)
		return -1;
	printf("I dont know how this was reached\n");
	return 1;
}

int check_cell_filled(struct puzzle_t *puzzle, int x, int y)
{
	return puzzle->board[x][y].val ? 2 : 0;
}
void print_puzzle(struct puzzle_t *puzzle)
{
	printf("size = %d\n", puzzle->size);
	printf("   ");
	for(int i = 0; i < puzzle->size; i++)
		printf("%d ", puzzle->top_hint[i]);
	printf("\n");
	for(int line = 0; line < puzzle->size; line++)
	{
		printf("%d| ", puzzle->left_hint[line]);
		for(int i = 0; i < puzzle->size; i++)
			printf("%d ", puzzle->board[line][i].val);
		printf("|%d\n", puzzle->right_hint[line]);
	}
	printf("   ");
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
	for(int line = 0; line < puzzle->size; line++)
	{
		for(int i = 0; i < puzzle->size; i++)
			free(puzzle->board[line][i].wrong);
		free(puzzle->board[line]);
	}
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
int see_check(int *array, int length)
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
	return seen;
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
	for(; !check_all_cells(puzzle); i++)
	{
		if(increment_board(puzzle, 0, 0))
		{
			printf("\nNo solutions found\nTried: %d different solutions\n", i);
			return;
		}
		if((i + 1) < i)
			printf("\tFlipping bits\n");
		if(FEEDBACK)
			print_puzzle(puzzle);
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
