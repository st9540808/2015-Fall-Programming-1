package hw5;
import hw5.Document;

public class Email extends Document {
	private String sender;
	private String recipient;
	private String title;

	public String toString() {
		return ("From: " + this.sender + '\n'
				+ "To: " + this.recipient + '\n'
				+ "Title: " + this.title + '\n'
				+ super.toString());
	}

	public void setSender(String sender) {
		this.sender = sender;
	}

	public String getSender() {
		return this.sender;
	}

	public void setRecipient(String recipient) {
		this.recipient = recipient;
	}

	public String getRecipient() {
		return this.recipient;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getTitle() {
		return this.title;
	}

	public String getText() {
		return super.toString();
	}
}
