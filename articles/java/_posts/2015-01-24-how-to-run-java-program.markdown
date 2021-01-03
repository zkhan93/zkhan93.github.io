---
layout: post
author: Zeeshan Khan
date: 2015-01-24 11:37:21
tags: update java
categories: java update
title: How To Run a Java Program
---

To run a java program you must have one, so lets have a sample code which prints "Hello world!!" to console.

### Source Code

If you have source code ready then ignore else create a new file `Hello.java` and copy the below code in it and save it.

Source code

```java
// file Hello.java
import java.io.*;
class Hello{
  public static void main(String args[]){
   System.out.println("Hello World!!");
 }
}
```

now you have your source code ready,

###Compile

Open a terminal or command prompt navigate to the directory where you source code resides using command `cd`, now to compile the source code type `javac <Source File Name>` so in our case we will write `javac Hello.java` this will create .class files in the directory.

If there is error in source code you will see error messages, resolve error and repeat the compilation process unless there is no message from compiler.

### Execute

 To execute the program type `java <Class Name>` so in this case we have `java Hello`.

```java
Hello world!!
```
