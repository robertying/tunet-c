#include <jni.h>

#include "../src/tunet.h"

JNIEXPORT jstring JNICALL Java_io_robertying_campusnet_MainActivity_tunetInit(JNIEnv *env,
                                                                              jobject thiz)
{
    tunet_init();
    return (*env)->NewStringUTF(env, "Hello from JNI!");
}
