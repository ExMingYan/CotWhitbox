#include "Proxy.h"
#include "Proxy.hpp"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

static HMODULE g_hOriginalDll = nullptr;
DirectSoundCreate_t oDirectSoundCreate = nullptr;
DirectSoundCreate8_t oDirectSoundCreate8 = nullptr;
DirectSoundEnumerateA_t oDirectSoundEnumerateA = nullptr;
DirectSoundEnumerateW_t oDirectSoundEnumerateW = nullptr;
DirectSoundCaptureCreate_t oDirectSoundCaptureCreate = nullptr;
DirectSoundCaptureCreate8_t oDirectSoundCaptureCreate8 = nullptr;
DirectSoundCaptureEnumerateA_t oDirectSoundCaptureEnumerateA = nullptr;
DirectSoundCaptureEnumerateW_t oDirectSoundCaptureEnumerateW = nullptr;
DirectSoundFullDuplexCreate_t oDirectSoundFullDuplexCreate = nullptr;
GetDeviceID_t oGetDeviceID = nullptr;
DllCanUnloadNow_t oDllCanUnloadNow = nullptr;
DllGetClassObject_t oDllGetClassObject = nullptr;

void InitializeProxy()
{
    char systemPath[MAX_PATH];
    GetSystemDirectoryA(systemPath, MAX_PATH);
    PathAppendA(systemPath, "dsound.dll");
    g_hOriginalDll = LoadLibraryA(systemPath);

    if (g_hOriginalDll)
    {
        oDirectSoundCreate = (DirectSoundCreate_t)GetProcAddress(g_hOriginalDll, "DirectSoundCreate");
        oDirectSoundCreate8 = (DirectSoundCreate8_t)GetProcAddress(g_hOriginalDll, "DirectSoundCreate8");
        oDirectSoundEnumerateA = (DirectSoundEnumerateA_t)GetProcAddress(g_hOriginalDll, "DirectSoundEnumerateA");
        oDirectSoundEnumerateW = (DirectSoundEnumerateW_t)GetProcAddress(g_hOriginalDll, "DirectSoundEnumerateW");
        oDirectSoundCaptureCreate = (DirectSoundCaptureCreate_t)GetProcAddress(g_hOriginalDll, "DirectSoundCaptureCreate");
        oDirectSoundCaptureCreate8 = (DirectSoundCaptureCreate8_t)GetProcAddress(g_hOriginalDll, "DirectSoundCaptureCreate8");
        oDirectSoundCaptureEnumerateA = (DirectSoundCaptureEnumerateA_t)GetProcAddress(g_hOriginalDll, "DirectSoundCaptureEnumerateA");
        oDirectSoundCaptureEnumerateW = (DirectSoundCaptureEnumerateW_t)GetProcAddress(g_hOriginalDll, "DirectSoundCaptureEnumerateW");
        oDirectSoundFullDuplexCreate = (DirectSoundFullDuplexCreate_t)GetProcAddress(g_hOriginalDll, "DirectSoundFullDuplexCreate");
        oGetDeviceID = (GetDeviceID_t)GetProcAddress(g_hOriginalDll, "GetDeviceID");
        oDllCanUnloadNow = (DllCanUnloadNow_t)GetProcAddress(g_hOriginalDll, "DllCanUnloadNow");
        oDllGetClassObject = (DllGetClassObject_t)GetProcAddress(g_hOriginalDll, "DllGetClassObject");
    }
}

void FinalizeProxy()
{
    if (g_hOriginalDll)
    {
        FreeLibrary(g_hOriginalDll);
		g_hOriginalDll = nullptr;
    }
}

extern "C"
{
    HRESULT WINAPI pDirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter) {
        return oDirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter);
    }

    HRESULT WINAPI pDirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8* ppDS8, LPUNKNOWN pUnkOuter) {
        return oDirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
    }

    HRESULT WINAPI pDirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext) {
        return oDirectSoundEnumerateA(pDSEnumCallback, pContext);
    }

    HRESULT WINAPI pDirectSoundEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext) {
        return oDirectSoundEnumerateW(pDSEnumCallback, pContext);
    }

    HRESULT WINAPI pDirectSoundCaptureCreate(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE* ppDSC, LPUNKNOWN pUnkOuter) {
        return oDirectSoundCaptureCreate(pcGuidDevice, ppDSC, pUnkOuter);
    }

    HRESULT WINAPI pDirectSoundCaptureCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8* ppDSC8, LPUNKNOWN pUnkOuter) {
        return oDirectSoundCaptureCreate8(pcGuidDevice, ppDSC8, pUnkOuter);
    }

    HRESULT WINAPI pDirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext) {
        return oDirectSoundCaptureEnumerateA(pDSEnumCallback, pContext);
    }

    HRESULT WINAPI pDirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext) {
        return oDirectSoundCaptureEnumerateW(pDSEnumCallback, pContext);
    }

    HRESULT WINAPI pDirectSoundFullDuplexCreate(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice, LPCDSCBUFFERDESC pcDscBufferDesc, LPCDSBUFFERDESC pcDsBufferDesc, HWND hWnd, DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDsfd, LPDIRECTSOUNDCAPTUREBUFFER8* ppDscb8, LPDIRECTSOUNDBUFFER8* ppDsb8, LPUNKNOWN pUnkOuter) {
        return oDirectSoundFullDuplexCreate(pcGuidCaptureDevice, pcGuidRenderDevice, pcDscBufferDesc, pcDsBufferDesc, hWnd, dwLevel, ppDsfd, ppDscb8, ppDsb8, pUnkOuter);
    }

    HRESULT WINAPI pGetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest) {
        return oGetDeviceID(pGuidSrc, pGuidDest);
    }

    HRESULT WINAPI pDllCanUnloadNow() {
        return oDllCanUnloadNow();
    }

    HRESULT WINAPI pDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
        return oDllGetClassObject(rclsid, riid, ppv);
    }
}
