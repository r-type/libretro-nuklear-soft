APP_ABI := armeabi-v7a
#all
ifeq ($(GLES), 3)
   APP_PLATFORM := android-21
else
   APP_PLATFORM := android-9
endif

