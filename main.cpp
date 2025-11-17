//
// Created by weikton on 17.11.25.
//
#include "main.h"
#include "jni.h"

#include <pthread.h>
#include <dlfcn.h>

JavaVM* g_java_vm = nullptr;
uintptr_t g_pGTASA = 0;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI(LIB_PROJECT_NAME " Starting..");
    LOGI("Thanks to use my plugin. Telegram: t.me/weikton_official");

    JNIEnv* env;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    if (env == nullptr) {
        return JNI_ERR;
    }

    g_java_vm = vm;

    LOGI("Version " VERSION " Inited");
    OnLibraryLoaded();

    return JNI_VERSION_1_4;
}

void OnLibraryLoaded()
{
    LOGI("OnLibraryLoaded() called");

    g_pGTASA = GlossGetLibBias("libGame.so");
    if(g_pGTASA == 0) {
		LOGI("ERROR: game address not found!");

		g_pGTASA = GlossGetLibBias("libGTASA.so");
        if(g_pGTASA == 0) {
            LOGI("ERROR: gtasa address not found!");
            return;
        } else {
            CHook::InitHookStuff("libGTASA.so");
        }
	} else {
        CHook::InitHookStuff("libGame.so");
    }

#if IS_ARM
    CHook::UnFuck(g_pGTASA + 0x60A484);
    strcpy((char*)(g_pGTASA + 0x60A484), ("WeiktonWrapper?"));

    CHook::UnFuck(g_pGTASA + 0x60A5B5);
    strcpy((char*)(g_pGTASA + 0x60A5B5), ("WeiktonNVEvent?"));
#elif IS_ARM64
    CHook::UnFuck(g_pGTASA + 0x7371E0);
    strcpy((char*)(g_pGTASA + 0x7371E0), ("WeiktonWrapper?"));

    CHook::UnFuck(g_pGTASA + 0x73740C);
    strcpy((char*)(g_pGTASA + 0x73740C), ("WeiktonNVEvent?"));
#endif
}
