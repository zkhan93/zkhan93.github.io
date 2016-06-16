---
layout: post
author: Swati Kesarwani
date: 	2016-06-15 05:53:34
categories: update c
tags: c factorial
title: Factorial in C using Charecter array
source_file: /c/factorial-using-character-array-in-c.c
---

Hey Guys, wondering why I am here once again with this "factorial" thing? You will get to know it in no time..just keep reading!<br/>
Let's calculate some of the factorials:

1! = 1<br/>
2! = 2<br/>
3! = 6<br/>
4! = 24<br/>
5! = 120<br/>
6! = 720

well there is no problem, isn't it?<br/>
Continue then...

7! = 5040<br/>
8! = 40320

Now let's look at 10! It is 3628800

Here comes the problem, in languages like C we need to keep an eye on the size of the variables. Even if we try storing the factorial in a `long` variable, it wont solve our problem.<br/>
So what's the solution? Think,think...<br/>
Well , it's to use a character array. Let me show you how to do it, check out the code below:

{% highlight c linenos%}
# include<stdio.h>

int factorial(int number){
	int j = 0;				/* 	Used as loop variable				*/
	int i = 0;				/* 	Used for filling the array			*/
	int index = 0;				/* 	To keep track of the index of the array		*/
	int temp = 0;				/* 	Temporary variable used to hold diff values	*/
	int carry = 0;				/*	Used to hold carry				*/
	char arr[200];				/*	Used to store factorial				*/
	if (number <= 0)	
		return 1;
	for(i = 0 ; i < 200 ; i++)
		arr[i] = 0;
	i = 0;
	temp = number;
	/* 	Putting the number in the character array 	*/
	while (temp != 0){
		arr[i++] = temp % 10;
		temp /= 10;
	}
	index = i;
	for ( j = number - 1 ; j > 1 ; j-- ) {		
		i = 0;	
		do {
			temp = arr[i] * j + carry;
			arr[i] = temp % 10;			
			carry = temp / 10;
			i++;
		} while(carry > 0 || i < index);
		index = i;	
	}
	/* 	Printing array in reverse order			*/
	printf("Factorial of %d is : ",number);
	for (i =index - 1 ; i >= 0 ; i--)
		printf("%d",arr[i]);
	return 0;
}

int main(){
    /*		Declaring variables				*/
    int num;
    /*		Asking and scanning input			*/
    printf("Enter a number : ");
    scanf("%d",&num);
    /*		Passing number to the function			*/
    factorial(num);
    return 0;
}
{% endhighlight%}

The output wil be as :
{% highlight c%}
Enter a number : 10
Factorial of 10 is : 3628800
{% endhighlight%} 

Let me explain you the working of the `factorial(int)`. As discussed earlier I have used character array `arr[]` for calculating the factorial, one digit at each index in reverse order.

First of all if the `number` is less than or equal to 0 we will simply return as there is no point in calculating its factorial.<br/>
Next I have stored a '0' at all the indeces of `arr` in order to avoid any absurd value using the `for` loop.(You will get to know the reason later on)
After that there is a `while` loop which I have used to store the `number` in the `arr` in the reverse order.<br/>
Suppose the user wants the factorial of 10,then we will have the following values in our array:<br/>
`arr[0] = 0, arr[1] = 1` 
Also we need to keep track of the `index` till where we have a valid value.<br/>
Now we are going to calculate the factorial, i.e. 10 * 9 * 8 ....* 2 * 1

### The for loop 

What I am gonna do is to loop through the `number`-1 (9) till 2. In this loop, I will multiply the number that is stored in the array to the loop variable (j) and store the result back in the character array(again digit wise and in reverse order).
For instance, in the first iteration of `for` loop, what I have done is that I have multiplied 10(`arr`) by 9(`j`) and stored the result back in `arr[]`  , the values will be as:<br/>
`arr[0] = 0, arr[1] = 9` (remember reverse order?)

Now lets understand how this 10 became 90

### The do-while loop.	
Look at the following line:<br/>
`temp = arr[i] * j + carry;`	(0 * 9 + 0)	<br/>
Here, temp is used to temporarily hold the value of the product of the last digit(0) and the iteration variable j(9).
The result comes out to be 0. Then, <br/>
`arr[i] = temp % 10` <br/>
stores the last digit of `temp` variable in the first index of `arr`, i.e., 0 will be stored at `arr[0]`. <br/>
`carry = temp / 10` <br/>
stores the remainging digits of the value of `temp` variable. After this, we'll do an `i++` in order to keep record of number of digits that have been multiplied in this iteration.<br/>
`while(carry > 0 || i < index)`<br/>
 line checks if we have multiplied `j` with all the digits of the `arr` or not. Notice the `carry > 0` statement, it is to check whether there is any carry left or not.
Coming out of the `while` at this time the values of `arr` will be as follows:<br/>
`arr[0] = 0, arr[1] = 1` and `carry = 0`. Now since the `i` is less than index (`i = 1, index = 2`), control will again go into the `do-while` loop, this time `j` will be multiplied to `arr[1]`<br/>
`temp = arr[i] * j + carry;`	(1 * 9 + 0)	<br/>
This results in the value of temp = 9 Now, `arr[i] = temp % 10` results in arr[1] = 9 and  `carry = temp / 10` results in carry = 0. Now the loop condition becomes false as `i` is now equal to `index` and after the coming out of the `do-while` loop, we need to update the `index` as well and the values of `arr` will be as follows:	<br/>
`arr[0] = 0, arr[1] = 9`
Likewise each value of `j`is multipied in the value stored in `arr`.	

Lastly, after coming out of the `for` loop we have to print the number stored in the `arr`, but remember in the reverse order, starting from `index`-1 to 0.

And that's it, now you can calculate factorial of any number, and if you want you can even increase the size of `arr` and now, even more bigger numbers. Easy it was..isn't it?