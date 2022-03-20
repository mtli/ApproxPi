/*
Contents:
One of the most famous Ramanujan's series of pi realized with LargeNumber lib

Written by LeOpArD, 2011-11-7
*/
#ifndef Ramanujan1_cpp
#define Ramanujan1_cpp

//bug with terms adding, the error escalates
#include"..\LargeNumbers.h"
#include"Sqrt2.cpp"

LDecimal* Ramanujan1(l_ui digcount, digit_t radix=10, l_ui *terms=0)
{
	if(!terms)
		if( !(terms = (l_ui*)malloc(sizeof(*terms))) )
		{
			insufficientmem();
			return 0;
		}
		else *terms = 0;
	if(!*terms)
		*terms=0.1284*digcount*log10((double)radix)+1;
	LDecimal *sqrtof2;
	if(radix == 9 && digcount <= 500000)
	{
		FILE *fp;
		if(!(fp = fopen("..\\Reference\\1,000,000DigitsOfSqrt2.txt", "r")))
		{
			perror("..\\Reference\\1,000,000DigitsOfSqrt2.txt");
			_getch();
			exit(-1);
		}
		if(!(sqrtof2 = (LDecimal*)malloc(sizeof(LDecimal))))
		{
			insufficientmem();
			return 0;
		}
		sqrtof2->LDecimal::LDecimal(fp, radix, 2*digcount);
		fclose(fp);
	}
	else
	{
		LFraction *sqrtof2f = Sqrt2(2*digcount, radix);
		sqrtof2=LF2D( *sqrtof2f, 2*digcount);
	}
	LDecimal res(radix,L_MEM_SIZE_BIG);
	if(*terms ==1)
	{
		LFraction m(9801LL, 4412ULL, radix);
		Lmul(*LF2D(m, 2*digcount), *sqrtof2, res);
		Lapprox(res, digcount);
		return res.transfermem();
	}
	const LInteger c6147814464(6147814464LL,radix);
	LFraction coef(radix,L_MEM_SIZE_BIG),cur0(radix,L_MEM_SIZE_BIG),cur1(radix,L_MEM_SIZE_BIG);
	bool cur01;
	LInteger k(radix,L_MEM_SIZE_BIG), k4_0(radix,L_MEM_SIZE_BIG),k4_1(radix,L_MEM_SIZE_BIG),k26390_1103(radix,L_MEM_SIZE_BIG), k26390_25287(radix,L_MEM_SIZE_BIG);
	LInteger tmp0(radix,L_MEM_SIZE_BIG);
	
	k=*terms-1;

	k26390_1103=26390;
	Lmul(k26390_1103, *terms-1, k26390_1103);
	Lsub(k26390_1103, 25287, k26390_25287);
	Ladd(k26390_1103, 1103, k26390_1103);

	k4_0=4*(*terms-1)-1;
	k4_1=4*(*terms-1)-2;
	Lmul(k4_0, k4_1, tmp0);
	k4_1--;
	Lmul(k4_1, k26390_1103, cur1.nu);
	Lmul(tmp0, cur1.nu, cur0.nu);

	Lmul(k, k, tmp0);
	Lmul(k, c6147814464, cur1.nu);
	Lmul(tmp0, cur1.nu, cur1.de);
	Lmul(cur1.de, k26390_25287, cur0.de);

	cur01=0;
	for(l_ui i=2;i<*terms;i++)
	{
		Lsub(k4_0, 4, k4_0);
		Lsub(k4_1, 3, k4_1);
		Lmul(k4_0, k4_1, tmp0);
		k4_1--;
		Lsub(k26390_1103, 26390, k26390_1103);
		if(cur01)
		{
			Lmul(k4_1, k26390_1103, cur0.nu);
			Lmul(tmp0, cur0.nu, coef.nu);

			k--;
			Lsub(k26390_25287, 26390, k26390_25287);
			Lmul(k, k, tmp0);
			Lmul(k, c6147814464, cur0.nu);
			Lmul(tmp0, cur0.nu, cur0.de);
			Lmul(cur0.de, k26390_25287, coef.de);

			cur1++;
			Lmul(cur1,coef,cur0);
			cur01=0;
		}
		else
		{
			Lmul(k4_1, k26390_1103, cur1.nu);
			Lmul(tmp0, cur1.nu, coef.nu);

			k--;
			Lsub(k26390_25287, 26390, k26390_25287);
			Lmul(k, k, tmp0);
			Lmul(k, c6147814464, cur1.nu);
			Lmul(tmp0, cur1.nu, cur1.de);
			Lmul(cur1.de, k26390_25287, coef.de);

			cur0++;
			Lmul(cur0,coef,cur1);
			cur01=1;
		}
	}
	if(cur01)
	{
		cur1++;
		Linverse(cur1);
		Lmul(cur1.nu,9801,cur1.nu);
		Lmul(cur1.de,4412,cur1.de);
		Lmul(*LF2D(cur1, 2*digcount), *sqrtof2, res);
	}
	else 
	{
		cur0++;
		Linverse(cur0);
		Lmul(cur0.nu,9801,cur0.nu);
		Lmul(cur0.de,4412,cur0.de);
		Lmul(*LF2D(cur0, 2*digcount), *sqrtof2, res);
	}
	Lapprox(res, digcount);
	return res.transfermem();
}

long double Ramanujan1(l_ui digcount, l_ui *terms)
{
	if(!terms)
		if( !(terms = (l_ui*)malloc(sizeof(*terms))) )
		{
			insufficientmem();
			return 0;
		}
		else *terms = 0;
	if(!*terms)
		*terms=0.1284*digcount+1;
	long double k=*terms-1,p= k? (4*k-1)*(4*k-2)*(4*k-3)*(1103+26390*k)/(6147814464*k*k*k*(26390*k-25287)): 0;
	for(l_ui i=2;i<*terms;i++)
	{
		p++;
		k--;
		p*=(4*k-1)*(4*k-2)*(4*k-3)*(1103+26390*k)/(6147814464*k*k*k*(26390*k-25287));		
	}
	return 1.41421356237309504880*9801/(4*(p+1)*1103);
}

#endif	//Ramanujan1_cpp