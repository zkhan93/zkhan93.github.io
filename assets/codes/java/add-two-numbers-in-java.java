import java.io.*;
class Addition{
	public static void main(String args[]) throws IOException{
		int a,b;
		BufferedReader br=new BufferedReader(new InputStreamReader(System.in));
		System.out.println("Enter first integer");
		a=Integer.parseInt(br.readLine());
		System.out.println("Enter second integer");
		b=Integer.parseInt(br.readLine());
		System.out.println("Sum of two integers is: "+(a+b));
	}
}
