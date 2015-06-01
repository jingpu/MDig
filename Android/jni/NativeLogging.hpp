//
//  NativeLogging.hpp
//  CS231M Core Utils
//
//  Created by Saumitro Dasgupta on 4/12/14.
//  Copyright (c) 2014 Stanford University. All rights reserved.
//

#ifndef __CS231M_Core_Utils__NativeLogging__
#define __CS231M_Core_Utils__NativeLogging__

#ifdef __ANDROID__

    #include <android/log.h>

    #define SU_LOG_LEVEL_DEBUG  ANDROID_LOG_DEBUG
    #define SU_LOG_LEVEL_INFO   ANDROID_LOG_INFO
    #define SU_LOG_LEVEL_WARN   ANDROID_LOG_WARN
    #define SU_LOG_LEVEL_ERROR  ANDROID_LOG_ERROR
    #define SU_PLATFORM_LOG(level, tag, ...) __android_log_print(level, tag, __VA_ARGS__)

#else

    #include <stdarg.h>
    #include <stdio.h>

    void su_log_message(const char* level, const char* tag, const char* fmt, ...);

    #define SU_LOG_LEVEL_DEBUG  "DEBUG"
    #define SU_LOG_LEVEL_INFO   "INFO"
    #define SU_LOG_LEVEL_WARN   "WARNING"
    #define SU_LOG_LEVEL_ERROR  "ERROR"
    #define SU_PLATFORM_LOG(level, tag, ...) su_log_message(level, tag, __VA_ARGS__)

#endif

#define  LOG_DEBUG(tag, ...)    SU_PLATFORM_LOG(SU_LOG_LEVEL_DEBUG, tag, __VA_ARGS__)
#define  LOG_INFO(tag, ...)     SU_PLATFORM_LOG(SU_LOG_LEVEL_INFO,  tag, __VA_ARGS__)
#define  LOG_WARN(tag, ...)     SU_PLATFORM_LOG(SU_LOG_LEVEL_WARN,  tag, __VA_ARGS__)
#define  LOG_ERROR(tag, ...)    SU_PLATFORM_LOG(SU_LOG_LEVEL_ERROR, tag, __VA_ARGS__)

#endif /* defined(__CS231M_Core_Utils__NativeLogging__) */
