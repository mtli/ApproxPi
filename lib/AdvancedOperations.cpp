/*
Contents:
Advanced operations over LInteger, LFraction, LDecimal and C-basic types

Functions						Operators					Running Time	   Additional Space   Whether in place*

Lpower						LInteger dight_t				(log(p))*Lmul(n)		3n					No
//Lsqrt							LInteger					(log(p))*Lmul(n)		3n					No


*:"in place" means it is safe to use any one of the operand as the buffer to hold the result
	for example it is safe to call Ladd(a,b,a) (a<-a+b) while it isn't to call Lmul(a,b,a) (a<-a*b)

Notice:
All operations listed above do not contain base conversion, and operations on numbers of different radixes
may lead to unknown result.

Written by LeOpArD, 2011-11-22
*/
#ifndef AdvancedOperations_cpp
#define AdvancedOperations_cpp

#include"LargeNumbers.h"

void Lpower(const LInteger &b, l_ui p, LInteger &res, LInteger *tmp1=0, LInteger *tmp2=0, LInteger *tmp3=0)
{// Exponentiation by squaring
	if(!p)
	{// including 0^0, which we define it 0
		res.len=1;
		res.mem[0]=0;
		return;
	}
	if(b.len==1)
		if(b.mem[0]==0)
		{
			res.len=1;
			res.mem[0]=0;
			return;
		}
		else if(b.mem[0]==1)
		{
			res.len=1;
			res.mem[0]=1;
			return;
		}
	if(!tmp1)
	{
		if( !(tmp1 =(LInteger*)malloc(sizeof(LInteger))) )
			{
				insufficientmem();
				return ;
			}
		tmp1->LInteger::LInteger(res.radix);
	}
	if(!tmp2)
	{
		if( !(tmp2 =(LInteger*)malloc(sizeof(LInteger))) )
			{
				insufficientmem();
				return ;
			}
		tmp2->LInteger::LInteger(res.radix);
	}
	if(!tmp3)
	{
		if( !(tmp3 =(LInteger*)malloc(sizeof(LInteger))) )
			{
				insufficientmem();
				return ;
			}
		tmp3->LInteger::LInteger(res.radix);
	}
	res = 1;
	*tmp2 = b;
	bool  restmp=0,curb=0;

	while(1)
	{
		if(p&1)
		{
			if(restmp)
				if(curb)
				{
					Lmul(*tmp1,*tmp3,res);
					restmp=0;
				}
				else 
				{
					Lmul(*tmp1,*tmp2,res);
					restmp=0;
				}
			else 
				if(curb)
				{
					Lmul(res,*tmp3,*tmp1);
					restmp=1;
				}
				else 
				{
					Lmul(res,*tmp2,*tmp1);
					restmp=1;
				}
			p--;
		}
		if( p>>=1 )
			if(curb)
			{
				Lmul(*tmp3,*tmp3,*tmp2);
				curb=0;
			}
			else 
			{
				Lmul(*tmp2,*tmp2,*tmp3);
				curb=1;
			}
		else break;
	}
	if(restmp)
		res=*tmp1;
}

#endif	//AdvancedOperations_cpp