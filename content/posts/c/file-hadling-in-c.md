---
title: File Handling in C
date: 2015-01-03 02:11:00
categories: [programming]
tags: [c,file I/O,pointers]
---

The following program demonstrate how we to perform read and write operations on files,

We can create and open a file `w+` (write plus) mode, this will open a file if it exists otherwise create a new file and open it in write mode
<!--more-->

```c
# include<stdio.h>
int main(void){
	FILE *fp;
	char str[100];
	// w+ create or open if exist in write mode
	fp = fopen("myfile.txt", "w+"); 
	fputs("hello world!!..this is a text",fp);
	fclose(fp);
	

	fp = fopen("myfile.txt", "r"); 
	fgets(str, 100, fp);
	printf("file contain:\n%s\n", str);
	fclose(fp);
	return 0;
}
```

Output:
```
file contain:
hello world!!..this is a text

```
