---
layout: post
title:  "Add Two Integers in Java"
date:   2014-12-28 20:04:04
categories: Java Update
author: "Zeeshan Khan"
---
Following programe will add two Integer numbers for you.

{% highlight java %}
# file Addition.java

import java.io.*
class Addition{
	public static void main(String args[]){
		int a,b;
		BufferedReader br=new BufferedReader(new InputStreamReader(System.in));
		System.out.println("Enter first integer");
		a=Integer.parseInt(br.readLine());
		System.out.println("Enter second integer");
		b=Integer.parseInt(br.readLine());
		
		System.out.println("Sum of two integers is: "+(a+b));
	}
}
{% endhighlight %}
The above programme will output the following result
{% highlight java %}
Enter first integer
10
Enter second integer
5423
Sum of two integers is 5433
{% endhighlight %}
