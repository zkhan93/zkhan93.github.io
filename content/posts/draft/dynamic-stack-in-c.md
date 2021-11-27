---
title:  "Stack Implementation in C"
date:   2015-12-22 9:00:00
categories: [programming]
tags: [stack, pointers, C]
draft: true
---
Typically stacks are like arrays, the difference is in a stack addition and deletion operations are performed from one end,
this end is called `top`.

Let's have a look at stack terminology

###Top
A pointer to top most item is called `top` of the stack, all operations are performed on `top` of the stack.

###Push
Pushing a item into stack means inserting a new item to the top of stack.

###Pop
Poping from stack means removing the top most item from stack.

###Function list

let's look at the code below, we have 5 function

* `getNewNode()` -> to create a new node
* `push()` -> to perform push operation on stack
* `pop()` -> top perform pop operation on stack
* `printStack()` -> to print the stack
* `freeStack()` -> free up the allocated memories for stack
* `main()` -> main method to check functions we created work as required

Apart from functions we have a structure `stack_node` which represent an item in our stack.

###`getNewNode()`
This function takes a interger value as input and return pointer to a new node of type `stack_node`,
it uses the `malloc()` funtion from `stdlib.h` header file to get a pointer to memory for new node

###`push()`
This function takes pointer of a pointer who points to the stack's top most item (you may want to read it again, slowly!),
and an integer value for the new node, it adds (formally pushes) a new `stack_node` to the stack.

###`pop()`
This function takes pointer of a pointer who points to the stack's top most item, 
it returns the value of top most item and delete that item from the stack

###`printStack()`
This function takes a pointer who points to stacks top most node, and prints all the items present in the stack.

###`freeStack()`
This function releases memory allocated for stack items, it uses recursive function call to achieve  its goal.

###`main()`
This function calls all the other method to demonstrate the working of stack implementation.

###Code
{% highlight c linenos %}
#include<stdio.h>
#include<stdlib.h>

/**
 *  Structure of node for stack
 **/
typedef struct stack_node{
    int value;
    struct stack_node* previous;
}stack_node;

/**
 * get a new stack_node from free memory pool with the provided value
 **/
stack_node* getNewNode(int value){
    //get memory from free pool using malloc
    stack_node* new_node=(stack_node*)malloc(sizeof(stack_node));
    //assign given value to the new node
    new_node->value=value;
    //prevoius pointer points to null
    new_node->previous='\0';
    //return the new node
    return new_node;
}

/**
 * add new item to the top of the stack (Push)
 **/
void push(stack_node** stack,int value){
    //create a node with given value
    stack_node* new_node=getNewNode(value);
    if(*stack=='\0'){
        //if stack is empty assign the new node as top most item
        *stack=new_node;
        return;
    }
    //new node points to the stack top
    new_node->previous=*stack;
    //new node is the top most item in stack
    *stack=new_node;
}

/**
 * remove the top most item from stack and return its value (Pop)
 **/ 
int pop(stack_node** stack){
    //check for empty stack
    if((*stack)=='\0'){
        printf("\nSTACK UNDERFLOW\n");
        return;
    }
    //hold a pointer to top most item
    stack_node* top_node=*stack;
    //assign the stack(top) pointer to the second item in the stack
    *stack=(*stack)->previous;
    //store the value of top most node before marking it as free
    int value=top_node->value;
    //free the top most node
    free(top_node);
    //return the value of top most node
    return value;
}

/**
 * Print the current state of the stack
 **/
void printStack(stack_node* stack){
    if(stack=='\0'){
        //if stack is empty
        printf("\nEMPTY STACK\n");
        return;
    }
    printf("\n------START--------\n");
    //loop til current node is not null
    while(stack!='\0'){
        //print stack's current node value
        printf("%d\n",stack->value);
        //move to lower node
        stack=stack->previous;
    }
    printf("-----------END---------------\n");
}

/**
 * free all the nodes linked with the stack
 **/
void freeStack(stack_node* stack){
    if(stack=='\0')
        return;
    //if we have a node attached to the current one, then release the attached node first
    if(stack->previous!='\0')
        freeStack(stack->previous);
    free(stack);
}

/**
 * Main driver method to check functions
 **/
int main(){
    stack_node* stack='\0';
    int i=0,j=0;
    //push 10 items to stack
    for(i=1;i<=10;i++){
        push(&stack,i);
        printStack(stack);
    }
    //pop 5 items
    for(j=0;j<5;j++){
        //print poped value
        printf("\n%d poped\n",pop(&stack));
        printStack(stack);
    }
    //push 5 more items
    for(;i<=15;i++){
        push(&stack,i);
        printStack(stack);
    }
    //remove 10 items 
    for(j=0;j<10;j++){
        //print poped value
        printf("\n%d poped\n",pop(&stack));
        printStack(stack);
    }
    return 0;
}
{% endhighlight %}
