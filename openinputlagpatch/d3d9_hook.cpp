// Hook for D3D9 (and, by extension, D3D8 wrappers) to upgrade to D3D9EX for some latency improvements
// Also forcefully disables vsync to use a custom frame limiter instead

#include <stdio.h>
#include <d3d9.h>
#include "patch_util.h"

// Replaces D3DPOOL_MANAGED with D3DPOOL_DEFAULT because it's deprecated on D3D9Ex
auto CreateTexture_orig = (HRESULT(__stdcall*)(IDirect3DDevice9Ex*, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*))nullptr;
HRESULT __stdcall CreateTexture_hook(IDirect3DDevice9Ex* self, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format,
	D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	printf("CreateTexture intercepted!\n");

	if (Pool == D3DPOOL_MANAGED) {
		Pool = D3DPOOL_DEFAULT;
		Usage = D3DUSAGE_DYNAMIC;
	}
	return CreateTexture_orig(self, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

// Same as CreateTexture_hook
auto CreateVertexBuffer_orig = (HRESULT(__stdcall*)(IDirect3DDevice9Ex*, UINT, DWORD, DWORD, D3DPOOL, IDirect3DVertexBuffer9**, HANDLE*))nullptr;
HRESULT __stdcall CreateVertexBuffer_hook(IDirect3DDevice9Ex* self, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool,
	IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	printf("CreateVertexBuffer intercepted!\n");

	if (Pool == D3DPOOL_MANAGED) {
		Pool = D3DPOOL_DEFAULT;
		Usage = D3DUSAGE_DYNAMIC;
	}
	return CreateVertexBuffer_orig(self, Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

// Hooks certain D3D9Ex device functions for better compatibility and to force disable vsync
HRESULT __stdcall CreateDevice_hook(IDirect3D9Ex* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
	D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
	printf("CreateDevice intercepted! DeviceType: %d\n", DeviceType);

	// Disable vsync on the parameters
	pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	pPresentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	pPresentationParameters->SwapEffect = D3DSWAPEFFECT_DISCARD;
	pPresentationParameters->BackBufferCount = 0;
	
	// Create the device
	IDirect3DDevice9Ex* device = nullptr;
	HRESULT res = 0;
	if (pPresentationParameters->Windowed == FALSE) {
		// TODO: This should be configurable!
		D3DDISPLAYMODEEX display_mode = {};
		display_mode.Size = sizeof(display_mode);
		display_mode.Width = 640;
		display_mode.Height = 480;
		display_mode.RefreshRate = D3DPRESENT_RATE_DEFAULT;
		display_mode.Format = D3DFMT_X8R8G8B8;
		display_mode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;

		res = self->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &display_mode, &device);
	} else {
		res = self->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, NULL, &device);
	}
	if (FAILED(res)) {
		MessageBoxW(NULL, L"CreateDeviceEx failed!", L"Error", MB_ICONERROR);
		exit(1);
	}
	*ppReturnedDeviceInterface = device;

	// Set the maximum amount of frames that can be queued to 1 instead of 3
	// Should shave off 0 to 2 frames of lag
	device->SetMaximumFrameLatency(1);

	// Overwrite some vtable entries in IDirect3DDevice9Ex
	DWORD* device_vtbl = *(DWORD**)device;
	CreateTexture_orig = (HRESULT(__stdcall*)(IDirect3DDevice9Ex*, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*))device_vtbl[23];
	auto patch_data = (DWORD)CreateTexture_hook;
	patch_bytes(&device_vtbl[23], &patch_data, sizeof(DWORD));

	CreateVertexBuffer_orig = (HRESULT(__stdcall*)(IDirect3DDevice9Ex*, UINT, DWORD, DWORD, D3DPOOL, IDirect3DVertexBuffer9**, HANDLE*))device_vtbl[26];
	patch_data = (DWORD)CreateVertexBuffer_hook;
	patch_bytes(&device_vtbl[26], &patch_data, sizeof(DWORD));

	return 0;
}

// Upgrades IDirect3D9 to IDirect3D9Ex
IDirect3D9* Direct3DCreate9_hook(UINT SDKVersion) {
	printf("Direct3DCreate9 intercepted!\n");

	IDirect3D9Ex* d3d9ex = nullptr;
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9ex)) || d3d9ex == nullptr) {
		// TODO: Handle this in case someone wants to use XP for some reason
		MessageBoxW(NULL, L"Direct3DCreate9Ex failed!", L"Error", MB_ICONERROR);
		exit(1);
	}

	// Hook IDirect3D9Ex::CreateDevice
	DWORD* d3d9ex_vtbl = *(DWORD**)d3d9ex;
	auto patch_data = (DWORD)CreateDevice_hook;
	patch_bytes(&d3d9ex_vtbl[16], &patch_data, sizeof(DWORD));
	return d3d9ex;
}

// Intercepts Direct3DCreate9 to Direct3DCreate9Ex
void hook_d3d9() {
	// Try to IAT hook Direct3DCreate9 from the main executable
	if (!iat_hook(NULL, "d3d9.dll", "Direct3DCreate9", Direct3DCreate9_hook)) {
		// Try to IAT hook the D3D8 wrapper
		if (!iat_hook(L"d3d8.dll", "d3d9.dll", "Direct3DCreate9", Direct3DCreate9_hook)) {
			// Okay, something went wrong
			printf("Failed to hook Direct3DCreate9, d3d8to9 is probably not loaded!\n");
		}
	}
}