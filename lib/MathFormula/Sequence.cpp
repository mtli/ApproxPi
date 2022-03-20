typedef unsigned long long SEQ_ELE_T;
#define SEQ_START_TERM	0
#define SEQ_END_TERM	11

//#define CAL_WITH_REC
//Calculate the terms with a recursive expression

//#define SEQ_CMP_EXP
// Compare the terms with another general-term expression
#ifdef SEQ_CMP_EXP
	#define CMP_EXP(n) (n)
#endif

//#define SEQ_SHOW_SUM
#ifdef SEQ_SHOW_SUM
	typedef unsigned long long SEQ_SUM_T;
	#define SEQ_SUM_START_TERM		SEQ_START_TERM
	#define SEQ_SUM_END_TERM		SEQ_END_TERM

	#define SEQ_SUM_CMP
	// Compare the accumulated sum with the values calculated with the general-sum-term expression
	#ifdef SEQ_SUM_CMP
		#define CMP_SUM_EXP(n) ((n)*((n)+1)/2) 
	#endif
#endif

#include<cstdlib>
#include<cmath>
#include<conio.h>
#include<iostream>
#include<iomanip>
using namespace std;

int main()
{
	unsigned long long	n;
	SEQ_ELE_T a[SEQ_END_TERM];

#ifdef CAL_WITH_REC
/************************************ Recursion Expression ******************************************/

	a[0]=0;
	a[1]=1;
	for(n=SEQ_START_TERM+2; n<=SEQ_END_TERM; n++)
	   a[n]=a[n-1]+a[n-2];

#else
/************************************ General Term Expression ****************************************/
	for(n=SEQ_START_TERM; n<=SEQ_END_TERM; n++)
	{
		unsigned long long k=n;
		a[n]=(120*k*k+151*k+47);
		//((2*k+1)*(4*k+3)*(8*k+1)*(8*k+5));
	}

#endif


#ifdef SEQ_CMP_EXP
	cout<<"    n:\t\tThe array a[n]:\t\tComparison expression:\n";
	for(n=SEQ_START_TERM; n<=SEQ_END_TERM; n++)
		cout<<setw(6)<<n<<':'<<setw(19)<<a[n]<<"\t "<<setw(19)<<CMP_EXP(n)<<'\n';
#else
	cout<<"    n:\t\tThe array a[n]:\n";
	for(n=SEQ_START_TERM; n<=SEQ_END_TERM; n++)
		cout<<setw(6)<<n<<':'<<setw(19)<<a[n]<<'\n';
#endif 

#ifdef  SEQ_SHOW_SUM
	SEQ_SUM_T S;
	#ifdef SEQ_SUM_CMP
		cout<<"\n\n    n:\t\tThe sum S[n]:\t   Calculated with the given expression:\n";
		for(S=0, n=SEQ_SUM_START_TERM; n <= SEQ_SUM_END_TERM; n++)
			cout<<setw(6)<<n<<':'<<setw(19)<<(S=S+a[n])<<"\t "<<setw(19)<<CMP_SUM_EXP(n)<<'\n';
	#else 
		cout<<"\n\n    n:\t\tThe sum S[n]:\n";
		for(S=0, n=SEQ_SUM_START_TERM; n <= SEQ_SUM_END_TERM; n++)
			cout<<setwth(6)<<n<<':'<<setw(19)<<(S=S+a[n])<<'\n';
	#endif
#endif

	_getch();
	return 0;
}




