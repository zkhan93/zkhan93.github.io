---
layout: post
title: "Swap Two Numbers in C"
date: 2014-12-30 04:26:00
categories: C Update
author: "Zeeshan Khan"
tags: c swapping simple
---

The code below swap two numbers
{% highlight c linenos %}
# include<stdio.h>
int swap(int* a,int *b){
	//the logical potion
	a=a+b;
	b=a-b;
	a=a-b;
	return 0;
}
int main(){
	//declaring variables
    int a,b;
    //asking and scanning input
    printf("Enter two numbers");
    scanf("%d %d",&a,&b);
    //printing the initial values
    printf("a=%d, b=%d",a,b);
    //passing pointers to the functions
    swap(&a,&b);
    //printing the final values
    printf("a=%d, b=%d",a,b);
    return 0;
}
{% endhighlight %}

output of above code will be 
{% highlight c %}
Enter two numbers
12 16
a=12, b=16
a=16, b=12
{% endhighlight %}
