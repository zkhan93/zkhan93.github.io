# include<stdio.h>
# include<stdlib.h>
int main(){
	int *arr[10],i;
 	printf("Enter 10 numbers\n");
	for(i=0;i<10;i++){
		arr[i]=(int*)malloc(sizeof(int));
		scanf("%d",arr[i]);
	}
	printf("the array you entered is ");
	for(i=0;i<10;i++){
		printf("%d ",*arr[i]);
		free(arr[i]);
	}
	printf("\n");
    	return 0;
}
