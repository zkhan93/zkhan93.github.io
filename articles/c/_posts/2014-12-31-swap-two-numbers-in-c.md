---
layout: default
title:Swap two numbers in c
---
{℅ highlight c℅}

int swap(int* a,int *b){
  a=a+b;
  b=a-b;
  a=a-b;
}
int main(){
    int a,b;
    printf("Enter two numbers");
    scanf("%d %d",&a,&b);
    
    printf("a=%d, b=%d",a,b);
    swap(&a,&b);
    printf("a=%d, b=%d",a,b);
}
{℅endhighlight%}

output of above code will be 
{%heighlight c lineno%}
Enter two numbers
12 16
a=12, b=16
a=16, b=12
{%endhighligt%}
