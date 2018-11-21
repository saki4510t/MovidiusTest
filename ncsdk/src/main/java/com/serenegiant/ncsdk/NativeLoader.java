package com.serenegiant.ncsdk;

public class NativeLoader {

	private static boolean isLoaded;
	static void loadNative() {
		if (!isLoaded) {
			System.loadLibrary("c++_shared");
			System.loadLibrary("common");
			System.loadLibrary("movidius");
			isLoaded = true;
		}
	}
}
