// This table generator is for this problem:
//
// http://codereview.stackexchange.com/questions/108074/optimize-program-to-test-for-divisibility-of-numbers-3-0
//
// It creates a table where each number is followed by all other numbers that
// are permutations of that number's digits.  For example, 102 should be
// followed by 120, 201, 210.  However, if a permutation has already been
// used, it should not contain its own entry.  For example, 120 should not
// have its own entry because it was already listed in the entry for 102.
//
// The table lists numbers as negative if they start a permutation sequence.
//
// So the table will look like -1, -2, -3, ..., -12, 21, -13, 31, -14, ...
// -102, 120, 201, 210, -103, ..., -999999, -1000000.
//
// The last entry, -1000000, is a sentinel entry that is used to mark the
// last entry.  The program that will use this table will need to use that
// entry.
//
// This generator should be invoked like this:
//
// ./gentable > table.h
//
// You can then compile permute.c, which includes table.h.
//
#include <stdio.h>
#include <stdlib.h>
#define MAX 1000000

int nums[MAX];
int *permutations[MAX];
int permCount[MAX];

int main()
{
    int i;

    for (i=0;i<MAX;i++) {
        int num=i;
        int j;
        int p[10] = {0};
        while (num>0) {p[num%10]++; num/=10;}

        for(j=1;j<10;j++) if(p[j]>0) {num=j; p[j]--; break;}
        for(j=0;j<10;j++) while (p[j]>0) {num=10*num+j; p[j]--;}

        // i -> num
        if (permutations[num] == NULL) {
            if (num >= 100000)
                permutations[num] = (int *) calloc(720, sizeof(int));
            else if (num >= 10000)
                permutations[num] = (int *) calloc(120, sizeof(int));
            else
                permutations[num] = (int *) calloc(24, sizeof(int));
        }
        // Append permutation.
        permutations[num][permCount[num]++] = i;
    }
    for (i=1;i<MAX;i++) {
        int j;
        if (permutations[i] != NULL) {
            printf("-%d, ", i);
            for (j=1;j<permCount[i];j++) {
                printf("%d, ", permutations[i][j]);
            }
            printf("\n");
        }
    }
    printf("-%d\n", MAX);
    return 0;
}
