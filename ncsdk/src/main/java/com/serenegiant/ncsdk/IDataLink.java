package com.serenegiant.ncsdk;

public interface IDataLink {
	public void release();
	public long getNativePtr();
	public void reset();
}
