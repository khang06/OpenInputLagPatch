#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include "overlay_font.h"
#include "d3d9_overlay.h"

D3D9Overlay* D3D9Overlay::Instance = nullptr;

struct CUSTOMVERTEX {
    float pos[3];
    D3DCOLOR col;
    float uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

D3D9Overlay::D3D9Overlay(IDirect3DDevice9* device, int width, int height) {
    d3d9_device = device;
    window_width = width;
    window_height = height;
    memset(text_buffer, 0, sizeof(text_buffer));

    SetupRenderState();
    SetupResources();
    SetupAtlasUVTable();
}

D3D9Overlay::~D3D9Overlay() {
    if (d3d9_state_block != nullptr)
        d3d9_state_block->Release();
    if (d3d9_font_tex != nullptr)
        d3d9_font_tex->Release();
    if (d3d9_vertex_buf != nullptr)
        d3d9_vertex_buf->Release();
    if (d3d9_index_buf != nullptr)
        d3d9_index_buf->Release();
}

// Sets up the desired render state and caches it into a state block
void D3D9Overlay::SetupRenderState() {
    // From https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_dx9.cpp
    // The game might rely on default values during initialization, so we'll save the default state
    IDirect3DStateBlock9* original_state = NULL;
    d3d9_device->CreateStateBlock(D3DSBT_ALL, &original_state);
    original_state->Capture();

    // Viewport
    D3DVIEWPORT9 viewport = {};
    viewport.Width = window_width;
    viewport.Height = window_height;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.MinZ = 0.0;
    viewport.MaxZ = 0.0;
    d3d9_device->SetViewport(&viewport);

    // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, fill mode, point sampling
    d3d9_device->SetPixelShader(NULL);
    d3d9_device->SetVertexShader(NULL);
    d3d9_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    d3d9_device->SetRenderState(D3DRS_SHADEMODE, D3DRS_FILLMODE);
    d3d9_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    d3d9_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    d3d9_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    d3d9_device->SetRenderState(D3DRS_ZENABLE, FALSE);
    d3d9_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    d3d9_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    d3d9_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3d9_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3d9_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    d3d9_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    d3d9_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    d3d9_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    d3d9_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d3d9_device->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    d3d9_device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    d3d9_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    d3d9_device->SetRenderState(D3DRS_CLIPPING, TRUE);
    d3d9_device->SetRenderState(D3DRS_LIGHTING, FALSE);
    d3d9_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    d3d9_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    d3d9_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    d3d9_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    d3d9_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    d3d9_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    d3d9_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    d3d9_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    d3d9_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    d3d9_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    // Orthographic projection matrix
    float L = 0.5f;
    float R = window_width + 0.5f;
    float T = 0.5f;
    float B = window_height + 0.5f;
    D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
    D3DMATRIX mat_projection =
    { { {
        2.0f / (R - L),    0.0f,               0.0f,  0.0f,
        0.0f,              2.0f / (T - B),     0.0f,  0.0f,
        0.0f,              0.0f,               0.5f,  0.0f,
        (L + R) / (L - R), (T + B) / (B - T),  0.5f,  1.0f
    } } };
    d3d9_device->SetTransform(D3DTS_WORLD, &mat_identity);
    d3d9_device->SetTransform(D3DTS_VIEW, &mat_identity);
    d3d9_device->SetTransform(D3DTS_PROJECTION, &mat_projection);

    // Save the current state
    d3d9_device->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block);
    d3d9_state_block->Capture();

    // Restore the original state
    original_state->Apply();
    original_state->Release();
}

// Decompresses and loads the font atlas to an A8R8G8B8 texture and sets up other resources
// This is overkill but pretty much guaranteed to work on anything
void D3D9Overlay::SetupResources() {
    // Create a temporary texture for uploading texture data from CPU
    // Hardcoded size rounded up to the nearest power of two
    IDirect3DTexture9* temp_tex;
    auto ret = d3d9_device->CreateTexture(FONT_ATLAS_WIDTH_P2, FONT_ATLAS_HEIGHT_P2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &temp_tex, NULL);
    if (FAILED(ret))
        panic_msgbox(L"Temporary font texture CreateTexture failed! Error: 0x%x", ret);

    // Map the texture into memory
    D3DLOCKED_RECT locked_rect;
    RECT target_rect = { 0, 0, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT };
    ret = temp_tex->LockRect(0, &locked_rect, &target_rect, 0);
    if (FAILED(ret))
        panic_msgbox(L"Temporary font texture LockRect failed! Error: 0x%x", ret);

    // Decompress and load the font atlas
    size_t cur_pos = 0;
    char cur_byte = 0;
    unsigned bits_left = 0;
    for (int y = 0; y < FONT_ATLAS_HEIGHT; y++) {
        for (int x = 0; x < FONT_ATLAS_WIDTH; x++) {
            if (bits_left == 0) {
                cur_byte = compressed_font_atlas[cur_pos++];
                bits_left = 8;
            }
            ((uint32_t*)locked_rect.pBits)[y * (locked_rect.Pitch / 4) + x] = cur_byte & 0x80 ? 0xFFFFFFFF : 0x00000000;
            cur_byte <<= 1;
            bits_left -= 1;
        }
    }
    temp_tex->UnlockRect(0);

    // Create the final texture and copy the temporary texture to it
    ret = d3d9_device->CreateTexture(FONT_ATLAS_WIDTH_P2, FONT_ATLAS_HEIGHT_P2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3d9_font_tex, NULL);
    if (FAILED(ret))
        panic_msgbox(L"Font texture CreateTexture failed! Error: 0x%x", ret);
    d3d9_device->UpdateTexture(temp_tex, d3d9_font_tex);

    // Clean up
    temp_tex->Release();

    // Set up the vertex and index buffer
    // Only supports a few characters since not much text is supposed to be written anyway
    // Dynamic vertex buffer resizing can be added later if needed
    // 1 is added for the background of the stuff
    ret = d3d9_device->CreateVertexBuffer((OVERLAY_MAX_CHARS + 1) * 4 * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC,
        D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &d3d9_vertex_buf, NULL);
    if (FAILED(ret))
        panic_msgbox(L"CreateVertexBuffer failed! Error: 0x%x", ret);
    ret = d3d9_device->CreateIndexBuffer((OVERLAY_MAX_CHARS + 1) * 2, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
        &d3d9_index_buf, NULL);
    if (FAILED(ret))
        panic_msgbox(L"CreateIndexBuffer failed! Error: 0x%x", ret);
}

// Sets up the font atlas UV lookup table
void D3D9Overlay::SetupAtlasUVTable() {
    // Width of 8, height of 8, underhang of 4, and padding of 1
    for (int y = 0; y < 96 / 16; y++) {
        for (int x = 0; x < 16; x++) {
            atlas_uvs[y * 16 + x][0] = (x * 9.0f / (float)FONT_ATLAS_WIDTH) * (FONT_ATLAS_WIDTH / (float)FONT_ATLAS_WIDTH_P2);
            atlas_uvs[y * 16 + x][1] = (y * 13.0f / (float)FONT_ATLAS_HEIGHT) * (FONT_ATLAS_HEIGHT / (float)FONT_ATLAS_HEIGHT_P2);
        }
    }
    char_width = (8.0f / (float)FONT_ATLAS_WIDTH) * (FONT_ATLAS_WIDTH / (float)FONT_ATLAS_WIDTH_P2);
    char_height = (12.0f / (float)FONT_ATLAS_HEIGHT) * (FONT_ATLAS_HEIGHT / (float)FONT_ATLAS_HEIGHT_P2);
}

int D3D9Overlay::UpdateBuffers(char* text) {
    // Get the length of the string
    auto text_len = strlen(text);
    if (text_len > OVERLAY_MAX_CHARS)
        panic_msgbox(L"Tried to draw too many characters");

    // Lock the buffers for writing
    CUSTOMVERTEX* vertex_data;
    d3d9_vertex_buf->Lock(0, (text_len + 1) * 4 * sizeof(CUSTOMVERTEX), (void**)&vertex_data, D3DLOCK_DISCARD);
    uint16_t* index_data;
    d3d9_index_buf->Lock(0, (text_len + 1) * 2 * 3 * sizeof(uint16_t), (void**)&index_data, D3DLOCK_DISCARD);

    // Calculate the offsets
    float padding = 1.0f;
    float x_offset = 0;
    float y_offset = (float)window_height - 12.0f - padding * 2.0f;

    // Write the background geometry
    // HACK: The bottom right pixel of the atlas is set so I don't have to switch textures for this
    float epsilon = 0.0001f;
    float pixel_u = FONT_ATLAS_WIDTH / (float)FONT_ATLAS_WIDTH_P2 - epsilon;
    float pixel_v = FONT_ATLAS_HEIGHT / (float)FONT_ATLAS_HEIGHT_P2 - epsilon;
    auto bg_color = D3DCOLOR_COLORVALUE(0.0, 0.0, 0.0, 0.8);
    float bg_width = (float)text_len * 7.0f + padding * 2.0f;
    float bg_height = 12.0f + padding * 2.0f;
    vertex_data[0] = CUSTOMVERTEX{
        {x_offset + bg_width, y_offset + bg_height, 0.5},
        bg_color,
        {pixel_u + epsilon, pixel_v + epsilon}
    };
    vertex_data[1] = CUSTOMVERTEX{
        {x_offset + bg_width, y_offset, 0.5},
        bg_color,
        {pixel_u + epsilon, pixel_v}
    };
    vertex_data[2] = CUSTOMVERTEX{
        {x_offset, y_offset, 0.5},
        bg_color,
        {pixel_u, pixel_v}
    };
    vertex_data[3] = CUSTOMVERTEX{
        {x_offset, y_offset + bg_height, 0.5},
        bg_color,
        {pixel_u, pixel_v + epsilon}
    };
    index_data[0] = 0; index_data[1] = 1; index_data[2] = 3;
    index_data[3] = 1; index_data[4] = 2; index_data[5] = 3;

    // Write the text geometry
    size_t cur_vertex = 4;
    size_t cur_index = 6;
    float cur_xpos = 0.0;
    for (unsigned i = 0; i < text_len; i++) {
        // Ensure that this character is actually ASCII text
        // Incorrectly skipped but this is just a safeguard anyway
        if (text[i] < 32 || text[i] > 126)
            continue;

        float u = atlas_uvs[text[i] - 32][0];
        float v = atlas_uvs[text[i] - 32][1];
        vertex_data[cur_vertex + 0] = CUSTOMVERTEX{
            {x_offset + cur_xpos + padding + 8.0f, y_offset + padding * 2.0f + 12.0f, 0.5f},
            text_color,
            {u + char_width, v + char_height}
        };
        vertex_data[cur_vertex + 1] = CUSTOMVERTEX{
            {x_offset + cur_xpos + padding + 8.0f, y_offset + padding * 2.0f, 0.5},
            text_color,
            {u + char_width, v}
        };
        vertex_data[cur_vertex + 2] = CUSTOMVERTEX{
            {x_offset + cur_xpos + padding, y_offset + padding * 2.0f, 0.5},
            text_color,
            {u, v}
        };
        vertex_data[cur_vertex + 3] = CUSTOMVERTEX{
            {x_offset + cur_xpos + padding, y_offset + padding * 2.0f + 12.0f, 0.5},
            text_color,
            {u, v + char_height}
        };
        index_data[cur_index + 0] = (uint16_t)cur_vertex + 0;
        index_data[cur_index + 1] = (uint16_t)cur_vertex + 1;
        index_data[cur_index + 2] = (uint16_t)cur_vertex + 3;
        index_data[cur_index + 3] = (uint16_t)cur_vertex + 1;
        index_data[cur_index + 4] = (uint16_t)cur_vertex + 2;
        index_data[cur_index + 5] = (uint16_t)cur_vertex + 3;

        cur_vertex += 4;
        cur_index += 6;
        cur_xpos += 7.0f; // This is intentional, otherwise the text will be too spaced out
    }

    // Unlock the buffers
    d3d9_vertex_buf->Unlock();
    d3d9_index_buf->Unlock();

    return text_len + 1;
}

void D3D9Overlay::Draw() {
    // Save the current render state so the game's graphics don't get messed up
    IDirect3DStateBlock9* original_state = NULL;
    d3d9_device->CreateStateBlock(D3DSBT_ALL, &original_state);
    original_state->Capture();

    // Load the desired text rendering render state
    d3d9_state_block->Apply();

    // Draw shit
    auto rect_count = UpdateBuffers(text_buffer);

    d3d9_device->SetStreamSource(0, d3d9_vertex_buf, 0, sizeof(CUSTOMVERTEX));
    d3d9_device->SetIndices(d3d9_index_buf);
    d3d9_device->SetFVF(D3DFVF_CUSTOMVERTEX);
    d3d9_device->SetTexture(0, d3d9_font_tex);
    d3d9_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rect_count * 2 * 3, 0, rect_count * 2);

    // Restore the original render state
    original_state->Apply();
    original_state->Release();
}

void D3D9Overlay::SetText(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf_s(text_buffer, format, args);
    va_end(args);
}