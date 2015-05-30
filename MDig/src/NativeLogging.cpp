//
//  NativeLogging.cpp
//  CS231M Core Utils
//
//  Created by Saumitro Dasgupta on 4/12/14.
//  Copyright (c) 2014 Stanford University. All rights reserved.
//

#include "NativeLogging.hpp"

#ifndef __ANDROID__

void su_log_message(const char* level, const char* tag, const char* fmt, ...)
{
    fprintf(stderr, "[%s] %s : ", level, tag);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

#endif
