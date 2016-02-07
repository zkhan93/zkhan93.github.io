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