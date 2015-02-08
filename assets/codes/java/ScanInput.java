import java.io.*;
public class ScanInput{
	public static void main(String args[]) throws IOException{
		
		BufferedReader br=new BufferedReader(new InputStreamReader(System.in));
		
		System.out.print("Enter a String: ");
		String msg=br.readLine();
		
		System.out.print("Enter a Integer: ");
		int integer=Integer.parseInt(br.readLine());
		
		System.out.print("Enter a Decimal Number: ");
		Float float_number=Float.parseFloat(br.readLine());
		
		System.out.println("String you entered is:"+msg);
		System.out.println("Integer you entered is:"+integer);
		System.out.println("Decimal Number you entered is:"+float_number);
	}
}
