---
layout: post
title:  "Add Two Numbers in C"
date:   2014-12-28 20:04:04
categories: C Update
author: "Zeeshan Khan"
tags: c addition simple
---
Following is the code that will add two float or integer values in C

{% highlight c linenos%}
#include<stdio.h> //include header files
int main(){
	//declaring variables to store values
	float a,b; 
	//printing message to stdout asking for input
	printf("Enter two numbers\n");
	//scanning input and storing it into vriables declared
	scanf("%f %f",&a,&b); 
	//printing the result
	printf("sum of %f and %f is %f",a,b,a+b);
	//programe terminated sucessfully thus returning 0
	return 0; 
}
{% endhighlight %}
The above code result in following output
{% highlight c %}
Enter two numbers
10 14.5
sum of 10 and 14.5 is 24.5
{% endhighlight %}
