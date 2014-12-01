/* Given two numbers N and M, partition the numbers 0..N randomly into groups
 * of size 1..M and output them (in random order).
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

typedef struct Seed {
    uint32_t seedW;
    uint32_t seedZ;
} Seed;

static uint32_t countGroups(const Seed *seedPart, uint32_t n, uint32_t m);
static void     shuffleGroups(const Seed *seedPart, const Seed *seedShuffle,
				uint32_t numGroups, uint32_t n, uint32_t m);
static void     printGroup(const Seed *seedPart, uint32_t groupIndex,
				uint32_t n, uint32_t m);
static uint32_t rng(Seed *seed);
static void     skipN(Seed *seed, uint32_t numToSkip);

int main(int argc, char *argv[])
{
    uint32_t n           = 0;
    uint32_t m           = 0;
    uint32_t numGroups   = 0;
    Seed     seedPart    = {0};   // Seed used to partition
    Seed     seedShuffle = {0};   // Seed used to shuffle

    if (argc < 3) {
	printf("Usage: part N M\n");
	exit(0);
    }

    // Get initial parameters.
    n = atoi(argv[1]);
    m = atoi(argv[2]);

    // Generate the initial random seeds.
    seedPart.seedW = (uint32_t) time(NULL);
    seedPart.seedZ = ~seedPart.seedW;
    seedShuffle.seedW = rng(&seedPart);
    seedShuffle.seedZ = rng(&seedPart);

    // Count the number of groups we will need.
    numGroups = countGroups(&seedPart, n, m);

#if 0
    {
	int i = 0;
	printf("Split %d into %d groups of max size %d\n", n, numGroups, m);
	for (i=0;i<numGroups;i++) {
	    printGroup(&seedPart, i, n, m);
	}
    }
#endif

    // Now shuffle the groups without maintaining any state.
    shuffleGroups(&seedPart, &seedShuffle, numGroups, n, m);

    return 0;
}

/**
 * Partitions the numbers 0..N into random groups of size 1..M and returns
 * a count of the number of partitions.
 */
static uint32_t countGroups(const Seed *seedPart, uint32_t n, uint32_t m)
{
    Seed     seed      = *seedPart;
    uint32_t count     = 0;
    uint32_t cur       = 0;
    uint32_t groupSize = 0;

    for (cur = 0; cur <= n; cur += groupSize) {
	groupSize = (rng(&seed) % m) + 1;
	count++;
    }
    return count;
}

/**
 * Shuffles the numGroups groups and outputs each group one by one.
 *
 * The shuffle works like a Fisher-Yates shuffle.  In that shuffle, you start
 * with array[0], and pick a random number r from 0..N-1.  Then you swap
 * array[0] with array[r].  Next you move to array[1] and pick a number r
 * from 1..N-1.  You swap array[1] with array[r].  You keep moving down the
 * array and picking a random element at or to the right of the current
 * position, and swap the current position with the random one.
 *
 * This function does the same thing, but it is different in that it doesn't
 * use any extra space (it doesn't store the array).  What it does instead is
 * it rewinds the rng in order to figure out what element is actually at the
 * slot that we picked.  Here is an example:
 *
 * Suppose with 5 elements, 0 1 2 3 4, we do a shuffle with the following
 * random number sequence in the Fisher-Yates shuffle:
 *
 * Original state: 0 1 2 3 4
 * Step 0: r = 3,  3 1 2 0 4 (swap [0] with [3])
 * Step 1: r = 2,  3 2 1 0 4 (swap [1] with [2])
 * Step 2: r = 4,  3 2 4 0 1 (swap [2] with [4])
 * Step 3: r = 4,  3 2 4 1 0 (swap [3] with [4])
 * Step 4: r = 4,  3 2 4 1 0 (swap [4] with [4]) (not really needed)
 *
 * So the randomized sequence in the end is: 3 2 4 1 0
 *
 * Now, without using the array, we can generate that same sequence like this:
 *
 * Step 0: r = 3, find out what is in array[3] at Step 0.
 * Step 1: r = 2, find out what is in array[2] at Step 1.
 * Step 2: r = 4, find out what is in array[4] at Step 2.
 * Step 3: r = 4, find out what is in array[4] at Step 3.
 * Step 4: r = 4, find out what is in array[4] at Step 4.
 *
 * To find out what is in array[index] at step[i], we need to replay the
 * sequence in backwards order.  Let's take Step 4 since it is the lengthiest
 * to compute.
 *
 * Find out what is in array[4] at Step 4:
 *
 * Look at the r picked in Step 3.  There are two possibilities:
 *
 *   a) If r is 4, then at Step 3 we swapped [3] with [4].  Which means that
 *      we should be now looking for what was at array[3] at Step 2.
 *   b) If r is not 4, then we should look for what was at array[4] at Step 2.
 *
 * Following this algorithm:
 *
 * Step 4: looking for slot 4
 * Step 3: r = 4, switch to looking for slot 3
 * Step 2: r = 4
 * Step 1: r = 4
 * Step 0: r = 3, switch to looking for slot 0
 *
 * Finished searching, slot 0 is the answer (i.e. 0 is the answer).
 *
 * In other words, we work backwards and whenever we find an r equal to the
 * slot we are looking for, we switch to looking for slot j, where j is the
 * Step number we are currently at.
 *
 * In order to run this algorithm effectively, we need to use the rng sequence
 * in reverse order.  This means that Step 0 uses the last rng number and
 * Step N-1 uses the first rng number.  That way we can work backwards in
 * linear fashion.
 */
static void shuffleGroups(const Seed *seedPart, const Seed *seedShuffle,
			    uint32_t numGroups, uint32_t n, uint32_t m)
{
    Seed seed = *seedShuffle;
    int  i    = 0;
    int  j    = 0;

    for (i=0;i<numGroups;i++) {
	int groupIndex  = 0;
	
	// The 0th groupIndex comes from the last random number in the sequence.
	// So here, we skip forward to the correct spot in the sequence.
	seed = *seedShuffle;
	skipN(&seed, numGroups - i - 1);

	// Select a number from [i..numGroups-1].  This is "r" in the
	// explanation above.
	groupIndex = i + (rng(&seed) % (numGroups - i));

	// Adjust the random index by examining all previously picked indices.
	for (j=i-1;j>=0;j--) {
	    int r = j + (rng(&seed) % (numGroups - j));

	    // Every time we see the slot we are looking for, we switch
	    // to looking for slot j instead.
	    if (r == groupIndex)
		groupIndex = j;
	}

	// printf("%d) Picked group %d\n", i, groupIndex);
	printGroup(seedPart, groupIndex, n, m);
    }
}

/**
 * This is similar to countGroups() except that it stops at the given
 * groupIndex and prints the group out.
 */
static void printGroup(const Seed *seedPart, uint32_t groupIndex,
			    uint32_t n, uint32_t m)
{
    Seed     seed      = *seedPart;
    uint32_t count     = 0;
    uint32_t cur       = 0;
    uint32_t groupSize = 0;

    // Skip over all groups up to the one we want.
    for (cur = 0; count < groupIndex; cur += groupSize) {
	groupSize = (rng(&seed) % m) + 1;
	count++;
    }

    // Get the last group size.
    groupSize = (rng(&seed) % m) + 1;

    // Handle special case of the last group exceeding N.
    if (cur + groupSize > n)
	groupSize = n - cur + 1;

    if (groupSize == 1) {
	printf("%d\n", cur);
    } else {
	printf("%d..%d\n", cur, cur + groupSize - 1);
    }
}

/**
 * My own random number generator, so that we can use two rngs without
 * interfering with each other.
 *
 * Returns a random number.  This function is based on the MWC generator,
 * which concatenates two 16-bit multiply with carry generators.  It uses
 * two 32-bit seeds stored in the state (seedW and seedZ).
 */
static uint32_t rng(Seed *seed)
{
    seed->seedW = 18000*(seed->seedW & 65535) + (seed->seedW >> 16);
    seed->seedZ = 36969*(seed->seedZ & 65535) + (seed->seedZ >> 16);

    return (seed->seedZ << 16) + seed->seedW;
}

/**
 * Skips N rng numbers in the sequence.
 */
static void skipN(Seed *seed, uint32_t numToSkip)
{
    uint32_t i;

    for (i=0;i<numToSkip;i++)
	rng(seed);
}

