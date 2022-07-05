#pragma once

#include <Windows.h>
#include <d3d9.h>

#define OVERLAY_MAX_CHARS 128

class D3D9Overlay {
public:
	// TODO: Release this properly instead of leaking it...
	static D3D9Overlay* Instance;

	D3D9Overlay(IDirect3DDevice9* device, int width, int height);
	~D3D9Overlay();

	void Draw();
	void SetText(const char* format, ...);
	
	D3DCOLOR text_color;

private:
	void SetupRenderState();
	void SetupResources();
	void SetupAtlasUVTable();

	int UpdateBuffers(char* text);

	IDirect3DDevice9* d3d9_device;
	IDirect3DStateBlock9* d3d9_state_block;
	IDirect3DTexture9* d3d9_font_tex;
	IDirect3DVertexBuffer9* d3d9_vertex_buf;
	IDirect3DIndexBuffer9* d3d9_index_buf;
	int window_width;
	int window_height;
	float atlas_uvs[128 - 32][2];
	float char_width;
	float char_height;
	char text_buffer[OVERLAY_MAX_CHARS];
};