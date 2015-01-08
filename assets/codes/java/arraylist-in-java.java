import java.io.*;
import java.util.*;
class ArrayListDemo{
	public static void main(String args[]) throws IOException{
		ArrayList<String> array_of_string=new ArrayList<String>();
		String str;
		int index;
		BufferedReader br=new BufferedReader(new InputStreamReader(System.in));
		System.out.println("Enter data  to insert in arraylist");
		//enter 'end' to stop 
		while((str=br.readLine()).equalsIgnoreCase("end")==false){
			array_of_string.add(str);
			System.out.println("Inserted a data size of arrayList is "+array_of_string.size());
		}
		System.out.println("\n------------\nEnter the index to delete data item");
		while(array_of_string.size()>0){
			index=Integer.parseInt(br.readLine());
			if(index>-1 && index<array_of_string.size()){
				array_of_string.remove(index);
				System.out.println("Item at "+index+" deleted now array contains "+array_of_string.size()+" elements");
			}
			else
				System.out.println("Enter a valid index");
		}
		System.out.println("Array is now empty");
	}
}
