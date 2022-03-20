/*
Contents:

Written by LeOpArD, 2011-11-7
*/
#ifndef MemAlloc_cpp
#define MemAlloc_cpp

#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<conio.h>

#include"PlatformDef.h"

#define L_MEM_SIZE_BIG true
#define L_MEM_SIZE_CUS false

l_ui GlobalInitSize=16;
l_ui GlobalInitBigSize=1048576;
// cannot be less than 4

inline void insufficientmem()
{
	perror("Insufficient memory.\nPress any key to exit.");
	_getch();
	exit(-1);
}

inline l_ui initmem(digit_t* &p)
{
	if(!(p=(digit_t*)malloc(GlobalInitSize*sizeof(digit_t))))
	{
		insufficientmem();
		return 0;
	}
	else return GlobalInitSize;
}

inline l_ui initmem_big(digit_t* &p)
{
	if(!(p=(digit_t*)malloc(GlobalInitBigSize*sizeof(digit_t))))
	{
		insufficientmem();
		return 0;
	}
	else return GlobalInitBigSize;
}

inline l_ui initmem_custom(digit_t* &p, l_ui cuslen)
{
	if(!(p=(digit_t*)malloc(cuslen*sizeof(digit_t))))
	{
		insufficientmem();
		return 0;
	}
	else return cuslen;
}

inline l_ui expandmem(digit_t* &p, l_ui oldsize, l_ui newsize)
{
	l_ui actualsize=2*oldsize;
	if(newsize > actualsize)
		actualsize=newsize;
	if(!(p=(digit_t*)realloc(p,actualsize*sizeof(digit_t))))
	{
		insufficientmem();
		return 0;
	}
	else return actualsize;
}

#endif	//MemAlloc_cpp