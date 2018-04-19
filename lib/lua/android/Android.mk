LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lua

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
			src/lapi.c \
			src/lauxlib.c \
			src/lbaselib.c \
			src/lbitlib.c \
			src/lcode.c \
			src/lcorolib.c \
			src/lctype.c \
			src/ldblib.c \
			src/ldebug.c \
			src/ldo.c \
			src/ldump.c \
			src/lfunc.c \
			src/lgc.c \
			src/linit.c \
			src/liolib.c \
			src/llex.c \
			src/lmem.c \
			src/loadlib.c \
			src/lobject.c \
			src/lopcodes.c \
			src/loslib.c \
			src/lparser.c \
			src/lstate.c \
			src/lstring.c \
			src/lstrlib.c \
			src/ltable.c \
			src/ltablib.c \
			src/ltm.c \
			src/lua.c \
			src/lundump.c \
			src/lutf8lib.c \
			src/lvm.c \
			src/lmathlib.c \
			src/lzio.c

LOCAL_CONLYFLAGS := -std=gnu99

cmd-strip := 

include $(BUILD_SHARED_LIBRARY)


