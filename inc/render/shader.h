#pragma once

typedef struct {
    unsigned int id;
    unsigned int vert_shader;
    unsigned int frag_shader;
} ShaderProgram;

void render_create_shaders(ShaderProgram *shader_prog);
int render_reload_shaders(ShaderProgram *shader_prog);
