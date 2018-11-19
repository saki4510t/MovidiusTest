# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in /Users/saki/android-sdks/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

-keepclasseswithmembernames class * {
    native <methods>;
	*** mNativePtr;
	*** mNativeValue;
	*** mContext;
}

-keep class * implements android.os.Parcelable {
  public static final android.os.Parcelable$Creator *;
}

-keep public class com.serenegiant.uvc.UVCClient {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uac.UACClient {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.UVCCamera {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.media.Encoder {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.media.VideoEncoder {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.media.VideoMuxer {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.Utils {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public interface com.serenegiant.uvc.ICaptureCallback { *; }
-keep public interface com.serenegiant.uvc.IFrameCallback { *; }
-keep public interface com.serenegiant.uvc.IButtonCallback { *; }
-keep public interface com.serenegiant.uvc.IStatusCallback { *; }
-keep public interface com.serenegiant.uvc.IUVCControl { *; }
-keep public interface com.serenegiant.uvc.IUVCControlEx { *; }

-keep public class com.serenegiant.uvc.IPipeline {*;}
-dontnote com.serenegiant.uvc.IPipeline

-keep public class com.serenegiant.uvc.ConvertPipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.DistributePipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.FrameCallbackPipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.FrameSavePipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.NativeCallbackPipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.PreviewPipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.SimpleBufferedPipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.UVCPipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.PipelineSource {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.PtsCalcPipeline {
    native <methods>;
	public *;
	protected *;
	private *;
}

-keep public class com.serenegiant.uvc.FuncMaxMinDef {
	public *;
	protected *;
	private *;
}

##---------------Begin: proguard configuration for Gson  ----------
# Gson uses generic type information stored in a class file when working with fields. Proguard
# removes such information by default, so configure it to keep all of it.
-keepattributes Signature

# For using GSON @Expose annotation
-keepattributes *Annotation*

# Gson specific classes
-keep class sun.misc.Unsafe
-keep class sun.misc.Unsafe.**
-dontnote sun.misc.Unsafe
#-keep class com.google.gson.stream.** { *; }

-keep class com.serenegiant.uvc.Size
-keep class com.serenegiant.uvc.Size { *; }

-keep public class com.serenegiant.aandusb.R
-keep public class com.serenegiant.aandusb.R.**
-keep public class com.serenegiant.aandusb.R.*$*
-keep public class com.serenegiant.aandusb.R.$**
-keep public class com.serenegiant.libaandusb.R
-keep public class com.serenegiant.libaandusb.R.**
-keep public class com.serenegiant.libaandusb.R.*$*
-keep public class com.serenegiant.libaandusb.R.$**
-keep public class com.serenegiant.usb.R
-keep public class com.serenegiant.usb.R.**
-keep public class com.serenegiant.usb.R.*$*
-keep public class com.serenegiant.usb.R.$**
