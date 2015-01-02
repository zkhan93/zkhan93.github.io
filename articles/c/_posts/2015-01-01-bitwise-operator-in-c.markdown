---
layout: post
title: "Bitwise operations in C"
date: 2015-01-01 04:26:00
categories: C Update
author: "Zeeshan Khan"
tags: c bitwise AND OR NOT XOR
---

The code below demonstrate common bitwise operations in c
{% highlight c linenos %}
# include<stdio.h>
int main(){
	int a,b,option,res;
	printf("Enter a and b\n");
	scanf("%d %d",&a,&b);
	do{
		printf("MENU\n 1.AND\n 2.OR\n 3.NOT \n 4.XOR\n 0.Exit\n");
		printf("Enter choice: ");
		scanf("%d",&option);
		switch(option){
			case 1:
				res=a&b;
				printf("%d AND %d = %d\n",a,b,res);
			break;
			case 2:
				res=a|b;
				printf("%d OR %d = %d\n",a,b,res);
			break;
			case 3:
				res=~a;
				printf("NOT of %d = %d\n",a,res);
				res=~b;
				printf("NOT of %d = %d\n",b,res);
			break;
			case 4:
			res=a^b;
				printf("%d XOR %d = %d\n",a,b,res);
			break;
			default:
			break;
		}
		printf("\n");
	}while(option);
    return 0;
}
{% endhighlight %}

output of above code will be 
{% highlight c %}
Enter a and b
2 3
MENU
 1.AND
 2.OR
 3.NOT 
 4.XOR
 0.Exit
Enter choice: 1
2 AND 3 = 2

MENU
 1.AND
 2.OR
 3.NOT 
 4.XOR
 0.Exit
Enter choice: 2
2 OR 3 = 3

MENU
 1.AND
 2.OR
 3.NOT 
 4.XOR
 0.Exit
Enter choice: 3
NOT of 2 = -3
NOT of 3 = -4

MENU
 1.AND
 2.OR
 3.NOT 
 4.XOR
 0.Exit
Enter choice: 4
2 XOR 3 = 1

MENU
 1.AND
 2.OR
 3.NOT 
 4.XOR
 0.Exit
Enter choice: 0
{% endhighlight %}
download source code [<i class="fa fa-code"></i>](/assets/codes/c/bitwise-operations-in-c.c)
