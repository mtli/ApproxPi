#define DEFAULT_STANDARDFILE "..\\Reference\\10,000,000DigitsOfPi.txt"
#define DEFAULT_TESTFILE "ArbitraryRadix.txt"

/*
"..\\Reference\\10,000,000DigitsOfPi.txt"
"..\\Reference\\1,000DigitsofPi.txt"
"..\\Reference\\100,000DigitsofPi.txt"
"..\\Reference\\1,000,000DigitsOfSqrt2.txt"
*/
#include<cstdio>
#include<conio.h>

int main(int argc,const char *argv[])
{
	const char *sfn,*tfn;
	FILE *sfp, *tfp;
	unsigned long long i;
	char sc,tc;
	bool flag;
	switch(argc)
    { 
		case 1:	sfn = DEFAULT_STANDARDFILE;
				tfn = DEFAULT_TESTFILE;
				break;
		case 2:	sfn = DEFAULT_STANDARDFILE;
				tfn = argv[1];
				break;
		case 3:
		default:
				sfn = argv[2];
				tfn = argv[1];
				break;
    }
	if(!(sfp=fopen(sfn, "r")))
	{
		perror(sfn);
		_getch();
		return -1;
	}
	if(!(tfp=fopen(tfn, "r")))
	{
		perror(tfn);
		fclose(sfp);
		_getch();
		return -1;
	}
	
	for(i=0,flag=false;;i++)
	{
		do sc=fgetc(sfp);
		while(sc == ' ' && sc == '\t' && sc =='\n' && sc == '\r');
		do tc=fgetc(tfp);
		while(tc == ' ' && tc == '\t' && tc =='\n' && tc == '\r');	
		if(sc == EOF || tc == EOF)
			break;
		if( sc != tc)
		{
			flag=true;
			break;
		}
	}
	if(flag)
	{
		printf("A difference is found at position %llu\nStandard \"%s\" :\n", i, sfn);
		char buf[256];
		fseek(sfp, -1, SEEK_CUR);
		fgets(buf, 20, sfp);
		printf("%s\nTest file \"%s\":\n", buf, tfn);
		fseek(tfp, -1, SEEK_CUR);
		fgets(buf, 20, tfp);
		puts(buf);
	}
	else printf("No difference is found between \n\"%s\"\n and\n\"%s\"\n", sfn, tfn);
	fclose(sfp);
	fclose(tfp);
	_getch();
	return 0;
}