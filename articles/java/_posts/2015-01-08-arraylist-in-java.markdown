---
layout: post
title: "ArrayList in java"
categories: Java Update
tags: arraylist dynamic java
author: Zeeshan Khan
date: 2015-01-08 6:35:36
source_file: /java/arraylist-in-java.java
---

`ArrayList` is a collection of data items that grows and shrinks depending on the number of data items present in it, 
you can think it as a dynamic array or a link lists in C, whose size increases when we insert a data and decreases as delete some data from it.

To insert a data item we use `add()` method and to delete we use `remove()` method as demostrated below in line 13 and 20 repectively.

following code shows how we can use ArrayList in java
{% highlight java linenos%}
# file ArrayListDemo.java
import java.io.*;
import java.util.*;
public class ArrayListDemo{
	public static void main(String args[]) throws IOException{
		ArrayList<String> array_of_string=new ArrayList<String>();
		String str;
		int index;
		BufferedReader br=new BufferedReader(new InputStreamReader(System.in)); 
		System.out.println("Enter data  to insert in arraylist");
		//type "end" to stop 
		while((str=br.readLine()).equalsIgnoreCase("end")==false){ //loop ends when user types "end"
			array_of_string.add(str); //adding data to arraylist
			System.out.println("Inserted a data size of arrayList is "+array_of_string.size());
		}
		System.out.println("\n------------\nEnter the index to delete data item");
		while(array_of_string.size()>0){
			index=Integer.parseInt(br.readLine());
			if(index>-1 && index<array_of_string.size()){
				array_of_string.remove(index); //deleting data from arraylist
				System.out.println("Item at "+index+" deleted now array contains "+array_of_string.size()+" elements");
			}
			else
				System.out.println("Enter a valid index");
		}
		System.out.println("Array is now empty");
	}
}
{%endhighlight%}

the output of above code will be 

{% highlight java linenos%}
Enter data  to insert in arraylist
hello
Inserted a data size of arrayList is 1
hi
Inserted a data size of arrayList is 2
anyone
Inserted a data size of arrayList is 3
there?
Inserted a data size of arrayList is 4
no one ?
Inserted a data size of arrayList is 5
:(
Inserted a data size of arrayList is 6
end

------------
Enter the index to delete data item
6
Enter a valid index
5
Item at 5 deleted now array contains 5 elements
4
Item at 4 deleted now array contains 4 elements
3
Item at 3 deleted now array contains 3 elements
3
Enter a valid index
2
Item at 2 deleted now array contains 2 elements
1
Item at 1 deleted now array contains 1 elements
0
Item at 0 deleted now array contains 0 elements
Array is now empty
{%endhighlight%}
