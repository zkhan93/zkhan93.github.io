---
layout: post
author: Zeeshan Khan
title: Pointers in C
categories: c update
tags: c  pointers moderate
date: 2015-01-02 07:39:00
source_file: /c/pointers-in-c.c
---
Pointers are variables that stores the address of a data value, the type of both pointer and the data type should be same,
pointer od `void` types can hold address of any data type, but necessary casting should be done explicitly.

The code below will demostrate how we can use pointers in C
{% highlight c linenos %}
# include<stdio.h>
int main(){
	int *pnum,num;
	printf("Enter a number\n");
	scanf("%d",&num);
	pnum=&num;
	// now you can modify the value from pnum as well as num.
	*pnum+=10;
	printf("new value of num=%d\n",num);
	return 0;
}
{% endhighlight %}

The above code will produce the following output

{% highlight c %}
Enter a number
27
new value of num=37
{% endhighlight %}
