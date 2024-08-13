#include "shader_handler.h"
#include "include/glad.h"
#include "include/glfw3.h"
#include "launcher.h"
#include "inputs.h"

#define FRAMERATE 120
#define ZOOM_MULTIPLIER 0.85
#define SCREEN_SIZE 800
#define FPS_SAMPLE_RATE 4

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

FractalWindowCTX f_ctx;

int main(void) {
    GLFWwindow* window;
    f_ctx.centre_x = 0;
    f_ctx.centre_y = 0;
    f_ctx.screen_position_x = 0;
    f_ctx.screen_position_x = 0;
    f_ctx.zoom = 1;
    f_ctx.max_iterations = 400;

    int current_fps = FRAMERATE;

    float triangle_vertices[] = {
        1.0f,  1.0f,  0.0f,  // top right
        1.0f,  -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f, 1.0f,  0.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCREEN_SIZE, SCREEN_SIZE, "Julia set renderer", NULL, NULL);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (window == NULL) {
        printf("Window pointer is null.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialise GLAD.\n");
        return -1;
    }

    glViewport(0, 0, SCREEN_SIZE, SCREEN_SIZE);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    C_Shader shader;

    populate_shader(&shader, "shaders/verts.vts", "shaders/julia_frags.frs");
    glUseProgram(shader.program);

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // game loop
    double currentTime, lastTime;
    glUniform3f(glGetUniformLocation(shader.program, "colour_1"), 0, 0, 0);
    glUniform3f(glGetUniformLocation(shader.program, "colour_2"), 131.0f / 255.0f, 182.0f / 255.0f,
                146.0f / 255.0f);
    glUniform3f(glGetUniformLocation(shader.program, "colour_3"), 249.0f / 255.0f, 173.0f / 255.0f,
                160.0f / 255.0f);
    glUniform3f(glGetUniformLocation(shader.program, "colour_4"), 174.0f / 255.0f, 148.0f / 255.0f,
                179.0f / 255.0f);
    glUniform3f(glGetUniformLocation(shader.program, "colour_5"), 177.0f / 255.0f, 195.0f / 255.0f,
                252.0f / 255.0f);

    double start_second = glfwGetTime();
    int frame_counter = 0;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();

        if (currentTime - start_second >= 1.0 / FPS_SAMPLE_RATE) {
            start_second = currentTime;

            current_fps = frame_counter * FPS_SAMPLE_RATE;
            frame_counter = 0;
            // terminal info:
            printf("\033[2J\033[HFPS: %d\nIteration cap: %d\n", current_fps, f_ctx.max_iterations);
        }

        if (currentTime - lastTime >= 1.0 / FRAMERATE) {
            frame_counter++;
            lastTime = currentTime;
            // render
            process_input(window);
            glfwGetWindowSize(window, &(f_ctx.s_width), &(f_ctx.s_height));

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUniform1f(glGetUniformLocation(shader.program, "centre_x"), f_ctx.centre_x);
            glUniform1f(glGetUniformLocation(shader.program, "centre_y"), f_ctx.centre_y);
            glUniform1f(glGetUniformLocation(shader.program, "posx"), f_ctx.screen_position_x);
            glUniform1f(glGetUniformLocation(shader.program, "posy"), f_ctx.screen_position_y);
            glUniform1f(glGetUniformLocation(shader.program, "zoom"), f_ctx.zoom);
            glUniform1f(glGetUniformLocation(shader.program, "screen_width"), (float)f_ctx.s_width);
            glUniform1f(glGetUniformLocation(shader.program, "screen_height"),
                        (float)f_ctx.s_height);
            glUniform1i(glGetUniformLocation(shader.program, "max_iter"), f_ctx.max_iterations);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
            // end render
        }
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset == 1) {
        f_ctx.zoom *= ZOOM_MULTIPLIER;
    }
    if (yoffset == -1) {
        f_ctx.zoom /= ZOOM_MULTIPLIER;
    }
}

FractalWindowCTX* query_ctx() {
    return &f_ctx;
}
