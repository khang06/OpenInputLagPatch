#pragma once
#include <d3d9.h>

static IDirect3DDevice9* d3d9_device;

IDirect3D9* WINAPI Direct3DCreate9_hook(UINT SDKVersion);
void hook_d3d9();