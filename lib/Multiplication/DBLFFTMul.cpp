/*
Contents:
Basic FFT multiplication for large integers (but limited, see Limitation) implemented in complex field 
with standard floating-point type ( IEEE 754 double )

Reference:
Donald E. Knuth, The Art of Computer Programming Vol II 3rd Ed
CLN-1.3.2

Running Time:
n*log(n)

Limitation:
assume k = ceil( log2(n) ), l = L_MAX_RADIX_BIT, 
to ensure we get the accurate result,
we require 
DBL_MANT_DIG(53) >= 2(k+l) + k + log2(k) + 3.5
All possible l and k
          k = 2    l <= 22
          k = 3    l <= 21
          k = 4    l <= 19
          k = 5    l <= 18
          k = 6    l <= 17
          k = 7    l <= 16
          k = 8    l <= 15
          k = 9    l <= 13
          k = 10   l <= 12
          k = 11   l <= 11
          k = 12   l <= 10
          k = 13   l <= 9
          k = 14   l <= 8
          k = 15   l <= 7
          k = 16   l <= 6
          k = 17   l <= 5
          k = 18   l <= 4
          k = 19   l <= 3
          k = 20   l <= 2

Written by LeOpArD, 2011-12-12
*/
#ifndef DBLFFTMul_cpp
#define DBLFFTMul_cpp

#include<cstring>
#include<cstdlib>
#include<cmath>   
#include<complex>  
#include<cfloat>
#include"..\LargeNumbers.h"
#include"bitReverse.cpp"

typedef std::complex<double> cplx_t;
#define L_DBL_CORRECTION_FOR_DBLFFT	0.00000095367431640625	// 2^(-20)

#ifndef M_LOG2E
	#define M_LOG2E		1.44269504088896340736
#endif
#ifndef M_PI
	#define M_PI		3.14159265358979323846
#endif


static void L_DBLFFT(const LInteger &num, cplx_t *cxbuf, l_ui fftlen, l_ui fftlenpower)
{
	l_ui i, k, s, m, mh;
	double sita;
	cplx_t t, r;
	// bit reverse copy
	for(i = 0; i < fftlen; i++)
		cxbuf[bitReverse(i, fftlenpower)] = i < num.len ? ldexp((double)num.mem[i], -(L_MAX_RADIX_BIT + fftlenpower)) : 0;
	for(s = 1; s <= fftlenpower; s++)
	{
		m = 1ULL << s;
		mh = m >> 1;
		for (k = 0; k < fftlen; k += m)
			for(i = 0 ; i < mh; i++)
			{
				sita = i*M_PI/mh;
				r.real(cos(sita));
				r.imag(-sin(sita));
				t = r * cxbuf[k + i + mh];
				cxbuf[k + i + mh] = cxbuf[k + i] - t;
				cxbuf[k + i] += t;
			}
	}
}

static void L_DBLIFFT(const cplx_t *cxopr, cplx_t *cxres, l_ui fftlen, l_ui fftlenpower)
{
	l_ui i, k, s, m, mh;
	double sita;
	cplx_t t, r;
	// bit reverse copy
	for(i = 0; i < fftlen; i++)
		cxres[bitReverse(i, fftlenpower)] = cxopr[i];
	for(s = 1; s <= fftlenpower; s++)
	{
		m = 1ULL << s;
		mh = m >> 1;
		for (k = 0; k < fftlen; k += m)
			for(i = 0; i < mh; i++)
			{
				sita = i*M_PI/mh;
				r.real(cos(sita));
				r.imag(sin(sita));
				t = r * cxres[k + i + mh];
				cxres[k + i + mh] = cxres[k + i] - t;
				cxres[k + i] += t;
			}
	}
}

void DBLFFTMul(const LInteger &m1,const LInteger &m2, LInteger &res)
{
	static cplx_t *cxbuf1, *cxbuf2;
	static l_ui cblen = 0;

	l_ui i, fftlenpower, llen = __max(m1.len, m2.len);
	res.sign = m1.sign ^ m2.sign;
	fftlenpower = (l_ui)ceil(log( (double)llen ) * M_LOG2E) + 1;
	res.len = 1ULL << fftlenpower  ;
	if(res.len > res.memsize)
		res.memsize = expandmem(res.mem, res.memsize, res.len);
	if(cblen)
		if( res.len > cblen)
		{
			cblen = res.len;
			free(cxbuf1);
			free(cxbuf2);
			if( !(cxbuf1 = (cplx_t*) malloc (cblen * sizeof(*cxbuf1) )) )
			{
				insufficientmem();
				return;
			}
			if( !(cxbuf2 = (cplx_t*) malloc (cblen * sizeof(*cxbuf2) )) )
			{
				insufficientmem();
				return;
			}
		}
		else;
	else 
	{
		cblen = res.len;
		if( !(cxbuf1 = (cplx_t*) malloc (cblen * sizeof(*cxbuf1) )) )
		{
			insufficientmem();
			return;
		}
		if( !(cxbuf2 = (cplx_t*) malloc (cblen * sizeof(*cxbuf2) )) )
		{
			insufficientmem();
			return;
		}
	}
	L_DBLFFT(m1, cxbuf1, res.len, fftlenpower);
	L_DBLFFT(m2, cxbuf2, res.len, fftlenpower);
	for(i = 0; i < res.len; i++ )
		cxbuf1[i] = cxbuf1[i] * cxbuf2[i];
	L_DBLIFFT(cxbuf1, cxbuf2, res.len, fftlenpower);
	//double carry = 0, dblrad = res.ex_radix;
	//for(i = 0; i < res.len; i++ )
	//{
	//	cxbuf2[i].real( ldexp(cxbuf2[i].real(),fftlenpower + (L_MAX_RADIX_BIT << 1)) + carry);
	//	carry = cxbuf2[i].real() / dblrad;
	//	res.mem[i] = fmod( cxbuf2[i].real(), dblrad) + L_DBL_CORRECTION_FOR_DBLFFT;
	//}
	digit_t carry = 0;
	for(i = 0; i < res.len; i++ )
	{
		res.mem[i] = (l_ui)(ldexp(cxbuf2[i].real(),fftlenpower + (L_MAX_RADIX_BIT << 1)) + L_DBL_CORRECTION_FOR_DBLFFT) + carry;
		carry = res.mem[i] / res.ex_radix;
		res.mem[i] %= res.ex_radix;
	}
	for( i--; i>0 && !res.mem[i];i--)
		res.len--;
}

#endif	//DBLFFTMul_cpp