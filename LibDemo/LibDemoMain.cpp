#include<cstdio>
#include<conio.h>

#include"..\.\lib\LargeNumbers.h"

int main()
{
	digit_t radix=10;
	LInteger ia(radix),ib(radix),ic(radix),id(radix),ir(radix);
	LDecimal da(radix),db(radix),dc(radix),dd(radix),dr(radix);
	LFraction fa(radix),fb(radix),fc(radix),fd(radix),fr(radix);

	printf("\t\t\t  Demo of LargeNumbers Lib\n\t\t\t\tBy LeOpArD\n\nRadix: %llu\n\n",radix);

	// Formatting I/O and Lmul (implemented with FFT)
	puts("Formatting I/O & Multiplication implemented with FFT");
	const char* cca="  -000.0098765432101234000 ";
	const char* ccb="  -003.1415";
	LDecimal dinita(cca,radix);
	FILE *fp;
	if(!(fp=fopen("Pi100.txt", "r")))
	{
		perror("Pi100.txt");
		_getch();
		return -1;
	}
	LDecimal dinitb(fp,radix,90);
	fclose(fp);
	printf("  \"%s\" * \"Pi100.txt\"\n= ", cca);
	Lmul(dinita, dinitb, dr);
	out2txt(dr,20);
	puts("\t(first 20 digits)\n");

	//Ldiv with approximation
	puts("Division with approximation");
	l_ui digcount=6;
	printf("\# of significant digits for approximation: %llu\n", digcount);
	ia=698024;
	ib=40442660;
	Ldiv(ia, ib, 2*digcount, dr);
	printf("  ");
	out2txt(ia);
	printf(" / ");
	out2txt(ib);
	printf("\n= ");
	out2txt(dr, 2*digcount,false);
	printf(" ... \n= ");
	out2txt(dr, digcount);
	puts("\t(approximation)\n");

	//Lpower
	puts("Function Lpower");
	ib=4268;
	l_ui pp=20;
	printf("  ");
	out2txt(ib);
	printf(" ^ %llu\n= ",pp);
	Lpower(ib, pp, ir);
	out2txt(ir);
	puts("\n");

	/*
	LDecimal test(" -1.0001234567890 ",10);
	Lapprox(test, digcount);
	out2txt(test, digcount);
	puts("\n");
	*/

	_getch();
	return 0;
}