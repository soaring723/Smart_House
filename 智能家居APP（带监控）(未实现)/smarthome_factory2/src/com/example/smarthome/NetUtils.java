package com.example.smarthome;

import java.io.OutputStream;
import java.net.Socket;

import android.os.Handler;
import android.widget.TextView;


public class NetUtils {

	public String message;
	public String reTurnMes;
	public Handler handler;
	//TextView tx;

	//public void wirteMsg(View v){};
	public NetUtils(String message) {
		this.message = message;

	}

	private static Socket client;

	public NetUtils(String message, Handler handler) {
		this.message = message;
		this.handler = handler;
	}


	// 客服端初始化
	public static void clientIint() {
		new Thread(new Runnable() {
			public void run() {
				try {
					client = new Socket("172.26.183.9", 8083);
					//client = new Socket("192.168.12.1", 8083);
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}).start();

	}


	// 接收服务器数据
	public static void readMessage( final Handler h,final Handler h1,final Handler h2,final Handler h3,final Handler h4) {
		new Thread(new Runnable() {
			public void run() {
				try {
					String str[] = new String[5];
					int len;
					java.io.InputStream in = client.getInputStream();
					while(true){
						
						
						byte[] data = new byte[500];
						len = in.read(data);
//						byte[][] temp = new byte[5][100];
//						temp = data;
//						String str[] = new String[5];
						str[0] = new String(data, 0, 100);
						 str[1] = new String(data, 100, 100);
						 str[2] = new String(data, 200, 100);
						 str[3] = new String(data, 300, 100);
						 str[4] = new String(data, 400, 100);
						
						android.os.Message msg = new android.os.Message();
						android.os.Message msg1 = new android.os.Message();
						android.os.Message msg2 = new android.os.Message();
						android.os.Message msg3 = new android.os.Message();
						android.os.Message msg4 = new android.os.Message();
						
						android.os.Bundle b = new android.os.Bundle();
						android.os.Bundle b1 = new android.os.Bundle();
						android.os.Bundle b2 = new android.os.Bundle();
						android.os.Bundle b3 = new android.os.Bundle();
						android.os.Bundle b4 = new android.os.Bundle();
//						
						b.putString("msg", str[0]);
						b1.putString("msg1", str[1]);
						b2.putString("msg2", str[2]);
						b3.putString("msg3", str[3]);
						b4.putString("msg4", str[4]);
//						
						msg.setData(b);
						msg1.setData(b1);
						msg2.setData(b2);
						msg3.setData(b3);
						msg4.setData(b4);
						
					    h.sendMessage(msg);
					    h1.sendMessage(msg1);
					    h2.sendMessage(msg2);
					    h3.sendMessage(msg3);
					    h4.sendMessage(msg4);
					    
					}

				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}).start();

	}

//	public  void sendMessageHandler( final Handler h)
//	{
//		new Thread(new Runnable() {
//			public void run() {
//				try {
//								
//					int len;
//					java.io.InputStream in = client.getInputStream();
//					byte[] data = new byte[128];
//					while(true){
//						len = in.read(data);
//						String str = new String(data, 0, len);
//						OutputStream out = client.getOutputStream(); // 获得数据发送通道
//						out.write(message.getBytes()); // 发送通道，发送数据
//						android.os.Message msg = new android.os.Message();
//						android.os.Bundle b = new android.os.Bundle();
//						b.putString("msg", str);
//						msg.setData(b);
//						h.handleMessage(msg);
//						
//						//tx = (TextView) findViewById(R.id.wd);
//					}
//					
//
//				} catch (Exception e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
//			}
//
//			
//			
//			
//		}).start();
//	}
	
	// 向服务器发送数据
	public static void sendMessages(final String command) {
		new Thread(new Runnable() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
				try {
					// client = new Socket("192.168.245.9", 8083);
					OutputStream out = client.getOutputStream(); // 获得数据发送通道
					//out.write(message.getBytes()); // 发送通道，发送数据
					out.write(command.getBytes()); // 发送通道，发送数据
					//out.close();
					// client.close();
                
				
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}).start();

	}
}

