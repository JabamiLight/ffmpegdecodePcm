package com.example.ty.ffmpegdecoder;

public class Mp3Decoder {
	// Used to load the 'native-lib' library on application startup.
	static {
		System.loadLibrary("native-lib");
	}


	public native int init(String mp3FilePath, String pcmFilePath);
	
	public native void decode();
	
	public native void destroy();

}
