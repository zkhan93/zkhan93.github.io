---
layout: post
author: Zeeshan Khan
date: 2015-02-03 10:14:51
categories: java update
tags: basic scan java buffered reader
title: How To Scan Input In Java
source_file: 
---

The best way to scan input in java includes the use of `BufferedReader` class, another method uses `Scanner` class but is slower, so here we will prefer BufferedReader.

The following code demostrate the use of BufferedReader for scanning input of different type.

{% highlight java linenos%}
# file Scanning.java
import java.io.*;
public class Scanning{
public static void main(String args[]){
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
{% endhighlight%}
output is
{% highlight java %}

{% endhighlight%} 