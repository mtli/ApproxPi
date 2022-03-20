/*
Contents:
1. The definitions of Large-number stuctures and their constructors
2. The realization of four arithmetic operations over Large number stuctures

Data Structure Details
Values				Descriptions
sign			:	the sign of the number
radix			:	the original radix of the number
ex_radix		:   the expanded radix of the number
ex_power		:	the expanding power of the number	(ex_radix = radix^ex_power)
len				:	the length of the digits in the expanded radix
radpoint		:	(LDecimal only)	the radix point position of the number
mem				:   the pointer to an allocated memory where the digits of the number in the
					expanded radix stored reversely, i.e. the LSB appears first in the array
memsize			:	the size of the allocated memory denoted by mem in unit digit_t,
					not in unit byte!

Written by LeOpArD, 2011-11-7
*/
#ifndef LargeNumbers_cpp
#define LargeNumbers_cpp

#include<cstdio>
#include<cstring>
#include<cctype>
#include<cmath>

#include"PlatformDef.h"
#include"MemAlloc.cpp"

#define Lroundhalfup(dig,rad)		((dig) >= (rad)/2) 
#ifndef L_USER_ROUND_RULE
	#define Lrounding(dig,rad)		Lroundhalfup(dig,rad)
#endif


class LInteger
{// digits are stored in a reversed array
public:
	explicit LInteger(digit_t r=10):len(0)
	{
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);
		memsize=initmem(mem);
	}
	LInteger(digit_t r, bool meminitmode, l_ui meminitlen=GlobalInitBigSize):len(0)
	{
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);

		if(meminitmode == L_MEM_SIZE_BIG)
			memsize=initmem_big(mem);
		else memsize=initmem_custom(mem, meminitlen);
	}
	LInteger(long long l, digit_t r)
	{
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);
		memsize=initmem(mem);
		if(l<0)
		{
			sign=1;
			l=-l;
		}
		else sign=0;
		unsigned i=0;
		while(l)
		{
			mem[i++]=l % ex_radix;
			l/=ex_radix;
		}
		if(i)
			len=i;
		else 
		{
			mem[0]=0;
			len=1;
		}
	}
	LInteger& operator = (long long l)
	{
		if(memsize<64)
			memsize=expandmem(mem, memsize, 64);
		if(l<0)
		{
			sign=1;
			l=-l;
		}
		else sign=0;
		unsigned i=0;
		while(l)
		{
			mem[i++]=l % ex_radix;
			l/=ex_radix;
		}
		if(i)
			len=i;
		else 
		{
			mem[0]=0;
			len=1;
		}
		return *this;
	}
	LInteger(const char *str, digit_t r=10)
	{/* the str should be a null-ended string and in integer format
		each digit can be represented by either a decimal digit or an uppercase or lowercase letter,
	    so under this circumstance, the radix is limited to being equal to or less than 36
	 */
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);
		if(!str)
		{
			memsize=len=0;
			mem=0;
			return;
		}
		bool frontzero=false;
		while( *str != '\0' && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r' || *str == '0'))
			if(*str++ == '0')
				frontzero=true;
		if( frontzero && !isalnum(*str))
		{
			memsize=initmem(mem);
			len=1;
			mem[0]=0;
			return;
		}
		l_ui i,j,start=0,slen;
		slen=strlen(str);
		memsize=initmem_custom(mem, slen/ex_power+1);
		l_ui rweight;
		for(i = slen-1, len=-1, j=ex_power;; i--,j++)
		{
			if( isalnum(str[i]) )
				if(j==ex_power)
				{
					mem[++len]=isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a');
					rweight=radix;
					j=0;
				}
				else 
				{
					mem[len]+= (isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a'))*rweight;
					rweight*=radix;
				}
			else return;
			if(!i)
			{
				len++;
				return;
			}
		}
	}
	LInteger(FILE *fp, digit_t r=10, l_ui len=0)
	{
		char *buf;
		l_ui flen;
		if(len)
			flen=len;
		else
		{
			fseek(fp,0L, SEEK_END);
			flen = ftell(fp)/sizeof(char);
		}
		if(!(buf = (char*) malloc( (flen+1)*sizeof(char)) ))
		{
			memsize=len=0;
			mem=0;
			return;
		}
		fseek(fp,0L, SEEK_SET);
		buf[fread(buf, sizeof(char), flen, fp)]='\0';
		this->LInteger::LInteger(buf, r);
	}
	LInteger& operator = (const LInteger &a)
	{
		len = a.len;
		if(len>memsize)
			memsize = expandmem(mem, memsize, len);
		memcpy( mem, a.mem, len*sizeof(*mem));
		radix = a.radix;
		ex_radix = a.ex_radix;
		ex_power = a.ex_power;
		sign = a.sign;
		return *this;
	}
	LInteger* transfermem(LInteger* p=0)
	{
		if(!p)
			if( !( p =(LInteger*)malloc(sizeof(LInteger))) )
			{
				insufficientmem();
				return 0;
			}
		p->radix = radix;
		p->ex_radix = ex_radix;
		p->ex_power = ex_power;
		p->sign = sign;
		p->len = len;
		p->memsize = memsize;
		p->mem = mem;

		len=0;
		sign=0;
		memsize=initmem(mem);
		return p;
	}

	inline void setzero()
	{
		sign=0;
		len=1;
		mem[0]=0;
	}
	
	~LInteger()
	{
		if(memsize)
			free(mem);
	}
	
	digit_t *mem;	
	l_ui memsize;
	l_ui len;
	digit_t radix;
	digit_t ex_radix;
	unsigned char ex_power;	// ex_radix = radix^ex_power
	bool sign;				// sign 0:+ ; 1:-
};

class LDecimal
{/*
  This class is intented to store number with a small integer part, but with a many-digit fraction part,
  so the focus is on the fraction part of the number.
  The digits are stored in a reversed array.
  LDecimal is realized by shifting the radix point of a LInteger and the radix point only floats within
  the fraction part. Thus it's a half-float number structure.
  To store a large integer in LDecimal is the same as in LInteger.
  The LDecimal itself doesn't not contain any precision infomation, e.g. 0.010 is equavilent to 0.001
  (with the same inner representation), and starting and ending zeros in the fraction part is removed during input 
  and after operations.
 */
public:
	explicit LDecimal(digit_t r=10):len(0),radix(r),radpoint(0)
	{
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);
		memsize=initmem(mem);
	}
	LDecimal(digit_t r, bool meminitmode, l_ui meminitlen=GlobalInitBigSize):len(0),radix(r),radpoint(0)
	{
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);

		if(meminitmode == L_MEM_SIZE_BIG)
			memsize=initmem_big(mem);
		else memsize=initmem_custom(mem, meminitlen);
	}
	LDecimal(long long l, digit_t r):radpoint(0)
	{
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);
		memsize=initmem(mem);
		if(l<0)
		{
			sign=1;
			l=-l;
		}
		else sign=0;
		unsigned i=0;
		while(l)
		{
			mem[i++]=l % ex_radix;
			l/=ex_radix;
		}
		if(i)
			len=i;
		else 
		{
			mem[0]=0;
			len=1;
		}
	}
	LDecimal& operator = (long long l)
	{
		if(memsize<64)
			memsize=expandmem(mem, memsize, 64);
		radpoint=0;
		if(l<0)
		{
			sign=1;
			l=-l;
		}
		else sign=0;
		unsigned i=0;
		while(l)
		{
			mem[i++]=l % ex_radix;
			l/=ex_radix;
		}
		if(i)
			len=i;
		else 
		{
			mem[0]=0;
			len=1;
		}
		return *this;
	}
	/*
	LDecimal(double d, unsigned char r):radix(r)
	{
		radpoint;
		memsize=initmem(mem);
		if(l<0)
		{
			sign=1;
			l=-l;
		}
		else sign=0;
		unsigned i=0;
		while(l)
		{
			mem[i++]=l % radix;
			l/=radix;
		}
		if(i)
			len=i;
		else 
		{
			mem[0]=0;
			len=1;
		}
	}
	/*
	LDecimal& operator = (double d)
	{
		radpoint=0;
		if(l<0)
		{
			sign=1;
			l=-l;
		}
		else sign=0;
		unsigned i=0;
		while(l)
		{
			mem[i++]=l % radix;
			l/=radix;
		}
		if(i)
			len=i;
		else 
		{
			mem[0]=0;
			len=1;
		}
		return *this;
	}
	*/
	LDecimal(const char *str, digit_t r=10)
	{/* the str should be a null-ended string and in decimal fraction format
		each digit can be represented by either a decimal digit or an uppercase or lowercase letter,
	    so under this circumstance, the radix is limited to being equal to or less than 36
	 */
		radix= ( r>=2 && r<=L_MAX_RADIX) ? r : 10;
		digit_t tmp=L_MAX_RADIX/radix;
		for(ex_power=1,ex_radix=radix; ex_radix<=tmp ;ex_radix*=radix, ex_power++);
		if(!str)
		{
			memsize=len=0;
			mem=0;
			return;
		}
		l_ui i;
		sign=0;
		radpoint=0;
		bool frontzero=false, radpointappeared=false, whilebreak=false;
		while( *str )
		{
			switch(*str)
			{
				case ' ':case '\t':case '\n':case '\r':
							str++;
							continue;
				case '0':	if(radpointappeared)
								radpoint++;
							else frontzero=true;
							break;
				case '-':	sign=1;
							break;
				case '+':	sign=0;
							break;
				case '.':	radpointappeared=true;
							break;
				default :	whilebreak=true;
							break;

			}
			if(whilebreak)
				break;
			str++;
		}
		if( (!radpointappeared && frontzero && !isalnum(*str)) || (radpointappeared && !isalnum(*str)))
		{
			memsize=initmem(mem);
			len=1;
			mem[0]=0;
			radpoint=0;
			sign=0;
			return;
		}
		l_ui j,end0=0;
		if(radpointappeared)
		{// the integer part is 0
			for(i=0;isalnum(str[i]);i++)
				if(str[i]=='0')
					if(!end0)
						end0=i;
					else;
				else end0=0;
			if(end0)
				i=end0;
			memsize=initmem_custom(mem, i/ex_power+1);
			i--;
			j = ex_power - (radpoint+i) % ex_power -1;
			radpoint = (radpoint+i)/ex_power + 1;
			
			l_ui rweight;
			if(j)
			{
				unsigned char k;
				for(rweight=radix, k=1; k<j; k++)
					rweight*=radix;
				len=0;
				mem[0]=0;
			}
			else 
			{
				len=-1;
				j=ex_power;
			}
			for(;; i--, j++)
			{
				if(j==ex_power)
				{
					mem[++len]=isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a');
					rweight=radix;
					j=0;
				}
				else 
				{
					mem[len]+= (isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a'))*rweight;
					rweight*=radix;
				}
				if(!i)
				{
					len++;
					return;
				}
			}
		}
		else 
		{// the integer part is not 0
			l_ui radstrpos,slen,fracpartlen;
			for(i=0;;i++)
				if( isalnum(str[i]) )
					if(str[i]=='0')
						if(!end0)
							end0=i;
						else;
					else end0=0;
				else if(str[i]=='.')
					{
						radpointappeared=true;
						radstrpos=i;
						end0=0;
					}
				else break;
			if(end0)
			{
				slen=end0;
				i=end0-1;
			}
			else slen=i--;
			if(radpointappeared)
			{
				slen--;
				memsize=initmem_custom(mem, slen/ex_power+2);
				fracpartlen=slen-radstrpos;
				j = ex_power - fracpartlen % ex_power;
				radpoint = fracpartlen/ex_power+ ((fracpartlen % ex_power ) ? 1: 0);
				l_ui rweight;
				if(j!=ex_power)
				{
					unsigned char k;
					for(rweight=radix, k=1; k<j; k++)
						rweight*=radix;
					len=0;
					mem[0]=0;
				}
				else len=-1;
				for(;; i--, j++)
				{
					if(i == radstrpos)
						i--;
					if(j==ex_power)
					{
						mem[++len]=isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a');
						rweight=radix;
						j=0;
					}
					else 
					{
						mem[len]+= (isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a'))*rweight;
						rweight*=radix;
					}
					if(!i)
					{
						len++;
						return;
					}
				}
			}
			else
			{// integer without fration part
				radpoint=0;
				memsize=initmem_custom(mem, slen/ex_power+1);
				l_ui rweight;
				for(len=-1, j=ex_power;; i--,j++)
				{
					if(j==ex_power)
					{
						mem[++len]=isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a');
						rweight=radix;
						j=0;
					}
					else 
					{
						mem[len]+= (isdigit(str[i]) ?  str[i]-'0' : (isupper(str[i]) ? str[i]-'A': str[i]-'a'))*rweight;
						rweight*=radix;
					}
					if(!i)
					{
						len++;
						return;
					}
				}
			}
		}
	}
	LDecimal(FILE *fp, digit_t r=10, l_ui len=0)
	{
		char *buf;
		l_ui flen;
		if(len)
			flen=len;
		else
		{
			fseek(fp,0L, SEEK_END);
			flen = ftell(fp)/sizeof(char);
		}
		if(!(buf = (char*) malloc( (flen+1)*sizeof(char)) ))
		{
			memsize=len=0;
			mem=0;
			return;
		}
		fseek(fp,0L, SEEK_SET);
		buf[fread(buf, sizeof(char), flen, fp)]='\0';
		this->LDecimal::LDecimal(buf, r);
	}
	LDecimal& operator = (const LInteger &a)
	{
		radpoint=0;
		len = a.len;
		if(len>memsize)
			memsize = expandmem(mem, memsize, len);
		memcpy( mem, a.mem, len*sizeof(*mem));
		radix = a.radix;
		ex_radix = a.ex_radix;
		ex_power = a.ex_power;
		sign = a.sign;
		return *this;
	}
	LDecimal& operator = (const LDecimal &a)
	{
		radpoint=a.radpoint;
		len = a.len;
		if(len>memsize)
			memsize = expandmem(mem, memsize, len);
		memcpy( mem, a.mem, len*sizeof(*mem));
		radix = a.radix;
		ex_radix = a.ex_radix;
		ex_power = a.ex_power;
		sign = a.sign;
		return *this;
	}
	LDecimal* transfermem(LDecimal* p=0)
	{
		if(!p)
			if( !( p =(LDecimal*)malloc(sizeof(LDecimal))) )
			{
				insufficientmem();
				return 0;
			}
		p->radpoint = radpoint;
		p->radix = radix;
		p->ex_radix = ex_radix;
		p->ex_power = ex_power;
		p->sign = sign;
		p->len = len;
		p->memsize = memsize;
		p->mem = mem;

		len=0;
		sign=0;
		memsize=initmem(mem);
		return p;
	}
	~LDecimal()
	{
		if(memsize)
			free(mem);
	}

	inline void setzero()
	{
		sign=0;
		len=1;
		mem[0]=0;
		radpoint=0;
	}

	digit_t *mem;	
	l_ui memsize;
	l_ui len;
	digit_t radix;
	digit_t ex_radix;
	unsigned char ex_power;	// ex_radix = radix^ex_power
	l_ui radpoint;
	bool sign;				// sign 0:+ ; 1:-
};

class LFraction
{/* comprises 2 LIntegers respectively as the numerator and the denominator
  we have the conventions that the denominator is always positive
  and the sign of the numerator is the sign of the whole fraction
  and the sign of the denominator should always be possitive
  Notice that the fraction may not be in its simplest term (it's reducible)
 */
public:
	explicit LFraction(unsigned char r=10):nu(r),de(r)
	{
		de.sign=0;
	}
	LFraction(digit_t r, bool meminitmode, l_ui meminitlen=GlobalInitBigSize):
		nu(r, meminitmode, meminitlen),de(r, meminitmode, meminitlen)
	{
	}
	LFraction(long long numerator,unsigned long long denominator,digit_t r=10):nu(r),de(r)
	{
		nu=numerator;
		de=denominator;
		de.sign=0;
	}
	LFraction& operator = (const LFraction &a)
	{
		nu=a.nu;
		de=a.de;
		return *this;
	}
	LFraction& operator = (const LInteger &a)
	{
		nu=a;
		de=1;
		return *this;
	}
	LFraction* transfermem(LFraction* p=0)
	{
		if(!p)
			if( !( p =(LFraction*)malloc(sizeof(LFraction))) )
			{
				insufficientmem();
				return 0;
			}
		nu.transfermem(&(p->nu));
		de.transfermem(&(p->de));
		return p;
	}
	LInteger nu;
	LInteger de;
};


#include".\Multiplication\LongMulti.cpp"
#include".\Multiplication\DBLFFTMul.cpp"
void Lmul(const LInteger &num,const digit_t d,LInteger &res)
{
	l_ui i;
	digit_t carry;
	res.sign=num.sign;
	res.len=num.len;
	if(res.len >= res.memsize)
		res.memsize=expandmem(res.mem, res.memsize, res.len+1);
	carry=0;
	for(i=0;i<num.len;i++)
	{
		res.mem[i] = num.mem[i] * d + carry;
		carry = res.mem[i] / res.ex_radix;
		res.mem[i] = res.mem[i] % res.ex_radix;
	}
	if(carry)
	{
		res.mem[i]=carry;
		res.len++;
	}
}
inline void Lmul(const digit_t d, const LInteger &num,LInteger &res)
{
	Lmul(num,d,res);
}
inline void Lmul(const LInteger &m1,const LInteger &m2,LInteger &res)
{
#ifdef BASIC_DBLFFT_MUL
	if(m1.len > m2.len)
		if( m2.len >= BASIC_DBLFFT_MUL_THRESHOLD)
			DBLFFTMul(m1, m2, res);
		else LongMulti(m1, m2, res);
	else if( m1.len >= BASIC_DBLFFT_MUL_THRESHOLD)
			DBLFFTMul(m1, m2, res);
		else LongMulti(m2, m1, res);
#else
	LongMulti(m2, m1, res);
#endif
}
inline void Lmul(const LFraction &m1,const LFraction &m2,LFraction &res)
{
	Lmul( m1.nu, m2.nu, res.nu);
	if( res.nu.len == 1 && res.nu.mem[0] == 0 )
	{
		res.de.len = 1;
		res.de.mem[0] = 0;
		return;
	}
	Lmul( m1.de, m2.de, res.de);
}
inline void Lmul(const LFraction &m1,const LInteger &m2,LFraction &res)
{
	Lmul( m1.nu, m2, res.nu);
	if( res.nu.len == 1 && res.nu.mem[0] == 0 )
	{
		res.de.len = 1;
		res.de.mem[0] = 0;
		return;
	}
	res.de = m1.de;
}
inline void Lmul(const LInteger &m1,const LFraction &m2,LFraction &res)
{
	Lmul( m2, m1, res);
}
inline void Lmul(const LDecimal &m1,const LDecimal &m2,LDecimal &res)
{
	LongMulti(m1,m2,res);
}

static void Laddabs(const LInteger &num, const digit_t d, LInteger &res)
{
	l_ui i=1;
	digit_t carry;
	res.len=num.len;
	if(res.len >= res.memsize )
		res.memsize = expandmem ( res.mem, res.memsize, res.len+1);
	res.mem[0] = num.mem[0] + d;
	carry = res.mem[0] / res.ex_radix;
	res.mem[0] = res.mem[0] % res.ex_radix;
	if(carry)
	{
		for(; i<num.len; i++)
			if( num.mem[i] < res.ex_radix-1)
			{
				res.mem[i]=num.mem[i]+1;
				break;
			}
			else res.mem[i]=0;
		if(i == num.len)
		{
			res.mem[i]=1;
			res.len++;
			return;
		}
	}
	if(&res != &num)
		memcpy(res.mem+i, num.mem+i, (num.len-i)*sizeof(*res.mem));
}
static void Lsubabs(const LInteger &num, const digit_t d, LInteger &res)
{// Assuming that abs(num) >= d
	l_ui i=1;
	res.len = num.len;
	if(res.memsize < res.len)
		res.memsize = expandmem ( res.mem, res.memsize, res.len);
	if( num.mem[0] >= d)
		res.mem[0] = num.mem[0]-d;
	else 
	{
		res.mem[0] = (num.mem[0] + res.ex_radix) -d;
		for(; i<num.len; i++)
			if( num.mem[i] > 0)
			{
				res.mem[i]= num.mem[i] - 1;
				break;
			}
			else res.mem[i] = res.ex_radix - 1;
		if( !res.mem[i] )
			res.len--;
	}
	if(&res != &num)
		memcpy(res.mem+i, num.mem+i, (num.len-i)*sizeof(*res.mem));
}
static void Laddabs(const LInteger &m1, const LInteger &m2, LInteger &res)
{
	l_ui i,slen,blen;
	bool longer;
	digit_t carry;
	if( m1.len > m2.len )
	{
		blen = m1.len;
		slen = m2.len;
		longer=0;
	}
	else 
	{
		blen = m2.len;
		slen = m1.len;
		longer=1;
	}
	if(res.memsize <= blen )
		res.memsize = expandmem ( res.mem, res.memsize, blen+1);
	for( i = 0, carry=0; i<slen; i++)
	{
		res.mem[i] = m1.mem[i] + m2.mem[i] + carry;
		carry = res.mem[i] / res.ex_radix;
		res.mem[i] = res.mem[i] % res.ex_radix;
	}
	if(carry)
	{
		for(; i<blen; i++)
			if(longer)
				if( m2.mem[i] < res.ex_radix-1)
				{
					res.mem[i]= m2.mem[i] + 1;
					break;
				}
				else res.mem[i]=0;
			else if( m1.mem[i] < res.ex_radix-1)
				{
					res.mem[i]= m1.mem[i] + 1;
					break;
				}
				else res.mem[i]=0;
		if(i == blen)
		{
			res.mem[i]=1;
			res.len = blen+1;
			return;
		}
		i++;
	}
	if(longer)
		if( &res != &m2)
			for(; i<blen; i++)
				res.mem[i]= m2.mem[i];
		else;
	else if( &res != &m1)
			for(; i<blen; i++)
				res.mem[i]= m1.mem[i];
	res.len=blen;
}
static void Lsubabs(const LInteger &m1, const LInteger &m2, LInteger &res)
{// Assuming that abs(m1) >= abs(m2), thus m1.len >= m2.len;
	l_ui i;
	digit_t borrow;
	if(res.memsize < m1.len)
		res.memsize = expandmem ( res.mem, res.memsize, m1.len);
	for( i = 0, borrow=0; i<m2.len; i++)
		if( m1.mem[i] >= m2.mem[i] + borrow)
		{
			res.mem[i] = m1.mem[i] - borrow - m2.mem[i];
			borrow=0;
		}
		else 
		{
			res.mem[i] = (m1.mem[i] + res.ex_radix) - borrow - m2.mem[i];
			borrow=1;
		}
	if(borrow)
		for(; i<m1.len; i++)
			if( m1.mem[i] > 0)
			{
				res.mem[i] = m1.mem[i] - 1;
				i++;
				break;
			}
			else res.mem[i]= res.ex_radix -1 ;
	if( &res != &m1)
	{
		for(; i<m1.len; i++)
			res.mem[i]= m1.mem[i];
		res.len=m1.len;
	}
	for( i-- ; i>0 && !res.mem[i]; i--)
		res.len--;
}
inline void Ladd(const LInteger &num, const digit_t d, LInteger &res)
{
	res.sign=num.sign;
	if(num.sign)
		if(num.len == 1)
		{
			res.len=1;
			if(d >= num.mem[0])
			{
				res.sign=0;
				res.mem[0] = d - num.mem[0];
			}
			else res.mem[0] = num.mem[0] - d;
		}
		else Lsubabs(num, d, res);
	else Laddabs(num, d, res);
}
inline void Ladd(const digit_t d, const LInteger &num, LInteger &res)
{
	Ladd(num, d, res);
}
inline void Ladd(const LInteger &m1,const LInteger &m2,LInteger &res)
{
	if(m1.sign == m2.sign)
	{
		Laddabs(m1,m2,res);
		res.sign = m1.sign;
	}
	else if(m1.len > m2.len)
		{
			Lsubabs(m1,m2,res);
			res.sign = m1.sign;
		}
		else if(m1.len < m2.len)
		{
			Lsubabs(m2,m1,res);
			res.sign = m2.sign;
		}
		else for(l_ui i=m1.len-1;;i--)
				if(m1.mem[i] == m2.mem[i])
					if(i)
						continue;
					else 
					{
						res.sign=0;
						res.len=1;
						res.mem[0]=0;
						return;
					}
				else if(m1.mem[i] > m2.mem[i])
				{
					Lsubabs(m1,m2,res);
					res.sign = m1.sign;
					return;
				}
				else if(m1.mem[i] < m2.mem[i])
				{
					Lsubabs(m2,m1,res);
					res.sign = m2.sign;
					return;
				}
}
inline void Ladd(const LFraction &m1, const LInteger &m2, LFraction &res)
{
	if( m1.de.len == 1 && m1.de.mem[0] == 1)
	{// the denominator is 1
		Ladd(m1.nu, m2, res.nu);
		res.de.len = 1;
		res.de.mem[0] = 1;
		return;
	}
	Lmul(m2, m1.de, res.nu);
	Ladd(res.nu, m1.nu, res.nu);
	if( res.nu.len == 1 && res.nu.mem[0] == 0)
	{	// the numerator is 0
		res.de.len = 1;
		res.de.mem[0] =1;
		return;
	}
	res.de = m1.de;
	// reducing()
}
inline void Ladd(const LInteger &m1, const LFraction &m2, LFraction &res)
{
	Ladd(m2, m1, res);
}
inline void Ladd(const LFraction &m1, const LFraction &m2, LFraction &res)
{
	if( (m1.nu.len == 1 && m1.nu.mem[0] == 0) || (m2.nu.len == 1 && m2.nu.mem[0] == 0) )
	{
		res.de.len = 1;
		res.de.mem[0] = 0;
		return;
	}
	Lmul(m1.nu, m2.de, res.nu);
	Lmul(m1.de, m2.nu, res.de);
	Ladd(res.nu, res.de, res.nu);
	if( res.nu.len == 1 && res.nu.mem[0] == 0)
	{	// the numerator is 0
		res.de.len = 1;
		res.de.mem[0] =1;
		return;
	}
	Lmul(m1.de, m2.de, res.de);
}
inline void Ladd(const LFraction &m1, const LInteger &m2, LFraction &res, LInteger &tmp)
{
	if( m1.de.len == 1 && m1.de.mem[0] == 1)
	{// the denominator is 1
		Ladd(m1.nu, m2, res.nu);
		res.de.len = 1;
		res.de.mem[0] = 1;
		return;
	}
	Lmul(m2, m1.de, tmp);
	Ladd(tmp, m1.nu, res.nu);
	if( res.nu.len == 1 && res.nu.mem[0] == 0)
	{	// the numerator is 0
		res.de.len = 1;
		res.de.mem[0] =1;
	}
	// reducing()
}
inline void Ladd(const LInteger &m1, const LFraction &m2, LFraction &res, LInteger &tmp)
{
	Ladd(m2, m1, res,tmp);
}

inline void Lsub(const LInteger &num,digit_t d,LInteger &res)
{
	res.sign=num.sign;
	if(num.sign)
		Laddabs(num, d, res);
	else if(num.len == 1)
		{
			res.len=1;
			if(d >= num.mem[0])
			{
				res.mem[0] = d - num.mem[0];
				res.sign=1;
			}
			else res.mem[0] = num.mem[0] - d;
		}
		else Lsubabs(num, d, res);
}
inline void Lsub(digit_t d, const LInteger &num,LInteger &res)
{
	res.sign=!num.sign;
	if(res.sign)
		if(num.len == 1)
		{
			res.len=1;
			if(d >= num.mem[0])
			{
				res.mem[0] = d - num.mem[0];
				res.sign=0;
			}
			else res.mem[0] = num.mem[0] - d;
		}
		else Lsubabs(num, d, res);
	else Laddabs(num, d, res);
}
inline void Lsub(const LInteger &m1,const LInteger &m2,LInteger &res)
{
	if(m1.sign == m2.sign)
	{
		if(m1.len > m2.len)
		{
			Lsubabs(m1,m2,res);
			res.sign = m1.sign;
		}
		else if(m1.len < m2.len)
		{
			Lsubabs(m2,m1,res);
			res.sign = m2.sign;
		}
		else for(l_ui i=m1.len-1;;i--)
				if(m1.mem[i] == m2.mem[i])
					if(i)
						continue;
					else 
					{
						res.sign=0;
						res.len=1;
						res.mem[0]=0;
						return;
					}
				else if(m1.mem[i] > m2.mem[i])
				{
					Lsubabs(m1,m2,res);
					res.sign = m1.sign;
					return;
				}
				else //m1.mem[i] < m2.mem[i]
				{
					Lsubabs(m2,m1,res);
					res.sign = m2.sign;
					return;
				}
	}
	else 
	{
		Laddabs(m1,m2,res);
		res.sign = m1.sign;
	}
}

static bool Lpartialadd(LInteger &m1, const LInteger &m2, const l_ui p)
{
	l_ui i;
	digit_t carry;
	for( i = p, carry=0; i< m2.len+p; i++)
	{
		m1.mem[i] = m1.mem[i] + m2.mem[i-p] + carry;
		carry = m1.mem[i] / m2.ex_radix;
		m1.mem[i] = m1.mem[i] % m2.ex_radix;
	}
	if(carry)
	{
		m1.mem[i] = m1.mem[i] + carry;
		carry = m1.mem[i] / m2.ex_radix;
		m1.mem[i] = m1.mem[i] % m2.ex_radix;
	}
	return carry;
}		
static bool Lpartialsub(LInteger &m1, const LInteger &m2, const l_ui p, const bool high0)
{
	l_ui i;
	digit_t borrow;
	for( i = p, borrow=0; i < m2.len+p; i++)
		if( m1.mem[i] >= m2.mem[i-p] + borrow)
		{
			m1.mem[i] = m1.mem[i] - borrow - m2.mem[i-p];
			borrow=0;
		}
		else 
		{
			m1.mem[i] = (m1.mem[i] + m2.ex_radix) - borrow - m2.mem[i-p];
			borrow=1;
		}
	if(high0)
		if( m1.mem[i] >= borrow)
		{
			m1.mem[i] = m1.mem[i] - borrow;
			borrow=0;
		}
		else 
		{
			m1.mem[i] = m2.ex_radix -1;
			borrow=1;
		}
	return borrow;
}

static inline void Lincreabs_atpos(LDecimal &num, const l_ui pos)
{
	l_ui i=pos;
	for(; i<num.len; i++)
		if( num.mem[i] < num.ex_radix-1)
		{
			num.mem[i]++;
			return;
		}
		else num.mem[i]=0;
	if(num.len >= num.memsize)
		num.memsize = expandmem ( num.mem, num.memsize, num.len+1);
	num.mem[i]=1;
	num.len++;
}
void Lapprox(LDecimal &num, const l_ui digcount)
{
	unsigned char msbdig=(unsigned char)(log((double)num.mem[num.len-1])/log((double)num.radix)) + 1;
	digit_t rpower,valtmp;
	l_ui i,setzero;

	if(msbdig > digcount)
	{
		for(i=digcount+1, rpower=1; i<msbdig; i++)
			rpower *= num.radix;
		valtmp = num.mem[num.len-1] / rpower;
		if(Lrounding( (valtmp % num.radix) , num.radix))
		{
			num.mem[num.len-1] = (valtmp / num.radix +1) * num.radix * rpower;
			if(num.mem[num.len-1] >= num.ex_radix)
			{
				num.mem[num.len-1]=0;
				num.len++;
				if(num.len > num.memsize)
					num.memsize = expandmem(num.mem, num.len, num.memsize);
				num.mem[num.len-1] = 1;
			}
		}
		else num.mem[num.len-1] = (valtmp / num.radix ) * num.radix * rpower;
		for(i=num.len-1; !num.mem[i]; i++);
		if(i)
			setzero=i-1;
		else return;
	}
	else if( msbdig == digcount )
	{
		if(  num.len >1 && Lrounding( ( (num.mem[num.len-2] / (num.ex_radix / num.radix) )  % num.radix) , num.radix)  )
			if( num.mem[num.len-1] != num.ex_radix )
			{
				num.mem[num.len-1] ++;
				if(num.len-1)
					setzero=num.len-2;
				else return;
			}
			else 
			{
				num.len++;
				if(num.len > num.memsize)
					num.memsize = expandmem(num.mem, num.len, num.memsize);
				num.mem[num.len-1] = 1;
				setzero=num.len-2;
			}
	}
	else 
	{//	msbdig < digcount
		l_ui digleft = digcount - msbdig;
		l_ui bigdig = digleft / num.ex_power;
		unsigned char partdig = digleft % num.ex_power;
		if( bigdig >= num.len-1 )
			return ;
		l_ui curdig = num.len-bigdig-2;
		if( partdig )
		{
			for(i=partdig+1, rpower=1; i<num.ex_power; i++)
				rpower *= num.radix;
			valtmp = num.mem[curdig] / rpower;
			if(Lrounding( (valtmp % num.radix) , num.radix))
			{
				num.mem[curdig] = (valtmp / num.radix +1) * num.radix * rpower;
				if(num.mem[curdig] >= num.ex_radix)
				{
					num.mem[curdig]=0;
					Lincreabs_atpos(num, curdig+1);
				}
			} 
			else num.mem[curdig] = (valtmp / num.radix ) * num.radix * rpower;
		}
		else 
		{
			if( Lrounding( ( (num.mem[curdig] / (num.ex_radix / num.radix) )  % num.radix) , num.radix)  )
				Lincreabs_atpos(num, curdig+1);
			num.mem[curdig]=0;
		}
		for(i=curdig; !num.mem[i]; i++);
		if(i)
			setzero=i-1;
		else return;
	}
	if(setzero >= num.radpoint)
	{
		if(num.radpoint)
		{
			memmove( num.mem + setzero +1 - num.radpoint, num.mem + setzero +1, (num.len-setzero-1)*sizeof(*num.mem) );
			num.len -= num.radpoint;
			setzero -= num.radpoint;
			num.radpoint = 0;
		}
		for(i=0;i <= setzero; i++)
			num.mem[i] = 0;
	}
	else 
	{
		memmove( num.mem, num.mem + setzero +1, (num.len-setzero-1)*sizeof(*num.mem) );
		num.len -= setzero +1;
		num.radpoint -= setzero +1;
	}
}
digit_t Ldiv(const LInteger &num,const digit_t d,LInteger &res)
{// return the reminder
	l_ui i;
	digit_t rem;
	if(d > num.mem[num.len-1])
		if(num.len == 1)
		{
			res.len=1;
			res.mem[0]=0;
			return num.mem[num.len-1];
		}
		else
		{
			i = num.len-2;
			res.len = num.len-1;
			rem = num.mem[res.len];
		}
	else
	{
		i=num.len-1;
		res.len=num.len;
		rem=0;
	}
	if(res.len > res.memsize)
		res.memsize=expandmem(res.mem, res.len, res.memsize);
	while(1)
	{
		rem = rem * res.ex_radix + num.mem[i];
		res.mem[i] = rem / d;
		rem = rem % d;
		if(!i)
			break;
		i--;
	}
	return rem;
}
void Ldiv(const LInteger &dividend,const LInteger &divisor,LInteger &res)
{// Ref The Art of Computer Programming 3rd Ed Donald E Knuth
	res.sign = dividend.sign ^ divisor.sign;
	if(divisor.len == 1)
	{
		Ldiv(dividend, divisor.mem[0], res);
		return;
	}
	if(dividend.len < divisor.len)
	{
		res.setzero();
		return;
	}
	else if(dividend.len == divisor.len)
			for(l_ui i=dividend.len-1;;i--)
				if(dividend.mem[i] == divisor.mem[i])
					if(i)
						continue;
					else 
					{
						res.len=1;
						res.mem[0]=1;
						return;
					}
				else if(dividend.mem[i] > divisor.mem[i])
						if(i == dividend.len-1)
							break;
						else 
						{
							res.len=1;
							res.mem[0]=1;
							return;
						}
					else
					{
						res.setzero();
						return;
					}
	res.len = dividend.len - divisor.len + 1;
	if(res.len >= res.memsize)
		res.memsize = expandmem ( res.mem, res.memsize, res.len+1);
	LInteger dtmp(res.radix,L_MEM_SIZE_CUS, dividend.len+1),stmp(res.radix,L_MEM_SIZE_CUS, divisor.len+1),tmp(res.radix,L_MEM_SIZE_CUS, divisor.len+1);
	digit_t d = res.ex_radix / (divisor.mem[divisor.len-1] + 1);
	Lmul(dividend, d, dtmp);
	if(dtmp.len == dividend.len)
		dtmp.mem[dividend.len] = 0;
	Lmul(divisor, d, stmp);
	digit_t q,r;
	l_ui i;
	for(i = res.len - 1;;i--)
	{
		q = dtmp.mem[i + divisor.len] * res.ex_radix + dtmp.mem[i + divisor.len -1] ;
		res.mem[i] = q / stmp.mem[divisor.len-1];
		r = q % stmp.mem[divisor.len-1];
		if( res.mem[i] == res.ex_radix || res.mem[i]*stmp.mem[divisor.len-2] > res.ex_radix * r + dtmp.mem[i + divisor.len -2])
		{
			res.mem[i]--;
			r+=stmp.mem[divisor.len-1];
			if(r < res.ex_radix)
				if( res.mem[i] == res.ex_radix || res.mem[i]*stmp.mem[divisor.len-2] > res.ex_radix * r + dtmp.mem[i + divisor.len -2])
				{
					res.mem[i]--;
					r+=stmp.mem[divisor.len-1];
				}
		}
		Lmul(res.mem[i], stmp, tmp);
		if(Lpartialsub(dtmp, tmp, i, stmp.len == tmp.len))
		{
			res.mem[i]--;
			Lpartialadd(dtmp, stmp, i);
		}
		if(!i)
			break;
	}
	if(!res.mem[res.len - 1])
		res.len--;
}
void Ldiv(const LInteger &num,const digit_t d,const l_ui digcount, LDecimal &res)
{
	l_ui i,exdigcnt= digcount/res.ex_power + ((digcount % res.ex_power) ? 2: 1);
	// the extra digit is used for rounding
	digit_t rem;
	res.sign = num.sign;
	if(exdigcnt < num.len)
	{// the result is an integer
		res.radpoint = 0;
		if(d > num.mem[num.len-1])
		{
			rem = num.mem[num.len-1];
			res.len = num.len-1;
		}
		else
		{
			rem = 0;
			res.len = num.len;
		}
		if(res.len > res.memsize)
			res.memsize=expandmem(res.mem, res.len, res.memsize);
		i = res.len-1;
		for(l_ui j=0;j<exdigcnt;i--,j++)
		{
			rem = rem * res.ex_radix + num.mem[i];
			res.mem[i] = rem / d;
			rem = rem % d;
		}
		if(i != -1)
			//i>=0
			for(;;i--)
			{
				res.mem[i]=0;
				if(!i)
					break;
			}
	}
	else
	{// the result is a decimal fraction
		LInteger dtmp(res.radix, L_MEM_SIZE_CUS, exdigcnt+1);
		if(d > num.mem[num.len-1])
		{
			res.radpoint = exdigcnt - num.len +1;
			rem = num.mem[num.len-1];
		}
		else
		{
			res.radpoint = exdigcnt - num.len;
			rem = 0;
		}
		memset(dtmp.mem, 0, res.radpoint*sizeof(*dtmp.mem));
		memcpy(dtmp.mem+res.radpoint, num.mem, num.len*sizeof(*dtmp.mem));
		res.len = exdigcnt;
		if(res.len > res.memsize)
			res.memsize=expandmem(res.mem, res.len, res.memsize);
		for(i = exdigcnt-1; ; i--)
		{
			rem = rem * res.ex_radix + dtmp.mem[i];
			res.mem[i] = rem / d;
			rem = rem % d;
			if(!i)
				break;
			if(i <= res.radpoint && !rem && !dtmp.mem[i-1])
			{// exactly devisable
				res.len = exdigcnt-i;
				res.radpoint -= i;
				memmove(res.mem, res.mem+i, res.len*sizeof(*res.mem));
				return;
			}
		}
	}
	Lapprox(res, digcount);
}
void Ldiv(const LInteger &dividend,const LInteger &divisor, const l_ui digcount, LDecimal &res)
{
	res.sign = dividend.sign ^ divisor.sign;
	if(divisor.len == 1)
	{
		Ldiv(dividend, divisor.mem[0], digcount, res);
		return;
	}
	l_ui i,j,exdigcnt= digcount/res.ex_power + ((digcount % res.ex_power) ? 2: 1);
	digit_t q,r,d = res.ex_radix / (divisor.mem[divisor.len-1] + 1);
	if(dividend.len >= divisor.len + exdigcnt)
	{// the dividend is long enough to produce all the digits we need,
	 // thus the result is an integer
		res.radpoint = 0;
		res.len = dividend.len - divisor.len + 1;
		if(res.len >= res.memsize)
			res.memsize = expandmem ( res.mem, res.memsize, res.len+1);
		LInteger dtmp(res.radix,L_MEM_SIZE_CUS, dividend.len+1),stmp(res.radix,L_MEM_SIZE_CUS, divisor.len+1),tmp(res.radix,L_MEM_SIZE_CUS, divisor.len+1);
		digit_t d = res.ex_radix / (divisor.mem[divisor.len-1] + 1);
		Lmul(dividend, d, dtmp);
		if(dtmp.len == dividend.len)
			dtmp.mem[dividend.len] = 0;
		Lmul(divisor, d, stmp);
		for(i = res.len - 1,j=0; j<exdigcnt; i--, j++)
		{
			q = dtmp.mem[i + divisor.len] * res.ex_radix + dtmp.mem[i + divisor.len -1] ;
			res.mem[i] = q / stmp.mem[divisor.len-1];
			r = q % stmp.mem[divisor.len-1];
			if( res.mem[i] == res.ex_radix || res.mem[i]*stmp.mem[divisor.len-2] > res.ex_radix * r + dtmp.mem[i + divisor.len -2])
			{
				res.mem[i]--;
				r+=stmp.mem[divisor.len-1];
				if(r < res.ex_radix)
					if( res.mem[i] == res.ex_radix || res.mem[i]*stmp.mem[divisor.len-2] > res.ex_radix * r + dtmp.mem[i + divisor.len -2])
					{
						res.mem[i]--;
						r+=stmp.mem[divisor.len-1];
					}
			}
			Lmul(res.mem[i], stmp, tmp);
			if(Lpartialsub(dtmp, tmp, i, stmp.len == tmp.len))
			{
				res.mem[i]--;
				Lpartialadd(dtmp, stmp, i);
			}
		}
		if(!res.mem[res.len - 1])
			res.len--;
		// the work to set zeros will be be done in Lapprox
	}
	else
	{// the dividend isn't long enough, zero expansion is required
		res.len = exdigcnt + 1;
		res.radpoint = divisor.len + exdigcnt - dividend.len;
		if(res.len >= res.memsize)
			res.memsize = expandmem ( res.mem, res.memsize, res.len+1);
		LInteger dtmp(res.radix,L_MEM_SIZE_CUS, divisor.len + exdigcnt +1), stmp(res.radix,L_MEM_SIZE_CUS, divisor.len+1),tmp(res.radix,L_MEM_SIZE_CUS, divisor.len+1);
		Lmul(dividend, d, dtmp);
		if(dtmp.len == dividend.len)
			dtmp.mem[dividend.len] = 0;
		memmove(dtmp.mem + res.radpoint, dtmp.mem, (dividend.len+1)*sizeof(*dtmp.mem));
		memset(dtmp.mem, 0, res.radpoint*sizeof(*dtmp.mem));
		Lmul(divisor, d, stmp);
		for(i = res.len - 1;;i--)
		{
			q = dtmp.mem[i + divisor.len] * res.ex_radix + dtmp.mem[i + divisor.len -1] ;
			res.mem[i] = q / stmp.mem[divisor.len-1];
			r = q % stmp.mem[divisor.len-1];
			if( res.mem[i] == res.ex_radix || res.mem[i]*stmp.mem[divisor.len-2] > res.ex_radix * r + dtmp.mem[i + divisor.len -2])
			{
				res.mem[i]--;
				r+=stmp.mem[divisor.len-1];
				if(r < res.ex_radix)
					if( res.mem[i] == res.ex_radix || res.mem[i]*stmp.mem[divisor.len-2] > res.ex_radix * r + dtmp.mem[i + divisor.len -2])
					{
						res.mem[i]--;
						r+=stmp.mem[divisor.len-1];
					}
			}
			Lmul(res.mem[i], stmp, tmp);
			if(Lpartialsub(dtmp, tmp, i, stmp.len == tmp.len))
			{
				res.mem[i]--;
				Lpartialadd(dtmp, stmp, i);
			}
			if(!i)
				break;
		}
		if(!res.mem[res.len - 1])
			res.len--;
	}
	Lapprox(res, digcount);
}

static inline void Lincreabs(LInteger &num)
{
	l_ui i;
	for( i = 0; i<num.len; i++)
		if( num.mem[i] < num.ex_radix-1)
		{
			num.mem[i]++;
			return;
		}
		else num.mem[i]=0;
	if(num.len >= num.memsize)
		num.memsize = expandmem ( num.mem, num.memsize, num.len+1);
	num.mem[i]=1;
	num.len++;
}
static inline void Ldecreabs(LInteger &num)
{//Assume num >0
	for(l_ui i = 0; i<num.len; i++)
		if( num.mem[i] > 0)
		{
			num.mem[i]--;
			return;
		}
		else num.mem[i]=num.ex_radix-1;
}
inline void operator ++ (LInteger &num, int)
{
	if(num.sign)
		if( (num.len == 1) && (num.mem[0] == 0))
		{
			num.mem[0]=1;
			num.sign=0;
		}
		else Ldecreabs(num);
	else Lincreabs(num);
}
/*
void operator ++ (LDecimal &num, int)
{
	l_ui i;
	if(num.sign)
	{
	}
	else
	{
		if(num.len <= num.radpoint)
		{
			num.memsize = expandptosize ( num.mem, num.memsize, num.radpoint+1);
			for(i=num.len; i< num.radpoint; i++)
				num.mem[i]=0;
			num.mem[i]=1;
			num.len=i+1;
			return;
		}
		for( i = num.radpoint; i<num.len; i++)
			if( num.mem[i] < num.radix-1)
			{
				num.mem[i]++;
				return;
			}
			else num.mem[i]=0;
		if(num.len >= num.memsize)
			num.memsize = expandptosize ( num.mem, num.memsize, num.len+1);
		num.mem[i]=1;
		num.len++;
	}
}
*/
inline void operator ++ (LFraction &num, int)
{
	Ladd(num.de, num.nu, num.nu);
}
inline void operator -- (LInteger &num, int)
{
	if(num.sign)
		Lincreabs(num);
	else if( (num.len == 1) && (num.mem[0] == 0))
		{
			num.mem[0]=1;
			num.sign=1;
		}
		else Ldecreabs(num);
}
inline void operator -- (LFraction &num, int)
{
	num.de.sign=1;
	Ladd(num.de, num.nu, num.nu);
	num.de.sign=0;
}

inline LFraction& Linverse(LFraction &num)
{
	digit_t *memtmp = num.nu.mem;
	num.nu.mem = num.de.mem;
	num.de.mem = memtmp;
	l_ui uitmp = num.nu.len;
	num.nu.len = num.de.len;
	num.de.len = uitmp;
	return num;
}

LDecimal* LF2D(LFraction &num, l_ui digcount, LDecimal* p=0)
{
	if(!p)
		if( !( p =(LDecimal*)malloc(sizeof(LDecimal))) )
		{
			insufficientmem();
			return 0;
		}
		else p->LDecimal::LDecimal(num.de.radix);
	Ldiv(num.nu, num.de, digcount, *p);
	return p;
}

#endif	//LargeNumbers_cpp