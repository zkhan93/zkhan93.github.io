---
layout: post
title:  "Add Two Numbers in C"
date:   2014-12-28 20:04:04
categories: C Update
author: "Zeeshan Khan"
tags: c addition simple
source_file: /c/add-two-numbers-in-c.c
---
To add two numeric values in C

{% highlight c linenos %}
#include<stdio.h>

int main(){
	float a, b; 
	printf("Enter two numbers\n");
	scanf("%f %f",&a,&b); 
	printf("sum of %f and %f is %f",a,b,a+b);
	return 0; 
}
{% endhighlight %}

Output:
{% highlight text %}
Enter two numbers
12 34
sum of 12.000000 and 34.000000 is 46.000000
{% endhighlight %}
