
#define TEST_TOTAL 5000ULL
#define PROBABILITY_INTERVAL 0.05
#define RADIX L_MAX_RADIX
#define START_DIGIT 10
#define CIRCULAR_LEN 5000
#define TERM_NUM 0

#define FractionFormula
#define PiFormula			PiNewton

#include<cstdio>
#include<cstring>
#include<conio.h>
#include"..\.\lib\LargeNumbers.cpp"
#include"..\.\lib\MathFormula\PiNewton.cpp"
#include"..\.\lib\MathFormula\PiBBP.cpp"
#include"..\.\lib\MathFormula\Sqrt2.cpp"
#include"..\.\lib\MathFormula\Ramanujan1.cpp"

// return one digit from Pi's num-base representation
digit_t RandomFromPi()
{// 0 <= return value < RADIX
	static bool ifinit=false;
	static LDecimal *pi_dec;
	static l_ui cnt=0;
	static l_ui terms=TERM_NUM;
	l_ui ind;
	if(!ifinit)
	{
		pi_dec=LF2D(*PiFormula(START_DIGIT+CIRCULAR_LEN-1, RADIX, &terms), START_DIGIT+CIRCULAR_LEN-1);
		ifinit=true;
	}
	if( pi_dec->len >= START_DIGIT + cnt)
		ind= pi_dec->len - START_DIGIT - cnt;
	else return 0;
	cnt= (cnt+1) % CIRCULAR_LEN;
	return pi_dec->mem[ind];
}

#define DBLMENDINGVAL (1E-12)
int main()
{
	l_ui i;
	const l_ui intervalnum= 1.0/PROBABILITY_INTERVAL + DBLMENDINGVAL;
	l_ui intervalcnt[(l_ui)(1.0/PROBABILITY_INTERVAL + DBLMENDINGVAL)];
	memset( intervalcnt, 0,sizeof(intervalcnt));
	printf("Uniformity test on Pi's %llu-base digits\nTotal test times: %llu\n\n", RADIX, TEST_TOTAL);
	for(i=0; i<TEST_TOTAL; i++)
		intervalcnt[ (l_ui)((RandomFromPi()/(double)RADIX) /PROBABILITY_INTERVAL + DBLMENDINGVAL)]++;
	for(i=0; i<intervalnum; i++)
		printf("%4g: %6g%%\n", (double)i*PROBABILITY_INTERVAL, (double)intervalcnt[i]/TEST_TOTAL *100);
	printf("%4g\n",1.0);
	_getch();
	return 0;
}
