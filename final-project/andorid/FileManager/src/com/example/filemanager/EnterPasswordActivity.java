package com.example.filemanager;
import com.example.filemanager.PasswordService.PasswordServiceBinder;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;

public class EnterPasswordActivity extends Activity {
	PasswordService passwordService;
	private boolean isService = false;
	
	ServiceConnection conn = new ServiceConnection() {
		public void onServiceConnected(ComponentName name, IBinder service) {
			PasswordServiceBinder pwServiceBinder = (PasswordServiceBinder)service;
			passwordService = pwServiceBinder.getService();
			isService = true;
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			isService = false;
		}
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_enter_password);
		
		Intent intent = new Intent(EnterPasswordActivity.this, PasswordService.class);
		bindService(intent, conn, Context.BIND_AUTO_CREATE);
		
		Button enter_pw_btn = (Button)findViewById(R.id.enter_password_login);
		Button try_login_btn = (Button)findViewById(R.id.try_login);
		isService = true;
		
		enter_pw_btn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				if(!isService) return;	
				passwordService.startEnterPasswordThread();
			}
		});
		
		 try_login_btn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Log.d("test", "save clicked");
				if(!isService) return;
				if(passwordService.tryLogin()){
					unbindService(conn);
					Intent intent = new Intent(EnterPasswordActivity.this, FileListActivity.class);
					intent.putExtra("cwd", "/"); // root directory
					startActivity(intent);
					finish();
				} else {
					// Trial--
					Log.d("test", "Login Failed");
				}
			}
			
		});
	}
}
