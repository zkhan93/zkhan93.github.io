---
layout: post
author: Zeeshan Khan
title: File Handling in C
categories: c update
tags: c  file I/O pointers moderate
date: 2015-01-03 02:11:00
source_file: c/file-handling-in-c.c
---

The code below will demostrate how we can handle files in C
{% highlight c linenos %}
# include<stdio.h>
int main(void){
	FILE *fp;
	char str[100];
	fp=fopen("myfile.txt","w+"); // create or open if exist in write mode
	fputs("hello world!!..this is a text",fp); // write into file
	fclose(fp); // close file
	
	fp=fopen("myfile.txt","r");//again open file in read mode
	fgets(str,100,fp); //read first 100 characters or first line 
	printf("file contain:\n%s\n",str);
	fclose(fp);//again close file
	return 0;
}
{% endhighlight %}

The above code will produce the following output

{% highlight c %}
file contain:
hello world!!..this is a text

{% endhighlight %}
