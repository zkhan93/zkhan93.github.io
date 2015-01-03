# include<stdio.h>
int main(void){
	FILE *fp;
	char str[100];
	fp=fopen("myfile.txt","w+"); // create or open if exist in write mode
	fputs("hello world!!..this is a text",fp); // write into file
	fclose(fp); // close file
	
	fp=fopen("myfile.txt","r");//again open file in read mode
	fgets(str,100,fp); //read first 100 characters or first line 
	printf("file contain:\n%s\n",str);
	fclose(fp);//again close file
	return 0;
}
