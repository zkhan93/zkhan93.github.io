---
layout: post
title: "Linked List in C"
date: 2015-11-18 15:24:00
categories: C Update
author: "Zeeshan Khan"
tags: c linked list
source_file: /c/linked-list-in-c.c
---

Following is the code that will demostrate the operations on linked list, 
although the code is self explanatory we will explain each portion seperately.

{% highlight c linenos %}

#include<stdio.h>
#include<stdlib.h>
/**
 * Structure of one node of the Linked List
 **/
typedef struct node{
    int value;
    struct node* next;
}node;

/**
 * This fucntion prints the whole linked list on console in the below format,
 * value1->value2->value3...->valuen
 * eg. 1->2->3->4
 * if the list is empty it simply prints a message 'List is Empty'
 * @parmeters
 *      node* head: head or root of the linked list,
 *      i.e., pointer pointing to the first item(more formally first node) in the linked list.
 *  
 **/
void printList(node* head){
    if(head!='\0'){
        node* tmpNode=head; 
        while(tmpNode!='\0'){
            printf("%d",tmpNode->value);
            tmpNode=tmpNode->next;
            if(tmpNode!='\0')
                printf("->");
            else
                printf("\n");
        }
    }
    else
        printf("List is Empty\n");
}

/**
 * This function add a node to the end of the linked list, i.e., the newNode will be the last node in the existing linked list,
 * if the list is empty the newNode become the first node of the list
 * @parmeters
 *      node* head: pointer pointing to the start of the list (head or root node)
 *      node* newNode: pointer pointing to the new node.
 **/
node* addNodeToEnd(node* head, node* newNode){
    if(head=='\0'){
        //newNode is the first node to the list
        head=newNode;
    }
    else{
        node* tmpNode=head;
        while(tmpNode->next!='\0'){
            tmpNode=tmpNode->next;
        }
        tmpNode->next=newNode;
    }
    return head;
}

/**
 * This function add a node to the start of the linked list
 * @parameters
 **/
node* addNodeToStart(node* head,node* newNode){
    if(head=='\0'){
        head=newNode;
    }
    else{
        node* tmpNode=head;
        head=newNode;
        newNode->next=tmpNode;
    }
    return head;
}

/**
 * This function add a new node, after a specified value in the list
 * @parameters
 *      node* head: pointer of type node, pointing to the start of the list
 *      node* newNode: pointer of type node, pointing to the newly created node(i.e., the new node to be inserted)
 *      int value: value after which newNode has to be inserted
 **/
node* addNodeAfterValue(node* head,node* newNode,int value){
    int found=0;
    if(head!='\0'){
        node* tmpNode=head;
        while(tmpNode->next!='\0' && tmpNode->value!=value){
            tmpNode=tmpNode->next;
        }
        if(tmpNode->value==value){
            newNode->next=tmpNode->next;
            tmpNode->next=newNode;
            found=1;
        }
    }
    if(!found)
        printf("cannot find %d in the list\n",value);
    return head;
}

/**
 * This function add a new node, before a specified value in the list.
 * @parameters
 *      node* head: pointer of type node, pointing to the start of the list
 *      node* newNode: pointer of type node, pointing to the newly created node(i.e., the new node to be inserted)
 *      int value: value before which newNode has to be inserted.
 **/ 
node* addNodeBeforeValue(node* head,node* newNode,int value){
    int found=0;
    if(head!='\0'){
        if(head->value==value){
            head=addNodeToStart(head,newNode);
            found=1;
        }
        else{
            node* tmpNode=head;
            while(tmpNode->next!='\0' && tmpNode->next->value!=value){
                tmpNode=tmpNode->next;
            }
            if(tmpNode->next->value==value){
                newNode->next=tmpNode->next;
                tmpNode->next=newNode;
                found=1;
            }
        }
    }
    if(!found)
        printf("cannot find %d in the list\n",value);
    return head;
}

/**
 * This function free up all the memory attached with specified node
 * @parmeters
 *      node* head: root of the list 
 **/
void freeNode(node* head){
    if(head->next=='\0')
        free(head);
    else
        freeNode(head->next);
}

/**
 * This function create a new node from memory pool,
 * assign a value to the value variable in the node and return it's pointer;
 * @parmeters
 *      int value: the value to be assigned to the new node.
 **/
node* getNewNodeWithValue(int value){
    //getting memory from free pool.
    node* newNode=(node*)malloc(sizeof(node));
    //assigning value to the node's value member
    newNode->value=value;
    //setting next pointer to null
    newNode->next='\0';
    return newNode;
}

int main(){
    //initially head points to null ie., list is empty
    node* head='\0';
    node* tmpNode;
    int i=0;
    
    //Adding 5 new nodes to the end of the list
    // result should be 1->2->3->4->5
    for(i=1;i<=5;i++){
        tmpNode=getNewNodeWithValue(i);
        //adding this new node to the end of the list
        head=addNodeToEnd(head,tmpNode);
        //printing the list
        printList(head);
    }
    
    //Adding next 5 values to the start of the list
    // result should be 10->9->8->7->6->1->2->3->4->5
    for(i=6;i<=10;i++){
        tmpNode=getNewNodeWithValue(i);
        //adding this new node to the start of the list
        head=addNodeToStart(head,tmpNode);
         //printing the list
        printList(head);
    }
    
    //Adding 11 after 6
    //result should be 10->9->8->7->6->11->1->2->3->4->5
    tmpNode=getNewNodeWithValue(11);
    head=addNodeAfterValue(head,tmpNode,6);
    printList(head);
    
    //Adding 12 befor 2
    //result should be 10->9->8->7->6->11->1->12->2->3->4->5
    tmpNode=getNewNodeWithValue(12);
    head=addNodeBeforeValue(head,tmpNode,2);
    printList(head);
    
    //free up all the allocated memory
    freeNode(head);
    
    return 0;
}
{% endhighlight %}

So, we have 7 functions excluding `main()` let's have a breif introduction,

*   `printList(node*)` print the current list on console.
*   `addNodeToEnd(node*,node*)` add a node to end of list.
*   `addNodeToStart(node*,node*)` add a new node to the begining of list.
*   `addNodeAfterValue(node*,node*,int)` add a new node after a specific value.
*   `addNodeBeforeValue(node*,node*,int)` add a new node before a specific value.
*   `freeNode(node*)` free up the memory associated with this node and other linked nodes.
*   `getNewNodeWithValue(int)` get a new node with a specific value.

