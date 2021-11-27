---
title: "Swap Two Numbers in C"
date: 2014-12-30 04:26:00
categories: [programming]
tags: [c, swapping]
---

Code below swaps two numbers without using a third variable
{% highlight c linenos %}
# include<stdio.h>
int swap(int* a, int *b){
    // swapping values using match trick
	*a = *a + *b;
	*b = *a - *b;
	*a = *a - *b;
	return 0;
}
int main(){
    int a, b;
    printf("Enter two numbers");
    scanf("%d %d", &a, &b);
    printf("a=%d, b=%d", a, b);
    swap(&a, &b);
    printf("a=%d, b=%d", a, b);
    return 0;
}
{% endhighlight %}

Output:
{% highlight text %}
Enter two numbers
10 34
before swapping a=10, b=34
After swapping a=34, b=10
{% endhighlight %}

