//#define SMALL_DEMO
#define TERM_NUM 0
#define RADIX 10
#define SIGNIFICANT_DIGITS 10000
//#define BASIC_DBLFFT_MUL
//#define TERMS_PER_CHECK 10
#define SMALL_TIMER
#define FractionFormula
#define PiFormula			PiBBP
#define PiFormulaLDouble	PiBBP

#define AROutputFn		"..\\PiOutput\\ArbitraryRadix.txt"
#define StandardFn		"..\\Reference\\10,000,000DigitsOfPi.txt"
/*
"..\\Reference\\10,000,000DigitsOfPi.txt"
"..\\Reference\\1,000DigitsofPi.txt"
"..\\Reference\\100,000DigitsofPi.txt"
"..\\Reference\\1,000,000DigitsOfSqrt2.txt"
*/

#include<cstdio>
#include<conio.h>
#include<cctype>
#include<float.h>
#include<ctime>

#include"..\.\lib\MathFormula\PiNewton.cpp"
#include"..\.\lib\MathFormula\PiBBP.cpp"
#include"..\.\lib\MathFormula\Sqrt2.cpp"
#include"..\.\lib\MathFormula\Ramanujan1.cpp"

#ifdef SMALL_DEMO
int main()
{
	digit_t radix=RADIX;
	l_ui digcount=SIGNIFICANT_DIGITS;
	l_ui terms=TERM_NUM;
	puts("Approximations of Pi\n");
	puts("Pi's decimal value from the Internet:\n3.1415926535897932384626433832795028841971693993751058209749...\n");
	
#ifdef FractionFormula
	LFraction *pi_frac=PiFormula(digcount, radix, &terms);
	printf("Pi calculated with LFraction, summing %llu terms in the series in base %llu:\n",terms, radix);
	out2txt(pi_frac->nu);
	putchar('\n');
	out2txt(pi_frac->de);

	printf("\nConverting the above to LDecimal in base %llu and approximating to %llu digits:\n",radix, digcount);
	LDecimal *pi_dec=LF2D(*pi_frac, digcount);
	out2txt(*pi_dec, digcount,false);
	free(pi_dec);
	free(pi_frac);
#else
	LDecimal *pi_dec=PiFormula(digcount, radix, &terms);
	printf("%llu of Pi's digits are calculated, summing %llu terms in the series in base %llu:\n", digcount, terms, radix);
	out2txt(*pi_dec, digcount, false);
	free(pi_dec);
#endif
#ifdef PiFormulaLDouble
	printf("\n\nPi calculated with long double in decimal:\n%.*Lg",LDBL_DIG,PiFormulaLDouble(digcount, &terms));
#endif

	_getch();
	return 0;
}
#else

int main()
{
	digit_t radix = RADIX;
	l_ui digcount = SIGNIFICANT_DIGITS;
	l_ui terms=TERM_NUM;
#ifdef SMALL_TIMER
	clock_t tstart, tend;
#else
	time_t tstart, tend;
#endif
	FILE *fp;
	printf("Approximations of Pi\nThe requirement is %llu digits in radix %llu.\n\n", digcount, radix);
	if(! (fp=fopen(AROutputFn,"w")) )
	{
		perror(AROutputFn);
		_getch();
		return -1;
	}
	puts("Start computing...\n");
#ifdef SMALL_TIMER
	tstart=clock();
#else
	tstart=time(NULL);
#endif
#ifdef FractionFormula
	LFraction *pi_frac=PiFormula(digcount, radix, &terms);
	LDecimal *pi_dec=LF2D(*pi_frac, digcount);
#else
	LDecimal *pi_dec=PiFormula(digcount, radix, &terms);
#endif
#ifdef SMALL_TIMER
	tend=clock();
	printf("Finish calculation. %llu terms computed.\n", terms);
	printf(((sizeof(clock_t)==8)? "Elapsed Time: %llu ms\n\n": "Elapsed Time: %lu ms\n\n"), (tend-tstart)*1000/CLOCKS_PER_SEC);
#else
	tend=time(NULL);
	printf("Finish calculation. %llu terms computed.\n", terms);
	printf(((sizeof(time_t)==8)? "Elapsed Time: %llu s\n\n": "Elapsed Time: %lu s\n\n"), tend-tstart);
#endif
	printf("Output to \"%s\"\n\n", AROutputFn);
#ifdef SMALL_TIMER
	tstart=clock();
#else
	tstart=time(NULL);
#endif

	out2txt(*pi_dec, digcount, false, fp);

#ifdef SMALL_TIMER
	tend=clock();
	puts("Finish output");
	printf(((sizeof(clock_t)==8)? "Elapsed Time: %llu ms\n\n": "Elapsed Time: %lu ms\n\n"), (tend-tstart)*1000/CLOCKS_PER_SEC);
#else
	tend=time(NULL);
	puts("Finish output");
	printf(((sizeof(time_t)==8)? "Elapsed Time: %llu s\n\n": "Elapsed Time: %lu s\n\n"), tend-tstart);
#endif
	fclose(fp);
	free(pi_dec);
#ifdef FractionFormula
	puts("Extra Information:");
	printf("The length of the numerator is: %llu\nThe length of the denominator is: %llu\n",pi_frac->nu.len, pi_frac->de.len);
	free(pi_frac);
#endif
	//void *p=0;
	//unsigned char radbyte;
	//exr2rmem(*pi_dec, radbyte, p, 1);
	//fopen("Pi2_24","w");
	//fwrite(p, radbyte, digcount, fp);
	//fclose(fp);
	char buf[256];
	puts("\nOpen file with notepad.exe ? [Y\\N]");
	if(toupper(_getche())=='Y')
	{
		putchar('\n');
		sprintf(buf, "start notepad.exe \"%s\"", AROutputFn);
		system(buf);
	}
	else putchar('\n');
	puts("Verify results with Verification.exe ? [Y\\N]");
	if(toupper(_getche())=='Y')
	{
		putchar('\n');
		sprintf(buf, "..\\PiOutput\\Verification.exe \"%s\" \"%s\"", AROutputFn, StandardFn);
		system(buf);
	}
	else putchar('\n');
	_getch();
	return 0;
}

#endif