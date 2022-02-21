/**
 * Client program requesting current date from server.
 *
 */ 

import java.net.*;
import java.io.*;

public class DateClient
{
	public static void main(String[] args)  {
		try {
			
			//Create a soccket
			Socket sock = new Socket("130.166.12.6", 6030);
			//Socket created
			InputStream in = sock.getInputStream();
			BufferedReader bin = new BufferedReader(new InputStreamReader(in));

			System.out.println("Fetching Date instance from " + sock.getInetAddress().toString()
																 + ":" + sock.getPort());
			String line;
			while( (line = bin.readLine()) != null)
				System.out.println(line);
				
			//Close the socket
			sock.close();
			//Socket closed
		}
		catch (IOException ioe) {
				System.err.println(ioe);
		}
	}
}
