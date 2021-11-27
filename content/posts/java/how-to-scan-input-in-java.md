---
author: Zeeshan Khan
date: 2015-02-03 10:14:51
categories: [programming]
tags: [Java]
title: How To Scan Input In Java
---

The best way to scan input in java includes the use of `BufferedReader` class, another method uses `Scanner` class but is slower, so here we will prefer BufferedReader.

The following code demonstrate the use of BufferedReader for scanning input of different type.

```java
// file ScanInput.java
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

```
output is

```java
Enter a String: Hello World!!
Enter a Integer: 12345
Enter a Decimal Number: 123.345221
String you entered is:Hello World!!
Integer you entered is:12345
Decimal Number you entered is:123.34522
```
