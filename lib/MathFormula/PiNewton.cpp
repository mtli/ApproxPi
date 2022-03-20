/*
Contents:
Newton's expansion of pi realized with LargeNumber lib

Written by LeOpArD, 2011-11-7
*/
#ifndef PiNewton_cpp
#define PiNewton_cpp

#include"..\LargeNumbers.h"

LFraction* PiNewton(l_ui digcount, digit_t radix=10, l_ui *terms=0)
{
	LFraction coef(radix,L_MEM_SIZE_BIG),cur1(radix,L_MEM_SIZE_BIG),cur2(radix,L_MEM_SIZE_BIG);
	bool sw12;

	if( !terms || !*terms)
		*terms=coef.nu.ex_power * digcount;

	coef.nu=*terms-1;
	Lmul(coef.nu, 2,coef.de);
	coef.de++;

	cur1.nu=1;
	cur1.de=1;
	sw12=0;

	for(l_ui i=1;i<*terms;i++)
	{
		if(sw12)
		{
			Lmul(cur2,coef,cur1);
			cur1++;			
			sw12=0;
		}
		else
		{
			Lmul(cur1,coef,cur2);
			cur2++;
			sw12=1;
		}
		Lsub(coef.de, 2,coef.de);
		coef.nu--;
	}

	if(sw12)
	{
		Lmul(cur2.nu,2,cur2.nu);
		return cur2.transfermem();
	}
	else 
	{
		Lmul(cur1.nu,2,cur1.nu);
		return cur1.transfermem();
	}
}

long double PiNewton(l_ui digcount, l_ui *terms)
{
	if( !terms || !*terms)
		*terms=9*digcount;
	long double k=*terms-1,p=1;
	for(l_ui i=1;i<*terms;i++,k--)
	{
		p*=k/(2*k+1);
		p++;
	}
	return 2*p;
}

#endif	//PiNewton_cpp