package hw2;
import java.text.DecimalFormat;
import java.util.Scanner;
import java.lang.Math;

public class QuadraticEquation {
	public static void main(String[] args) {
		Scanner keyboard = new Scanner(System.in);
		double a = keyboard.nextDouble();
		double b = keyboard.nextDouble();
		double c = keyboard.nextDouble();
		String format = keyboard.next();
		DecimalFormat formatPattern = new DecimalFormat(format);

		System.out.println(formatPattern.format(formula1(a, b, c)));
		System.out.println(formatPattern.format(formula2(a, b, c)));
	}

	private static double formula1(double a, double b, double c) {
		return (-b + Math.sqrt(b * b - 4. * a * c)) / (2. * a);
	} 

	private static double formula2(double a, double b, double c) {
		return (-b - Math.sqrt(b * b - 4. * a * c)) / (2. * a);
	} 
}
