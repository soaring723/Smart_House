package com.example.smarthome;
import android.app.Activity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {
	Button kct;
	Button gct;
	Button kws;
	Button gws;
	Button kkt;
	Button gkt;
	Button kys;
	Button gys;
	Button kyc;
	Button gyc;
	Button kqb;
	Button gqb;
	Button kdfs;
	Button gdfs;
	Button kjk;
	Button gjk;
	Button gjms;
	Button ljms;
	Button face;
	
	Handler handler;
	Handler handler1;
	Handler handler2;
	Handler handler3;
	Handler handler4;
	TextView tx;
	TextView tx1;
	TextView tx2;
	TextView tx3;
	TextView tx4;
	
//	private EditText mEditText;
//	 private TextView mTextView;
//	 private StringBuffer stringBuffer;
//	    private String receiveString = "";  
	    
//	private Handler handler = new Handler() {
//        @Override
//        public void handleMessage(Message msg) {
//            super.handleMessage(msg);
//            
//			mTextView.setText(stringBuffer.append(receiveString+"\n"));
//        }
//    };


	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	   NetUtils.clientIint();  //客服端初始化，建立连接
	  // new NetUtils("111").readMessage();  //客服端接收
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		  setContentView(R.layout.activity_main);
        WebView wb = (WebView) findViewById(R.id.web);
        wb.setWebViewClient(new WebViewClient());
        String s="http://172.26.183.9:8080/?action=stream";
        wb.loadUrl(s);
  
       
    	kct=(Button)findViewById(R.id.kct);
    	gct=(Button)findViewById(R.id.gct);
    	kws=(Button)findViewById(R.id.kws);
    	gws=(Button)findViewById(R.id.gws);
    	kkt=(Button)findViewById(R.id.kkt);
    	gkt=(Button)findViewById(R.id.gkt);
    	kys=(Button)findViewById(R.id.kys);
    	gys=(Button)findViewById(R.id.gys);
    	gjms=(Button)findViewById(R.id.gjms);
    	ljms=(Button)findViewById(R.id.ljms);
    	face=(Button)findViewById(R.id.face);
    	
    	Button kyc = (Button)findViewById(R.id.kyc);
    	Button gyc = (Button)findViewById(R.id.gyc);
    	Button kqb = (Button)findViewById(R.id.kqb);
    	Button gqb = (Button)findViewById(R.id.gqb);
    	Button kdfs = (Button)findViewById(R.id.kdfs);
    	Button gdfs = (Button)findViewById(R.id.gdfs);
    	Button khw = (Button)findViewById(R.id.khw);
    	Button ghw = (Button)findViewById(R.id.ghw);
    	Button kjk = (Button)findViewById(R.id.kjk);
    	Button gjk = (Button)findViewById(R.id.gjk);
    	Button ljms = (Button)findViewById(R.id.ljms);
    	Button gjms = (Button)findViewById(R.id.gjms);
    	
    	tx = (TextView) findViewById(R.id.wd);
    	tx1 = (TextView) findViewById(R.id.sd);
    	tx2 = (TextView) findViewById(R.id.fire);
    	tx3 = (TextView) findViewById(R.id.shake);
    	tx4 = (TextView) findViewById(R.id.infrared);
    	//tx.setText("dsfv");
    	 handler = new Handler(){	
     		@Override
     		public void handleMessage(Message msg){
     			// TODO Auto-generated method stub
   			  super.handleMessage(msg);	
   			  Bundle b =  msg.getData();		  
   			 String string = b.getString("msg");
   			 tx.setText(string);
     		}
     	};
     	
     	 handler1 = new Handler(){	
      		@Override
      		public void handleMessage(Message msg1){
      			// TODO Auto-generated method stub
    			  super.handleMessage(msg1);	
    			  Bundle b1 =  msg1.getData();		  
    			 String string1 = b1.getString("msg1");
    			 tx1.setText(string1+" %");
      		}
      	};
     	
      	 handler2 = new Handler(){	
      		@Override
      		public void handleMessage(Message msg2){
      			// TODO Auto-generated method stub
    			  super.handleMessage(msg2);	
    			  Bundle b =  msg2.getData();		  
    			 String string2 = b.getString("msg2");
    			 tx2.setText(string2);
      		}
      	};
      	
      	 handler3 = new Handler(){	
      		@Override
      		public void handleMessage(Message msg3){
      			// TODO Auto-generated method stub
    			  super.handleMessage(msg3);	
    			  Bundle b3 =  msg3.getData();		  
    			 String string3 = b3.getString("msg3");
    			 tx3.setText(string3);
      		}
      	};
      	
      	 handler4 = new Handler(){	
      		@Override
      		public void handleMessage(Message msg4){
      			// TODO Auto-generated method stub
    			  super.handleMessage(msg4);	
    			  Bundle b4 =  msg4.getData();		  
    			 String string4 = b4.getString("msg4");
    			 tx4.setText(string4);
      		}
      	};
      	
     	 NetUtils.readMessage(handler,handler1,handler2,handler3,handler4);
    	//new NetUtils("11").sendMessageHandler(handler);
    	
    	kct.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
			  NetUtils.sendMessages("kct");
			}
		});
    	gct.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("gct");
			}
		});
    	kws.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				 NetUtils.sendMessages("kws");
			}
		});
    	gws.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				 NetUtils.sendMessages("gws");
			}
		});
    	kkt.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				 NetUtils.sendMessages("kkt");
			}
		});
    	gkt.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				 NetUtils.sendMessages("gkt");
			}
		});
    	kys.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("kys");
			}
		});
    	gys.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("gys");
			}
		});
        kyc.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("kyc");
			}
		});
    	gyc.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("gyc");
			}
		});
       kqb.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("kqb");
			}
		});
    	gqb.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("gqb");
			}
		});
        kdfs.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("kfs");
			}
		});
    	gdfs.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("gfs");
			}
		});
        khw.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("khw");
			}
		});
    	ghw.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("ghw");
			}
		});
       kjk.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("kjk");
				 try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				 WebView wb = (WebView) findViewById(R.id.web);
			        wb.setWebViewClient(new WebViewClient());
			        String s="http://172.26.183.9:8080/?action=stream";
			        wb.loadUrl(s);	
			}
			});

    	gjk.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("gjk");
				 
				 try {
						Thread.sleep(200);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				 WebView wb = (WebView) findViewById(R.id.web);
				        wb.setWebViewClient(new WebViewClient());
				        String s="http://172.26.183.9:8080/?action=stream";
				        wb.loadUrl(s);	  
			}
		});
    	
      gjms.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("gjms");
			}
		});
    	ljms.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				NetUtils.sendMessages("ljms");
			}
		});
//    	face.setOnClickListener(new OnClickListener() {
//			
//			@Override
//			public void onClick(View arg0) {
//				NetUtils.sendMessages("face");
//			}
//		});
	
   }
   
}



































   /* void initView() {
    	kd=(Button)findViewById(R.id.kd);
    	gd=(Button)findViewById(R.id.gd);
    	dkds=(Button)findViewById(R.id.dkds);
    	gbds=(Button)findViewById(R.id.gbds);
    	ht=(Button)findViewById(R.id.ht);
    	kj=(Button)findViewById(R.id.kj);
    	kd.setOnTouchListener(new OnTouchListener() {
			
			@Override
			public boolean onTouch(View arg0, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					new NetUtils("3").sendMessage();
					break;
				case MotionEvent.ACTION_UP: {
					new NetUtils("3").sendMessage();
					break;
				}

				}
				return false;
			}
		});
   	gd.setOnTouchListener(new OnTouchListener() {
			
			@Override
			public boolean onTouch(View arg0, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					new NetUtils("4").sendMessage();
					break;
				case MotionEvent.ACTION_UP: {
					new NetUtils("3").sendMessage();
					break;
				}

				}
				return false;
			}
		});
    	dkds.setOnTouchListener(new OnTouchListener() {
			
			@Override
			public boolean onTouch(View arg0, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					new NetUtils("1").sendMessage();
					break;
				case MotionEvent.ACTION_UP: {
					new NetUtils("3").sendMessage();
					break;
				}

				}
				return false;
			}
		});
    	gbds.setOnTouchListener(new OnTouchListener() {
			
			@Override
			public boolean onTouch(View arg0, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					new NetUtils("2").sendMessage();
					break;
				case MotionEvent.ACTION_UP: {
					new NetUtils("3").sendMessage();
					break;
				}

				}
				return false;
			}
		});
    	ht.setOnTouchListener(new OnTouchListener() {
			
			@Override
			public boolean onTouch(View arg0, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					new NetUtils("6").sendMessage();
					break;
				case MotionEvent.ACTION_UP: {
					new NetUtils("3").sendMessage();
					break;
				}

				}
				return false;
			}
		});
    	 
    	kj.setOnTouchListener(new OnTouchListener() {
			
			@Override
			public boolean onTouch(View arg0, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					new NetUtils("5").sendMessage();
					b++;
					break;
				case MotionEvent.ACTION_UP: {
					if(b%2==0)
					{
					kj.setText("关闭电视");
					}
					else
					{kj.setText("打开电视");}
					break;
				}

				}
				return false;
			}
		});
    	
    	
    }*/
    
