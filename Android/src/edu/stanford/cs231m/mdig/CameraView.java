package edu.stanford.cs231m.mdig;

import java.io.FileOutputStream;

import org.opencv.android.JavaCameraView;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.util.AttributeSet;
import android.util.Log;

public class CameraView extends JavaCameraView {
	public CameraView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	private static final String TAG = "CameraView";

	public void takePicture(final String outputPath) {
		PictureCallback pictureCallback = new PictureCallback() {
			@Override
			public void onPictureTaken(byte[] data, Camera cam) {
				Bitmap picture = BitmapFactory.decodeByteArray(data, 0,
						data.length);
				try {
					float aspectRatio = picture.getHeight()
							/ (float) picture.getWidth();
					int scaledWidth = 640;
					int scaledHeight = (int) (aspectRatio * scaledWidth);
					Bitmap scaled = Bitmap.createScaledBitmap(picture,
							scaledWidth, scaledHeight, true);
					FileOutputStream out = new FileOutputStream(outputPath);
					scaled.compress(Bitmap.CompressFormat.JPEG, 95, out);
					scaled.recycle();
					picture.recycle();
					mCamera.startPreview();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};
		Log.i(TAG, "Taking picture.");
		mCamera.takePicture(null, null, pictureCallback);
	}
}
