---
layout: post
title: "Swap Two Numbers in C"
date: 2014-12-18 04:26:00 -0800
categories: c update
author: "Zeeshan Khan"
url: "/c/"
---

The code below swap two numbers
{% highlight c linenos %}
# include<stdio.h>
int swap(int* a,int *b){
  a=a+b;
  b=a-b;
  a=a-b;
  return 0;
}
int main(){
    int a,b;
    printf("Enter two numbers");
    scanf("%d %d",&a,&b);
    
    printf("a=%d, b=%d",a,b);
    swap(&a,&b);
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
