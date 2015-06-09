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
// 3) After generating primes, go through the isPrime array and set all
//    primes with a "1" digit to false.  That way, when we start counting
//    primes we don't have to check for "1" digits again.
public class p3 {
    final static int BASE = 10;
    static boolean[] isPrime;

    public static void main(String args[] ) throws Exception {
	BufferedReader reader = new BufferedReader(new InputStreamReader(
		    System.in));
	int noOfTestCaseT = Integer.parseInt(reader.readLine().trim());
	StringBuilder output = new StringBuilder(noOfTestCaseT);
	int [][] testCases = new int[noOfTestCaseT][2];
	int maxNum = 0;

	for (int i=0;i<noOfTestCaseT;i++) {
	    String[] tempInt = reader.readLine().split(" ");
	    testCases[i][0] = Integer.parseInt(tempInt[0].trim());
	    testCases[i][1] = Integer.parseInt(tempInt[1].trim());
	    if (testCases[i][1] > maxNum)
		maxNum = testCases[i][1];
	}

	isPrime = generatePrime(maxNum+1);

	// Get rid of all primes with a "1" digit beforehand.
	for (int i=3;i<=maxNum;i+=2) {
	    if (isPrime[i] && isDigitOnePresent(i))
		isPrime[i] = false;
	}

	for (int i=0;i<noOfTestCaseT;i++) {
	    int n = testCases[i][0];
	    int m = testCases[i][1];
	    int primesWithoutOnes = getPrimesWithoutOneCount(n, m);
	    output.append(primesWithoutOnes);
	    output.append("\n");

	}
	System.out.print(output);
    }

    private static int getPrimesWithoutOneCount(int n, int m) {
	int totalCount = 0;
	// Check for 2 as a special case.
	if (n <= 2 && m >= 2)
	    totalCount++;
	// Start at an odd number and only check odd numbers.
	for (int i = n|1 ; i <= m ; i += 2) {
	    if (isPrime[i]) {
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
