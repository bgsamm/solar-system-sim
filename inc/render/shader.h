#pragma once

// TODO(sean) I'd really like to avoid including HMM in headers
#include <HandmadeMath.h>

typedef struct {
    unsigned int id;
    int model_mtx_loc;
    int project_mtx_loc;
    int view_mtx_loc;
} VertexShader;

typedef struct {
    unsigned int id;
} FragmentShader;

typedef struct {
    unsigned int id;
    VertexShader vert_shader;
    FragmentShader frag_shader;
} ShaderProgram;

void shader_create_shaders(ShaderProgram *shader_prog);
int shader_reload_shaders(ShaderProgram *shader_prog);
// TODO(sean) Pass by reference?
void shader_set_matrices(ShaderProgram *shader_prog, HMM_Mat4 model_mtx, HMM_Mat4 view_mtx,
                         HMM_Mat4 project_mtx);
