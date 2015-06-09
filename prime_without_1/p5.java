import java.util.Arrays;
import java.lang.StringBuilder;
import java.io.BufferedReader;
import java.io.InputStreamReader;

// Changes:
//
// 1) Instead of generating primes to 1000001, find the max "m" requested
//    and generate to m+1 instead.
// 2) Only generate and check odd numbers.  Make a special case for 2 as
//    a prime when counting primes.
// 3) Use a binary indexed tree (see https://www.topcoder.com/community/
//    data-science/data-science-tutorials/binary-indexed-trees/) to keep
//    track of cumulative counts of primes.  By doing this, we can count the
//    number of primes in a range in O(log n) time.
public class p5 {
    final static int BASE = 10;
    static boolean[] isPrime;

    public static void main(String args[] ) throws Exception {
	BufferedReader reader = new BufferedReader(new InputStreamReader(
		    System.in));
	int noOfTestCaseT = Integer.parseInt(reader.readLine().trim());
	StringBuilder output = new StringBuilder(noOfTestCaseT);
	int [][] testCases = new int[noOfTestCaseT][2];
	int maxNum = 0;
	int [] tree;

	for (int i=0;i<noOfTestCaseT;i++) {
	    String[] tempInt = reader.readLine().split(" ");
	    testCases[i][0] = Integer.parseInt(tempInt[0].trim());
	    testCases[i][1] = Integer.parseInt(tempInt[1].trim());
	    if (testCases[i][1] > maxNum)
		maxNum = testCases[i][1];
	}
	maxNum++;

	isPrime = generatePrime(maxNum);
	tree = new int[maxNum];
	for (int i=3;i<maxNum;i+=2) {
	    if (isPrime[i] && !isDigitOnePresent(i)) {
		updateTree(tree, i);
	    }
	}
	// Also add 2.
	updateTree(tree, 2);

	for (int i=0;i<noOfTestCaseT;i++) {
	    int n = testCases[i][0];
	    int m = testCases[i][1];
	    if (n > 0)
		n--;
	    int primesWithoutOnes = readTree(tree, m) - readTree(tree, n);
	    if (primesWithoutOnes == 0) {
		output.append("-1\n");
	    } else {
		output.append(primesWithoutOnes);
		output.append("\n");
	    }
	}
	System.out.print(output);
    }

    private static void updateTree(int [] tree, int index)
    {
	int maxNum = tree.length - 1;
	while (index <= maxNum) {
	    tree[index]++;
	    index += (index & -index);
	}
    }

    private static int readTree(int [] tree, int index)
    {
	int maxNum = tree.length - 1;
	int sum = 0;
	while (index > 0) {
	    sum += tree[index];
	    index -= (index & -index);
	}
	return sum;
    }

    private static int getPrimesWithoutOneCount(int n, int m) {
	int totalCount = 0;
	if (n <= 2)
	    totalCount++;
	n = n | 1;
	for ( int i = n ; i <= m ; i += 2) {
	    if(isPrime[i] && ! isDigitOnePresent(i) ){
		totalCount++;
	    }
	}

	if (totalCount == 0) totalCount = -1;
	return totalCount;
    }

    private static boolean isDigitOnePresent(int i) {
	while(i !=0 ){
	    int temp = i % BASE;
	    if( temp == 1)
		return true;
	    i /= BASE;
	}
	return false;
    }

    private static boolean[] generatePrime(int maxNum) {
	int root = (int) Math.sqrt(maxNum) + 1;
	boolean[] isPrime = new boolean[maxNum];
	Arrays.fill(isPrime, true);
	isPrime[0] = false; isPrime[1] = false;
	for (int i = 3; i < root; i+=2) {
	    if (isPrime[i]) {
		int increment = i+i;
		for (int j = i * i ; j < maxNum; j = j + increment) {
		    isPrime[j] = false;
		}
	    }

	}
	return isPrime;
    }
}
