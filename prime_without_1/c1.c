// This program counts the number of primes between n and m which do not
// contain the digit "1".  The input consists of the number of test cases
// following by two numbers (n, m) per test case.
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define	BASE	10

static bool *generatePrime(int maxNum);
static bool isDigitOnePresent(int i);

int main(void)
{
    int   numTests  = 0;
    int  *testCases = NULL;
    int   i         = 0;
    int   j         = 0;
    int   maxNum    = 0;
    int  *counts    = NULL;
    int   count     = 0;
    bool *isPrime   = NULL;

    scanf("%d", &numTests);
    testCases = malloc(2*numTests*sizeof(int));

    for (i=j=0;i<numTests;i++,j+=2) {
	scanf("%d %d", &testCases[j], &testCases[j+1]);
	if (testCases[j+1] > maxNum)
	    maxNum = testCases[j+1];
    }
    maxNum++;

    isPrime = generatePrime(maxNum);

    counts    = calloc(maxNum, sizeof(*counts));
    count     = 1;
    counts[2] = count;
    for (i=3;i<maxNum;i++) {
	if ((i&1) && isPrime[i] && !isDigitOnePresent(i))
	    count++;
	counts[i] = count;
    }

    for (i=0;i<numTests;i++) {
	int n     = testCases[i+i];
	int m     = testCases[i+i+1];

	if (n > 0)
	    n--;
	count = counts[m] - counts[n];
	if (count == 0)
	    puts("-1");
	else
	    printf("%d\n", count);
    }
    return 0;
}

static bool isDigitOnePresent(int i)
{
    while(i !=0) {
	int temp = i % BASE;
	if( temp == 1)
	    return true;
	i /= BASE;
    }
    return false;
}

static bool *generatePrime(int maxNum)
{
    int   root    = (int) sqrt(maxNum) + 1;
    bool *isPrime = calloc(maxNum, sizeof(bool));
    int   i       = 0;

    for (i = 3; i < root; i+=2) {
	if (!isPrime[i]) {
	    int increment = i+i;
	    int j;
	    for (j = i * i ; j < maxNum; j += increment)
		isPrime[j] = true;
	}
    }
    for (i = 3; i < maxNum; i+=2)
	isPrime[i] = !isPrime[i];
    return isPrime;
}
