//學號：F74042086
//姓名：郭泰佑
//編譯方式：gcc -o hw2 hw2.c -lm
//執行方式：./hw2 (N) (P)
//程式功能：the game of mastermind
//更新日期：2015.10.27
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int random_num(int a);
void random_initialize(void);
void printerror(void);


int main(int argc, char *argv[])
{
	int chosen, i, j, k;
	long unsigned int answer;

	answer = 0;

	//如果輸入格式錯誤 立刻跳出程式
	if (argc != 3)
		exit(1);

	//n:數字的範圍
	//p:數字的個數
	int n = (int) * argv[1] - 48;
	int p = (int) * argv[2] - 48;

	//如果p>n 立刻跳出程式
	if (p > n)
		exit(1);

	int temp[p], ansarray[p], countH = 0, countX = 0;
	int checkrepnum[10] = {0}, anstemp;

	//隨機數字產生器 但數字不包括"0"

	random_initialize();

	for (i = 0; i < p; i++)
	{
		chosen = random_num(n);

		while (chosen == 0)
			chosen = random_num(n);

		for (j = 0; j < i; j++) //此迴圈檢查是否有重複的數字
		{
			if (chosen == temp[j])
			{
				chosen = random_num(n);
				while (chosen == 0)
					chosen = random_num(n);
				j = -1 ;
			}
		}

		temp[i] = chosen;
	}


	while (countH != p)
	{
		while (1) //將輸入的數值存入陣列 直到輸入格式正確為止
		{
start :

			for (i = 0; i < 10; i++) //清空checkrepum[10]的數值
				checkrepnum[i] = 0;

			anstemp = 0;

			for (i = 0; i < p ; i++) //清空ansarray[p]的數值
				ansarray[i] = 0;

			printf("answer : ");
			for (i = 0; i < p; i++) //印出答案
				printf("%d", temp[i]);

			printf(" please enter a %d digits number : ", p);
			scanf("%lu", &answer);

			for (i = 0; i < p; i++) //將輸入的數值存入陣列裡
			{
				ansarray[i] = answer / pow(10, (p - 1) - i);
				answer %= (int)pow(10, (p - 1) - i);
			}

			if (ansarray[0] >= 10 || ansarray[0] == 0) //如果輸入大於或小於ｐ位數 則重複迴圈
			{
				printerror();
				printf("please try again\n\n");
				continue;
			}

			for (i = 0; i < p; i++) //這個迴圈檢查是否有０在輸入的答案中
			{
				if (ansarray[i] == 0)
				{
					printerror();
					printf("there's no 0 in the answer,please try again\n\n");
					goto start;
				}
			}

			//下面兩迴圈檢查是否有重複的數字在答案中
			for (i = 0; i < p; i++)
			{
				anstemp = ansarray[i];
				checkrepnum[anstemp]++;
			}

			for (i = 1; i < 10; i++)
			{
				if (checkrepnum[i] >= 2)
				{
					printerror();
					printf("there's no repetitive number in the answer, please try again\n\n");
					goto start ;
				}
				else
					continue;
			}
			break ;
		}

		countH = 0;
		countX = 0;

		for (i = 0; i < p; i++) //此迴圈用來計算"H"的數量
		{
			if (temp[i] == ansarray[i])
				++countH;
		}

		for (i = 0; i < p; i++) //此巢狀迴圈用來計算"X"的數量
		{
			for (j = 0; j < p; j++)
			{
				if (temp[i] == ansarray[j] && i != j)
					++countX;
			}
		}

		printf("%dH%dX\n", countH, countX);
		continue;

	}
	printf("bingo!\n");
	return 0;
}

void random_initialize(void)
{
	srand((unsigned) time(NULL));
}

int random_num(int a)
{
	return rand() % a + 1;
}

void printerror(void)
{
	printf("\n input error, ");
}
