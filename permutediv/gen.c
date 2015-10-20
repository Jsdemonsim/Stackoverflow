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
