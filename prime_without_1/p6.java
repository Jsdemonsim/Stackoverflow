import java.util.Arrays;
import java.lang.StringBuilder;
import java.io.BufferedReader;
import java.io.InputStreamReader;

// Changes:
//
// 1) Just do the input/output.  This is to test what the minimum time
//    will be for our program when it is doing absolutely no work at all.
//
//    I measured this program to take 0.14 seconds, which is a lot of overhead
//    considering the C program finished the whole thing in 0.03 seconds
//    including doing the full work.
public class p6 {
    final static int BASE = 10;
    static boolean[] isPrime;

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
	    output.append(n);
	    output.append("\n");
	}
	System.out.print(output);
    }
}
