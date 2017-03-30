package hw3;
import java.util.Scanner;
import java.lang.Math;

public class SpecialNum {
	public static void main(String[] args) {
		Scanner keyboard = new Scanner(System.in);
		String input = keyboard.next();

		if (input.length() <= 2) {
			System.out.println("invalid input string");
			System.exit(1);
		}
		if (Integer.parseInt(input.substring(2)) <= 0) {
			System.exit(1);
		}

		switch (input.charAt(0)) {
			case 'x': ;
			case 'X':
				if (isSpecial(Integer.parseInt(input.substring(2))))
					System.out.println("true");
				else
					System.out.println("false");
				break;
			
			case 'y': ;
			case 'Y':
				System.out.println(nthSpecialNum(
				                   Integer.parseInt(input.substring(2))));
				break;
		}
	}
	
	private static boolean isSpecial(int M) {
		while (M % 2 == 0 && M > 1)
			M /= 2;
		while (M % 3 == 0 && M > 1)
			M /= 3;
		while (M % 5 == 0 && M > 1)
			M /= 5;
		
		return M == 1;
	}

	private static int nthSpecialNum(int N) {
		int[] numList = new int[N];
		int c2 = 0, c3 = 0, c5 = 0;
		numList[0] = 1;
		
		for (int i = 1; i < N; ++i) {
			numList[i] = Math.min(numList[c2] * 2,
			             Math.min(numList[c3] * 3, numList[c5] * 5));
			if (numList[i] == numList[c2] * 2) c2++;
			if (numList[i] == numList[c3] * 3) c3++;
			if (numList[i] == numList[c5] * 5) c5++;
		}
		
		return numList[N - 1];
	}
}
