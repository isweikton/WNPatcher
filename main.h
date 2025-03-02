#pragma once 

#include <jni.h>
#include <android/log.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "ARMHook.h"
#include "str_obfuscate.hpp"

extern uintptr_t g_libGTASA;
#define SA_ADDR(addr) (g_libGTASA + (addr))

extern const char* g_pAPKPackage;
extern JavaVM *mVm;
extern JNIEnv *mEnv;

uint32_t GetTickCount();
void SetFunc(uintptr_t addr, uintptr_t *orig);

uint32_t s2h(const char* enter);
int s2d(const char* enter);
float s2f(const char* enter);
unsigned long pack(const char* enter);
int decPACK(const char* enter);
#define pack(a) pack(OBFUSCATE(a))
#define decPACK(a) decPACK(OBFUSCATE("a"))