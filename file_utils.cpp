#include "file_utils.h"
#include "string_utils.h"
#include "log.h"
#include <sys/stat.h>
#include <dirent.h>

#ifdef ANDROID
	#include "android/ndk/android_native_app_glue.h"
	#include "SDL.h"
	#include <jni.h>
#endif


namespace FileUtils
{
// chdir(ANDROID_DATA_PATH);

std::vector<std::string> listDir(const std::string& dir, const std::string& suffix)
{
	DIR *d;
	struct dirent *rddir;
	d = opendir(".");

	std::vector<std::string> ret;

	if(!d)
		return ret;

	while ((rddir = readdir(d)) != NULL)
	{
		std::string name(rddir->d_name);

		if(!suffix.empty() && !StringUtils::endsWith(name, suffix))
			continue;

		// Log() << "FileUtils: listDir found: " << name;
		ret.push_back(name);
	}

	closedir(d);

	return ret;
}

bool fileExists(const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0); 
}

std::string dataFolderLocation()
{
#if ! defined(ANDROID)
	return ".";

#else
    static std::string temp_folder;

	if(temp_folder) return temp_folder; // Cache variable

	// Obtain cache folder location
    JNIEnv *env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());

    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jmethodID getCacheDir = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
    jobject cache_dir = env->CallObjectMethod(activity, getCacheDir);


    jclass fileClass = env->FindClass("java/io/File");
    jmethodID getPath = env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");
    jstring path_string = (jstring)env->CallObjectMethod( cache_dir, getPath);

    const char *path_chars = env->GetStringUTFChars(path_string, NULL);
    temp_folder = path_chars;

    env->ReleaseStringUTFChars(path_string, path_chars);
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(cache_dir);
    env->DeleteLocalRef(fileClass);

    return temp_folder;
#endif
}

}

