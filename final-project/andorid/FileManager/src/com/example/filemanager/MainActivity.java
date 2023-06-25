package com.example.filemanager;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;

public class MainActivity extends Activity {
	
	public native boolean passwordExists();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		System.loadLibrary("file-manager");
		
		Button login_btn = (Button) findViewById(R.id.button_login);
		
		login_btn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Log.d("test", "login btn clicked!");
				if (passwordExists()){
					// go to enter_password activity
					Intent intent = new Intent(MainActivity.this, EnterPasswordActivity.class);
					startActivity(intent);
				} else {
					// go to set_password activity
					Intent intent = new Intent(MainActivity.this, SetPasswordActivity.class);
					startActivity(intent);
					finish();
				}
			}
		});
	}
	
}
