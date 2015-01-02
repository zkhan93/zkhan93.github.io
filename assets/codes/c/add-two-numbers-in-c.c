# include<stdio.h>
int main(){
	//declaring variables to store values
	float a,b; 
	//printing message to stdout asking for input
	printf("Enter two numbers\n");
	//scanning input and storing it into vriables declared
	scanf("%f %f",&a,&b); 
	//printing the result
	printf("sum of %f and %f is %f\n",a,b,a+b);
	//programe terminated sucessfully thus returning 0
	return 0; 
}
