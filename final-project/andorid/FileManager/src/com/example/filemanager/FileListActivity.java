package com.example.filemanager;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.example.filemanager.PasswordService.BackThread;

public class FileListActivity extends Activity {
	public static int SESSION_SEC = 180;
	
	/* native functions */
	public native ArrayList<File> fetchFiles(String cwd);
	public native boolean canOpenDir(String path);
	public native boolean deleteFile(String filePath);
	public native boolean startSession(int sec);
	
	private LinearLayout verticalLayout;
	private String cwd;
	private ArrayList<File> files;
	private SessionThread thread;
	
	/* inner class */
	class SessionThread extends Thread {
		public void run() {
			Log.d("test", "Session Thread start");
			 if(startSession(SESSION_SEC)){
				 /*TODO : kill the app */
				 Toast.makeText(getApplicationContext(), "Session 만료", Toast.LENGTH_LONG).show();
				 Intent intent = new Intent(FileListActivity.this, MainActivity.class);
                 intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
                 startActivity(intent);
                 finish();
			 }
		}
	}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_list);

        Intent intent = getIntent();
        if (intent != null) {
        	cwd = intent.getStringExtra("cwd");
        	Log.d("test", "intent not null: " + cwd);
        } else {
        	cwd = "/"; //root directory
        }
        
        /* If you first enter in root directory, start session */
        if(cwd.equals("/")){
        	thread = new SessionThread();
        	thread.setDaemon(true);
        	thread.start();
        }
    }
    
    @Override
    protected void onStart(){
    	super.onStart();
	    	verticalLayout = (LinearLayout)findViewById(R.id.verticalLayout);
	    	verticalLayout.removeAllViews();
	        List<File> files = fetchFiles(cwd);
	        for (File file : files) {
	        	addFileComponent(file);
	        }
    }
       
    private void addFileComponent(final File file) {
    	LinearLayout fileLayout = new LinearLayout(this);
    	fileLayout.setOrientation(LinearLayout.HORIZONTAL);
    	fileLayout.setPadding(8, 8, 8, 8);
    	
    	/* file name */
    	Button titleButton = new Button(this);
    	titleButton.setText(file.getName());
    	titleButton.setBackgroundColor(Color.TRANSPARENT);
        LinearLayout.LayoutParams titleButtonParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 0.8f);
        fileLayout.addView(titleButton, titleButtonParams);

        titleButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                	// 파일명 버튼 클릭 시 수행할 동작
            	if(file.getIsDir()) {
            		// move directory
            		if (canOpenDir(file.getAbsolutePath())) {
            			// can open
            			Intent intent = new Intent(FileListActivity.this, FileListActivity.class);
            			intent.putExtra("cwd", file.getAbsolutePath());
            			startActivity(intent);            			
            		} else {
            			// permission denied
            			Toast.makeText(getApplicationContext(), "Permission denied: " + file.getName(), Toast.LENGTH_LONG).show();
            		}
            	} else {
            		// cat file
            		Intent intent = new Intent(FileListActivity.this, FileActivity.class);
        			intent.putExtra("filePath", file.getAbsolutePath());
        			intent.putExtra("mode", "read");
        			startActivity(intent);  
            	}
            }
        });
        
        if(!file.getIsDir()) {
        	/* edit button */
        	Button editButton = new Button(this);
        	editButton.setText("edit");
        	LinearLayout.LayoutParams editButtonParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 0.1f);
        	fileLayout.addView(editButton, editButtonParams);
        	
        	/* delete button */
        	Button deleteButton = new Button(this);
        	deleteButton.setText("delete");
        	LinearLayout.LayoutParams deleteButtonParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 0.1f);
        	fileLayout.addView(deleteButton, deleteButtonParams);
        	
        	editButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    	// 수정 버튼 클릭 시 수행할 동작
                	Intent intent = new Intent(FileListActivity.this, FileActivity.class);
        			intent.putExtra("filePath", file.getAbsolutePath());
        			intent.putExtra("mode", "edit");
        			startActivity(intent);  
                }
            });
        	
        	deleteButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    	// 삭제 버튼 클릭 시 수행할 동작
                	/* TODO */
                	AlertDialog.Builder builder = new AlertDialog.Builder(FileListActivity.this);
                    builder.setTitle(file.getName() + " 삭제");
                    builder.setMessage("정말 파일을 삭제하시겠습니까?");
                    builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            // 파일 삭제 동작 수행
                            boolean deleted = deleteFile(file.getAbsolutePath());
                            if (deleted) {
                                Log.d("test", "파일이 삭제되었습니다.");
                                Toast.makeText(getApplicationContext(), "파일이 삭제되었습니다", Toast.LENGTH_LONG).show();
                            } else {
                                Log.d("test", "파일 삭제에 실패하였습니다.");
                                Toast.makeText(getApplicationContext(), "파일 삭제에 실패하였습니다", Toast.LENGTH_LONG).show();
                            }
                            dialog.dismiss();
                        }
                    });
                    builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                        }
                    });
                    
                    AlertDialog dialog = builder.create();
                    dialog.show();
                }
            });
        }
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        verticalLayout.addView(fileLayout, layoutParams);
        
        /* border line */
        View borderView = new View(this);
        LinearLayout.LayoutParams borderParams = new LinearLayout.LayoutParams(
        		LinearLayout.LayoutParams.MATCH_PARENT,
                	1 // 테두리 선의 높이
        );
        borderView.setBackgroundColor(Color.BLACK); // 테두리 색상
        verticalLayout.addView(borderView, borderParams);
    }
}
