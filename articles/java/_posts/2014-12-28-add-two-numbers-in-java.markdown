---
layout: post
title:  "Add Two Integers in Java"
date:   2014-12-28 8:04:04
categories: Java Update
author: "Zeeshan Khan"
tags: java addition simple
source_file: /java/add-two-numbers-in-java.java
---
Following program will add two Integer numbers for you.

```java
// file Addition.java
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
```
To run the above code navigate to the source code folder from cmd(Windows) or terminal(Linux) then
type the following code to compile
`javac <filename>.java`
after sucessfully compiling, type `java Addition` to start the programe.

The above programme will output the following result

````java
Enter first integer
12
Enter second integer
123
Sum of two integers is: 135
```
