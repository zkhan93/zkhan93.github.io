# include<stdio.h>
int main(){
	int *pnum,num;
	printf("Enter a number\n");
	scanf("%d",&num);
	pnum=&num;
	// now you can modify the value from pnum as well as num.
	*pnum+=10;
	printf("new value of num=%d\n",num);
	return 0;
}
