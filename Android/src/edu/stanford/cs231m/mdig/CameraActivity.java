package edu.stanford.cs231m.mdig;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.JavaCameraView;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
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

    private JavaCameraView mCameraView;

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

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        // Copy necessary resources.
        deployResources();

        // Setup our camera view
        setContentView(R.layout.activity_camera);
        mCameraView = (JavaCameraView) findViewById(R.id.primary_camera_view);
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
        mRequiresInit = true;
        mIsReadyForTracking = true;
        return false;
    }

    @Override
    public void onCameraViewStarted(int width, int height)
    {
        Toast.makeText(this, "Tap the screen to begin tracking.", Toast.LENGTH_LONG).show();
    }

    @Override
    public void onCameraViewStopped()
    {
    }

    @Override
    public Mat onCameraFrame(CvCameraViewFrame inputFrame)
    {
		Mat frame = inputFrame.rgba();
        if (mIsReadyForTracking)
        {
			HandleFrame(mNativeController, frame.getNativeObjAddr(), false);
        }
		return frame;
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

    public native void HandleFrame(long addr_native_controller, long addr_rgba, boolean is_init);

    public native void SetDataLocation(String path);
}
