package hw6;
import hw6.IOperation;
import java.lang.Math;
import java.lang.Character;

public class Addition extends IOperation {
	public String perform(String num1, String num2) {
		boolean num1Sign = num1.charAt(0) == '-'; // true if it is negative
		boolean num2Sign = num2.charAt(0) == '-';
		if (num1Sign) num1 = num1.substring(1); 
		if (num2Sign) num2 = num2.substring(1); 
		
		int maxDigits = Math.max(num1.length(), num2.length());
		int[] num1Array = new int[maxDigits + 1];
		int[] num2Array = new int[maxDigits + 1];
		parseStrtoArray(num1, num1Array);
		parseStrtoArray(num2, num2Array);

		boolean resultSign = false;
		int[] result = new int[maxDigits + 1];
		if (num1Sign == false && num2Sign == false) {
			add(num1Array, num2Array, result);
		}
		else if (num1Sign == true && num2Sign == true) {
			resultSign = true;
			add(num1Array, num2Array, result);
		}
		else {
			if (num1Sign == true) { // swap, maintain A + -B structure
				num1Sign = false;
				num2Sign = true;
				String tempStr = num1;
				num1 = num2;
				num2 = tempStr;
				int[] temp = num1Array;
				num1Array = num2Array;
				num2Array = temp;
			}

			Comparsion cmp = new Comparsion();
			if (cmp.perform(num1, num2).equals("1")) { // A > B
				tensComplement(num2Array);
				add(num1Array, num2Array, result);
				result[0] = 0;
			}
			else if (cmp.perform(num1, num2).equals("-1")) { // A < B
				tensComplement(num2Array);
				add(num1Array, num2Array, result);
				tensComplement(result);
				resultSign = true;
			}
		}

		return ArraytoStr_withoutLeadingZeros(result, resultSign);
	}

	private void tensComplement(int[] numArray) {
		int[] add1 = new int[numArray.length];
		add1[numArray.length - 1] = 1;
		for (int i = 1; i < numArray.length; ++i) // substract
			numArray[i] = 9 - numArray[i];
		add(numArray, add1, numArray);
	}

	private void add(int[] num1Array, int[] num2Array, int[] result) {
		int Carry = 0;
		for (int i = num1Array.length - 1; i >= 0; --i) {
			int Cout = num1Array[i] + num2Array[i] + Carry;
			if (Cout >= 10) {
				result[i] = Cout - 10;
				Carry = 1;
			}
			else {
				result[i] = Cout;
				Carry = 0;
			}
		}
	}

	private void parseStrtoArray(String str, int[] array) {
		for (int i = str.length() - 1, j = array.length - 1; i >= 0; --i, --j)
			array[j] = Character.getNumericValue(str.charAt(i));
	}

	private String ArraytoStr_withoutLeadingZeros(int[] array, boolean sign) {
		String str = "";
		boolean reachFirstNonzeroDigit = false;
		for (int i = 0; i < array.length; ++i) {
			if (reachFirstNonzeroDigit == false && array[i] != 0)
				reachFirstNonzeroDigit = true;
			if (reachFirstNonzeroDigit)
				str = str + Character.forDigit(array[i], 10);
		}
		if (reachFirstNonzeroDigit == false)
			return "0";
		
		if (sign)
			return '-' + str;
		return str;
	}
}
