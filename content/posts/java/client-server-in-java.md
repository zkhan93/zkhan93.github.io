---
author: Zeeshan Khan
date: 2015-01-21 00:42:53
tags: [Java, Networking]
categories: [programming]
title: Client Server in Java
excerpt_separator: <!--more-->
---

The following program demonstrate how we can implement simple client server architecture in java
Start `Server.java` in a terminal/command prompt then open another command prompt/terminal and start `Client.java` in it
Server can handle new Clients on disconnection of previous one.

<!--more-->

Code for Server

```java
// file Server.java
import java.io.*;
import java.net.*;
class Server{
	static ServerSocket ss;
	Socket cs;
	static int PORT=12345;
	static String EXIT_STRING="terminate";
	static String SERVER_PREFIX="Eco msg: ";
	BufferedReader reader;
	PrintWriter writer;
	static boolean connected;
	public static void main(String args[]){
		Server mServer=new Server();
		try{
			ss=new ServerSocket(PORT);
			while(true){
				mServer.getClient();
				mServer.echoToClient();
			}
		}catch(Exception e){
				e.printStackTrace();
		}
	}
	public void getClient() throws Exception{
		System.out.println("Server waiting for Client..");
		cs=ss.accept();
		reader=new BufferedReader(new InputStreamReader(cs.getInputStream()));
		writer=new PrintWriter(cs.getOutputStream(),true);
		System.out.println("Client Connected and configured.");
	}
	public void echoToClient() throws Exception{
		String msg;
		while(!((msg=reader.readLine()).equalsIgnoreCase(EXIT_STRING))){
			writer.println(msg);
			System.out.println(SERVER_PREFIX+msg);
		}
		writer.println(msg);
		System.out.println("Client quits..");
	}
}

```

Code for client

```java
// file Client.java
import java.io.*;
import java.net.*;
class Client{
	Socket s;
	static int PORT=12345;
	static String EXIT_STRING="terminate";
	static String CLIENT_PREFIX="Server said: ";
	static BufferedReader user_reader;
	static BufferedReader server_reader;
	static PrintWriter writer;
	static boolean terminate;
	public static void main(String args[]) throws IOException{
		Client mClient=new Client();
		user_reader=new BufferedReader(new InputStreamReader(System.in));
		if(mClient.connect()){
			System.out.println("Connected to Server");
			//new Thread(new ListenToUser()).start();
			new Thread(new ListenToServer()).start();
			mClient.ListenToUser();
		}
	}
	public boolean connect(){
		try{
			s=new Socket("127.0.0.1",PORT);
			server_reader=new BufferedReader(new InputStreamReader(s.getInputStream()));
			writer=new PrintWriter(s.getOutputStream(),true);
			return true;
		}catch(Exception e){
			e.printStackTrace();
			return false;
		}
	}
	public void ListenToUser(){				
			String msg;
			try{
				do{
					msg=user_reader.readLine();
					writer.println(msg);
				}while(!msg.equalsIgnoreCase(EXIT_STRING));
				terminate=true;
			}catch(Exception e){
					e.printStackTrace();
			}
	}

	static class ListenToServer implements Runnable{
		@Override
		public void run(){
			String msg;
			try{
				while(!terminate){
					msg=server_reader.readLine();
					if(msg.equalsIgnoreCase(EXIT_STRING))
						break;
					else
						System.out.println(CLIENT_PREFIX+msg);
				}
			}catch(Exception e){
				e.printStackTrace();
			}
			System.out.println("Disconnected from server");
		}
	}

}

```

Client output

```java
Connected to Server
Hi Server
Server said: Hi Server
you just eco what I say        
Server said: you just eco what I say
okay bye
Server said: okay bye
terminate
Disconnected from server
```

Server output

```java
Server waiting for Client..
Client Connected and configured.
Eco msg: Hi Server
Eco msg: you just eco what I say
Eco msg: okay bye
Client quits..
Server waiting for Client..
```
