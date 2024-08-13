#ifndef SHADER_HANDLER_C_
#define SHADER_HANDLER_C_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int vert_shader, frag_shader, program;
} C_Shader;

void populate_shader(C_Shader* shader, const char* vert_path, const char* frag_path);
void use(C_Shader* shader);

#endif