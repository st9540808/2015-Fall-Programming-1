package hw6;
import java.util.Scanner;
import hw6.*;

public class Demo {
	public static void main(String[] args) {
		Scanner keyboard = new Scanner(System.in);
		String expression = keyboard.nextLine();
		String[] tokens = expression.split("\\s+");

		if (tokens[1].equals(">")
				|| tokens[1].equals("=")
				|| tokens[1].equals("<"))
			OperationComparsion(tokens);
		else
			arithmetic(tokens);
	}

	private static void OperationComparsion(String[] tokens) {
		boolean result = false;
		Comparsion cmp = new Comparsion();

		if (tokens[1].equals(">"))
			result = cmp.perform(tokens[0], tokens[2]).equals("1");
		else if (tokens[1].equals("="))
			result = cmp.perform(tokens[0], tokens[2]).equals("0");
		else if (tokens[1].equals("<"))
			result = cmp.perform(tokens[0], tokens[2]).equals("-1");
		
		System.out.println(result ? "true" : "false");
	}

	private static void arithmetic(String[] tokens) {
		IOperation arith = null;
		
		for (int i = 0; i < tokens.length - 1; i += 2) {
			if (tokens[i + 1].equals("+"))
				arith = new Addition();
			else if (tokens[i + 1].equals("-"))
				arith = new Subtraction();

			tokens[i + 2] = arith.perform(tokens[i], tokens[i + 2]);
		}

		System.out.println(tokens[tokens.length - 1]);
	}
}
