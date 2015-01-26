---
layout: post
title: "Swap Two Numbers in C"
date: 2014-12-30 04:26:00
categories: C Update
author: "Zeeshan Khan"
tags: c swapping simple
source_file: /c/swap-two-numbers-in-c.c
---

The code below swap two numbers
{% highlight c linenos %}
# include<stdio.h>
int swap(int* a,int *b){
	//the logical potion
	*a=*a+*b; //* is use to acess a value pointed by a pointer
	*b=*a-*b;
	*a=*a-*b;
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
10 34
before swapping a=10, b=34
After swapping a=34, b=10
{% endhighlight %}

