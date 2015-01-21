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
			System.out.println("Disconneted from server");
		}
	}
	
}
