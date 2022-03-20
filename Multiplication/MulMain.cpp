//#define SINGLE_CORRECTION_TEST

#define TEST_N_10POWER_MAX 4

#define ENABLE_TEST_GROUP_1
//#define ENABLE_TEST_GROUP_2
//#define ENABLE_TEST_GROUP_3
//#define ENABLE_TEST_GROUP_4

#include<cstdio>
#include<conio.h>
#include<windows.h>
#include<time.h>
#define random(num)     (rand() % (num))
#define randomize()     srand((unsigned)time(NULL))

#define BASIC_DBLFFT_MUL
#include"..\.\lib\LargeNumbers.h"
#include"..\.\lib\Multiplication\LongMulti.cpp"
//#include"..\.\lib\Multiplication\Schonhage.cpp"
#include"..\.\lib\Multiplication\DBLFFTMul.cpp"

#ifndef SINGLE_CORRECTION_TEST
int main()
{
	l_ui i, j, n, nmax = pow(10.0,TEST_N_10POWER_MAX);;
	digit_t radix=10;
	LARGE_INTEGER tfreq,tstart,tend;
	double ElapseTime;
	bool same = true;
	printf("\t\tTest of Multiplication of Large Integers\n\t\t\t\tBy LeOpArD\n\nRadix: %llu\n",radix);
	LInteger itest(radix);
	n = (nmax + itest.ex_power -1) / itest.ex_power;
	LInteger ia(radix, L_MEM_SIZE_CUS, n);
	LInteger ib(radix, L_MEM_SIZE_CUS, n);
	LInteger irl(radix, L_MEM_SIZE_CUS, 2 * n);
	LInteger irf(radix, L_MEM_SIZE_CUS, 2 * n);
	randomize();
	ia.sign = 0;
	for(i = 0; i < n; i++)
		ia.mem[i] = random(itest.ex_radix-1) + 1;
	ib.sign = 0;
	for(i = 0; i < n; i++)
		ib.mem[i] = random(itest.ex_radix-1) + 1;

#ifdef ENABLE_TEST_GROUP_1
	printf("\nTest group 1: complexity comparison between LongMulti and DBLFFTMul\(same length\)"
		"Given random a,b \(a.len = b.len = 10^p, 0 <= p <= %u\)\n Compute a * b -> r\n\tn\tDescription"
		"\t\t\tRunning Time (s)\n", TEST_N_10POWER_MAX);
	for(i = 0, n = 1; i <= TEST_N_10POWER_MAX; i++, n *= 10)
	{
		ib.len = ia.len = (n + itest.ex_power -1)/ itest.ex_power;
		QueryPerformanceFrequency(&tfreq);
		QueryPerformanceCounter(&tstart);
		LongMulti(ia, ib, irl);
		QueryPerformanceCounter(&tend);
		ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
		printf("%9llu : %-37s%g\n", n, "Long Multiplication", ElapseTime);

		QueryPerformanceFrequency(&tfreq);
		QueryPerformanceCounter(&tstart);
		DBLFFTMul(ia, ib, irf);
		QueryPerformanceCounter(&tend);
		ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
		printf("%9llu :  %-36s%g\n", n, "Complex FFT Multiplication", ElapseTime);

		if( irl.len != irf.len)
			same = false;
		else for( j = 0; j < irl.len; j++)
				if( irl.mem[j] != irf.mem[j])
				{
					same = false;
					break;
				}
		if( !same )
		{
			puts("\nResults incoherent!");
			FILE *fp = fopen("ia.txt", "w");
			out2txt(ia,fp);
			fclose(fp);
			fp = fopen("ib.txt", "w");
			out2txt(ib,fp);
			fclose(fp);
			fp = fopen("LongMulRes.txt", "w");
			out2txt(irl,fp);
			fclose(fp);
			fp = fopen("DBLFFTRes.txt", "w");
			out2txt(irf,fp);
			fclose(fp);
			char buf[1024];
			sprintf(buf, "..\\PiOutput\\Verification.exe \"%s\" \"%s\"", "DBLFFTRes.txt", "LongMulRes.txt");
			system(buf);
			break;
		}
	}
#endif
#ifdef ENABLE_TEST_GROUP_2
	if(same)
	{
		printf("\nTest group 2: complexity comparison between LongMulti and DBLFFTMul\(diff length\)"
			"Given random a,b \(a.len = %llu, b.len = 10^p, 0 <= p <= %u\)\n Compute a * b -> r\n\tn\tDescription"
			"\t\t\tRunning Time (s)\n", nmax, TEST_N_10POWER_MAX);
		ia.len = (nmax + itest.ex_power -1)/ itest.ex_power;
		for(i = 0, n = 1; i <= TEST_N_10POWER_MAX; i++, n *= 10)
		{
			ib.len = (n + itest.ex_power -1)/ itest.ex_power;
			QueryPerformanceFrequency(&tfreq);
			QueryPerformanceCounter(&tstart);
			LongMulti(ib, ia, irl);
			QueryPerformanceCounter(&tend);
			ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
			printf("%9llu : %-37s%g\n", n, "Long Multiplication", ElapseTime);

			QueryPerformanceFrequency(&tfreq);
			QueryPerformanceCounter(&tstart);
			DBLFFTMul(ia, ib, irf);
			QueryPerformanceCounter(&tend);
			ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
			printf("%9llu :  %-36s%g\n", n, "Complex FFT Multiplication", ElapseTime);

			same = true;
			if( irl.len != irf.len)
				same = false;
			else for( j = 0; j < irl.len; j++)
					if( irl.mem[j] != irf.mem[j])
					{
						same = false;
						break;
					}
			if( !same )
			{
				puts("\nResults incoherent!");
				FILE *fp = fopen("LongMulRes.txt", "w");
				out2txt(irl,fp);
				fclose(fp);
				fp = fopen("DBLFFTRes.txt", "w");
				out2txt(irf,fp);
				fclose(fp);
				char buf[1024];
				sprintf(buf, "..\\PiOutput\\Verification.exe \"%s\" \"%s\"", "DBLFFTRes.txt", "LongMulRes.txt");
				system(buf);
				break;
			}
		}
	}
#endif
#ifdef ENABLE_TEST_GROUP_3
	if(same)
	{
		printf("\nTest group 3: complexity comparison between LongMulti and DBLFFTMul\(small range\)"
			"Given random a,b \(a.len = %llu, b.len = 10^p, 0 <= p <= %u\)\n Compute a * b -> r\n\tn\tDescription"
			"\t\t\tRunning Time (s)\n", nmax, TEST_N_10POWER_MAX);
		for(n = 400; n <= 500; n += 10)
		{
			ia.len = ib.len = (n + itest.ex_power -1)/ itest.ex_power;
			QueryPerformanceFrequency(&tfreq);
			QueryPerformanceCounter(&tstart);
			LongMulti(ia, ib, irl);
			QueryPerformanceCounter(&tend);
			ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
			printf("%9llu : %-37s%g\n", n, "Long Multiplication", ElapseTime);

			QueryPerformanceFrequency(&tfreq);
			QueryPerformanceCounter(&tstart);
			DBLFFTMul(ia, ib, irf);
			QueryPerformanceCounter(&tend);
			ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
			printf("%9llu :  %-36s%g\n", n, "Complex FFT Multiplication", ElapseTime);

			same = true;
			if( irl.len != irf.len)
				same = false;
			else for( j = 0; j < irl.len; j++)
					if( irl.mem[j] != irf.mem[j])
					{
						same = false;
						break;
					}
			if( !same )
			{
				puts("\nResults incoherent!");
				FILE *fp = fopen("LongMulRes.txt", "w");
				out2txt(irl,fp);
				fclose(fp);
				fp = fopen("DBLFFTRes.txt", "w");
				out2txt(irf,fp);
				fclose(fp);
				char buf[1024];
				sprintf(buf, "..\\PiOutput\\Verification.exe \"%s\" \"%s\"", "DBLFFTRes.txt", "LongMulRes.txt");
				system(buf);
				break;
			}
		}
	}
#endif
#ifdef ENABLE_TEST_GROUP_4
	if(same)
	{
		printf("\nTest group 4: cache spatial locality test on long multiplication\n Given random a,b"
			"\(a.len = %llu, b.len = 10^p, 0 <= p < %u\)\n Compute a * b -> r\n\tn\tDescription"
			"\t\t\tRunning Time (s)\n", nmax, TEST_N_10POWER_MAX);
		ia.len = (nmax + itest.ex_power -1)/ itest.ex_power;
		for(i = 0, n = 1; i < TEST_N_10POWER_MAX; i++, n *= 10)
		{
			ib.len = (n + itest.ex_power -1)/ itest.ex_power;
			QueryPerformanceFrequency(&tfreq);
			QueryPerformanceCounter(&tstart);
			LongMulti(ia, ib, irl);
			QueryPerformanceCounter(&tend);
			ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
			printf("%9llu : %-37s%g\n", n, "LongMulti a * b -> r", ElapseTime);

			QueryPerformanceFrequency(&tfreq);
			QueryPerformanceCounter(&tstart);
			LongMulti(ib, ia, irf);
			QueryPerformanceCounter(&tend);
			ElapseTime=(double)(tend.QuadPart-tstart.QuadPart)/(double)(tfreq.QuadPart);
			printf("%9llu :  %-36s%g\n", n, "LongMulti b * a -> r", ElapseTime);

			same = true;
			if( irl.len != irf.len)
				same = false;
			else for( j = 0; j < irl.len; j++)
					if( irl.mem[j] != irf.mem[j])
					{
						same = false;
						break;
					}
			if( !same )
			{
				puts("\nResults incoherent!");
				FILE *fp = fopen("LongMulRes.txt", "w");
				out2txt(irl,fp);
				fclose(fp);
				fp = fopen("DBLFFTRes.txt", "w");
				out2txt(irf,fp);
				fclose(fp);
				char buf[1024];
				sprintf(buf, "..\\PiOutput\\Verification.exe \"%s\" \"%s\"", "DBLFFTRes.txt", "LongMulRes.txt");
				system(buf);
				break;
			}
		}
	}
#endif
	if(same)
		puts("\nAll done!\nNo difference between the corresponding results is found.");
	_getch();
	return 0;
}

#else

int main()
{
	digit_t radix=10;
	const char* cca="134516716037827420296226391052";
	const char* ccb="13451671603782742029328659518";
	LInteger ia(cca, radix);
	LInteger ib(ccb, radix);
	LInteger ir(radix);
	
	printf("%s\n*\n%s\n=\nLongMul: ", cca, ccb);
	LongMulti(ia, ib, ir);
	out2txt(ir);
	printf("\nDBLFFTMul: ");
	DBLFFTMul(ia, ib, ir);
	out2txt(ir);
	_getch();
	return 0;
}

#endif