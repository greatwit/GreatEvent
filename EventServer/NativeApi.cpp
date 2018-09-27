

#include "spserver.hpp"

#include <jni.h>
#include "common.h"

#define REG_PATH "com/great/happyness/mediacodecmy/NativeCodec"

JavaVM*		g_javaVM		= NULL;
SP_Server	*mpServer		= NULL;


/////////////////////////////////////////////////////Server////////////////////////////////////////////////////////

static jboolean StartServer(JNIEnv *env, jobject, jstring destip, jint destport)
{
	if(mpServer==NULL){
		mpServer = new SP_Server("", destport);
		mpServer->setTimeout( 60 );
		mpServer->setMaxThreads( 10 );
		mpServer->runForever();
	}
	return true;
}

static jboolean StopServer(JNIEnv *env, jobject)
{
	if(mpServer!=NULL){
		mpServer->shutdown();
		delete mpServer;
		mpServer = NULL;
	}
	return true;
}


static JNINativeMethod video_method_table[] = {

		{"StartServer", "(Ljava/lang/String;I)Z", (void*)StartServer },
		{"StopServer", "()Z", (void*)StopServer },
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

