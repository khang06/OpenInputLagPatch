// Hook for D3D9 (and, by extension, D3D8 wrappers) to upgrade to D3D9EX for some latency improvements
// Also forcefully disables vsync to use a custom frame limiter instead

#include <stdio.h>
#include <d3d9.h>
#include "patch_util.h"
#include "common.h"
#include "config.h"

static IDirect3D9Ex* d3d9ex = nullptr;
static IDirect3DDevice9Ex* d3d9ex_device = nullptr;

// Replaces D3DPOOL_MANAGED with D3DPOOL_DEFAULT because it's deprecated on D3D9Ex
auto CreateTexture_orig = (HRESULT(__stdcall*)(IDirect3DDevice9Ex*, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*))nullptr;
HRESULT __stdcall CreateTexture_hook(IDirect3DDevice9Ex* self, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format,
	D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	//printf("CreateTexture intercepted!\n");

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

// Gets the target fullscreen refresh rate
// TODO: This might not be so great on multi-monitor setups
UINT max_refresh_rate = 0;
UINT get_target_refresh_rate(D3DPRESENT_PARAMETERS* present_params) {
	switch (Config::FullscreenRefreshRate) {
		case TargetRefreshRate::Max:
			return D3DPRESENT_RATE_DEFAULT;
		case TargetRefreshRate::Sixty:
			return 60;
		case TargetRefreshRate::MultipleOfSixty: {
			if (max_refresh_rate == 0) {
				// Enumerate all supported resolutions and refresh rates
				// TODO: Probably shouldn't always use D3DADAPTER_DEFAULT, but the games probably do that too
				auto count = d3d9ex->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
				D3DDISPLAYMODE* modes = new D3DDISPLAYMODE[count];
				printf("Supported display modes:\n");
				for (UINT i = 0; i < count; i++) {
					d3d9ex->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &modes[i]);
					printf("%d x %d @ %dhz\n", modes[i].Width, modes[i].Height, modes[i].RefreshRate);
				}

				// Pick the highest multiple of 60 refresh rate at the selected resolution
				for (UINT i = 0; i < count; i++) {
					if (modes[i].Width == present_params->BackBufferWidth && modes[i].Height == present_params->BackBufferHeight && (modes[i].RefreshRate == 59 || modes[i].RefreshRate % 60 == 0)) {
						auto rate = modes[i].RefreshRate == 59 ? 60 : modes[i].RefreshRate;
						if (rate > max_refresh_rate)
							max_refresh_rate = rate;
					}
				}
				if (max_refresh_rate == 0)
					panic_msgbox(L"Failed to find a suitable display mode");

				printf("Picked %dhz\n", max_refresh_rate);
				delete[] modes;
			}
			return max_refresh_rate;
		}
		default:
			panic_msgbox(L"Invalid target refresh rate %d", Config::FullscreenRefreshRate);
	}
}

// Modifies presentation parameters to work properly with Direct3D9Ex
void modify_presentation_parameters(D3DPRESENT_PARAMETERS* params) {
	params->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	params->FullScreen_RefreshRateInHz = params->Windowed ? D3DPRESENT_RATE_DEFAULT : get_target_refresh_rate(params);
	params->SwapEffect = D3DSWAPEFFECT_DISCARD;
	params->BackBufferCount = 0;
}

// Gets the display mode for fullscreen
void get_fullscreen_display_mode(D3DPRESENT_PARAMETERS* present_params, D3DDISPLAYMODEEX* mode) {
	mode->Size = sizeof(D3DDISPLAYMODEEX);
	mode->Width = present_params->BackBufferWidth;
	mode->Height = present_params->BackBufferHeight;
	mode->RefreshRate = get_target_refresh_rate(present_params);
	mode->Format = present_params->BackBufferFormat;
	mode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
}

// Same as CreateDevice_hook but for Reset
HRESULT __stdcall Reset_hook(IDirect3DDevice9Ex* self, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	printf("Reset intercepted!\n");
	modify_presentation_parameters(pPresentationParameters);

	D3DDISPLAYMODEEX display_mode = {};
	get_fullscreen_display_mode(pPresentationParameters, &display_mode);

	return self->ResetEx(pPresentationParameters, pPresentationParameters->Windowed ? NULL : &display_mode);
}

// Hooks certain D3D9Ex device functions for better compatibility and to force disable vsync
HRESULT __stdcall CreateDevice_hook(IDirect3D9Ex* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
	D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
	printf("CreateDevice intercepted! DeviceType: %d\n", DeviceType);

	// Disable vsync on the parameters
	modify_presentation_parameters(pPresentationParameters);
	
	// Create the device
	IDirect3DDevice9Ex* device = nullptr;
	HRESULT res = 0;
	if (pPresentationParameters->Windowed == FALSE) {
		D3DDISPLAYMODEEX display_mode = {};
		get_fullscreen_display_mode(pPresentationParameters, &display_mode);

		res = self->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &display_mode, &device);
	} else {
		res = self->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, NULL, &device);
	}
	if (FAILED(res))
		panic_msgbox(L"CreateDeviceEx failed!\nCode: 0x%x", res);
	*ppReturnedDeviceInterface = device;

	// D3D9Ex breaks the window style for some reason
	SetWindowPos(hFocusWindow, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

	// Set the maximum amount of frames that can be queued to 1 instead of 3
	// Should shave off 0 to 2 frames of lag
	device->SetMaximumFrameLatency(1);

	// Set the CPU and GPU threads to high priority
	device->SetGPUThreadPriority(7);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	// Overwrite some vtable entries in IDirect3DDevice9Ex
	DWORD* device_vtbl = *(DWORD**)device;
	auto patch_data = (DWORD)Reset_hook;
	patch_bytes(&device_vtbl[16], &patch_data, sizeof(DWORD));

	CreateTexture_orig = (HRESULT(__stdcall*)(IDirect3DDevice9Ex*, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*))device_vtbl[23];
	patch_data = (DWORD)CreateTexture_hook;
	patch_bytes(&device_vtbl[23], &patch_data, sizeof(DWORD));

	CreateVertexBuffer_orig = (HRESULT(__stdcall*)(IDirect3DDevice9Ex*, UINT, DWORD, DWORD, D3DPOOL, IDirect3DVertexBuffer9**, HANDLE*))device_vtbl[26];
	patch_data = (DWORD)CreateVertexBuffer_hook;
	patch_bytes(&device_vtbl[26], &patch_data, sizeof(DWORD));

	return 0;
}

// Upgrades IDirect3D9 to IDirect3D9Ex
IDirect3D9* WINAPI Direct3DCreate9_hook(UINT SDKVersion) {
	printf("Direct3DCreate9 intercepted!\n");

	if (!Config::D3D9Ex)
		panic_msgbox(L"Direct3DCreate9_hook was called despite D3D9Ex being disabled.");

	auto create_ret = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9ex);
	if (FAILED(create_ret) || d3d9ex == nullptr)
		panic_msgbox(L"Direct3DCreate9Ex failed!\nCode: 0x%x", create_ret);

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
			MessageBox(
				NULL,
				L"Couldn't hook Direct3DCreate9.\n"
				L"This usually means you don't have a D3D8 wrapper installed or the one you have is incompatible.\n"
				L"The game will run fine, but D3D9Ex features will be unavailable and you'll have worse input lag.\n"
				L"Please install d3d8to9 for best results.",
				L"OpenInputLagPatch",
				MB_ICONWARNING
			);
		}
	}
}