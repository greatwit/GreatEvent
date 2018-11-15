#include <jni.h>
#include "basedef.h"

#include "FileDeCodec.h"

//#include <android/native_window.h>
#include <android/native_window_jni.h>

#define REG_PATH "com/great/happyness/mediacodecmy/NativeCodec"

JavaVM*		g_javaVM		= NULL;
FileDeCodec *mFileDec		= NULL;

/////////////////////////////////////////////////////decodec////////////////////////////////////////////////////////

static jboolean StartFileDecodec(JNIEnv *env, jobject, jobject surface) {
	bool bRes = false;
	ANativeWindow *pAnw = ANativeWindow_fromSurface(env, surface);
	mFileDec = new FileDeCodec();
	mFileDec->StartVideo(pAnw);

	return bRes;
}

static jboolean StopFileDecodec(JNIEnv *env, jobject)
{
	 bool bRes = false;
	 if(mFileDec){
		 mFileDec->StopVideo();
		 delete mFileDec;
		 mFileDec = NULL;
		 return true;
	 }

	 return bRes;
}

static JNINativeMethod video_method_table[] = {
		{"StartFileDecodec", "(Landroid/view/Surface;)Z", (void*)StartFileDecodec },
		{"StopFileDecodec", "()Z", (void*)StopFileDecodec },
};

int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* methods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        GLOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
        GLOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		GLOGE("GetEnv failed!");
		return result;
	}

	g_javaVM = vm;

	GLOGW("JNI_OnLoad......");
	registerNativeMethods(env,
			REG_PATH, video_method_table,
			NELEM(video_method_table));

	return JNI_VERSION_1_4;
}


