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
static void updateTree(int *tree, int index, int maxNum);
static int readTree(int *tree, int index);

int main(void)
{
    int   numTests  = 0;
    int  *testCases = NULL;
    int   i         = 0;
    int   j         = 0;
    int   maxNum    = 0;
    int  *tree      = NULL;
    bool *isPrime   = NULL;

    scanf("%d", &numTests);
    testCases = malloc(2*numTests*sizeof(int));

    for (i=j=0;i<numTests;i++,j+=2) {
	scanf("%d %d", &testCases[j], &testCases[j+1]);
	if (testCases[j+1] > maxNum)
	    maxNum = testCases[j+1];
    }

    isPrime = generatePrime(maxNum+1);

    tree = calloc(maxNum+1, sizeof(*tree));
    for (i=3;i<=maxNum;i+=2) {
	if (isPrime[i] && !isDigitOnePresent(i))
	    updateTree(tree, i, maxNum);
    }
    // Also add 2
    updateTree(tree, 2, maxNum);

    for (i=0;i<numTests;i++) {
	int n     = testCases[i+i];
	int m     = testCases[i+i+1];
	int count = 0;

	if (n > 0)
	    n--;
	count = readTree(tree, m) - readTree(tree, n);
	if (count == 0)
	    puts("-1");
	else
	    printf("%d\n", count);
    }
    return 0;
}

static void updateTree(int *tree, int index, int maxNum)
{
    while (index <= maxNum) {
	tree[index]++;
	index += (index & -index);
    }
}

static int readTree(int *tree, int index)
{
    int sum = 0;
    while (index > 0) {
	sum += tree[index];
	index -= (index & -index);
    }
    return sum;
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
    int   root    = (int) sqrt(maxNum);
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
