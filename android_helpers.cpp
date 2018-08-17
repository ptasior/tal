#include "android_helpers.h"
#include "android/ndk/android_native_app_glue.h"
#include "SDL.h"
#include <jni.h>


std::string androidTempFolder()
{
    JNIEnv *env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jmethodID getCacheDir = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
    jobject cache_dir = env->CallObjectMethod(activity, getCacheDir);


    jclass fileClass = env->FindClass("java/io/File");
    jmethodID getPath = env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");
    jstring path_string = (jstring)env->CallObjectMethod( cache_dir, getPath);

    const char *path_chars = env->GetStringUTFChars(path_string, NULL);
    std::string temp_folder(path_chars);

    env->ReleaseStringUTFChars(path_string, path_chars);
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(cache_dir);
    env->DeleteLocalRef(fileClass);

    return temp_folder;
}

