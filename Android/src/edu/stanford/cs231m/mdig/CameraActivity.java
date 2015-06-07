package edu.stanford.cs231m.mdig;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.Toast;

public class CameraActivity extends Activity implements CvCameraViewListener2, OnTouchListener
{

    private static final String TAG = "CameraActivity";

    private static final String DEPLOY_RESOURCES_DIR_NAME = "deploy";

	private CameraView mCameraView;
	private String mImagePaths;

    private long mNativeController = 0;

    private boolean mIsReadyForTracking = false;

    private boolean mRequiresInit = false;

    private void deployResources()
    {
        AssetManager assetManager = this.getAssets();
        try
        {
            String[] files = assetManager.list(DEPLOY_RESOURCES_DIR_NAME);
            String deployTargetDir = this.getFilesDir().getAbsolutePath();            
            for(String filename: files)
            {
                copyAsset(DEPLOY_RESOURCES_DIR_NAME+"/"+filename, deployTargetDir+"/"+filename);
                        
            }
            SetDataLocation(deployTargetDir);
        } catch (IOException e)
        {
            Log.e(TAG, "Failed to deploy necessary assets!");
            e.printStackTrace();
        }
    }

    private void copyAsset(String srcPath, String dstPath)
    {
        AssetManager assetManager = this.getAssets();
        InputStream in = null;
        OutputStream out = null;
        byte[] buffer = new byte[1024];
        try
        {
            in = assetManager.open(srcPath);
            new File(dstPath).createNewFile();
            out = new FileOutputStream(dstPath);
            int read;
            while ((read = in.read(buffer)) != -1)
            {
                out.write(buffer, 0, read);
            }
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
        } catch (IOException e)
        {
            Log.e(TAG, "Failed to copy asset!");
            e.printStackTrace();
        }
    }

	static private File getPictureStorageDir() {
		File picDir = new File(Environment.getExternalStorageDirectory(),
				"MDig-Pictures");
		if (!picDir.exists()) {
			try {
				picDir.mkdir();
			} catch (Exception e) {
				Log.e(TAG, "Encountered exception while creating picture dir.");
				e.printStackTrace();
			}
		}
		return picDir;
	}

	static private File makeUniqueImagePath() {
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss.SSS",
				Locale.US);
		File picDir = getPictureStorageDir();
		String imageName = sdf.format(new Date()) + ".jpg";
		return new File(picDir, imageName);
	}

	private void takePicture() {
		File imageFile = makeUniqueImagePath();
		mCameraView.takePicture(imageFile.getPath());
		Toast.makeText(this, "Picture saved as " + imageFile.getName(),
				Toast.LENGTH_SHORT).show();
		mImagePaths = imageFile.getPath();
	}

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        // Copy necessary resources.
        deployResources();

        // Setup our camera view
        setContentView(R.layout.activity_camera);
		mCameraView = (CameraView) findViewById(R.id.primary_camera_view);
        mCameraView.setVisibility(SurfaceView.VISIBLE);
        mCameraView.setCvCameraViewListener(this);
        mCameraView.setMaxFrameSize(640, 480);
        // mCameraView.enableFpsMeter();

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        if (mNativeController == 0)
        {
            mNativeController = CreateNativeController();
        }
    }

    @Override
    public void onResume()
    {
        super.onResume();
        mNativeController = CreateNativeController();
        mCameraView.enableView();
        mCameraView.setOnTouchListener(CameraActivity.this);
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if (mCameraView != null)
        {
            mCameraView.disableView();
        }
    }

    public void onDestroy()
    {
        super.onDestroy();
        if (mCameraView != null)
        {
            mCameraView.disableView();
        }
        if (mNativeController != 0)
        {
            DestroyNativeController(mNativeController);
        }
    }

    @Override
    public boolean onTouch(View view, MotionEvent event)
    {
		takePicture();

		// mCameraView.disableView();
		// File picDir = getPictureStorageDir();
		// File picFile = new File(picDir, "test.jpg");
		// mImagePaths = picFile.getPath();
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Continue with this pictures?").setTitle(
				"Extract numbers");

		final AlertDialog.Builder resultbuilder = new AlertDialog.Builder(this);
		resultbuilder.setTitle("Extracted results");
		builder.setPositiveButton("Continue",
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						Mat frame = org.opencv.highgui.Highgui
								.imread(mImagePaths);
						String[] numbers = HandleFrame(mNativeController,
								frame.getNativeObjAddr(), mRequiresInit);
						for (int i = 0; i < 5; ++i)
							HandleFrame(mNativeController,
									frame.getNativeObjAddr(), mRequiresInit);

						StringBuilder sb = new StringBuilder(String.format(
								"Find %d number:\n", numbers.length));
						for (String s : numbers)
							sb.append(s+"\n");
						
						resultbuilder.setMessage(sb);
						AlertDialog resultDialog = resultbuilder.create();
						resultDialog.show();

						// mCameraView.enableView();
					}
				});
		builder.setNegativeButton("Restart",
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						File imgFile = new File(mImagePaths);
						imgFile.delete();
					}
				});
		AlertDialog dialog = builder.create();
		dialog.show();
        return false;
    }

    @Override
    public void onCameraViewStarted(int width, int height)
    {
		Toast.makeText(this, "Tap the screen to take a picture.",
				Toast.LENGTH_LONG).show();
    }

    @Override
    public void onCameraViewStopped()
    {
    }

    @Override
    public Mat onCameraFrame(CvCameraViewFrame inputFrame)
    {
		return inputFrame.rgba();
    }

    static
    {
        if (!OpenCVLoader.initDebug())
        {
            Log.e(TAG, "Failed to initialize OpenCV!");
        }
		System.loadLibrary("MDig");
    }

    public native long CreateNativeController();

    public native void DestroyNativeController(long addr_native_controller);

	public native String[] HandleFrame(long addr_native_controller,
			long addr_rgba, boolean is_init);

    public native void SetDataLocation(String path);
}
