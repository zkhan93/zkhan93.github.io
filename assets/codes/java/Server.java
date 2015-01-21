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
