package hw4;
import java.util.Scanner;
import java.lang.Math;
import java.text.DecimalFormat;

public class BabylonianAlgo {
	public static void main(String args[]) {
		Scanner keyboard = new Scanner(System.in);
		DecimalFormat formatPattern = new DecimalFormat("#.##");
		double input = keyboard.nextDouble();
		double inputSqrt = mysqrt(input, input / 2);
		System.out.println(formatPattern.format(inputSqrt));
	}

	private static double mysqrt(double n, double guess) {
		double r = n / guess;
		guess = (guess + r) / 2;
		if (Math.abs(r - guess) / Math.max(r, guess) <= 0.01) {
			return guess;
		}
		else {
			return mysqrt(n, guess);
		}
	}
}
