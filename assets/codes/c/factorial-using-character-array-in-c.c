/**
* Author: Swati Kesarwani
**/

# include<stdio.h>

int factorial(int number){
	int j = 0;				// Used as loop variable
	int i = 0;				// Used for filling the array
	int index = 0;			// To keep track of the index of the array
	int temp = 0;			// Temporary variable used to hold values
	int carry = 0;			// Used to hold carry
	char arr[200];			// Used to store factorial
	if (number == 0)	
		return 1;
		for(i = 0 ; i < 200 ; i++)
		arr[i] = 0;
	i = 0;
	temp = number;
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
	printf("Factorial of %d is : ",number);
	for (i =index - 1 ; i >= 0 ; i--)
		printf("%d",arr[i]);
	return 0;
}

int main(){
    /**		Declaring variables						**/
    int num;
    /**		asking and scanning input				**/
    printf("Enter a number : ");
    scanf("%d",&num);
    /**		passing number to the function			**/
    factorial(num);
    return 0;
}
