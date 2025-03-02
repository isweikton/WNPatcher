#include "main.h"
#include "jniutil.h"

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
const char* g_pAPKPackage;

jobject appContext;
JavaVM *mVm;
JNIEnv *mEnv;

void WNGTA() 
{
#ifdef IS_ARM64
	toasty(OBFUSCATE("[WNPatch]: Install GTA Modification's for x64"));

	// 2.10 x64
	// ARMHook::NOP(GTA(0x4D8EB8), 2);
#elif defined(IS_ARM32)
    toasty(OBFUSCATE("[WNPatch]: Install GTA Modification's for x32"));

	// 1.08
    // ARMHook::makeNOP(GTA(0x39B1E4), 2);

	// 2.01
	// ARMHook::makeNOP(GTA(0x3F6942), 2);

	// 2.10
	// ARMHook::NOP(GTA(0x3F6992), 2);
#endif
}

void WNSAMP()
{
#ifdef IS_ARM64
	toasty(OBFUSCATE("[WNPatch]: Install SAMP Modification's for x64"));

	// ARMHook::NOP(SAMP(0x0), 2);
#elif defined(IS_ARM32)
    toasty(OBFUSCATE("[WNPatch]: Install SAMP Modification's for x32"));

	// ARMHook::NOP(SAMP(0x0), 2);
#endif
}

JNIEnv *getEnv() 
{
	JNIEnv* env = nullptr;
    JavaVM* javaVM = mVm;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

	if (getEnvStat == JNI_EDETACHED)
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		  return nullptr;

	if (getEnvStat == JNI_EVERSION)
	    return nullptr;

	if (getEnvStat == JNI_ERR)
	   return nullptr;

	return env;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    mVm = vm;
    mEnv = getEnv();

	appContext = GetGlobalActivity(mEnv);
    if(appContext != NULL) 
	{ 
        g_pAPKPackage = mEnv->GetStringUTFChars(GetPackageName(mEnv, appContext), NULL);

        char the_end_of_the_world[0xFF];
		sprintf(the_end_of_the_world, OBFUSCATE("Package: %s"), g_pAPKPackage);

		AndroidLog("Powered by Weikton");
		__android_log_write(ANDROID_LOG_INFO, OBFUSCATE("WNPatch"), the_end_of_the_world);
#ifdef IS_ARM32 
		AndroidLog("Arch: x32");
#elif defined(IS_ARM64)
		AndroidLog("Arch: x64");
#endif
    }
    
	g_libGTASA = ARMHook::getLibraryAddress(OBFUSCATE("libGTASA.so"));
	if(!g_libGTASA) 
		g_libGTASA = ARMHook::getLibraryAddress(OBFUSCATE("libGAME.so"));
	if(g_libGTASA)
	{
		AndroidLog("Install GTA...");
		WNGTA(); // <<<
	}

	g_libSAMP = ARMHook::getLibraryAddress(OBFUSCATE("libsamp.so"));
	if(!g_libSAMP)
		g_libSAMP = ARMHook::getLibraryAddress(OBFUSCATE("libmultiplayer.so"));
	if(!g_libSAMP)
		g_libSAMP = ARMHook::getLibraryAddress(OBFUSCATE("libsampvoice.so"));
	if(g_libSAMP)
	{
		AndroidLog("Install SAMP...");
		WNSAMP(); // <<<
	}

	return JNI_VERSION_1_6;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);

	return (tv.tv_sec*1000 + tv.tv_usec/1000);
}

#undef pack(a)
unsigned long pack(const char* enter)
{
	unsigned long uiAddr = strtoul(enter, nullptr, 16);
	uiAddr = g_libGTASA + uiAddr;
    return uiAddr;
}

#undef decPACK(a)
int decPACK(const char* enter)
{
	int result;
    sscanf(enter, OBFUSCATE("%d"), &result);
    return result;
}

uint32_t s2h(const char* enter)
{
    uint32_t result;
    sscanf(enter, OBFUSCATE("%x"),&result);
    return result;
}

int s2d(const char* enter)
{
    int result;
    sscanf(enter, OBFUSCATE("%d"), &result);
    return result;
}

float s2f(const char* enter)
{
    float result;
    sscanf(enter, OBFUSCATE("%f"), &result);
    return result;
}