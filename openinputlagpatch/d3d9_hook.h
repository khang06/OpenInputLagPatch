#pragma once
#include <d3d9.h>

static IDirect3DDevice9Ex* d3d9ex_device;

IDirect3D9* WINAPI Direct3DCreate9_hook(UINT SDKVersion);
void hook_d3d9();