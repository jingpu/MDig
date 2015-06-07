LOCAL_PATH := $(call my-dir)

# Deep Belief
include $(CLEAR_VARS)
LOCAL_MODULE := jpcnn
LOCAL_SRC_FILES := DeepBelief/libjpcnn.so
#LOCAL_SRC_FILES := DeepBelief/libjpcnn.batch.so
include $(PREBUILT_SHARED_LIBRARY)

# Tegra optimized OpenCV.mk
include $(CLEAR_VARS)
OPENCV_INSTALL_MODULES:=on
include $(OPENCV_PATH)/sdk/native/jni/OpenCV-tegra3.mk

# MDig
LOCAL_LDLIBS += -llog
LOCAL_SHARED_LIBRARIES  += jpcnn
LOCAL_MODULE    := MDig
LOCAL_SRC_FILES := NativeLogging.cpp NativeCore.cpp  MDApp.cpp ConvNet.cpp segmentation.cpp ResourceLocator.cpp

include $(BUILD_SHARED_LIBRARY)
