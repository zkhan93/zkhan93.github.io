---
layout: post
author: Zeeshan Khan
title: Dynamic array in C
categories: c update
tags: c  array dynamic
date: 2015-01-02 07:57:00
source_file: /c/dynamic-array-in-c.c
---
Dynamic array is a array that has a fixed number of items on it, but the space required for data storage is alloted at runtime on data input.

The code below will demonstrate how we can use dynamic array in C
{% highlight c linenos %}
# include<stdio.h>
# include<stdlib.h>

int main() {
	int *arr[10], i;
 	printf("Enter 10 numbers\n");
	for(i = 0; i < 10; i++){
		//asking for space to store an int
		arr[i] = (int*) malloc(sizeof(int)); 
		scanf("%d", arr[i]);
	}
	printf("the array you entered is ");
	for(i = 0; i < 10; i++){
		printf("%d ", *arr[i]);
		//returning memory to system
		free(arr[i]); 
	}
	printf("\n");
	return 0;
}
{% endhighlight %}

The above code will produce the following output

{% highlight c %}
Enter 10 numbers
1
2
3
4
5
6
7
8
9
10
the array you entered is
1 2 3 4 5 6 7 8 9 10 
{% endhighlight %}
