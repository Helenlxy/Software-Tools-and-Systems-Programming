#include <stdio.h>

/* Each of the n elements of array elements, is the address of an
 * array of n integers.
 * Return 0 if every integer is between 1 and n^2 and all
 * n^2 integers are unique, otherwise return 1.
 */
int check_group(int **elements, int n) {
    // TODO: replace this return statement with a real function body
    int temp[n*n];
    int key = 0;
    for (int i = 0; i < n; i++)
    {
    	for (int j = 0; j < n; j++)
    	{
            for (int k = 0; k < key; k++)
    		{
    			if (elements[i][j] == temp[k])
    			{
    				return 1;
    			}	
    		}
    		if (elements[i][j] < 1 || elements[i][j] > n*n)
    		{
    			return 1;
    		}else{
    			temp[key] = elements[i][j];
    			key ++;
    		}

    		
    	}
    }
    return 0;
}

/* puzzle is a 9x9 sudoku, represented as a 1D array of 9 pointers
 * each of which points to a 1D array of 9 integers.
 * Return 0 if puzzle is a valid sudoku and 1 otherwise. You must
 * only use check_group to determine this by calling it on
 * each row, each column and each of the 9 inner 3x3 squares
 */
int check_regular_sudoku(int **puzzle) {

    for (int i = 0; i < 9; i++)
    {
        int row_arr_1[3] = {puzzle[i][0], puzzle[i][1], puzzle[i][2]};
        int row_arr_2[3] = {puzzle[i][3], puzzle[i][4], puzzle[i][5]};
        int row_arr_3[3] = {puzzle[i][6], puzzle[i][7], puzzle[i][8]};
        int *row_arr[3] = {row_arr_1, row_arr_2, row_arr_3};
    	if (check_group(row_arr, 3) == 1)
    	{
    		return 1;
    	}
    }

    for (int i = 0; i < 9; i++)
    {
        int column_arr_1[3] = {puzzle[0][i], puzzle[1][i], puzzle[2][i]};
        int column_arr_2[3] = {puzzle[3][i], puzzle[4][i], puzzle[5][i]};
        int column_arr_3[3] = {puzzle[6][i], puzzle[7][i], puzzle[8][i]};
        int *column_arr[3] = {column_arr_1, column_arr_2, column_arr_3};
    	if (check_group(column_arr, 3) == 1)
    	{
    		return 1;
    	}
    }

    for (int i = 0; i < 3; i++)
    {
    	int r = 3*i;
    	for (int j = 0; j < 3; j++)
    	{
    		int c = 3*j;
            int square_arr_1[3] = {puzzle[r][c], puzzle[r][c+1], puzzle[r][c+2]};
            int square_arr_2[3] = {puzzle[r+1][c], puzzle[r+1][c+1], puzzle[r+1][c+2]};
            int square_arr_3[3] = {puzzle[r+2][c], puzzle[r+2][c+1], puzzle[r+2][c+2]};
            int *square_arr[3] = {square_arr_1, square_arr_2, square_arr_3};
            if (check_group(square_arr, 3) == 1)
    		{
    			return 1;
    		}
    	}
    	
    }

    return 0;
}