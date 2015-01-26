---
layout: post
author: Zeeshan Khan
title: File Handling in C
categories: c update
tags: c  file I/O pointers moderate
date: 2015-01-03 02:11:00
source_file: /c/file-handling-in-c.c
excerpt_separator: <!--more-->
---

The following program demostrate how we to perform read and write operations on files,
first we will create a open `myfile.txt` in `w+` i.e., write plus mode that means if the file exist open it in write mode else create a new file and open it in write mode as well, and write some text to it and close it,
 then we will try to open the same file in `r` i.e., read only mode and try to read what is in there.
<!--more-->

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
