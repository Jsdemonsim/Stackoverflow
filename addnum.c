// Given three numbers: a, b, c.
//
// At each step, either do: a += b, or b += a.
// Can you make either a or b equal to c?
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static int solve(uint64_t a, uint64_t b, uint64_t c);

int main(int argc, char *argv[])
{
    uint64_t a = 0, b = 0, c = 0;

    if (argc < 4) {
        printf("Usage: %s a b c\n", argv[0]);
        exit(0);
    }
    a = strtoull(argv[1], NULL, 0);
    b = strtoull(argv[2], NULL, 0);
    c = strtoull(argv[3], NULL, 0);

    // Note, by checking to see if a or b are solutions here, solve() can
    // be made simpler by only checking a + b == c.  That speeds up solve().
    if (solve(a, b, c))
        printf("There is a solution\n");
    else
        printf("There is NO solution\n");
    return 0;
}

int solve(uint64_t a, uint64_t b, uint64_t c)
{
    // Make the first two stack elements be a fake solution.  That way, we
    // can simplify the termination logic because we will be sure to find
    // a "solution".  But if the solution is at stack index 0, we will know
    // that we actually failed to find a solution.
    uint64_t stack[256] = {0, c};
    int      i          = 2;

    // Check the initial parameters for a solution and order a < b.
    if (a == c || b == c)
	return 1;
    if (a > b) {
	uint64_t tmp = a;
	a = b;
	b = tmp;
    }

    // In this loop, a will always be less than b.
    do {
        uint64_t sum = a + b;

        // If we went past the goal, backtrack by popping off the stack.
        if (sum > c) {
	    b = stack[--i];
	    a = stack[--i];
	    sum = a + b;
	}

        // If we reached the goal exactly, we can stop.
        if (sum == c) {
	    // If it was the fake solution, then there was none.
	    if (i == 0)
		return 0;
	    // Otherwise it was a real solution.
            return 1;
	}

	// Push the slower solution on the stack, where the lower value
	// stays the same.  Since memory operations are relatively expensive,
	// do a precheck here to make sure that what we push is valid.
	if (a + sum <= c) {
	    stack[i++] = a;
	    stack[i++] = sum;
	}

	// Try the faster solution (where the lower value doubles).
	a = b;
	b = sum;
    } while(1);
}
