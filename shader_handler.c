#include "shader_handler.h"
#include "include/glad.h"
#include "include/glfw3.h"

char* read_shader_file(FILE* fp);

void populate_shader(C_Shader* shader, const char* vert_path, const char* frag_path) {
    FILE* fp;

    fp = fopen(vert_path, "r");

    if (fp == NULL) {
        printf("File at \"%s\" could not be found.\n", vert_path);
        return;
    }
    printf("Reading vertex shader at \"%s\". \n", vert_path);
    char* vert_shader_source = read_shader_file(fp);
    fclose(fp);
    printf("Vertex shader read successfully.\n");

    fp = fopen(frag_path, "r");

    if (fp == NULL) {
        printf("File at \"%s\" could not be found.\n", frag_path);
        return;
    }
    printf("Reading fragment shader at \"%s\". \n", frag_path);
    char* frag_shader_source = read_shader_file(fp);
    fclose(fp);
    printf("Fragment shader read successfully.\n");

    const char* vertex_shader_source = vert_shader_source;
    const char* fragment_shader_source = frag_shader_source;

    shader->vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader->vert_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(shader->vert_shader);

    // check for compilation errors:
    int success;
    char infoLog[512];
    glGetShaderiv(shader->vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader->vert_shader, 512, NULL, infoLog);
        printf("ERROR: Vertex shader compilation failed:\n %s\n", infoLog);
    }

    shader->frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader->frag_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(shader->frag_shader);

    glGetShaderiv(shader->frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader->vert_shader, 512, NULL, infoLog);
        printf("ERROR: Fragment shader compilation failed:\n %s\n", infoLog);
    }

    shader->program = glCreateProgram();
    glAttachShader(shader->program, shader->vert_shader);
    glAttachShader(shader->program, shader->frag_shader);
    glLinkProgram(shader->program);

    // check for linker errors
    glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader->program, 512, NULL, infoLog);
        printf("ERROR. Shader program linking failed: %s\n", infoLog);
    }
    free(vert_shader_source);
    free(frag_shader_source);

    glDeleteShader(shader->vert_shader);
    glDeleteShader(shader->frag_shader);
}

void use(C_Shader* shader) {
}

char* read_shader_file(FILE* fp) {
    char* shader_source = NULL;
    /*credit to https://stackoverflow.com/users/217476/michael for buffer reading code:*/
    if (fseek(fp, 0L, SEEK_END) == 0) {
        /* Get the size of the file. */
        long bufsize = ftell(fp);
        if (bufsize == -1) {
            printf("Could not parse shader file size.\n"); /* Error */
        }

        /* Allocate our buffer to that size. */
        shader_source = malloc(sizeof(char) * (bufsize + 1));

        /* Go back to the start of the file. */
        if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */
        }

        /* Read the entire file into memory. */
        size_t newLen = fread(shader_source, sizeof(char), bufsize, fp);
        if (ferror(fp) != 0) {
            fputs("Error reading file", stderr);
        } else {
            shader_source[newLen++] = '\0'; /* Just to be safe. */
        }
    }
    return shader_source;
}
