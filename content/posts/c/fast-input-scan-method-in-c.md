---
title: Faster Input Scan Method in C
author: Zeeshan Khan
date: 2015-01-26 08:03:44
categories: [programming]
tags: [c,fast scan]
---

Sometimes we need to scan input really fast like when we are participating in coding competitions like codechef or SPOJ where the time limit is very strict and inputs are large, we can save some time by adopting faster scanning methods.

We can achieve our gaol using `getchar_unlocked` function.

code below scans 100000 integers and has two different methods one is `scan_normal()` this will scan integers using standard `scanf()` function and the other
 one is `scan_fast()` this scan integers using `getchar_unlocked` function, we will execute the program twice one for each 
 method mark the time taken for execution to evaluate the performance.
  
```c
#include<stdio.h>
#define gc getchar_unlocked // gc for saving for getchar_unlocked
int scan_fast(){
	int n=0;
	char ch=gc();
	while(ch < '0' && ch >'9'){
		ch=gc(); // ignoring chars that are not digit
	}
	while(ch>='0' && ch<='9'){
		n=n*10+ch-'0'; // for each digit add it to n in proper place
		ch=gc();
	}
	return n; 
}

int scan_normal(){
	int n;
	scanf("%d",&n);
	return n;
}

int main(){
	int num=1000000; //we have 100000 integers to test
	while(num--){
	//	scan_normal(); // uncomment this for 1st execution
	//	scan_fast(); // uncomment this for 2nd execution
	}
	return 0;
}

```
for checking i am using linux `time` command to check the execution time and a file `text-file.txt` containing 100000 integers from 0 to 99999

command is as follows

```bash
time ./scan_methods < text-file.txt
```

output using `scan_normal`
```text
real	0m0.112s
user	0m0.101s
sys	0m0.012s
```

output using `scan_fast`
```bash
real	0m0.040s
user	0m0.033s
sys	0m0.008s
```
