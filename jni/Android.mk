LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := retro-nuklear-gl2

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -DANDROID_ARM
LOCAL_ARM_MODE := arm
endif

ifeq ($(TARGET_ARCH),x86)
LOCAL_CFLAGS += -DANDROID_X86
endif

ifeq ($(TARGET_ARCH),mips)
LOCAL_CFLAGS += -DANDROID_MIPS
endif

GLES_LIB := -lGLESv3 -lEGL

LOCAL_SRC_FILES += $(addprefix ../,$(wildcard *.c) ./libretro.c ./app.c ./libretro-common/glsym/rglgen.c ./libretro-common/glsym/glsym_es2.c)
LOCAL_CFLAGS += -O2 -Wall -std=gnu99 -ffast-math -DHAVE_OPENGLES2 -DHAVE_OPENGLES3 -DGLES3 -DGLES -I../libretro-common/include -I../../../example -I../../ -DAND -I/ubuntu14/home/not6/android-ndk-r11c/platforms/android-18/arch-arm/usr/include
LOCAL_LDLIBS += $(GLES_LIB)

include $(BUILD_SHARED_LIBRARY)

