package com.serenegiant.ncsdk;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import java.lang.ref.WeakReference;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

abstract class NativeObject {

	/** 未初期化 */
	protected static final int STATE_UNINITIALIZED	= 0x00000000;
	/** 初期化中フラグ */
	protected static final int STATE_INITIALIZING	= 0x00000001;
	/** 初期化済フラグ */
	protected static final int STATE_INITIALIZED	= 0x00000010;

	private final WeakReference<Context> mWeakContext;
	private final ReentrantReadWriteLock mSensorLock = new ReentrantReadWriteLock();
	protected final Lock mReadLock = mSensorLock.readLock();
	protected final Lock mWriteLock = mSensorLock.writeLock();
	private int mState = STATE_UNINITIALIZED;
	protected long mNativePtr;
	
	public NativeObject(@NonNull final Context context) {
		setState(STATE_INITIALIZING);
		mWeakContext = new WeakReference<Context>(context);
		mNativePtr = nativeCreate();
		if (mNativePtr == 0) {
			throw new UnsupportedOperationException();
		}
		setState(STATE_INITIALIZED);
	}

	@Override
	protected void finalize() throws Throwable {
		try {
			release();
		} finally {
			super.finalize();
		}
	}

	public void release() {
		if (mNativePtr != 0) {
			nativeDestroy(mNativePtr);
			mNativePtr = 0;
		}
	}

	public long getNativePtr() {
		return mNativePtr;
	}

	@NonNull
	protected Context requireContext() throws IllegalStateException {
		final Context context = mWeakContext.get();
		if (context == null) {
			throw new IllegalStateException("already released?");
		}
		return context;
	}

	@Nullable
	protected Context getContext() {
		return mWeakContext.get();
	}

	/**
	 * 現在のステートを取得
	 * @return
	 */
	protected int state() {
		mReadLock.lock();
		try {
			return mState;
		} finally {
			mReadLock.unlock();
		}
	}

	/**
	 * ステートをセット
	 * @param newState
	 * @return ステートが変化した時はtrue
	 */
	protected boolean setState(final int newState) {
		boolean changed;
		mWriteLock.lock();
		try {
			changed = mState != newState;
			mState = newState;
		} finally {
			mWriteLock.unlock();
		}
		return changed;
	}

	/**
	 * 現在のステートにマスクを適用した後新しいステートを付加する
	 * state = (state & mask) | newState
	 * @param newState
	 * @param mask
	 * @return ステートが変化した時はtrue
	 */
	protected boolean setState(final int newState, final int mask) {
		boolean changed;
		mWriteLock.lock();
		try {
			final int state = mState & mask;
			changed = state != newState;
			if (changed) {
				mState = state | newState;
			}
		} finally {
			mWriteLock.unlock();
		}
		return changed;
	}

	protected abstract long nativeCreate();
	protected abstract void nativeDestroy(final long id);
}
