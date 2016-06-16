# include<stdio.h>

int factorial(int number){
	int i,factorial=1;
	for ( i = number ; i > 0 ; i-- ) 
		factorial*=i;
	//	Printing the factorial of the number	**/
	printf("Factorial of %d = %d",number,factorial);
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

