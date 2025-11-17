//
// Created by weikton on 17.11.25.
//
#pragma once

#include "Gloss.h"
#include "../main.h"

#include <android/log.h>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cstring>

#ifndef LOGI
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "CHook", __VA_ARGS__)
#endif

// ------------------------------------------------------------
//     CPU ARCH MACROS
// ------------------------------------------------------------
#if defined(IS_ARM)
    #define ARCH_32
#elif defined(IS_ARM64)
    #define ARCH_64
#else
    #error "Define IS_ARM or IS_ARM64 when building CHook"
#endif

#ifdef ARCH_32
    #define __32BIT
    #define THUMBMODE(_a)   ((((uintptr_t)(_a)) & 1) != 0)
    #define DETHUMB(_a)     (((uintptr_t)(_a)) & ~1)
    #define RETHUMB(_a)     (((uintptr_t)(_a)) | 1)
#elif defined(ARCH_64)
    #define __64BIT
    #define THUMBMODE(_a)   (false)
    #define DETHUMB(_a)     (_a)
    #define RETHUMB(_a)     (_a)
#endif

#define GET_LR(dwRetAddr) \
    do { \
        uintptr_t lr = reinterpret_cast<uintptr_t>(__builtin_return_address(0)); \
        dwRetAddr = lr - g_libGTASA; \
    } while (0)

#define SET_TO(a,b) *(void**)&(a) = (void*)(b)

// ======================================================================
//                             CHook
// ======================================================================
class CHook {
public:
    static inline GHandle libHandle = nullptr;
    static inline std::string libName;

public:

// ------------------------------------------------------------
//     InitHookStuff
// ------------------------------------------------------------
    static void InitHookStuff(const char* lib) {
        if (lib && lib[0]) {
            libName = lib;
            libHandle = GlossOpen(lib);
            if (!libHandle) {
                LOGI("CHook::InitHookStuff -> GlossOpen FAILED for %s", lib);
            } else {
                LOGI("CHook::InitHookStuff -> OK (%s)", lib);
            }
        }
    }


// ------------------------------------------------------------
//     UnFuck
// ------------------------------------------------------------
    static void UnFuck(uintptr_t addr, size_t len = 4096) {
        if (!addr) return;

        p_flag flags = {1,1,1,0,0,0};
        if (!SetMemoryPermission(addr & ~0xFFF, len, &flags)) {
            SetMemoryPermission(addr & ~0xFFF, len, nullptr);
        }
    }


// ------------------------------------------------------------
//     getSym
// ------------------------------------------------------------
    static uintptr_t getSym(const char* sym) {
        if (!libHandle) {
            LOGI("CHook::getSym -> library not opened! (%s)", sym);
            return 0;
        }

        size_t outSize = 0;
        uintptr_t addr = GlossSymbol(libHandle, sym, &outSize);
        if (!addr) {
            LOGI("CHook::getSym FAILED %s", sym);
        }
        return addr;
    }


// ------------------------------------------------------------
//     WriteMemory
// ------------------------------------------------------------
    template<typename Src>
    static void WriteMemory(uintptr_t dest, const Src* src, size_t size) {
        if (!dest || !src || size == 0) return;
        UnFuck(dest, size);
        ::WriteMemory((void*)dest, (void*)src, size, true);
    }

    template<typename Src>
    static void Write(uintptr_t dest, Src value, size_t size = sizeof(Src)) {
        UnFuck(dest, size);
        ::WriteMemory((void*)dest, &value, size, true);
    }


// ------------------------------------------------------------
//     NOP (Gloss::Inst)
// ------------------------------------------------------------
    template<typename AddrT>
    static void NOP(AddrT adr, size_t count) {
        uintptr_t addr = (uintptr_t)adr;
        if (!addr || !count) return;

#ifdef ARCH_32
        UnFuck(addr, count * 4 + 32);
        if (THUMBMODE(addr)) {
            uint32_t base = DETHUMB(addr);
            for (size_t i=0; i<count; i++)
                Gloss::Inst::MakeThumb16NOP(base + i*2, 2);
        } else {
            for (size_t i=0; i<count; i++)
                Gloss::Inst::MakeArmNOP((uint32_t)(addr + i*4), 4);
        }
#elif defined(ARCH_64)
        UnFuck(addr, count * 4 + 32);
        for (size_t i=0; i<count; i++)
            Gloss::Inst::MakeArm64NOP((uint64_t)(addr + i*4), 4);
#endif
    }


// ------------------------------------------------------------
//     RET
// ------------------------------------------------------------
    static void RET(const char* sym) {
        RET(getSym(sym));
    }

    static void RET(uintptr_t addr) {
        if (!addr) return;

#ifdef ARCH_32
        if (THUMBMODE(addr)) {
            Gloss::Inst::MakeThumbRET(DETHUMB(addr), 1); // BX LR
        } else {
            Gloss::Inst::MakeArmRET((uint32_t)addr, 1);  // BX LR
        }
#elif defined(ARCH_64)
        Gloss::Inst::MakeArm64RET((uint64_t)addr, 0);
#endif
    }


// ------------------------------------------------------------
//     CallFunction
// ------------------------------------------------------------
    template<typename Ret, typename... Args>
    static Ret CallFunction(uintptr_t address, Args... args) {
        if (!address) {
            LOGI("CallFunction: NULL address");
            exit(0);
        }
        return ((Ret(*)(Args...))address)(args...);
    }

    template<typename Ret, typename... Args>
    static Ret CallFunction(const char* sym, Args... args) {
        static std::unordered_map<std::string, uintptr_t> cache;
        uintptr_t addr;

        auto it = cache.find(sym);
        if (it == cache.end()) {
            addr = getSym(sym);
            cache[sym] = addr;
        } else {
            addr = it->second;
        }

        return ((Ret(*)(Args...))addr)(args...);
    }


// ------------------------------------------------------------
//     InlineHook (GlossHookByName)
// ------------------------------------------------------------
    template<typename Func, typename Orig>
    static void InlineHook(const char* sym, Func newFunc, Orig* oldFunc) {
        if (libName.empty()) {
            LOGI("InlineHook(sym) FAILED — libName not set");
            return;
        }

        GlossHookByName(libName.c_str(), sym,
                        (void*)newFunc, (void**)oldFunc,
                        nullptr);
    }


// ------------------------------------------------------------
//     InlineHook (GlossHookAddr)
// ------------------------------------------------------------
    template<typename Addr, typename Func, typename Orig>
    static void InlineHook(Addr addr, Func func, Orig old) {
        uintptr_t a = (uintptr_t)addr;
        if (!a) return;

#ifdef ARCH_32
        i_set mode = THUMBMODE(a) ? i_set::I_THUMB : i_set::I_ARM;
        GlossHookAddr((void*)DETHUMB(a), (void*)func, (void**)&old, false, mode);
#elif defined(ARCH_64)
        GlossHookAddr((void*)a, (void*)func, (void**)&old, false, i_set::I_ARM64);
#endif
    }


// ------------------------------------------------------------
//     Redirect (GlossHookRedirect)
// ------------------------------------------------------------
    template<typename Func>
    static void Redirect(const char* sym, Func func) {
        uintptr_t a = getSym(sym);
        Redirect((void*)a, func);
    }

    template<typename Addr, typename Func>
    static void Redirect(Addr addr, Func func) {
        uintptr_t a = (uintptr_t)addr;
        if (!a) return;

#ifdef ARCH_32
        i_set mode = THUMBMODE(a) ? i_set::I_THUMB : i_set::I_ARM;
        GlossHookRedirect((void*)DETHUMB(a), (void*)func, false, mode);
#elif defined(ARCH_64)
        GlossHookRedirect((void*)a, (void*)func, false, i_set::I_ARM64);
#endif
    }


// ------------------------------------------------------------
//     PLT/GOT
// ------------------------------------------------------------
    template<typename Addr, typename Func, typename Orig>
    static void InstallPLT(Addr addr, Func hook, Orig* orig) {
        uintptr_t a = (uintptr_t)addr;
        if (!a) return;

        UnFuck(a, sizeof(uintptr_t));
        if (orig)
            *orig = (Orig)*(uintptr_t*)a;

        *(uintptr_t*)a = (uintptr_t)hook;
    }

    template<typename Addr, typename Func>
    static void InstallPLT(Addr addr, Func hook) {
        InstallPLT(addr, hook, (void**)nullptr);
    }


// ------------------------------------------------------------
//     VTable helpers
// ------------------------------------------------------------
    template<typename T, typename A>
    static void SetVTable(T* obj, A newVTable) {
        *reinterpret_cast<uintptr_t**>(obj) = (uintptr_t*)newVTable;
    }

    template<typename Ret, typename T, typename... Args>
    static Ret CallVTableFunctionByNum(T* obj, int index, Args... args) {
        auto vtable = *reinterpret_cast<uintptr_t**>(obj);
        auto fn = reinterpret_cast<Ret(*)(T*, Args...)>(vtable[index]);
        return fn(obj, std::forward<Args>(args)...);
    }
};