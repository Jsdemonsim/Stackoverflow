/* Given a number N, shuffle the elements from 0..N-1 and print them. */
/* This algorithm uses O(1) space. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

static void shuffle(int n);

int main(int argc, char *argv[])
{
    if (argc < 2) {
	printf("Usage: shuffle N\n");
	exit(0);
    }

    shuffle(atoi(argv[1]));
    return 0;
}

static void shuffle(int n)
{
    uint32_t seedOriginal = time(NULL);
    uint32_t seed         = 0;
    int      i            = 0;
    int      j            = 0;
    int      slot         = 0;

    for (i=0;i<n;i++) {
	seed = seedOriginal;
	srand(seed);

	// Skip n-i-1 random numbers.
	for (j=n-i-1;j>0;j--)
	    rand();

	// Select an array slot from [i..n-1].
	slot = i + (rand() % (n - i));

	// Find out what that slot corresponds to in the original order.
	// We do this by backtracking through all the previous steps.
	for (j=i-1;j>=0;j--) {
	    int r = j + (rand() % (n - j));

	    // Every time we see the slot we are looking for, we switch
	    // to looking for slot j instead, because at this previous step
	    // we swapped array[j] with array[slot].
	    if (r == slot)
		slot = j;
	}

	// Slot is now the correct element we are looking for.
	printf("%d\n", slot);
    }
}
