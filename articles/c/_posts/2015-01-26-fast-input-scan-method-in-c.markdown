---
layout: post
author: Zeeshan Khan
date: 2015-01-26 08:03:44
categories: update c
tags: c fast scan
title: Faster Scan Method in C
---

Sometimes we need to scan input really fast like when we are participating in coding competitions like codechef or spoj where the time limit is very strict and inputs are large, we can atain speed by adopting faster scanning methods.

We can acheive our gaol using `getchar_unlocked` funtion.

I  code below we will see how can we scan integers using `getchar_unlocked`.

{% highlight c linenos%}
#include<stduo.h>
#define gc getchar_unlocked
int scan(){
int n=0;
char ch=gc();
while(ch < '0' || ch> '9'){
ch=gc();
}
while(ch<='0' && ch>='9'){
n=n*10+ch-'0';
ch=gc();
}
}
int main(){
int x=scan();
printf("%d\n");
}
{% endhighlight%}
output is
{% highlight c%}

{% endhighlight%} 