package hw6;
import hw6.IOperation;
import java.lang.Math;

public class Comparsion extends IOperation {
	public String perform(String num1, String num2) {
		if (num1.equals("+0") || num1.equals("-0")) num1 = "0";
		if (num2.equals("+0") || num2.equals("-0")) num2 = "0";
		
		if (num1.equals(num2))
				return "0";
		if (isLessThan(num1, num2))
			return "-1";
		return "1"; // greater than
	}

	private boolean isLessThan(String num1, String num2) {
		boolean num1Sign = num1.charAt(0) == '-'; // true if it is negative
		boolean num2Sign = num2.charAt(0) == '-';
		if (num1Sign) num1 = num1.substring(1); 
		if (num2Sign) num2 = num2.substring(1); 
		
		if (num1Sign == false && num2Sign == true)
			return false;
		else if (num1Sign == true && num2Sign == false)
			return true;
		
		boolean isnum1_AbsLessThan_num2 = isAbsLessThan(num1, num2);
		if (num1Sign == false && num2Sign == false) {
			if (isnum1_AbsLessThan_num2)	
				return true;
			else
				return false;
		}
		else if (num1Sign == true && num2Sign == true) {
			if (isnum1_AbsLessThan_num2)
				return false;
			else
				return true;
		}

		System.out.println("error in Comparsion");
		return false;
	}

	private boolean isAbsLessThan(String num1, String num2) {
		if (num1.length() < num2.length())
			return true;
		else if (num1.length() > num2.length())
			return false;

		for (int i = 0; i < num1.length(); ++i) {
			if (num1.charAt(i) > num2.charAt(i))
				return false;
		}
		return true;
	}
}
