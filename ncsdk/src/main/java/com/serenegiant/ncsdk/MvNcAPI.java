package com.serenegiant.ncsdk;

import android.content.Context;
import android.support.annotation.NonNull;
import android.util.Log;

import com.serenegiant.utils.ReentrantReadWriteList;

public class MvNcAPI extends NativeObject {
	private static final boolean DEBUG = true; // set false on production
	private static final String TAG = MvNcAPI.class.getSimpleName();

	private final ReentrantReadWriteList<IDataLink>
		mDataLinks = new ReentrantReadWriteList<>();

	public MvNcAPI(@NonNull final Context context) {
		super(context);
	}
	
	public void add(@NonNull final IDataLink dataLink)
		throws IllegalArgumentException, IllegalStateException {

		if (DEBUG) Log.v(TAG, "add:" + dataLink);
		if (!mDataLinks.contains(dataLink)) {
			final long id_datalink = dataLink.getNativePtr();
			if ((mNativePtr != 0) && (id_datalink != 0)) {
				final int result = nativeAddDataLink(mNativePtr, id_datalink);
				if (result != 0) {
					throw new IllegalArgumentException("failed to add datalink");
				}
				mDataLinks.add(dataLink);
			} else {
				throw new IllegalStateException("datalink or this object is already released");
			}
		} else {
			Log.w(TAG, "datalink is already added");
		}
	}

	public void remove(@NonNull final IDataLink dataLink) {
		if (DEBUG) Log.v(TAG, "remove:" + dataLink);
		mDataLinks.remove(dataLink);
		final long id_datalink = dataLink.getNativePtr();
		if ((mNativePtr != 0) && (id_datalink != 0)) {
			final int result = nativeRemoveDataLink(mNativePtr, id_datalink);
			if (result != 0) {
				Log.w(TAG, "failed to add datalink");
			}
		} else {
			Log.w(TAG, "datalink or this object is already released");
		}
	}

	@Override
	protected native long nativeCreate();
	@Override
	protected native void nativeDestroy(final long id);
	
	private native int nativeAddDataLink(final long id_api, final long id_datalink);
	private native int nativeRemoveDataLink(final long id_api, final long id_datalink);
}
