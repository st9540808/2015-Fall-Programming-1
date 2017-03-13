package hw1;

public class PigLatin {
	public static void main(String[] args) {
		for (int i = 0; i < args.length; ++i) {
			if (isVowel(args[i].charAt(0))) {
				System.out.print(args[i].toUpperCase().charAt(0) +
				                 args[i].substring(1) + "ay");
			}
			else if (Character.isLowerCase(args[i].charAt(0))) {
				if (args[i].length() >= 2) {
					System.out.print(args[i].substring(1).toUpperCase().charAt(0) +
					                 args[i].substring(2) +
					                 args[i].charAt(0) + "ay");
				}
				else {
					System.out.print(args[i].toUpperCase().charAt(0) + "ay");
				}
			}

			if (i != args.length - 1) {
				System.out.print(" ");
			}
		}
	}

	private static boolean isVowel(char c) {
		return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
	}
}
