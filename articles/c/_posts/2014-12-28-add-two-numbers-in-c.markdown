---
layout: post
title:  "Add Two Numbers in C"
date:   2014-12-28 20:04:04
categories: C update
author: "Zeeshan Khan"
---
Following is the code that will add two float or integer values in C

{% highlight c %}
#include<stdio.h> //include header files
int main(){
	float a,b; //declaring variables to store values
	printf("Enter two numbers\n"); //printing message to stdout asking for input
	scanf("%f %f",&a,&b); //scanning input and storing it into vriables declared
	printf("sum of %f and %f is %f",a,b,a+b);//printing the result
	return 0; //programe terminated sucessfully thus returning 0
}
{% endhighlight %}
The above code result in following output
{% highlight c %}
Enter two numbers
10 14.5
sum of 10 and 14.5 is 24.5
{% endhighlight %}
