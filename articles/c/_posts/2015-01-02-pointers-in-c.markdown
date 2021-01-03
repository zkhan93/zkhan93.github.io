---
layout: post
author: Zeeshan Khan
title: Pointers in C
categories: c update
tags: c  pointers moderate
date: 2015-01-02 07:39:00
source_file: /c/pointers-in-c.c
---
Pointers are variables that stores the reference (address) of a data value, datatype of pointers represent what kind of values they can hold reference of, however, a pointer of type `void` can hold reference of any datatype, but explicitly type casting is required to use them.

The code below will demonstrate how we can use pointers in C
{% highlight c linenos %}
# include<stdio.h>
int main(){
	int *pnum, num;
	printf("Enter a number\n");
	scanf("%d", &num);
	pnum = &num;
	// now you can modify the value from pnum as well as num.
	*pnum += 10;
	printf("new value of num=%d\n", num);
	return 0;
}
{% endhighlight %}

Output:
{% highlight c %}
Enter a number
27
new value of num=37
{% endhighlight %}
