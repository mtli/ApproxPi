/*
Contents:
Multiplication of 2 LIntegers implemented by long multiplication

Running Time:
theta(n^2)

Optimization:
RunningTime( LongMulti(a, b, r) ) !=  RunningTime( LongMulti(b, a, r) )
Concerning cache spatial locality, it is recommended to put the 
shorter operand in the first place, while the longer the second place.

Written by LeOpArD, 2011-11-7
*/
#ifndef LongMulti_cpp
#define LongMulti_cpp

#include<cstring>
#include"..\LargeNumbers.h"

void LongMulti(const LInteger &m1,const LInteger &m2,LInteger &res)
{
	l_ui i,j;
	digit_t carry,val;
	res.sign=m1.sign^m2.sign;
	res.len=m1.len+m2.len;
	if(res.len > res.memsize)
		res.memsize=expandmem(res.mem, res.memsize, res.len);
	memset(res.mem, 0,res.len*sizeof(*res.mem));
	for(i=0;i<m1.len;i++)
	{
		carry=0;
		for(j=0;j<m2.len;j++)
		{
			val = res.mem[i+j] + m1.mem[i] * m2.mem[j] + carry;
			carry = val / res.ex_radix;
			res.mem[i+j] = val % res.ex_radix;
		}
		res.mem[i+j] = carry;
	}
	if(!res.mem[res.len-1])
		res.len--;
}

void LongMulti(const LDecimal &m1,const LDecimal &m2,LDecimal &res)
{
	l_ui i,j;
	digit_t carry,val;
	res.sign=m1.sign^m2.sign;
	res.radpoint = m1.radpoint + m2.radpoint;
	res.len=m1.len+m2.len;
	if(res.len > res.memsize)
		res.memsize=expandmem(res.mem, res.memsize, res.len);
	memset(res.mem, 0,res.len*sizeof(*res.mem));
	for(i=0;i<m1.len;i++)
	{
		carry=0;
		for(j=0;j<m2.len;j++)
		{
			val = res.mem[i+j] + m1.mem[i] * m2.mem[j] + carry;
			carry = val / res.ex_radix;
			res.mem[i+j] = val % res.ex_radix;
		}
		res.mem[i+j] = carry;
	}
	if(!res.mem[res.len-1])
		res.len--;
	for( i=0 ; i < res.len && i < res.radpoint && !res.mem[i]; i++);
	if(i)
		if(i!=res.len)
		{
			res.len -= i;
			res.radpoint -= i;
			memmove(res.mem, res.mem+i, res.len*sizeof(*res.mem));
		}
		else 
		{
			res.len=1;
			res.radpoint=0;
		}
}

#endif	//LongMulti_cpp