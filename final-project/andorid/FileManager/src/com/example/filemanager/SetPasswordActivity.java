package com.example.filemanager;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import com.example.filemanager.PasswordService.PasswordServiceBinder;

public class SetPasswordActivity extends Activity {
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
		Log.d("test", "SetPasswordActivity onCreate");
		setContentView(R.layout.activity_set_password);
		
		Intent intent = new Intent(SetPasswordActivity.this, PasswordService.class);
		bindService(intent, conn, Context.BIND_AUTO_CREATE);
		
		Button enter_pw_btn = (Button)findViewById(R.id.enter_password);
		Button save_pw_btn = (Button)findViewById(R.id.save_password);
		isService = true;
		
		enter_pw_btn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				if(!isService) return;	
				passwordService.startEnterPasswordThread();
			}
		});
		
		save_pw_btn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Log.d("test", "save clicked");
				if(!isService) return;
				if(passwordService.trySavePassword()){
					unbindService(conn);
					Intent intent = new Intent(SetPasswordActivity.this, MainActivity.class);
					startActivity(intent);
					finish();
				}
			}
			
		});
	}
}
