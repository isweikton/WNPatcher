//
// Created by weikton on 17.11.25.
//
#pragma once
#include "wn.h"
#include "include/Gloss.h"
#include "include/CHook.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <list>
#include <string>
#include <fstream>
#include <vector>
#include <sys/mman.h>
#include <android/log.h>
#include <jni.h>

extern uintptr_t g_pGTASA;

#define g_libGTASA g_pGTASA;

void OnLibraryLoaded();

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LIB_PROJECT_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LIB_PROJECT_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LIB_PROJECT_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LIB_PROJECT_NAME, __VA_ARGS__)

#ifndef PAGESIZE
#define PAGESIZE PAGE_SIZE
#endif