/*
Contents:
BBP series for pi realized with LargeNumber lib

Written by LeOpArD, 2011-11-7
*/

#ifndef PiBBP_cpp
#define PiBBP_cpp

#include<cmath>
#include"..\LargeNumbers.h"

LFraction* PiBBP(l_ui digcount, digit_t radix=10, l_ui *terms=0)
{
	if(!terms)
		if( !(terms = (l_ui*)malloc(sizeof(*terms))) )
		{
			insufficientmem();
			return 0;
		}
		else *terms = 0;
	if(!*terms)
		*terms=digcount*log((double)radix)/log(16.0)+2;
	//p=(120*k*k+151*k+47)/((2*k+1)*(4*k+3)*(8*k+1)*(8*k+5));
	LInteger k2(radix,L_MEM_SIZE_BIG), k4(radix,L_MEM_SIZE_BIG),k8(radix,L_MEM_SIZE_BIG),k151_47(radix,L_MEM_SIZE_BIG);
	LInteger tmp0(radix,L_MEM_SIZE_BIG);
	LFraction coef(radix,L_MEM_SIZE_BIG),cur0(radix,L_MEM_SIZE_BIG),cur1(radix,L_MEM_SIZE_BIG);
	bool cur01=0;

	k2=2*(*terms-1)+1;
	k4=4*(*terms-1)+3;
	k8=8*(*terms-1)+5;
	Lmul(k2,k8,cur1.nu);
	Lsub(k8,4,k8);
	Lmul(k4,k8,cur0.nu);
	Lmul(cur0.nu, cur1.nu, cur0.de);

	k151_47=(*terms-1);
	Lmul(k151_47, 151, k151_47);
	Ladd(k151_47, 47, k151_47);
	k2--;
	Lsub(k4,3,k4);
	Lmul(k2,k4,cur0.nu);
	Lmul(cur0.nu,15,cur0.nu);
	Ladd(cur0.nu,k151_47,cur0.nu);
	for(l_ui i=1;i<*terms;i++)
	{
		if(cur01)
		{
			Lmul(cur1.de, 16, cur1.de);

			k2--;
			k4--;
			Lsub(k8,4,k8);
			Lmul(k2,k8,cur0.nu);
			Lsub(k8,4,k8);
			Lmul(k4,k8,coef.nu);
			Lmul(coef.nu, cur0.nu, coef.de);

			Lsub(k151_47, 151, k151_47);
			k2--;
			Lsub(k4,3,k4);
			Lmul(k2,k4,coef.nu);
			Lmul(coef.nu,15,coef.nu);
			Ladd(coef.nu,k151_47,coef.nu);

			Ladd(coef,cur1,cur0);
			cur01=0;
		}
		else
		{
			Lmul(cur0.de, 16, cur0.de);

			k2--;
			k4--;
			Lsub(k8,4,k8);
			Lmul(k2,k8,cur1.nu);
			Lsub(k8,4,k8);
			Lmul(k4,k8,coef.nu);
			Lmul(coef.nu, cur1.nu, coef.de);

			Lsub(k151_47, 151, k151_47);
			k2--;
			Lsub(k4,3,k4);
			Lmul(k2,k4,coef.nu);
			Lmul(coef.nu,15,coef.nu);
			Ladd(coef.nu,k151_47,coef.nu);

			Ladd(coef,cur0,cur1);
			cur01=1;
		}
	}
	return cur01 ? cur1.transfermem(): cur0.transfermem();
}

long double PiBBP(l_ui digcount, l_ui *terms)
{
	if(!terms)
		if( !(terms = (l_ui*)malloc(sizeof(*terms))) )
		{
			insufficientmem();
			return 0;
		}
		else *terms = 0;
	if(!*terms)
		*terms=digcount/log10(16.0)+2;
	long double k=*terms-1,p=(120*k*k+151*k+47)/((2*k+1)*(4*k+3)*(8*k+1)*(8*k+5));
		//p= 4/(8*k+1) - 2/(8*k+4) - 1/(8*k+5) - 1/(8*k+6);	//	Alternative form
	for(l_ui i=1;i<*terms;i++)
	{
		k--;
		p/=16;
		p+=(120*k*k+151*k+47)/((2*k+1)*(4*k+3)*(8*k+1)*(8*k+5));
		//p+=4/(8*k+1) - 2/(8*k+4) - 1/(8*k+5) - 1/(8*k+6);	//	Alternative form
	}
	return p;
}

#endif	//PiBBP_cpp