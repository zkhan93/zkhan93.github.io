---
layout: post
title: "Linked List in C"
date: 2015-11-18 15:24:00
categories: c update
author: "Zeeshan Khan"
tags: c linked list
source_file: /c/linked-list.c
---
First let's get familiar with some basic terminologies used in linked list.

*   A linked list is a type of array in which each elements points to the next element.
*   Each element in the list is called a node
*   The very first node of the list is called the root node or head of the list.
*   Linked list is represented only by the root node i.e., we store only the pointer to the root node.
*   Memory for each node is dynamically allocated using `malloc()` or `calloc()` function from `stdlib.h` header file.
*   Allocated memory should be released after use, using `free()` function.


Now let's look at the code below, although the code is self explanatory we will explain each portion seperately.

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

Function `main()` 
====================
Let's start with the `main()`, we define two node type pointers `head` and `tmpNode`, `head` will point to the root node, let's leave `tmpNode` for now it will be used later.
Initially the root node is `null` , now we are adding 5 new nodes (values 1 to 5) at the end of the list using `addNodeToEnd()`, we are printing the entire list after each operation,
so that we can track the changes in the list, after that we are adding 5 more new nodes (values 6 to 10) at the start of the list using `addNodeToStart()` , again we are printing the entire list after each operation,
at this point we will be having a linked list similar to `10->9->8->7->6->1->2->3->4->5`, now we try to add 3 new nodes using `addNodeAfterValue()`, nodes will be having values 11, 12 and 13, they will be added after 10, 5 and 8 repectively.
at this point our link list will look like `10->11->9->8->13->7->6->1->2->3->4->5->12`, after this we will test `addNodeBeforeValue()` by adding 3 new nodes, nodes will be having values 14, 15 and 16, they will be added before 10, 12 and 13 respectively.

Function `addNodeToEnd()`
=========================
`addNodeToEnd()` take two arguments, first argument is a pointer to root node and second is pointer to new node,
let's look at the definition of `addNodeToEnd()` (44-57) to understand the working, first we check `head` for `null` if so we simply make the new node as the root node by `head=newNode`, else we take a temporary pointer variable `tmpNode` initially pointing to root node,
we move forward by `tmpNode=tmpnode->next` till we reach the end of the list ie., `tmpNode->next=='\0'`, at this point `tmpNode` will be pointing to the last node, now the last node should point to the new node we do this by `tmpNode->next=newNode`, finally we return the root node.


Function `addNodeToStart()`
============================
`addNodeToStart()` takes two arguments, first argument is a pointer to root node and second is pointer to new node,
looking at the definition of `addNodeToEnd()` (63-73) we see that we first check `head` for `null`, if so then we simple make the the new node as the root node by `head=newNode`, otherwise `newNode` should point to root node i.e., `newNode->next=head` now `newNode` can be trated as the root node so `head=newNode`, finally returning the root node,
we can also skip the `null` check part so that our function look like
{% highlight c linenos %}
node* addNodeToStart(node* head, node* newNode){
    newNode->next=head;
    head=newNode;
    return head;
}
{% endhighlight %}
either version of the function will do the job.

Function `addNodeAfterValue()`
===============================
`addNodeAfterValue()` takes three arguments, first argument is a pointer to the root node, second is a pointer to new node and third argument is a value after which new node will be inserted,
looking at the definition (81-97), here again we check `head` for `null`, if so we have nothing to do as the list is empty,
else we take a temporary variable `tmpNode`, now we move forward till we reach at the end of the list i.e.,
`tmpNode->next=='\0'` or find the desired value i.e., `tmpNode->value==value`
(equivalent while loop condition will be `while(tmpNode->next!='\0' && tmpNode->value!=value)`),
after the loop we will check if the current node has the desired value or not because we have two conditions on which the loop terminates,
if the value matches with the desired value then will make the `newNode` to point to whatever is pointed by the current node i.e., `newNode->next=tmpNode->next`,
and then make the current node to point to the new node i.e., `tmpNode->next=newNode`.
We use `found` variable to track that we found the value in list or not depending on which we print a suitable message and finally return the root node;

Function `addNodeBeforeValue()`
===============================
`addNodeBeforeValue()` takes three arguments, first argument is a pointer to the root node, second is the pointer to new node and third argument is a value before which a new node will be inserted,
looking at the definition (107-129), we first check `head` for `null`, 
if so we have nothing to do as the list is empty, 
otherwise we check head's value by `head->value==value` if so then `newNode` must be added before the root/head node, we use our `addNodeToStart()` method for this i.e., `head=addNodeToStart(head,nodeNode)`,
on the other hand if the value of root node does not match with the desired value then we take a temporary variable `tmpNode=head` initially pointing to root node and move forward till we reach the end `tmpNode->next=='\0'` 
or find the desired value in the node next to the current one `tmpnode->next->value==value` we do this to stay behind the node with desired value (equivalent while loop condition will be `while(tmpNode->next!='\0' && tmpnode->next->value!=value)`),
after the loop ends we may be on the last node or before the desired node, so we check the value of next node with value i.e., `tmpNode->next->value==value` if this condition is true then we insert the new node before the current node, 
this is accomplished by first making `newNode` to point whatever `tmpNode` is pointing at `newNode->next=tmpNode->next` then make the current node point to new node i.e., `tmpNode->next=newNode`, 
we also use `found` variable to track whether the desired value exist in the list or not and printing a suitable message respectivly and finally return the root node.
