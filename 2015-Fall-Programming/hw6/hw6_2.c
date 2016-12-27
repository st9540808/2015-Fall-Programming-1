//學號：F74042086
//姓名：郭泰佑
//編譯方式：gcc -o hw6_2 hw6_2.c -lm
//執行方式：./hw6_2
//程式功能：convert binary to float or double and vice versa
//更新日期：2015.12.15
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

char getche()//copy from stackoverflow
{
	char buf = 0;
	struct termios old = {0};
	fflush(stdout);
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
	printf("%c", buf);
	return buf;
}

//宣告union包含 float&double 的 bit field
union sem
{
	struct sem_float
	{
		unsigned int mantissa : 23;
		unsigned int exp : 8;
		unsigned int sign : 1;
	} fl_part;
	struct sem_double
	{
		unsigned long long mantissa : 52;
		unsigned int exp : 11;
		unsigned int sign : 1;
	} dou_part;
} binarycode, *ptr;

int main()
{
//float
	float *floatVar, input;

	printf("Input float number : ");
	scanf("%f", &input);
	floatVar = &input;

	int *fl = (int*)floatVar;
	int binary[32], i;

	//印出float的binary representation
	for (i = 0; i < 32; i++) binary[i] = ((1 << i) & *fl) != 0 ? 1 : 0;
	for (i = 31; i >= 0; i--) printf("%d", binary[i]);

	printf("\n");
	printf("Input binary number to convert to float :\n");
	for (i = 0; i < 32; i++)//每按一個鍵就存入binary陣列中 不用按enter 打錯無法使用backspace
		binary[i] = (int)getche() - 48;
	printf("\n");

	binarycode.fl_part.sign = 0;
	binarycode.fl_part.exp = 0;
	binarycode.fl_part.mantissa = 0;

	for (i = 0; i < 32; i++)//把相對應的部份存入bit field
	{
		if (i == 0) binarycode.fl_part.sign = (binary[0] != 0 ? 1 : 0) ;
		if ((i >= 1 && i <= 8)  && binary[i] == 1)
			binarycode.fl_part.exp += (int)pow(2, 8 - i);
		if ((i >= 9 && i <= 31) && binary[i] == 1)
			binarycode.fl_part.mantissa += (int)pow(2, 31 - i);
	}

	ptr = &binarycode;
	floatVar = (float *)ptr;//強制轉型
	printf("%e\n\n", *floatVar); //印出float

//double
	double *doubleVar, input_dou;
	printf("Input double number : ");
	scanf("%lf", &input_dou);
	doubleVar = &input_dou;

	long long *dou = (long long*)doubleVar;
	int binary_dou[64];

	for (i = 0; i < 64; i++) binary_dou[i] = (((long long)1 << i) & *dou) != 0 ? 1 : 0;
	for (i = 63; i >= 0; i--) printf("%d", binary_dou[i]);

	printf("\n");
	printf("Input binary number to convert to double :\n");
	for (i = 0; i < 64; i++) binary_dou[i] = (int)getche() - 48;
	printf("\n");

	binarycode.dou_part.sign = 0;
	binarycode.dou_part.exp = 0;
	binarycode.dou_part.mantissa = 0;

	for (i = 0; i < 64; i++)//把相對應的部份存入bit field
	{
		if (i == 0) binarycode.dou_part.sign = (binary_dou[0] != 0 ? 1 : 0) ;
		if ((i >= 1 && i <= 11)  && binary_dou[i] == 1)
			binarycode.dou_part.exp += (int)pow(2, 11 - i);
		if ((i >= 12 && i <= 63) && binary_dou[i] == 1)
			binarycode.dou_part.mantissa += (long long)pow(2, 63 - i);
	}

	ptr = &binarycode;
	doubleVar = (double *)ptr;
	printf("%e\n", *doubleVar);

	return 0;
}


