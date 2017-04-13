package hw5;
import hw5.Document;
import hw5.Email;
import java.util.Scanner;
import java.util.StringTokenizer;

public class Demo {
	public static void main(String args[]) {
		Scanner keyboard = new Scanner(System.in);
		char mode = keyboard.next().charAt(0);
		switch (mode) {
			case 'A':
				excuModeA();
				break;
			case 'B':
				excuModeB();
				break;
			case 'C':
				excuModeC();
				break;
		}
	}

	private static void excuModeA() {
		Scanner keyboard = new Scanner(System.in);
		String input = keyboard.next();
		if (input.length() < 4)
			return;
		
		if (input.substring(0, 4).equals("File")) {
			File myFile = setFile();
			System.out.println(myFile.toString());
		}
		else if (input.substring(0, 5).equals("Email")) {
			Email myEmail = setEmail();
			System.out.println(myEmail.toString());
		}
		else if (input.substring(0, 8).equals("Document")) {
			Document mydoc = setDocument();
			System.out.println(mydoc.toString());	
		}
	}
	
	private static void excuModeB() {
		Scanner keyboard = new Scanner(System.in);
		Document obj = new Document();
		String input = keyboard.next();
		if (input.length() < 4)
			return;
		
		if (input.substring(0, 4).equals("File"))
			obj = setFile();
		else if (input.substring(0, 5).equals("Email"))
			obj = setEmail();
		else if (input.substring(0, 8).equals("Document"))
			obj = setDocument();
		
		input = keyboard.next();
		StringTokenizer wordFactory = new StringTokenizer(obj.getText().replaceAll("[^a-zA-Z]", " "));

		boolean keywordExists = false;
		while (wordFactory.hasMoreTokens())
			if (wordFactory.nextToken().equals(input))
				keywordExists = true;

		if (keywordExists)
			System.out.println("true");
		else
			System.out.println("false");
	}
	
	private static void excuModeC() {
		Scanner keyboard = new Scanner(System.in);
		String input = keyboard.next();
		if (input.length() < 4)
			return;

		if (input.substring(0, 4).equals("File")) {
			File obj = setFile();
			input = keyboard.next(); keyboard.nextLine();
			if (input.length() < 4)
				return;

			if (input.substring(0, 4).equals("text"))
				obj.setText(keyboard.nextLine());
			else if (input.substring(0, 8).equals("pathname"))
				obj.setPathname(keyboard.nextLine());
			
			System.out.println(obj.toString());
		}
		else if (input.substring(0, 5).equals("Email")) {
			Email obj = setEmail();
			input = keyboard.next(); keyboard.nextLine();
			if (input.length() < 4)
				return;

			if (input.substring(0, 4).equals("text"))
				obj.setText(keyboard.nextLine());
			else if (input.substring(0, 5).equals("title"))
				obj.setTitle(keyboard.nextLine());
			else if (input.substring(0, 6).equals("sender"))
				obj.setSender(keyboard.nextLine());
			else if (input.substring(0, 9).equals("recipient"))
				obj.setRecipient(keyboard.nextLine());
			
			System.out.println(obj.toString());
		}
		else if (input.substring(0, 8).equals("Document")) {
			Document obj = setDocument();
			input = keyboard.next(); keyboard.nextLine();
			if (input.length() < 4)
				return;

			if (input.substring(0, 4).equals("text"))
				obj.setText(keyboard.nextLine());
			
			System.out.println(obj.toString());
		}
	}

	private static Document setDocument() {
		Scanner keyboard = new Scanner(System.in);
		Document mydoc = new Document();
		mydoc.setText(keyboard.nextLine());
		return mydoc;
	}
	
	private static Email setEmail() {
		Scanner keyboard = new Scanner(System.in);
		Email myEmail = new Email();
		myEmail.setSender(keyboard.nextLine());
		myEmail.setRecipient(keyboard.nextLine());
		myEmail.setTitle(keyboard.nextLine());
		myEmail.setText(keyboard.nextLine());
		return myEmail;
	}

	private static File setFile() {
		Scanner keyboard = new Scanner(System.in);
		File myFile = new File();
		myFile.setPathname(keyboard.nextLine());
		myFile.setText(keyboard.nextLine());
		return myFile;
	}
}
