#include<stdio.h>
#define gc getchar_unlocked
int scan_fast(){
	int n=0;
	char ch=gc();
	while(ch < '0' && ch >'9'){
		ch=gc();
	}
	while(ch>='0' && ch<='9'){
		n=n*10+ch-'0';
		ch=gc();
	}
	return n;
}

int scan_normal(){
	int n;
	scanf("%d",&n);
	return n;
}

int main(){
	int num=1000000;
	while(num--){
		//scan_normal(); //uncomment this for 1st execution
		//scan_fast(); //uncomment this for 2nd execution
	}
	return 0;
}
