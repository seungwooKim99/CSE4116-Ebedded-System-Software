package com.example.filemanager;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class PasswordService extends Service {
	/* native methods */
	public native int enterAndGetPassword();
	public native boolean savePassword();
	public native boolean comparePassword();
	
	private int curr_password = 0;
	private boolean isPWFixed = false;
	private boolean isThreadRunning = false;
	IBinder pwServiceBinder = new PasswordServiceBinder();
	BackThread bThread;
	
	/* inner class */
	class PasswordServiceBinder extends Binder {
		PasswordService getService() {
			Log.d("test", "getService");
			return PasswordService.this;
		}
	}
	class BackThread extends Thread {
		public void run() {
			Log.d("test", "Thread Start!");
			curr_password = enterAndGetPassword();
			isPWFixed = true;
			isThreadRunning = false;
		}
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		Log.d("test", "binder");
		return pwServiceBinder;
	}
	
	void startEnterPasswordThread(){
		if(!isThreadRunning){
			isPWFixed = false; //refresh
			curr_password = 0;
			bThread = new BackThread();
			bThread.setDaemon(true);
			bThread.start();
			isThreadRunning = true;
		}
		return;
	}
	
	boolean trySavePassword() {
		Log.d("test", "save password called");
		if(!isPWFixed){
			Toast.makeText(getApplicationContext(), "Enter and Fix your password first", Toast.LENGTH_LONG).show();
			Log.d("test", "Save Password Not ready");
			return false;
		}
		if(!savePassword()){
			Log.d("test", "Save Password Error");
			Toast.makeText(getApplicationContext(), "Something went wrong", Toast.LENGTH_LONG).show();
			return false;
		}
		Log.d("test", "Save Password OK : " + curr_password);
		return true;
	}
	
	boolean tryLogin() {
		if(!isPWFixed){
			Toast.makeText(getApplicationContext(), "Enter and Fix your password first", Toast.LENGTH_LONG).show();
			Log.d("test", "Login Not ready");
			return false;
		}
		if(!comparePassword()){
			Log.d("test", "Login Error");
			Toast.makeText(getApplicationContext(), "Invalid Password", Toast.LENGTH_LONG).show();
			return false;
		}
		return true;
	}
	
	int getCurrPassword(){
		return curr_password;
	}
	
	public int onStartCommand(Intent intent, int flags, int startId) {
		return super.onStartCommand(intent, flags, startId);
	}
	
	@Override
	public void onCreate() {
		super.onCreate();
	}
	
	@Override
	public void onDestroy() {
		super.onDestroy();
	}
	
}
