LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ssl
LOCAL_SRC_FILES := ../openssl-curl-android/build/openssl/$(TARGET_ARCH_ABI)/lib/libssl.a
LOCAL_EXPORT_CFLAGS := -I$(LOCAL_PATH)/../openssl-curl-android/build/openssl/$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := crypto
LOCAL_SRC_FILES := ../openssl-curl-android/build/openssl/$(TARGET_ARCH_ABI)/lib/libcrypto.a
LOCAL_EXPORT_CFLAGS := -I$(LOCAL_PATH)/../openssl-curl-android/build/openssl/$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := curl
LOCAL_SRC_FILES := ../openssl-curl-android/build/curl/$(TARGET_ARCH_ABI)/lib/libcurl.a
LOCAL_EXPORT_CFLAGS := -I$(LOCAL_PATH)/../openssl-curl-android/build/curl/$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../openssl/include $(LOCAL_PATH)/../curl/include $(LOCAL_PATH)/../cJSON $(LOCAL_PATH)/../sds $(LOCAL_PATH)/../utf8proc
LOCAL_MODULE := tunet
LOCAL_SRC_FILES := tunet-jni.c ../src/tunet.c ../lib/portal.c ../lib/list.c ../lib/utf.c ../utf8proc/utf8proc.c ../sds/sds.c ../cJSON/cJSON.c
LOCAL_STATIC_LIBRARIES := libcurl libssl libcrypto
LOCAL_LDLIBS := -lz
include $(BUILD_SHARED_LIBRARY)
