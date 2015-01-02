# include<stdio.h>
int swap(int* a,int *b){
	//the logical potion
	*a=*a+*b; // * is use to acess a value pointed by a pointer
	*b=*a-*b;
	*a=*a-*b;
	return 0;
}
int main(){
	//declaring variables
    int a,b;
    //asking and scanning input
    printf("Enter two numbers\n");
    scanf("%d %d",&a,&b);
    //printing the initial values
    printf("before swapping a=%d, b=%d\n",a,b);
    //passing pointers to the functions
    swap(&a,&b);
    //printing the final values
    printf("After swapping a=%d, b=%d\n",a,b);
    return 0;
}
