package hw5;
import hw5.Document;

public class File extends Document {
	private String pathname;

	public String toString() {
		return "Path: " + this.pathname + '\n'
				+ super.toString();
	}

	public void setPathname(String pathname) {
		this.pathname = pathname;
	}

	public String getPathname() {
		return this.pathname;
	}

	public String getText() {
		return super.toString();
	}
}
