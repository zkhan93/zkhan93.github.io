/**
 * Doubly linked list 
 * @author  Zeeshan Khan
 **/
#include<stdio.h>
#include<stdlib.h>
/**
 * Stucture of node is doubly linked list
 * @members:
 *      int value: integer type variable to hold some data.
 *      struct node* next: pointer to a node structure, this will hold a reference to next node in the list.
 *      struct node* previous: pointer to a node structure, this will hold a reference to prevoius node in the list.
 **/
typedef struct node{
    int value;
    struct node* next;
    struct node* previous;
}node;

/**
 * This function creates a new node from memory, assign a new value to it and return a pointer to this new node
 * @parameters:
 *      int value: value to be assigned to newly created node
 * @return
 *      pointer pointing to newly created node
 **/ 
node* getNodeWithvalue(int value){
    node* newNode=(node*)malloc(sizeof(node));
    newNode->value=value;
    newNode->next='\0';
    newNode->previous='\0';
    return newNode;
}

/**
 * adds a node to the begining of the list
 * @parameters
 *      node* head: pointer pointing to the start of the list.
 *      node* newNode: pointer to a new node that is to be added in the list.
 * @return
 *      pointer pointing to the start of the list i.e., root or head
 **/
node* addNodeToStart(node* head,node* newNode){
    if(head=='\0'){
        //list is empty
        head=newNode;
    }else{
        head->previous=newNode;
        newNode->next=head;
        head=newNode;
    }
    return head;
}

/**
 * adds a node to the end of the list
 * @parameters
 *      node* head: pointer pointing to the start of the list.
 *      node* newNode: pointer to a new node that is to be added in the list.
 * @return
 *      pointer pointing to the start of the list i.e., root or head
 **/
node* addNodeToEnd(node* head,node* newNode){
    if(head=='\0'){
        head=newNode;
    }else{
        node* tmpNode=head;
        while(tmpNode->next!='\0'){
            tmpNode=tmpNode->next;
        }
        newNode->previous=tmpNode;
        tmpNode->next=newNode;
    }
    return head;
}

/**
 * adds a new node after a specified value in the list if the specified value is in the list,
 * else prints a error message.
 * @parameters
 *      node* head: pointer pointing to the start of the list.
 *      node* newNode: pointer to a new node that is to be added in the list.
 *      int value: value after which new node has to be inserted
 * @return
 *      pointer pointing to the start of the list i.e., root or head
 **/ 
node* addNodeAfterValue(node* head,node* newNode,int value){
    if(head!='\0'){
        node* tmpNode=head;
        while(tmpNode->next!='\0' && tmpNode->value!=value){
            tmpNode=tmpNode->next;
        }
        if(tmpNode->value==value){
            newNode->next=tmpNode->next;
            tmpNode->next=newNode;
            if(newNode->next!='\0')
                newNode->next->previous=newNode;
            newNode->previous=tmpNode;
        }else{
            printf("cannot find %d in list",value);
        }
    }else{
        printf("List is empty\n");
    } 
    return head;
}

/**
 * adds a new node before a specified value in the list if the specified value is in the list,
 * else prints a error message.
 * @parameters
 *      node* head: pointer pointing to the start of the list.
 *      node* newNode: pointer to a new node that is to be added in the list.
 *      int value: value before which new node has to be inserted
 * @return
 *      pointer pointing to the start of the list i.e., root or head
 **/ 
node* addNodeBeforeValue(node* head,node* newNode,int value){
    if(head!='\0'){
        node* tmpNode=head;
        int found=0;
        if(tmpNode->value==value){
            newNode->next=tmpNode;
            tmpNode->previous=newNode;
            head=newNode;
            found=1;
        }else{
            while(tmpNode->next!='\0' && tmpNode->next->value!=value){
                tmpNode=tmpNode->next;
            }
            if(tmpNode->next!='\0' && tmpNode->next->value==value){
                newNode->next=tmpNode->next;
                tmpNode->next=newNode;
                if(newNode->next!='\0')
                    newNode->next->previous=newNode;
                newNode->previous=tmpNode;
                found=1;
            }
        }
        if(!found)
            printf("cannot find %d in list\n",value);
    }else{
        printf("List is empty\n");
    }
    return head;
}
/**
 * print the list starting with the provided node.
 * @parametes
 *      node* head: pointer of a node in the list,
 *                  may point to list head or any intermediate node in the list.
 **/
void printList(node* head){
    if(head=='\0')
        printf("Empty List\n");
    else{
        node* tmpNode=head;
        printf("forward traversal: ");
        while(tmpNode->next!='\0'){
            printf("%d",tmpNode->value);
            tmpNode=tmpNode->next;
            printf("->");
        }
        printf("%d",tmpNode->value);
        printf("\nReverse traversal: ");
        while(tmpNode!='\0'){
            printf("%d",tmpNode->value);
            tmpNode=tmpNode->previous;
            if(tmpNode!='\0')
                printf("<-");
        }
        printf("\n");
    }
}

void freeList(node* head){
    if(head->next!='\0'){
        freeList(head->next);
    }
    free(head);
}

int main(){
    node* head='\0';
    node* tmpNode='\0';
    int i=0;
    //adding 5 nodes to the end of list.
    for(i=1;i<=5;i++){
        //get a new node
        tmpNode=getNodeWithvalue(i);
        //adding new node to end of the list
        head=addNodeToEnd(head,tmpNode);
        //print the list
        printList(head);
    }
    //adding 5 nodes to the start of list.
    for(i=6;i<=10;i++){
        //get a new node
        tmpNode=getNodeWithvalue(i);
        //adding new node to the start of list
        head=addNodeToStart(head,tmpNode);
        //print the list
        printList(head);
    }
    //adding 11 after 5
    tmpNode=getNodeWithvalue(11);
    head=addNodeAfterValue(head,tmpNode,5);
    printList(head);
    //adding 12 after 10
    tmpNode=getNodeWithvalue(12);
    head=addNodeAfterValue(head,tmpNode,10);
    printList(head);
    //adding 13 after 1
    tmpNode=getNodeWithvalue(13);
    head=addNodeAfterValue(head,tmpNode,1);
    printList(head);
    
    
    //adding 14 before 10
    tmpNode=getNodeWithvalue(14);
    head=addNodeBeforeValue(head,tmpNode,10);
    printList(head);
    //adding 15 before 11
    tmpNode=getNodeWithvalue(15);
    head=addNodeBeforeValue(head,tmpNode,11);
    printList(head);
    //adding 16 before 1
    tmpNode=getNodeWithvalue(16);
    head=addNodeBeforeValue(head,tmpNode,1);
    printList(head);
    
    freeList(head);
    
}
