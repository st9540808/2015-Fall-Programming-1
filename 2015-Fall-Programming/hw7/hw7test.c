#include <stdio.h>
#include <stdlib.h>
int bucket_union(int **, int cols, const int [cols]);

int **dynamic_alloc_2darray(int);//generate a 1 * cols 2D array
int **add_1_row(int**, int, int);//add a row to 2D array (must given cuurent size)
//(array name, how "many" rows it has, cols)

void scanfile(FILE *, int *, int *);//read the maximum cols needed and total rows
void Print(int, int cols, int[][cols]); //print DirectMapping only

int main()
{
	int  rows = 0, cols = 0, i = 0, j = 0;
	char ch;
	FILE *fp;
	fp = fopen("bucket.in", "r");

	if (fp == NULL)
	{
		printf("file cannot open");
		exit(1);
	}

	scanfile(fp, &rows, &cols);//determine the size of array
	rewind(fp);
	printf("%d %d \n", rows, cols);

	int DirectMapping[rows][cols], *p,
	    **NewMapping = dynamic_alloc_2darray(cols);
	for (p = &DirectMapping[0][0]; p <= &DirectMapping[rows - 1][cols - 1]; p++)
		*p = 0;//initalize to 0
	add_1_row(NewMapping, 1, cols);

	//store data into two dimensional array
	while (fscanf(fp, "%d%c", &DirectMapping[i][j], &ch) != EOF)
	{
		j++;
		if (ch == '\n') {j = 0; i++;} //change to '\r'for file generated in windows
	}

	Print(rows, cols, DirectMapping);
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < cols; j++)
			printf("%d ", NewMapping[i][j]);
		printf("\n");
	}

	fclose(fp);
	for (i = 0; i < 2; i++)
		free(NewMapping[i]);
	free(NewMapping);

//	printf("\n");
	return 0;
}

int bucket_union(int **NewMapping, int cols, const int DirectMapping[cols])
{
	int i;

	for (i = 0; i < cols; i++)
	{

	}
}

int new_mapping()
{

}

void print_result()
{

}


int ** dynamic_alloc_2darray(int cols)
{
	int **array;
	array = calloc(1 , sizeof(int *));
	if (array == NULL)
	{
		printf("out of memory(1)\n");
		exit(1);
	}

	array[0] = calloc(cols , sizeof(int));
	if (array[0] == NULL)
	{
		printf("out of memory(2)\n");
		exit(1);
	}
	return array;
}

int ** add_1_row(int ** array, int size, int cols)
{
	array = realloc(array, (size + 1) * sizeof(int *));

	array[size] = calloc(cols, sizeof(int));
	if (array[size] == NULL)
	{
		printf("out of memory(3)\n");
		exit(1);
	}
	return array;
}

void scanfile(FILE *fp, int *count_rows, int *max_cols)
{
	int count_cols = 0;
	char ch;
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == ' ')
			count_cols++;

		if (ch == '\n')
		{
			(*count_rows)++;

			if (count_cols > *max_cols)
				*max_cols = count_cols;

			count_cols = 0;
		}
	}
//	(*count_rows)++; //delete it on linux
	(*max_cols)++;
}

void Print(int rows, int cols , int array[][cols])
{
	int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
			printf("%d ", array[i][j]);

		printf("\n");
	}
}
