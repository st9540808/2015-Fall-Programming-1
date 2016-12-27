//學號：F74042086
//姓名：郭泰佑
//編譯方式：gcc -o hw5 hw5.c -lm
//執行方式：./hw2 (N) (P)
//程式功能：guessing the answer of the game mastermind
//更新日期：2015.11.23
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void guess(int [], unsigned long [], unsigned long, int);//猜答案的方選("使用遞迴")
void rearrange(unsigned long [], unsigned long);//重新整理答案資料庫(在guess()中使用)
void realanswer_generator(int [], int, int); //隨機產生真正的答案
void guessHX(int [], int [], int);//計算H&X
int check_repetitive(int [], int);//在資料庫檢查每一個元素是否有重複的數字
void convert(int [], unsigned long, int);//把數值拆成一個一個數字存入陣列
void database(unsigned long [], int, int);//產生所有可能的答案("使用遞迴")
unsigned long database_attach(int [10], int);//database的附屬方選,回傳個位數之前的數值
unsigned long factorial(int);//階乘

int position_external, countH = 0, countX = 0;
unsigned long element_ori, times = 0;

int main(int argc, char *argv[])
{
	if (argc != 3)
		exit(1);

	int number 	 = (int) * argv[1] - 48;
	int position = (int) * argv[2] - 48;
	position_external = position;

	if (position > number)
		exit(1);

	element_ori = factorial(number) / factorial(number - position);
	printf("\ninitial possibilites : %lu\nthe answer : ", element_ori); //計算所有可能答案的個數

	int i, realans[position];//由亂數產生器產生的真實答案存入realans[]
	unsigned long int data[element_ori];

	realanswer_generator(realans, number, position);

	for (i = 0; i < position; i++)
		printf("%d", realans[i]);

	printf("\n");

	database(data, number, position);
	guess(realans, data, element_ori, position);

	printf("\n\ntotal : %lu times\n\n", times); //計算總共猜了多少次

	return 0;
}

void guess(int realans[], unsigned long possans_ori[], unsigned long element, int position)
{
	int temp[position], pivot[position], i,
	    tempH, tempX;
	unsigned long  chosen, tempdata, element_left = 0;

	countH = 0;
	countX = 0;

	chosen = rand() % element;
	tempdata = possans_ori[chosen];
	possans_ori[chosen] = 0;

	convert(pivot, tempdata, position);
	guessHX(pivot, realans, position);

	printf("|  %lu  |  %dH%dX  |", tempdata, countH, countX);

	if (countH == position)
	{
		times++;
		return;
	}
	else
	{
		times++;
		tempH = countH;
		tempX = countX;

		for (i = 0; i < element; i++)
		{

			countH = 0;
			countX = 0;

			tempdata = possans_ori[i];
			convert(temp, tempdata, position);
			guessHX(temp, pivot, position);

			if (tempdata != 0)
			{
				if (tempH == countH && tempX == countX)
				{
					element_left++;
					continue;
				}
			}
			possans_ori[i] = 0;
		}
		rearrange(possans_ori, element);

		printf("  %6lu possibility(s) left\n", element_left);

		guess(realans, possans_ori, element_left, position);
	}
}

void rearrange(unsigned long possans_array[], unsigned long element)
{
	int i;
	unsigned long countdata = 0;

	if (element == 1)
		return;

	for (i = 0; i < element; i++)
	{
		if (possans_array[i] != 0)
		{
			possans_array[countdata] = possans_array[i];
			countdata++;
		}
	}
}

void realanswer_generator(int realans[], int number, int position)
{
	int chosen, i, j;

	//隨機數字產生器 但數字不包括"0"
	srand((unsigned) time(NULL));

	for (i = 0; i < position; i++)
	{
		chosen = rand() % number + 1;

		while (chosen == 0)
			chosen = rand() % number + 1;

		for (j = 0; j < i; j++) //此迴圈檢查產生的答案中是否有重複的數字
		{
			if (chosen == realans[j])
			{
				chosen = rand() % number + 1;

				while (chosen == 0)
					chosen = rand() % number + 1;
				j = -1 ;
			}
		}
		realans[i] = chosen;
	}
}

void guessHX(int ansarray[], int tempans[], int position)
{
	int i, j;
	for (i = 0; i < position; i++) //此迴圈用來計算"H"的數量
	{
		if (tempans[i] == ansarray[i])
			++countH;
	}

	for (i = 0; i < position; i++) //此巢狀迴圈用來計算"X"的數量
	{
		for (j = 0; j < position; j++)
		{
			if (tempans[i] == ansarray[j] && i != j)
				++countX;
		}
	}
}


int check_repetitive(int temp[], int position)
{
	int	checkrep[10], temp_, i;

	for (i = 0; i < 10; i++)
		checkrep[i] = 0;

	for (i = 0; i < position; i++)
	{
		temp_ = temp[i];
		checkrep[temp_]++;
	}

	for (i = 0; i < 10; i++)
		if (checkrep[i] >= 2)
			return 1;

	return 0;
}

void convert(int ansarray[], unsigned long inputanswer, int position)//convert(要存入的陣列,要存入的數值)
{
	int i;

	for (i = 0; i < position; i++) //將輸入的數值存入陣列裡
	{
		ansarray[i] = inputanswer / pow(10, (position - 1) - i);
		inputanswer %= (int)pow(10, (position - 1) - i);
	}
}



int digits[10] = {0};
unsigned long data_current = 0;
void database(unsigned long data[], int number, int position)
{
	int i;

	if (position == 1)
	{
		int temp[position_external];
		unsigned long tempdata;

		for (i = 1; i < number + 1; i++)
		{
			tempdata = database_attach(digits, position_external) + i;
			convert(temp, tempdata, position_external);

			if (check_repetitive(temp, position_external) == 0)
			{
				data[data_current] = tempdata;
				data_current++;
			}
		}
	}
	else
	{
		for (i = 1; i < number + 1; i++)
		{
			digits[position - 1] = i;
			database(data, number, position - 1);
		}
	}
}

unsigned long database_attach(int a[10], int position)
{
	if (position == 1)
		return 0;
	else
	{
		unsigned long temp;
		temp = digits[position - 1] * pow(10, position - 1);
		return (temp + database_attach(digits, position - 1));
	}
}

unsigned long factorial(int n)
{
	if (n == 0)
		return 1;
	else
		return (n * factorial(n - 1));
}
