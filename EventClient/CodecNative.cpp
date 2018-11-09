#include <jni.h>
#include "basedef.h"
#include "TcpSender.h"


#define REG_PATH "com/great/happyness/mediacodecmy/NativeCodec"

JavaVM*		g_javaVM		= NULL;
TcpSender	*mpTcpSender	= NULL;


/////////////////////////////////////////////////////TcpSender////////////////////////////////////////////////////////

static jboolean TcpConnect(JNIEnv *env, jobject, jstring destip, jint destport)
{
	jboolean bRet = JNI_FALSE;

	if(NULL == mpTcpSender) {
		mpTcpSender = new TcpSender();
	}

	const char *ip = env->GetStringUTFChars(destip, NULL);

	bRet = mpTcpSender->connect(ip, destport);

	env->ReleaseStringUTFChars(destip, ip);

	return bRet;
}

static jboolean TcpDisconnect(JNIEnv *env, jobject) {
	bool bRes = false;
	if(mpTcpSender) {
		bRes = mpTcpSender->disConnect();
		SAFE_DELETE(mpTcpSender);
	}
	return bRes;
}

static jboolean TcpStartFileSender(JNIEnv *env, jobject, jstring sfilename)
{
	 bool bRes = false;

	 jboolean isCopy = JNI_FALSE;
	 const char *fn = env->GetStringUTFChars(sfilename, NULL);
	 GLOGV("Enter:AudioConnectDest->strIP:%s,isCopy:%c",fn, isCopy);

	 mpTcpSender->startFileSend((char*)fn);

	 env->ReleaseStringUTFChars(sfilename, fn);

	 return bRes;
}

static jboolean TcpStopFileSender(JNIEnv *env, jobject){
	return mpTcpSender->stopFileSend();
}

static JNINativeMethod video_method_table[] = {

		{"TcpConnect", "(Ljava/lang/String;I)Z", (void*)TcpConnect },
		{"TcpDisconnect", "()Z", (void*)TcpDisconnect },
		{"TcpStartFileSender", "(Ljava/lang/String;)Z", (void*)TcpStartFileSender },
		{"TcpStopFileSender", "()Z", (void*)TcpStopFileSender },
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


