LIBSRCS:=$(shell find src/lib -name '*.cpp' -not -name '._*')
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(LIBSRCS)
LOCAL_MODULE := jpcnn

LOCAL_C_INCLUDES += ./src/lib/include \
./src/lib/graph \
./src/lib/math \
./src/lib/third_party \
./src/lib/utility \
./src/lib/svm \
./src/lib/opengl \
./src/lib \
./src/include \
../eigen 
#$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.8/include \
#$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/include

LOCAL_CFLAGS := -DUSE_EIGEN_GEMM -DUSE_NEON -DEIGEN_DONT_PARALLELIZE
LOCAL_CFLAGS += -mfloat-abi=softfp -mfpu=neon -march=armv7

LOCAL_CFLAGS += -fopenmp -O3
LOCAL_LDFLAGS += -fopenmp -llog


include $(BUILD_SHARED_LIBRARY)
