/*
Contents:
Platform-related definitions and optimization

Written by LeOpArD, 2011-11-20
*/
#ifndef PlatformDef_h
#define PlatformDef_h

#if defined(_M_X64) || defined(_M_AMD64)
	#define L_X64_ENV
	typedef unsigned long long			l_ui;
	typedef unsigned long long			digit_t;
	typedef long long					l_i;
	#define L_SINGLE_DIGIT_BIT			64
	#define pf_itoa						_ui64toa
	#pragma warning(disable: 4996) 
#else //default _M_IX86
	#define L_X86_ENV
	typedef unsigned long				l_ui;
	typedef unsigned long				digit_t;
	typedef long						l_i;
	#define L_SINGLE_DIGIT_BIT			32
	#define pf_itoa						_ultoa
	#pragma warning(disable: 4996) 
#endif

#ifndef BASIC_DBLFFT_MUL
	#define L_MAX_RADIX_BIT				( L_SINGLE_DIGIT_BIT >> 1 )					// x64:32 ; x86:16
#else
	#define L_MAX_RADIX_BIT				10
	#define BASIC_DBLFFT_MUL_THRESHOLD  395
#endif
#define L_MAX_RADIX					( 1ULL << L_MAX_RADIX_BIT )					// x64:x0000000100000000 ; x86:x00010000
#define L_MIN_EX_RADIX				( (1ULL << (L_MAX_RADIX_BIT >> 1)) + 1 )	// x64:x0000000000010001 ; x86:x00000101


#endif	//PlatformDef_h