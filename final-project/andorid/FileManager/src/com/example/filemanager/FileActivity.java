package com.example.filemanager;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class FileActivity extends Activity {
	public native String readFile(String filePath);
	public native boolean editFile(String filePath, String body);
	private EditText editText;
	private String filePath;
	private String mode;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_file);

		Log.d("test", "File Activity!");
        editText = (EditText) findViewById(R.id.fileEditText);
        
        Intent intent = getIntent();
        if (intent != null) {
        	filePath = intent.getStringExtra("filePath");
        	mode = intent.getStringExtra("mode");
        }
        Log.d("test", "textView ok");
	}
	
	@Override
	protected void onStart(){
		super.onStart();
		editText.setText("");
	    String fileContent = readFile(filePath);
	    editText.setBackground(null);
	    if (fileContent != null) {
	    	editText.setText(fileContent);
	    	if (mode.equals("read")) {
	    		editText.setFocusable(false);
	    		editText.setClickable(false);
	    		editText.setLongClickable(false);
	    	}
	    } else {
	      	Log.d("test", "WRONG");
	      	editText.setFocusable(false);
	      	editText.setText("Failed to read file: " + filePath);
	    }
	}
	
	@Override
	public void onStop() {
		super.onStop();
		if(mode.equals("edit")){
			Log.d("test", "Edit mode done. Now save.");
			if (editFile(filePath, editText.getText().toString())){
				Log.d("test", "Save Success");
			} else {
				Log.d("test", "Save Fail");
				Toast.makeText(getApplicationContext(), "Save fail: " + filePath, Toast.LENGTH_LONG).show();
			}
		}
	}
}
