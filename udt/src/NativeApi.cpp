
#include "sendfile.h"
#include "recvfile.h"

#include <jni.h>
#include "basedef.h"

#define REG_PATH "com/great/happyness/netcodec/NativeCodec"

JavaVM*		g_javaVM		= NULL;


/////////////////////////////////////////////////////Server////////////////////////////////////////////////////////


static jboolean StartSendFile(JNIEnv *env, jobject, jstring destport )
{
	int ret = 0;
	jboolean isOk = JNI_FALSE;
	const char *port = env->GetStringUTFChars(destport, &isOk);

	ret = startFileSend((char*)port);

	env->ReleaseStringUTFChars(destport, port);

	return true;
}

static jboolean StopSendFile(JNIEnv *env, jobject)
{
	stopFileSend();
	return true;
}

static jboolean StartRecvFile(JNIEnv *env, jobject, jstring destip, jstring destport, jstring needFile, jstring saveFile )
{
	int ret = 0;
	jboolean isOk = JNI_FALSE;
	const char *addr = env->GetStringUTFChars(destip, &isOk);
	const char *port = env->GetStringUTFChars(destport, &isOk);
	const char *need = env->GetStringUTFChars(needFile, &isOk);
	const char *save = env->GetStringUTFChars(saveFile, &isOk);

	startFileRecv((char*)addr, (char*)port, (char*)need, (char*)save);

	env->ReleaseStringUTFChars(destip, addr);
	env->ReleaseStringUTFChars(destport, port);
	env->ReleaseStringUTFChars(needFile, need);
	env->ReleaseStringUTFChars(saveFile, save);

	return false;
}

static jboolean StopRecvFile(JNIEnv *env, jobject)
{
	stopFileRecv();
	return true;
}

static JNINativeMethod video_method_table[] = {

		{"StartSendFile", "(Ljava/lang/String;)Z", (void*)StartSendFile },
		{"StopSendFile", "()Z", (void*)StopSendFile },
		{"StartRecvFile", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void*)StartRecvFile },
		{"StopRecvFile", "()Z", (void*)StopRecvFile },
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

	return JNI_VERSION_1_6;
}

