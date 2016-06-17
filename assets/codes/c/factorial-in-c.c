/**
* Author: Swati Kesarwani
**/

# include<stdio.h>

void factorial(int number){
	int i;
	long long int factorial=1;
	for ( i = number ; i > 0 ; i-- ) 
		factorial*=i;
	//	Printing the factorial of the number	**/
	printf("Factorial of %d = %lld",number,factorial);
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

