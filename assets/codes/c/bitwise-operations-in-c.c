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
