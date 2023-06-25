package com.example.filemanager;

public class File {
	private String name;
	private String cwd;
	private boolean isDir;
	
	private long size; //bytes
	private boolean R_OTH;
	private boolean W_OTH;
	private boolean X_OTH;
	
	File(){}
	
	File(String name, String cwd, boolean isDir, long size, boolean R_OTH, boolean W_OTH, boolean X_OTH) {
		this.setName(name);
		this.setCwd(cwd);
		this.setIsDir(isDir);
		this.setSize(size);
		this.setR_OTH(R_OTH);
		this.setW_OTH(W_OTH);
		this.setX_OTH(X_OTH);
	}

	public String getAbsolutePath() {
		return (cwd.equals("/")) ? cwd + name : cwd + "/" + name;
	}
	
	public String getName(){
		return name;
	}
	
	public String getCwd() {
		return cwd;
	}
	
	public void setCwd(String cwd) {
		this.cwd = cwd;
	}
	
	public boolean getIsDir(){
		return isDir;
	}
	
	public void setName(String name){
		this.name = name;
	}
	
	public void setIsDir(boolean isDir){
		this.isDir = isDir;
	}

	public boolean isR_OTH() {
		return R_OTH;
	}

	public void setR_OTH(boolean r_OTH) {
		R_OTH = r_OTH;
	}

	public long getSize() {
		return size;
	}

	public void setSize(long size) {
		this.size = size;
	}

	public boolean isW_OTH() {
		return W_OTH;
	}

	public void setW_OTH(boolean w_OTH) {
		W_OTH = w_OTH;
	}

	public boolean isX_OTH() {
		return X_OTH;
	}

	public void setX_OTH(boolean x_OTH) {
		X_OTH = x_OTH;
	}
}
