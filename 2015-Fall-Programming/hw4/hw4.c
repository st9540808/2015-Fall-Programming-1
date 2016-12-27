//學號：F74042086
//姓名：郭泰佑
//編譯方式：gcc -o hw4 hw4.c
//執行方式：./hw4 (n個數字) (0為整數 1為浮點數)
//程式功能：randomly generate n integers or n floating numbers and sorted in descending order
//更新日期：2015.11.6
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int spilt_prime(float a[], int low, int high);		//浮點數的快速排序法
void quicksort_prime(float a[], int low, int high);

int spilt(int a[], int low, int high);			//整數的快速排序法
void quicksort(int a[], int low, int high);


void generatefloat(float a[], int n);	//隨機浮點數產生器
void generateint(int a[], int n);	//隨機整數產生器
int main(int argc, char *argv[])
{
	if (argc != 3)
		exit(1);

	int n = atoi(argv[1]);
	int int_float = (int) * argv[2] - 48;
	int i;

	if (int_float == 0)
	{
		int array[n];
		generateint(array, n); //將產生的整數亂數存入陣列

		quicksort(array, 0, n - 1);

		printf("in sorted order:\n");
		for (i = n - 1 ; i > -1; i--)
			printf("%d\n", array[i]);
		printf("\n");

	}

	else if (int_float == 1)
	{
		float array[n];
		generatefloat(array, n); //將產生的浮點數亂數存入陣列

		quicksort_prime(array, 0, n - 1);

		printf("in sorted order:\n");
		for (i = n - 1 ; i > -1; i--)
			printf("%f\n", array[i]);
		printf("\n");


	}

	else exit(1);	//如果輸入格式錯誤則跳出程式

	return 0;

}

void generateint(int a[], int n)
{
	srand((unsigned) time(NULL));

	int i, temp;
	for (i = 0; i < n; i++)
	{
		temp = rand() % 10001;
		a[i] = temp;
	}
}

void generatefloat(float a[], int n)
{
	srand((unsigned) time(NULL));

	int i;
	float temp;

	for (i = 0; i < n; i++)
	{
		temp = (float)10000 * rand() / ((float)RAND_MAX);
		a[i] = temp;
	}

}

void quicksort(int a[], int low, int high)
{
	int middle;

	if (low >= high) return;
	middle = spilt(a, low, high);
	quicksort(a, low, middle - 1);
	quicksort(a, middle + 1, high);
}

int spilt(int a[], int low, int high)
{
	int part_element = a[low];

	for (;;)
	{
		while (low < high && part_element <= a[high])
			high--;
		if (low >= high) break;
		a[low++] = a[high];

		while (low < high && a[low] <= part_element)
			low++;
		if (low >= high) break;
		a[high--] = a[low];
	}

	a[high] = part_element;
	return high;
}

void quicksort_prime(float a[], int low, int high)
{
	float middle;

	if (low >= high) return;
	middle = spilt_prime(a, low, high);
	quicksort_prime(a, low, middle - 1);
	quicksort_prime(a, middle + 1, high);
}

int spilt_prime(float a[], int low, int high)
{
	float part_element = a[low];

	for (;;)
	{
		while (low < high && part_element <= a[high])
			high--;
		if (low >= high) break;
		a[low++] = a[high];

		while (low < high && a[low] <= part_element)
			low++;
		if (low >= high) break;
		a[high--] = a[low];
	}

	a[high] = part_element;
	return high;
}
