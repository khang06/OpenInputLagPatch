// Hooks D3DX9 functions for Direct3D 9Ex compatibility

#include <Windows.h>
#include <d3d9.h>
#include <stdio.h>
#include "patch_util.h"

// Replaces D3DPOOL_MANAGED with D3DPOOL_DEFAULT because it's deprecated on D3D9Ex
typedef HRESULT(__stdcall* D3DXCreateTexture_ty)(LPDIRECT3DDEVICE9, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, LPDIRECT3DTEXTURE9*);
auto D3DXCreateTexture_orig = (D3DXCreateTexture_ty)nullptr;
HRESULT __stdcall D3DXCreateTexture_hook(LPDIRECT3DDEVICE9 pDevice, UINT Width, UINT Height, UINT MipLevels,
	DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9* ppTexture)
{
	if (Pool == D3DPOOL_MANAGED) {
		Pool = D3DPOOL_DEFAULT;
		Usage |= D3DUSAGE_DYNAMIC;
	}
	return D3DXCreateTexture_orig(pDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
}

// Same as D3DXCreateTexture_hook
// The amount of parameters on this holy fuck
typedef HRESULT(__stdcall* D3DXCreateTextureFromFileInMemoryEx_ty)(LPDIRECT3DDEVICE9, LPCVOID, UINT, UINT, UINT, UINT, DWORD, D3DFORMAT,
	D3DPOOL, DWORD, DWORD, D3DCOLOR, void*, PALETTEENTRY*, LPDIRECT3DTEXTURE9*);
auto D3DXCreateTextureFromFileInMemoryEx_orig = (D3DXCreateTextureFromFileInMemoryEx_ty)nullptr;
HRESULT __stdcall D3DXCreateTextureFromFileInMemoryEx_hook(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize,
	UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter,
	D3DCOLOR ColorKey, void* pSrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture)
{
	if (Pool == D3DPOOL_MANAGED) {
		Pool = D3DPOOL_DEFAULT;
		Usage |= D3DUSAGE_DYNAMIC;
	}
	return D3DXCreateTextureFromFileInMemoryEx_orig(pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format,
		Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
}

void hook_d3dx9() {
	// Try to IAT hook every possible D3DX9 version
	// Inefficient but it works
	for (int i = 37; i <= 43; i++) {
		char dll[128] = {};
		sprintf_s(dll, "d3dx9_%d.dll", i);

		auto orig = iat_hook(NULL, dll, "D3DXCreateTexture", (void*)&D3DXCreateTexture_hook);
		if (orig)
			D3DXCreateTexture_orig = (D3DXCreateTexture_ty)orig;
		orig = iat_hook(NULL, dll, "D3DXCreateTextureFromFileInMemoryEx", (void*)&D3DXCreateTextureFromFileInMemoryEx_hook);
		if (orig)
			D3DXCreateTextureFromFileInMemoryEx_orig = (D3DXCreateTextureFromFileInMemoryEx_ty)orig;
	}
}