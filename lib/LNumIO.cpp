/*
Contents:
I/O over Large number stuctures

I/O Format:
There's no direct I/O for LFraction since its a simple combination of 2 LInteger.
For LInteger and LDecimal, we have the following conventions.
Assume the radix of the number is r.

Text output:
2 <= r <= 36	: each character in character set "0~9A~Z" represents one single digits 
				  in radix r, and there's no space interval between characters
  r > 36		: first, each digits in radix r will be converted into a decimal string "dd...d",
				  then we connect each element string with 1 space interval before each string
				  to form the final result: " dd...d dd...d ...dd...d". Notice that the radix 
				  point will not take up an additional space, instead, it replace the space
				  the integer part and the fraction part of the number, e.g. " 03.59 04 31"

In both case, only significant 0s will be output.

Text input:
    Currently, we only support the input for 2 <= r <= 36, where the format is the same as the 
one in text output.

Binary input and output:

4-byte format
Bytes		Values
4		:	(Large Number Lib Version)
2		:   BinType (with the LSB 0 to signify the 4-byte format)
1		:	sign
1		:	ex_power
4		:	radix
4		:   ex_radix
4		:	len
(4)		:	(LDecimal only) radpoint
4*len	:   (the digits of the number in the expanded radix stored reversely, 
			 i.e. the LSB appears first in the array)

Total length in byte (20 + (4) + 4*len)

8-byte format
Bytes		Values
4		:	(Large Number Lib Version)
2		:   BinType (with the LSB 1 to signify the 8-byte format)
1		:	sign
1		:	ex_power
8		:	radix
8		:   ex_radix
8		:	len
(8)		:	(LDecimal only) radpoint
8*len	:   (the digits of the number in the expanded radix stored reversely, 
			 i.e. the LSB appears first in the array)

Total length in byte (32 + (8) + 8*len)

Written by LeOpArD, 2011-12-8
*/

#ifndef LNumIO_cpp
#define LNumIO_cpp

#include<cstdio>
#include<cstring>
#include<cctype>
#include<cmath>

#include"LargeNumbers.h"

//#define LBT_4BYTE		0x0000
//#define LBT_8BYTE		0x0001
//
//#define LBT_LINTEGER		0x0000
//#define LBT_LDECIMAL		0x0001

// opt for radix = 2^p
// memory storage in ex_radix to radix
template <class pointer_t>
void exr2rmem_t(const LInteger &num, pointer_t *p, bool order)
{
	l_ui i,j,k=0;
	digit_t d;
	if(order)
		for( i = num.len -1; ; i--)
		{
			if(!num.mem[i])
				for(j=0; j < num.ex_power; j++)
					p[k++]=0;
			else 
			{
				d = num.mem[i];
				j = 1;
				k += num.ex_power;
				while(d)
				{
					p[k - j++] = d % num.radix;
					d /= num.radix;
				}
				while(j <= num.ex_power)
				{
					p[k - j++]=0;
				}
			}
			if(!i)
				break;
		}
	else
		for(i = 0; i < num.len; i++)
			if(!num.mem[i])
				for(j=0; j < num.ex_power; j++)
					p[k++]=0;
			else 
			{
				d = num.mem[i];
				j = 1;
				k += num.ex_power;
				while(d)
				{
					p[k - j++] = d % num.radix;
					d /= num.radix;
				}
				while(j <= num.ex_power)
				{
					p[k - j++]=0;
				}
			}
}
l_ui exr2rmem(const LInteger &num, unsigned char &radbyte, void* &p, bool order = 0)
{/*	order:
 0	: LSB to MSB (reverse)
 1	: MSB to LSB
 */

	if( num.radix <= 0x100 )
		radbyte = 1;
	else if( num.radix <= 0x10000 )
		radbyte = 2;
	else if( num.radix <= 0x100000000 )
		radbyte = 4;
	else radbyte = 8;
	if(!p)
		if(! (p = malloc(radbyte * num.ex_power * num.len) ) )
		{
			insufficientmem();
			return 0;
		}
	switch(radbyte)
	{
	case 1:	exr2rmem_t<unsigned char> (num, (unsigned char*)p, order);
			break;
	case 2:	exr2rmem_t<unsigned short> (num, (unsigned short*)p, order);
			break;
	case 4:	exr2rmem_t<unsigned long> (num, (unsigned long*)p, order);
			break;
	case 8:	exr2rmem_t<unsigned long long> (num, (unsigned long long*)p, order);
			break;
	}
	return radbyte * num.ex_power * num.len;
}

template <class pointer_t>
void exr2rmem_t(const LDecimal &num, pointer_t *p, bool order)
{
	l_ui i,j,k=0;
	digit_t d;
	if(order)
		for( i = num.len -1; ; i--)
		{
			if(!num.mem[i])
				for(j=0; j < num.ex_power; j++)
					p[k++]=0;
			else 
			{
				d = num.mem[i];
				j = 1;
				k += num.ex_power;
				while(d)
				{
					p[k - j++] = d % num.radix;
					d /= num.radix;
				}
				while(j <= num.ex_power)
				{
					p[k - j++]=0;
				}
			}
			if(!i)
				break;
		}
	else
		for(i = 0; i < num.len; i++)
			if(!num.mem[i])
				for(j=0; j < num.ex_power; j++)
					p[k++]=0;
			else 
			{
				d = num.mem[i];
				j = 1;
				k += num.ex_power;
				while(d)
				{
					p[k - j++] = d % num.radix;
					d /= num.radix;
				}
				while(j <= num.ex_power)
				{
					p[k - j++]=0;
				}
			}
}
l_ui exr2rmem(const LDecimal &num, unsigned char &radbyte, void* &p, bool order = 0)
{/*	order:
 0	: LSB to MSB (reverse)
 1	: MSB to LSB
 */

	if( num.radix <= 0x100 )
		radbyte = 1;
	else if( num.radix <= 0x10000 )
		radbyte = 2;
	else if( num.radix <= 0x100000000 )
		radbyte = 4;
	else radbyte = 8;
	if(!p)
		if(! (p = malloc(radbyte * num.ex_power * num.len) ) )
		{
			insufficientmem();
			return 0;
		}
	switch(radbyte)
	{
	case 1:	exr2rmem_t<unsigned char> (num, (unsigned char*)p, order);
			break;
	case 2:	exr2rmem_t<unsigned short> (num, (unsigned short*)p, order);
			break;
	case 4:	exr2rmem_t<unsigned long> (num, (unsigned long*)p, order);
			break;
	case 8:	exr2rmem_t<unsigned long long> (num, (unsigned long long*)p, order);
			break;
	}
	return radbyte * num.ex_power * num.len;
}

char* dig2decimalstr(digit_t d, char *buf, l_ui radix, unsigned char ex_power,l_ui diglen, bool fillzero=true)
{
//# of digit in radix	  /----------- ex_power -------------\
//length:       	   1	diglen    1	   diglen ......   1	diglen
//decimal format    [space]ddd...dd[space]ddd...dd......[space]ddd...dd
	unsigned i,j,cur;
	if(fillzero)
	{
		if(!d)
		{
			for(i=0, cur=0; i < ex_power; i++)
			{
				buf[cur++]=' ';
				for(j=0; j<diglen; j++)
					buf[cur++]='0';
			}
			buf[cur++]='\0';
			return buf;
		}
		l_ui raddig;
		cur=(diglen+1)*ex_power;
		buf[cur--]='\0';
		i=0;
		while(d)
		{
			raddig = d % radix;
			d/=radix;
			i++;
			if(!raddig)
				for(j=0; j<diglen; j++)
					buf[cur--]='0';
			else 
			{
				j=0;
				while(raddig)
				{
					buf[cur--]=raddig % 10 +'0';
					raddig/=10;
					j++;
				}
				for(; j<diglen; j++)
					buf[cur--]='0';
			}
			buf[cur--]=' ';
		}
		for(;i<ex_power;i++)
		{
			for(j=0; j<diglen; j++)
				buf[cur--]='0';
			buf[cur--]=' ';
		}
		return buf;
	}
	else 
	{
		if(!d)
		{
			buf[0]=' ';
			for(j=1; j<=diglen; j++)
				buf[j++]='0';
			buf[j]='\0';
			return buf;
		}
		l_ui raddig;
		unsigned totlen=(diglen+1)*ex_power;
		cur=totlen;
		buf[cur--]='\0';
		i=1;
		while(d)
		{
			raddig = d % radix;
			d/=radix;
			i++;
			if(!raddig)
				if(d)
					for(j=0; j<diglen; j++)
						buf[cur--]='0';
				else return (char*)memmove(buf, buf+cur+1, (totlen-cur)*sizeof(*buf));
			else 
			{
				j=0;
				while(raddig)
				{
					buf[cur--]=raddig % 10 +'0';
					raddig/=10;
					j++;
				}
				for(; j<diglen; j++)
					buf[cur--]='0';
			}
			buf[cur--]=' ';
		}
		if(cur == -1)
			return buf;
		else return (char*)memmove(buf, buf+cur+1, (totlen-cur)*sizeof(*buf));
	}
}
l_ui out2txt(const LInteger& num, FILE *fp=stdout)
{// return the total number of character output
	if(!num.len || !num.memsize)
		return 0;
	l_ui charcount=0;
	bool fillzero;
	char *buf;
	if(num.sign)
		charcount += (fputc('-', fp) == EOF) ? 0: 1;
	if(num.radix <= 36)
	{// single character representation
		if( !(buf=(char*)malloc((num.ex_power+1)*sizeof(char*))) )
		{
			insufficientmem();
			return 0;
		}
		fillzero=false;
		for(l_ui i=num.len-1;; i--)
		{
			pf_itoa( num.mem[i], buf, num.radix );
			if(fillzero)
				charcount += fprintf(fp, "%0*s", num.ex_power, buf);
			else
			{
				charcount += fprintf(fp, "%s", buf);
				fillzero=true;
			}
			if(!i)
				break;
		}
	}
	else
	{// multiple decimal number characters representation
		// buf size = ex_power * ([log(10)(radix)]+2) + 1
		l_ui diglen= (l_ui)log10((double)num.radix) +1;
		if( !(buf=(char*)malloc( (num.ex_power*(diglen+1) +1 )*sizeof(char*))) )
		{
			insufficientmem();
			return 0;
		}
		fillzero=false;
		for(l_ui i=num.len-1;; i--)
		{
			charcount += fprintf(fp, "%s", dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen, fillzero));
			fillzero=true;
			if(!i)
				break;
		}
	}
	return charcount;
}
l_ui out2txt(const LDecimal& num, FILE *fp=stdout)
{// return the total number of character output
	if(!num.len || !num.memsize)
		return 0;
	l_ui i,j, charcount=0;
	bool fillzero;
	char *buf;
	if(num.sign)
		charcount += (fputc('-', fp) == EOF) ? 0: 1;
	if(num.radix <= 36)
	{// single character representation
		if( !(buf=(char*)malloc((num.ex_power+1)*sizeof(char*))) )
		{
			insufficientmem();
			return 0;
		}
		fillzero=false;
		if( num.radpoint >= num.len)
		{// the integer part of num is 0
			charcount += (fputc('0', fp) == EOF) ? 0: 1;
			charcount += (fputc('.', fp) == EOF) ? 0: 1;
			for(i = num.radpoint-num.len ; i > 0; i--)
				charcount += (fputc('0', fp) == EOF) ? 0: 1;
			for( i= num.len-1;; i--)
			{
				pf_itoa( num.mem[i], buf, num.radix );
				if(!i)
				{
					unsigned char en0=0,k;
					for(j=0;buf[j];j++)
						if(buf[j]!='0')
							en0=j;
					for(k=0; k<num.ex_power-j ;k++)
						charcount += (fputc('0', fp) == EOF) ? 0: 1;
					for(k=0; k<=en0 ;k++)
						charcount += (fputc(buf[k], fp) == EOF) ? 0: 1;
					return charcount;
				}
				else charcount += fprintf(fp, "%0*s", num.ex_power, buf);
			}
		}
		else 
		{
			bool fillzero=false;
			for(i = num.len-1;; i--)
			{
				pf_itoa( num.mem[i], buf, num.radix );
				if(!i)
				{
					unsigned char en0=0,k;
					for(j=0;buf[j];j++)
						if(buf[j]!='0')
							en0=j;
					if(fillzero)
						for(k=0; k<num.ex_power-j ;k++)
							charcount += (fputc('0', fp) == EOF) ? 0: 1;
					if(i < num.radpoint)
						for(k=0; k<=en0 ;k++)
							charcount += (fputc(buf[k], fp) == EOF) ? 0: 1;
					else charcount += fprintf(fp,"%s",buf);
					return charcount;
				}
				if(fillzero)
					charcount += fprintf(fp, "%0*s", num.ex_power, buf);
				else
				{
					charcount += fprintf(fp, "%s", buf);
					fillzero=true;
				}
				if(i== num.radpoint)
					charcount += (fputc('.', fp) == EOF) ? 0: 1;
			}
		}
	}
	else
	{// multiple decimal number characters representation
		// buf size = ex_power * ([log(10)(radix)]+2) + 1
		l_ui k,diglen= (l_ui)log10((double)num.radix) +1;
		if( !(buf=(char*)malloc( (num.ex_power*(diglen+1) +1 )*sizeof(char*))) )
		{
			insufficientmem();
			return 0;
		}
		bool first=true;
		if( num.radpoint >= num.len)
		{// the integer part of num is 0
			charcount += fprintf(fp, "%0*d", num.ex_power, 0);
			for(i = num.radpoint-num.len ; i > 0; i--)
				if(first)
				{
					charcount += fprintf(fp, "%.0*d", num.ex_power, 0);
					first=false;
				}
				else charcount += fprintf(fp, "% 0*d", num.ex_power, 0);
			for( i= num.len-1;; i--)
				if(first)
					if(i)
					{
						dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen);
						buf[0]='.';
						charcount += fprintf(fp, "%s", buf);
						first=false;
					}
					else 
					{
						dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen);
						buf[0]='.';
						for(j=num.ex_power *(diglen+1)-1; j>diglen+1;j-=diglen+1)
						{
							for(k=0;k<diglen;k++)
								if(buf[j-k]!='0')
									break;
							if(k!=diglen)
								break;
						}
						buf[j+1]='\0';
						charcount += fprintf(fp, "%s", buf);
						break;
					}
				else if(i)
						charcount += fprintf(fp, "%s", dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen));
					else
					{
						dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen);
						for(j=num.ex_power *(diglen+1)-1; j>diglen+1;j-=diglen+1)
						{
							for(k=0;k<diglen;k++)
								if(buf[j-k]!='0')
									break;
							if(k!=diglen)
								break;
						}
						buf[j+1]='\0';
						charcount += fprintf(fp, "%s", buf);
						break;
					}
		}
		else 
		{
			bool fillzero=false, radbefore=false;
			for(i = num.len-1;; i--)
			{
				if(fillzero)
					if(radbefore)
						if(!i)
						{
							dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen);
							buf[0]='.';
							for(j=num.ex_power *(diglen+1)-1; j>diglen+1;j-=diglen+1)
							{
								for(k=0;k<diglen;k++)
									if(buf[j-k]!='0')
										break;
								if(k!=diglen)
									break;
							}
							buf[j+1]='\0';
							charcount += fprintf(fp, "%s", buf);
							break;
						}
						else 
						{
							dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen);
							buf[0]='.';
							charcount += fprintf(fp, "%s", buf);
							radbefore=false;
						}
					else if(!i)
						if(num.radpoint)
						{
							dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen);
							for(j=num.ex_power *(diglen+1)-1; j>diglen+1;j-=diglen+1)
							{
								for(k=0;k<diglen;k++)
									if(buf[j-k]!='0')
										break;
								if(k!=diglen)
									break;
							}
							buf[j+1]='\0';
							charcount += fprintf(fp, "%s", buf);
							break;
						}
						else 
						{
							charcount += fprintf(fp, "%s", dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen));
							break;
						}
					else charcount += fprintf(fp, "%s", dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen));
				else 
				{
					charcount += fprintf(fp, "%s", dig2decimalstr(num.mem[i],buf,num.radix, num.ex_power, diglen, false));
					if(!i)
						break;
					fillzero=true;
				}
				if(i== num.radpoint)
					radbefore=true;
			}
		}
	}
	return charcount;
}
l_ui out2txt(LDecimal& num, const l_ui digcount,const bool iftruncate=true, FILE *fp=stdout)
{// return the total number of character output
/* iftruncate	
true	: truncate extra digits after rounding	(LDecimal num will be modified!)
false	: perserve all digits
extra zeros will be added if the number of significant digits in LDecimal num is less than the digcount
*/
	if(!num.len || !num.memsize)
		return 0;
	if(iftruncate)
		Lapprox(num, digcount);
	l_ui i,supposedlen;
	if(num.radix <= 36)
	{// single character representation
		supposedlen=digcount;
		i=out2txt(num,fp);
		if(num.sign)
			supposedlen++;
		if( num.radpoint >= num.len)
			supposedlen+=2+num.radpoint-num.len;
		else if( num.radpoint)
			supposedlen++;
		else if( supposedlen > i)
				fputc('.', fp);
		for(;i<supposedlen;i++)
			fputc('0', fp);
	}
	else 
	{// multiple decimal number characters representation
		l_ui singlediglen=(l_ui)log10((double)num.radix)+2;
		supposedlen=digcount*singlediglen;
		i=out2txt(num,fp);
		bool nospace=false;
		if(num.sign)
			supposedlen++;
		if( num.radpoint >= num.len)
			supposedlen+=(1+num.radpoint-num.len)*singlediglen;
		else if( !num.radpoint && supposedlen > i)
			{
				fputc('.', fp);
				nospace=true;
			}
		for(;i<supposedlen;i+=singlediglen)
		{
			if(nospace)
				nospace=false;
			else fputc(' ', fp);
			for(l_ui j=1; j<singlediglen; j++)
				fputc('0', fp);
		}
	}
	return supposedlen;
}

#endif	//LNumIO_cpp