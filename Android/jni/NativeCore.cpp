#include "NativeCore.hpp"
#include "MDApp.hpp"
#include "ResourceLocator.hpp"
#include <string>

static std::string ConvertString(JNIEnv* env, jstring js)
{
    const char* stringChars = env->GetStringUTFChars(js, 0);
    std::string s = std::string(stringChars);
    env->ReleaseStringUTFChars(js, stringChars);
    return s;
}

JNIEXPORT jlong JNICALL Java_edu_stanford_cs231m_mdig_CameraActivity_CreateNativeController
  (JNIEnv *, jobject)
{
	return (jlong)(new MDApp);
}

JNIEXPORT void JNICALL Java_edu_stanford_cs231m_mdig_CameraActivity_DestroyNativeController
  (JNIEnv *, jobject, jlong addr_native_controller)
{
	delete (MDApp*)(addr_native_controller);
}

JNIEXPORT void JNICALL Java_edu_stanford_cs231m_mdig_CameraActivity_HandleFrame
  (JNIEnv *, jobject, jlong addr_native_controller, jlong addr_rgba, jboolean is_init)
{
	MDApp* app = (MDApp*)(addr_native_controller);
	cv::Mat* frame = (cv::Mat*)(addr_rgba);
	if(is_init)
	{
		app->initialize();
	}
	else
	{
		app->process_frame(*frame);
	}
}

JNIEXPORT void JNICALL Java_edu_stanford_cs231m_mdig_CameraActivity_SetDataLocation
  (JNIEnv* env, jobject, jstring path)
{
	SetResourceBasePath(ConvertString(env, path));
}
