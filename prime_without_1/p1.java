import java.util.Arrays;
import java.lang.StringBuilder;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class p1 {
    final static int MAX = 1000001;
    final static int BASE = 10;

    final static boolean[] isPrime = generatePrime();

    public static void main(String args[] ) throws Exception {
	BufferedReader reader = new BufferedReader(new InputStreamReader(
		    System.in));
	int noOfTestCaseT = Integer.parseInt(reader.readLine().trim());
	StringBuilder output = new StringBuilder(noOfTestCaseT);

	while (noOfTestCaseT != 0) {
	    noOfTestCaseT--;

	    String[] tempInt = reader.readLine().split(" ");
	    int n = Integer.parseInt(tempInt[0].trim());
	    int m = Integer.parseInt(tempInt[1].trim());
	    int primesWithoutOnes = getPrimesWithoutOneCount(n, m);
	    output.append(primesWithoutOnes);
	    output.append("\n");

	}
	System.out.print(output);
    }

    private static int getPrimesWithoutOneCount(int n, int m) {
	int totalCount = 0;
	for ( int i = n ; i <= m ; i++){
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

    private static boolean[] generatePrime() {
	int root = (int) Math.sqrt(MAX) + 1;
	boolean[] isPrime = new boolean[MAX];
	Arrays.fill(isPrime, true);
	isPrime[0] = false; isPrime[1] = false;
	for (int i = 0; i < root; i++) {
	    if (isPrime[i]) {
		for (int j = i * i ; j < MAX; j = j + i) {
		    isPrime[j] = false;
		}
	    }

	}
	return isPrime;
    }
}
