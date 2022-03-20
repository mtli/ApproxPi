/*
Contents:
A expansion of the square root of 2

Written by LeOpArD, 2011-12-4
*/
#ifndef Sqrt2_cpp
#define Sqrt2_cpp

#include"..\LargeNumbers.h"

LFraction* Sqrt2(l_ui digcount, digit_t radix=10, l_ui *terms=0)
{
	if(!terms)
		if( !(terms = (l_ui*)malloc(sizeof(*terms))) )
		{
			insufficientmem();
			return 0;
		}
		else *terms = 0;
	if(!*terms)
		*terms=10*log10((double)radix)*digcount;
	if(*terms == 1)
	{
		LFraction res(1,2ULL, radix);
		return res.transfermem();
	}
	LFraction coef(radix,L_MEM_SIZE_BIG);
	LFraction cur0(radix,L_MEM_SIZE_BIG),cur1(radix,L_MEM_SIZE_BIG);
	
	coef.nu=2*(*terms-1)+1;
	coef.de=4*(*terms-1);
	cur0=coef;
	bool cur01=0;
	for(l_ui i=2;i<*terms;i++)
	{
		Lsub(coef.nu, 2, coef.nu);
		Lsub(coef.de, 4, coef.de);
		if(cur01)
		{
			cur1++;
			Lmul(cur1, coef, cur0);
			cur01=0;
		}
		else
		{
			cur0++;
			Lmul(cur0, coef, cur1);
			cur01=1;
		}
	}
	if(cur01)
	{
		cur1++;
		Lmul(cur1.de, 2, cur1.de);
		return cur1.transfermem();
	}
	else
	{
		cur0++;
		Lmul(cur0.de, 2, cur0.de);
		return cur0.transfermem();
	}
}

long double Sqrt2(l_ui digcount, l_ui *terms)
{
	if(!terms)
		if( !(terms = (l_ui*)malloc(sizeof(*terms))) )
		{
			insufficientmem();
			return 0;
		}
		else *terms = 0;
	if(!*terms)
		*terms=10*digcount;
	long double k=*terms-1,p= k? (2*k+1)/(4*k): 0;
	for(l_ui i=2;i<*terms;i++)
	{
		p++;
		k--;
		p*=(2*k+1)/(4*k);		
	}
	return (p+1)/2;
}

#endif	//Sqrt2_cpp