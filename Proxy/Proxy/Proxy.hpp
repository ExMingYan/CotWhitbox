#ifndef PROXY_HPP
#define PROXY_HPP
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")

using DirectSoundCreate_t = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUND*, LPUNKNOWN);
using DirectSoundCreate8_t = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUND8*, LPUNKNOWN);
using DirectSoundEnumerateA_t = HRESULT(WINAPI*)(LPDSENUMCALLBACKA, LPVOID);
using DirectSoundEnumerateW_t = HRESULT(WINAPI*)(LPDSENUMCALLBACKW, LPVOID);
using DirectSoundCaptureCreate_t = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUNDCAPTURE*, LPUNKNOWN);
using DirectSoundCaptureCreate8_t = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUNDCAPTURE8*, LPUNKNOWN);
using DirectSoundCaptureEnumerateA_t = HRESULT(WINAPI*)(LPDSENUMCALLBACKA, LPVOID);
using DirectSoundCaptureEnumerateW_t = HRESULT(WINAPI*)(LPDSENUMCALLBACKW, LPVOID);
using DirectSoundFullDuplexCreate_t = HRESULT(WINAPI*)(LPCGUID, LPCGUID, LPCDSCBUFFERDESC, LPCDSBUFFERDESC, HWND, DWORD, LPDIRECTSOUNDFULLDUPLEX*, LPDIRECTSOUNDCAPTUREBUFFER8*, LPDIRECTSOUNDBUFFER8*, LPUNKNOWN);
using GetDeviceID_t = HRESULT(WINAPI*)(LPCGUID, LPGUID);
using DllCanUnloadNow_t = HRESULT(WINAPI*)();
using DllGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);

extern DirectSoundCreate_t oDirectSoundCreate;
extern DirectSoundCreate8_t oDirectSoundCreate8;
extern DirectSoundEnumerateA_t oDirectSoundEnumerateA;
extern DirectSoundEnumerateW_t oDirectSoundEnumerateW;
extern DirectSoundCaptureCreate_t oDirectSoundCaptureCreate;
extern DirectSoundCaptureCreate8_t oDirectSoundCaptureCreate8;
extern DirectSoundCaptureEnumerateA_t oDirectSoundCaptureEnumerateA;
extern DirectSoundCaptureEnumerateW_t oDirectSoundCaptureEnumerateW;
extern DirectSoundFullDuplexCreate_t oDirectSoundFullDuplexCreate;
extern GetDeviceID_t oGetDeviceID;
extern DllCanUnloadNow_t oDllCanUnloadNow;
extern DllGetClassObject_t oDllGetClassObject;

#pragma comment(linker, "/export:DllCanUnloadNow=pDllCanUnloadNow,@1")
#pragma comment(linker, "/export:DllGetClassObject=pDllGetClassObject,@2")
#pragma comment(linker, "/export:DirectSoundCreate=pDirectSoundCreate,@3")
#pragma comment(linker, "/export:DirectSoundEnumerateA=pDirectSoundEnumerateA,@4")
#pragma comment(linker, "/export:DirectSoundEnumerateW=pDirectSoundEnumerateW,@5")
#pragma comment(linker, "/export:DirectSoundCaptureCreate=pDirectSoundCaptureCreate,@6")
#pragma comment(linker, "/export:DirectSoundCaptureEnumerateA=pDirectSoundCaptureEnumerateA,@7")
#pragma comment(linker, "/export:DirectSoundCaptureEnumerateW=pDirectSoundCaptureEnumerateW,@8")
#pragma comment(linker, "/export:GetDeviceID=pGetDeviceID,@9")
#pragma comment(linker, "/export:DirectSoundFullDuplexCreate=pDirectSoundFullDuplexCreate,@10")
#pragma comment(linker, "/export:DirectSoundCreate8=pDirectSoundCreate8,@11")
#pragma comment(linker, "/export:DirectSoundCaptureCreate8=pDirectSoundCaptureCreate8,@12")

#endif // !PROXY_HPP
