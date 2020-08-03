#include "draw.h"

#include <hal/video.h>
#include <xgu/xgu.h>
#include <xgu/xgux.h>

// Use freetype for font stuff
#include <ft2build.h>
#include FT_FREETYPE_H

void xbox_draw_init() {
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    
    pb_init();
    pb_show_front_screen();
    
    uint32_t *p = pb_begin();
    
    // Just default to the sprite pixel combiner setup
    #include "../shaders/sprite-frag.inl"
    
    const float m_identity[4*4] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    // Basic "alpha = transparency" setup
    p = xgu_set_blend_enable(p, true);
    p = xgu_set_blend_func_sfactor(p, XGU_FACTOR_SRC_ALPHA);
    p = xgu_set_blend_func_dfactor(p, XGU_FACTOR_ONE_MINUS_SRC_ALPHA);
    
    // Here, we'll set some default GPU states to be safe,
    // As well as the more important MVP/Viewport matrices
    p = xgu_set_skin_mode(p, XGU_SKIN_MODE_OFF);
    p = xgu_set_normalization_enable(p, false);
    p = xgu_set_lighting_enable(p, false);
    
    for(int i = 0; i < XGU_TEXTURE_COUNT; i++) {
        p = xgu_set_texgen_s(p, i, XGU_TEXGEN_DISABLE);
        p = xgu_set_texgen_t(p, i, XGU_TEXGEN_DISABLE);
        p = xgu_set_texgen_r(p, i, XGU_TEXGEN_DISABLE);
        p = xgu_set_texgen_q(p, i, XGU_TEXGEN_DISABLE);
        p = xgu_set_texture_matrix_enable(p, i, false);
        p = xgu_set_texture_matrix(p, i, m_identity);
    }
    
    for(int i = 0; i < XGU_WEIGHT_COUNT; i++) {
        p = xgu_set_model_view_matrix(p, i, m_identity);
        p = xgu_set_inverse_model_view_matrix(p, i, m_identity);
    }
    
    p = xgu_set_transform_execution_mode(p, XGU_FIXED, XGU_RANGE_MODE_PRIVATE);
    p = xgu_set_projection_matrix(p, m_identity);
    p = xgu_set_composite_matrix(p, m_identity);
    p = xgu_set_viewport_offset(p, 0.0f, 0.0f, 0.0f, 0.0f);
    p = xgu_set_viewport_scale(p, 1.0f, 1.0f, 1.0f, 1.0f);
    
    pb_end(p);
}

void xbox_clear_screen(uint32_t clr) {
    uint32_t *p = pb_begin();
    
    p = xgu_set_color_clear_value(p, clr);
    p = xgu_clear_surface(p, (XguClearSurface)(XGU_CLEAR_Z | XGU_CLEAR_STENCIL | XGU_CLEAR_COLOR));
    
    pb_end(p);
}

uint32_t *xbox_load_texture(const uint8_t texture_rgba[], int texture_width, int texture_height) {
    // TODO: We could load image data from files here instead of hardcoded data so we don't have to
    // manage extra files in the source tree, as well as enabling easier user customization of the artwork
    
    uint32_t *alloc_texture = (uint32_t *)MmAllocateContiguousMemoryEx((texture_width * 4) * texture_height, 0, 0x03FFAFFF, 0, PAGE_WRITECOMBINE | PAGE_READWRITE);
    memcpy(alloc_texture, texture_rgba, (texture_width * 4) * texture_height);
    
    return alloc_texture;
}

void xbox_free_texture(uint32_t *alloc_texture) {
    MmFreeContiguousMemory(alloc_texture);
}

void xbox_draw_sprite(int xpos, int ypos, int width, int height, uint32_t *texture, int texture_width, int texture_height) {
    uint32_t *p = pb_begin();
    
    #include "../shaders/sprite-frag.inl"
    
    // Update texture 0 properties
    p = xgu_set_texture_offset(p, 0, (void *)((uint32_t)texture & 0x03ffffff));
    p = xgu_set_texture_format(p, 0, 2, false, XGU_SOURCE_COLOR, 2, XGU_TEXTURE_FORMAT_A8B8G8R8, 1, 0, 0, 0);
    p = xgu_set_texture_control0(p, 0, true, 0, 0);
    p = xgu_set_texture_control1(p, 0, texture_width * 4);
    p = xgu_set_texture_image_rect(p, 0, texture_width, texture_height);
    p = xgu_set_texture_filter(p, 0, 0, XGU_TEXTURE_CONVOLUTION_GAUSSIAN, 4, 4, false, false, false, false);
    
    p = xgu_begin(p, XGU_TRIANGLE_STRIP);
    
    p = xgux_set_color3f(p, 1.0f, 1.0f, 1.0f); // TODO: Add args for these?
    
    p = xgux_set_texcoord3f(p, 0, 0, 0, 1);                                        p = xgu_vertex4f(p, (float)xpos, (float)ypos, 1, 1);
    p = xgux_set_texcoord3f(p, 0, (float)texture_width, 0, 1);                     p = xgu_vertex4f(p, (float)(xpos + width), (float)ypos, 1, 1);
    p = xgux_set_texcoord3f(p, 0, 0, (float)texture_height, 1);                    p = xgu_vertex4f(p, (float)xpos, (float)(ypos + height), 1, 1);
    p = xgux_set_texcoord3f(p, 0, (float)texture_width, (float)texture_height, 1); p = xgu_vertex4f(p, (float)(xpos + width), (float)(ypos + height), 1, 1);
    
    p = xgu_end(p);
    
    pb_end(p);
}

void xbox_draw_trajectory(int playerX, int playerY, double velocityX, double velocityY) {
    uint32_t *p = pb_begin();

    #include "../shaders/guideline-frag.inl"
    
    p = xgu_begin(p, XGU_LINE_STRIP);
    
    p = xgux_set_color3f(p, 1.0f, 0.0f, 0.0f); // TODO: Add args for these?
    
    double XvelStep = velocityX;
    double YvelStep = velocityY;
    
    double XStep = playerX;
    double YStep = playerY;
    
    int gravity = 1; // TODO: Cleanup
    
    for(int i = 0; i < 32; i++) {
        p = xgu_vertex4f(p, (float)XStep, (float)YStep, 1, 1);
        
        XStep += XvelStep;
        YStep += YvelStep;
        
        YvelStep += gravity;
        
        // FIXME: Not accurate; will cause trajectory to be off slightly
        if (XStep < 16) {
            XvelStep *= -1;
        }
        if (XStep + 16 > 640) { // TODO: Cleanup
            XvelStep *= -1;
        }
    }
    
    p = xgu_end(p);
    
    pb_end(p);
}

struct Character {
    uint32_t *TextureMem;
    int width;
    int height;
    int left;
    int top;
    long Advance;
};

Character *Characters;

void xbox_text_init(int size) {
    FT_Library ft;
    FT_Init_FreeType(&ft);
    
    FT_Face face;
    FT_New_Face(ft, "D:\\media\\font.ttf", 0, &face); // TODO: Add arg for font path as well?
    
    FT_Set_Pixel_Sizes(face, 0, size);
    
    Characters = (Character *)MmAllocateContiguousMemoryEx(256 * sizeof(Character), 0, 0x03FFAFFF, 0, PAGE_WRITECOMBINE | PAGE_READWRITE);
    
    for(int i = 0; i < 256; i++) {
        FT_Load_Char(face, i, FT_LOAD_RENDER);
        
        // FIXME(?): XQEMU/xemu emulators hang here, but this behaves fine on hardware
        uint32_t *alloc_texture = (uint32_t *)MmAllocateContiguousMemoryEx(face->glyph->bitmap.width * face->glyph->bitmap.rows, 0, 0x03FFAFFF, 0, PAGE_WRITECOMBINE | PAGE_READWRITE);
        memcpy(alloc_texture, face->glyph->bitmap.buffer, face->glyph->bitmap.width * face->glyph->bitmap.rows);
        
        Character character = {
            alloc_texture,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            face->glyph->advance.x
        };
        Characters[i] = character;
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void xbox_draw_text(const char *text, int x, int y, float scale, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t *p = pb_begin();

    #include "../shaders/sprite-frag.inl"
    
    pb_end(p);
    
    for(int i = 0; i < strlen(text); i++) {
        Character ch = Characters[text[i]];
        
        float xpos = x + ch.left * scale;
        float ypos = y - (ch.height - ch.top) * scale;
        
        float w = ch.width * scale;
        float h = ch.height * scale;
        
        // HACK: Set minimum width/height to 8 to prevent GPU crash (use a larger font size to compensate!)
        if(ch.width < 8) ch.width = 8;
        if(ch.height < 8) ch.height = 8;
        
        p = pb_begin();
        
        // Update texture 0 properties (Note the glyph textures only use 1 color channel that controls transparency)
        p = xgu_set_texture_offset(p, 0, (void *)((uint32_t)ch.TextureMem & 0x03ffffff));
        p = xgu_set_texture_format(p, 0, 2, false, XGU_SOURCE_COLOR, 2, XGU_TEXTURE_FORMAT_A8, 1, 0, 0, 0);
        p = xgu_set_texture_control0(p, 0, true, 0, 0);
        p = xgu_set_texture_control1(p, 0, ch.width);
        p = xgu_set_texture_image_rect(p, 0, ch.width, ch.height);
        p = xgu_set_texture_filter(p, 0, 0, XGU_TEXTURE_CONVOLUTION_GAUSSIAN, 4, 4, false, false, false, false);
        
        p = xgu_begin(p, XGU_TRIANGLE_STRIP);
        
        p = xgux_set_color3f(p, (float)r/255.0f, (float)g/255.0f, (float)b/255.0f); // TODO: Cleanup
        
        p = xgux_set_texcoord3f(p, 0, 0, 0, 1);                              p = xgu_vertex4f(p, (float)xpos,       (float)ypos, 1, 1);
        p = xgux_set_texcoord3f(p, 0, (float)ch.width, 0, 1);                p = xgu_vertex4f(p, (float)(xpos + w), (float)ypos, 1, 1);
        p = xgux_set_texcoord3f(p, 0, 0, (float)ch.height, 1);               p = xgu_vertex4f(p, (float)xpos,       (float)(ypos + h), 1, 1);
        p = xgux_set_texcoord3f(p, 0, (float)ch.width, (float)ch.height, 1); p = xgu_vertex4f(p, (float)(xpos + w), (float)(ypos + h), 1, 1);
        
        p = xgu_end(p);
        
        pb_end(p);
        
        x += (ch.Advance >> 6) * scale;
    }
}

void xbox_text_free() {
    for(int i = 0; i < 256; i++) {
        MmFreeContiguousMemory(Characters[i].TextureMem);
    }
    MmFreeContiguousMemory(Characters);
}

void xbox_draw_free() {
    pb_show_debug_screen();
    pb_kill();
}

void xbox_target_back_buffer() {
    pb_wait_for_vbl();
    pb_reset();
    pb_target_back_buffer();
    
    while(pb_busy());
}

void xbox_swap_buffers() {
    // Wait until previous GPU commands have processed
    while(pb_busy());
    while(pb_finished());
}
