---
layout: post
title: "Factorial in C"
date: 2016-06-01 04:26:00
categories: c update
author: "Swati Kesarwani"
tags: c factorial simple
source_file: /c/factorial-in-c.c
---

Hey! Today i am gonna show you how to calculate factorial of a number.
The code below depicts the basic methodology of how to calculate factorial of a number.

{% highlight c linenos %}
# include<stdio.h>

void factorial(int number){
	int i;
	long long int factorial=1;
	for ( i = number ; i > 0 ; i-- ) 
		factorial*=i;
	//	Printing the factorial of the number	**/
	printf("Factorial of %d = %lld",number,factorial);
	return 0;
}

int main(){
    	//		Declaring variables				**/
    	int num;
    	//		Asking and scanning input			**/
    	printf("Enter a number : ");
    	scanf("%d",&num);
    	//		Passing number to the function			**/
    	factorial(num);
    	return 0;
}
{% endhighlight %}

output of above code will be 
{% highlight c %}
Enter a number : 5
Factorial of 5 = 120
{% endhighlight %}

So, as you have seen the code simply takes up a integer and passes it to the `factorial()` function which in turns prints up the factorial of that number.

`factorial()` is simply based on the technique of how we calculate factorial on pen and paper as : 

5! = 5 * 4* 3* 2* 1 = 120

Well, this was just a simple program on factorial, I will come up soon with some awesome programs! Keep coding till then!