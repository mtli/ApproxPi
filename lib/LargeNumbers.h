/*
Large Numbers Lib Header File

---- The only file you need to include in your source file, when using the Large Numbers Lib.

Contents:
1. Data structures
2. Functions list
3. File structure


1. Data Structures:
l_ui			:	the unsigned type for index or length 
digit_t			:	the unsigned type for each digit in a number
LInteger		:	Signed integer
LFraction		:	A combination of sign, numerator and denominator
LDecimal		:	Fixed point decimal fraction


2. Lib Function List:
Functions						Operators					Running Time	   Additional Space   Whether in place*

Ladd						LInteger dight_t				O(n)					O(1)				Yes
Ladd						dight_t LInteger				(same as above)
Ladd						LInteger LInteger				theta(n)				O(1)				Yes
Ladd						LFraction LFraction				theta(n^2)				O(1)				No
Ladd(given LInteger temp)	LFraction LInteger				theta(n^2)				n					Yes
Ladd(given LInteger temp)	LInteger LFraction				(same as above)

Lsub						LInteger LInteger				theta(n)				O(1)				Yes
Lsub						LInteger dight_t				O(n)					O(1)				Yes
Lsub						dight_t LInteger				O(n)					O(1)				Yes

Lmul						LInteger dight_t				theta(n)				O(1)				Yes
Lmul						dight_t LInteger				theta(n)				O(1)				Yes
Lmul						LInteger LInteger				theta(n^2)|O(nlogn)		O(1)|O(n)			No
Lmul						LFraction LFraction				theta(n^2)				O(1)				No
Lmul						LFraction LInteger				theta(n^2)				O(1)				No
Lmul						LInteger LFraction				(same as above)
Lmul						LDecimal LDecimal				theta(n^2)				O(1)				No

Ldiv						LInteger digit_t				theta(n)				O(1)				Yes
Ldiv						LInteger LInteger				theta(n^2)				3n					Yes
Ldiv(digcount) 			  LInteger digit_t l_ui				theta(max(n,d))			O(1) or d/ex_power	No
Ldiv(digcount) 			  LInteger LInteger l_ui			theta(max(n,d)^2))		2n+max(n,d)			No

++(post)						LInteger					O(n)					O(1)				Yes
++(post)						LFraction					theta(n)				O(1)				Yes

--(post)						LInteger					O(n)					O(1)				Yes
--(post)						LFraction					theta(n)				O(1)				Yes

Linverse						LFraction					O(1)					O(1)				Yes

Lapprox							LDecimal					O(n)					O(1)				Yes

LF2D							LFraction					(same as "Ldiv(digcount) LInteger LInteger l_ui")

out2txt							LInteger					theta(n)				O(1)				
out2txt							LDecimal					theta(n+radpoint)		O(1)				
out2txt(digcount)			 LDecimal l_ui	bool			theta(d)				O(1)				Yes

*:"in place" means it is safe to use any one of the operand as the buffer to hold the result
	for example it is safe to call Ladd(a,b,a) (a<-a+b) while it isn't to call Lmul(a,b,a) (a<-a*b)

Notice:
All operations listed above do not contain radix conversion, and operations on numbers of different radixes
may lead to unknown result.

3. File structure
LargeNumbers.h					the header file
LargeNumbers.cpp				the core file, consisting of the data structures and four arithmetic operations 
PlatformDef.h					platform and environment related definitions and lib version definitions
MemAlloc.cpp					memory management functions
LNumIO.cpp						I/O interface
AdvancedOperations.cpp			advanced functions mainly based on four arithmetic operations

Written by LeOpArD, 2011-12-7
*/
#ifndef LargeNumbers_h
#define LargeNumbers_h

#include"PlatformDef.h"
#include"MemAlloc.cpp"
#include"LargeNumbers.cpp"
#include"LNumIO.cpp"
#include"AdvancedOperations.cpp"

#endif	// LargeNumbers_h