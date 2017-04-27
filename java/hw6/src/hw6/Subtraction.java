package hw6;
import hw6.IOperation;
import java.lang.Math;
import java.lang.Character;

public class Subtraction extends IOperation {
	public String perform(String num1, String num2) {
		Addition adder = new Addition();
		if (num2.charAt(0) == '-')
			return adder.perform(num1, num2.substring(1));
		else
			return adder.perform(num1, "-" + num2);
	}
}
