/*
* File: ex0a.c
*meeting and knowing c language
*=================================================
*Written by:
*	Ori Vered, id = 315545400, login = orive
*	Damian Tissembaum, id = 340888122, login = damianti
*
*This program gets input and output files by argument vector. The program
*builds a dynamic array inside called lines_len and in each cell it puts from
*the input file the first element of every row that represents the
*length of that row.
*The program will put the next elements of the input row in a two dimension
*dynamic array called the_data.
*The program search for the first row that is max_row, meaning that every
*cell has the maximum value in all the rows of its column
*
*The program will print in the output file the first max_row or -1 if that
*row does not exist
*
*Compile: gcc -Wall ex0a.c -o ex0a
*Run: ./ex0a
*
*input:
*- input file with integer numbers in it
*
*output:
*- in the output file it will print the number of thwe first max_row found
*or -1 if that row does not exist
*
*--------------------------------------------------------------------------
*
*
*runtime is O(rows*cols) because we run two times in _the_data (2d array)
* once by its rows and once by its columns
*/
//-----include section---------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//-----struct section----------------------------
struct Data {
	int _num_of_lines;
	int** _the_data;
	int* _lines_len;
};

//-----prototype section-------------------------
struct Data read_rows(char* fname);
void out_max(struct Data* data, char* fname);
int find_max_row(struct Data data);
int check_max_row(struct Data data, int col, int max_row, int row);
bool is_max_row(struct Data data, int analyzed_row);
int fill_row(FILE* iptr, struct Data* data);
void malloc_line(int* capacity, struct Data* data);
void realloc_lines(int* capacity, struct Data* data);
void check_alloc_1dimension(int* data);
void check_alloc_2dimension(int** data);
void free_struct(struct Data* data);
//-----main section------------------------------
int main(int argc, char** argv)
{
	struct Data data; //Our data

	data = read_rows(argv[1]);

	out_max(&data, argv[2]);
	free_struct(&data);

	return EXIT_SUCCESS;
}
//-----functions section-------------------------
/* Function use: insert data from file to dynamic array struct, starting by
	allocating data._lines_len
	the function gets input filename and returns struct Data with the data
	from file in it
*/
struct Data read_rows(char* fname)
{
	struct Data data;
	data._num_of_lines = 0;//start dinamic array with 0

	FILE* iptr; //input streaming variable
	int num, //num input collector
		capacity = 1;//dinamic array capacity

	iptr = fopen(fname, "r");

	fscanf(iptr, "%d", &num); //first int in file

	while (!feof(iptr))
	{
		if (num == 0)//the row got 0 numbers
		{
			fscanf(iptr, "%d", &num);
			continue;
		}
		//first orign in row
		if (!data._num_of_lines)
			malloc_line(&capacity, &data);
		//resize dynamic arrays
		else if (data._num_of_lines == capacity) {
			capacity *= 2;
			realloc_lines(&capacity, &data);
		}


		//putting current line lenght in lines_lenght array
		data._lines_len[data._num_of_lines] = num;
		//alloc new row in 2d array
		data._the_data[data._num_of_lines] =
			(int*)malloc(data._lines_len[data._num_of_lines] * sizeof(int));
		check_alloc_1dimension(data._lines_len);

		//filling row with data and getting next row lenght from file
		num = fill_row(iptr, &data);

		data._num_of_lines++;
	}
	fclose(iptr);
	//free unused rows
	if (capacity > data._num_of_lines)
		realloc_lines(&data._num_of_lines, &data);

	return data;
}
//------------------------------------
void malloc_line(int* capacity, struct Data* data)
{
	//alloc lines lengh array
	data->_lines_len = (int*)malloc(*capacity * sizeof(int));
	check_alloc_1dimension(data->_lines_len);

	//alloc lines 2d array
	data->_the_data = (int**)malloc(*capacity * sizeof(int*));
}
//------------------------------------
void realloc_lines(int* capacity, struct Data* data) {
	//realloc lines lengh array
	data->_lines_len =
		(int*)realloc(data->_lines_len, *capacity * sizeof(int));
	check_alloc_1dimension(data->_lines_len);

	//realloc lines 2d array
	data->_the_data =
		(int**)realloc(data->_the_data, *capacity * sizeof(int*));
	check_alloc_2dimension(data->_the_data);
}
//------------------------------------
/* function use: putting the max_row into the output file
	function gets pointer to struct Data and output file name
*/
void out_max(struct Data* data, char* fname)
{
	FILE* optr;//output file streaming

	optr = fopen(fname, "w");

	//printing data to output file
	if(data->_num_of_lines)
		fprintf(optr, "%d", find_max_row(*data));
	else
		fprintf(optr, "%d", -1);
	fclose(optr);
}
//------------------------------------
/* function use: filling the dynamic array with data from input file
	the function gets the input streaming var and pointer to struct Data
	and returns the next row lenght in the file
*/
int fill_row(FILE* iptr, struct Data* data)
{
	int num;
	fscanf(iptr, "%d", &num);
	int i;
	for (i = 0; i < data->_lines_len[data->_num_of_lines]; ++i)
	{
		//collect num to right place
		data->_the_data[data->_num_of_lines][i] = num;
		fscanf(iptr, "%d", &num);
	}
	//returning the next row length
	return num;
}
//------------------------------------
/* function use: finding the row that all its cells are the maximum values
	of all the rows in that column
	the function gets the struct Data and returns the found row or -1 if 
	not found the max runtime of the program is in this function -
	2(rows * cols)  =  O(rows * cols)
*/
int find_max_row(struct Data data)
{
	int row;
	int max_row = 0;
	//found the row that have max num in columns 0;
	for (row = 1; row < data._num_of_lines; row++)
	{
		//find the row thet have max val in first column
		if(data._the_data[row][0] > data._the_data[max_row][0]) 
			max_row = row;
		//find the max row with rows that have same values in first column
		else if (data._the_data[row][0] == data._the_data[max_row][0])
			max_row = check_max_row(data, 1, max_row, row);
	}

	return (is_max_row(data, max_row) ? max_row : -1);
}
//------------------------------------
/* function use: when the first element of two rows are equal this function
*	will check between those two rows which is the max_row by the other 
*	columns
* 
*	the function gets the struct Data, the second column (the first has 
*	already	been checked), the max_row and the row that has the same first 
*	value as the max_row.
*	the function returns the max_row
*/
int check_max_row(struct Data data, int col, int max_row, int row)
{
	for (col; col < data._lines_len[max_row]; col++)
	{
		if (col > data._lines_len[row])//rows is end, max_row is max
			return max_row;
		if (data._the_data[max_row][col] == data._the_data[row][col])
			continue;
		return (data._the_data[max_row][col] > 
				data._the_data[row][col]) ? max_row : row;
	}
	return max_row;//max_row is end, row is max
}
//------------------------------------
/* function use: to find if a row of the data has the condition searched.
	the function gets the struct data and the number of the row analyzed
	and returns true if that row is a max_row or false if its not
*/
bool is_max_row(struct Data data, int max_row)
{
	int col,
		row;

	for (col = 1; col < data._lines_len[max_row]; col++)
	{
		for (row = 0; row < data._num_of_lines; row++)
		{
			// if we are in the analyzed row we dont check
			if (row == max_row)
				continue;

			// if the cell exists and is bigger than the one analyzed
			//then is not a max_row and returns 0
			if (col < data._lines_len[row]
				&& (data._the_data[row][col] >
					data._the_data[max_row][col]))
				return false;
		}
	}
	return true;
}
//------------------------------------
/* function use: checking allocation of data in one dimension array
	the function gets the dynamic array
*/
void check_alloc_1dimension(int* data)
{
	if (data == NULL)
	{
		printf("allocate failure\n");
		exit(1);
	}
}
//------------------------------------
/* function use: checking allocation of data in two dimension array
	the function gets the dynamic array
*/
void check_alloc_2dimension(int** data)
{
	if (data == NULL)
	{
		printf("allocate failure\n");
		exit(1);
	}
}
//-----------------------------------
/* function use: to free allocated data
	the function gets a pointer to struct Data, and it frees
	all the dynamic arrays
*/
void free_struct(struct Data* data)
{
	int i;
	for (i = 0; i < data->_num_of_lines; i++)
		free(data->_the_data[i]);

	free(data->_the_data);
	free(data->_lines_len);
}
