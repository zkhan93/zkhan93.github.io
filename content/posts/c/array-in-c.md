---
title: Array in C
date: 2015-01-01 11:04:00
categories: [programming]
tags: [c,array]
---
Arrays in C are collection of values that have same data types.

The code below initializes and assigns values to an each position in an array

{% highlight c linenos %}

# include<stdio.h>

int main(){
    int arr[10];
    int i;
    
    printf("Enter 10 numbers\n");
    for(i=0;i<10;i++){
        scanf("%d",&arr[i]);
    }

    printf("the array you entered is\n");
    for(i=0;i<10;i++){
        printf("%d ",arr[i]);
    }
    
    printf("\n");
    return 0;
}

{% endhighlight %}

Output:
{% highlight c linenos%}
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
