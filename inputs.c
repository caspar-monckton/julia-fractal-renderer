#include "inputs.h"
#include "launcher.h"

#define STEP_ADJUSTMENT_SIZE 0.001
#define ITERATION_ADJUSTMENT_SIZE 1
int mouse_button_state_left = 0;
int mouse_button_state_right = 0;

double start_drag_x = 0;
double start_drag_y = 0;

float centre_x_at_start_drag;
float centre_y_at_start_drag;

void process_input(GLFWwindow *window) {
    FractalWindowCTX *ctx = query_ctx();
    int prev_mouse_button_state_left = mouse_button_state_left;
    int prev_mouse_button_state_right = mouse_button_state_right;

    mouse_button_state_left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    mouse_button_state_right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    double c_xpos, c_ypos;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ctx->centre_y += STEP_ADJUSTMENT_SIZE * ctx->zoom;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ctx->centre_y -= STEP_ADJUSTMENT_SIZE * ctx->zoom;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        ctx->centre_x += STEP_ADJUSTMENT_SIZE * ctx->zoom;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        ctx->centre_x -= STEP_ADJUSTMENT_SIZE * ctx->zoom;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        ctx->max_iterations -= ITERATION_ADJUSTMENT_SIZE;
        if (ctx->max_iterations <= 0) {
            ctx->max_iterations += ITERATION_ADJUSTMENT_SIZE;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        ctx->max_iterations += ITERATION_ADJUSTMENT_SIZE;
    }

    if ((mouse_button_state_left == GLFW_PRESS) && (prev_mouse_button_state_left == GLFW_RELEASE)) {
        glfwGetCursorPos(window, &start_drag_x, &start_drag_y);
        centre_x_at_start_drag = ctx->screen_position_x;
        centre_y_at_start_drag = ctx->screen_position_y;
        // printf("drag starting at: (%f, %f)\n", centre_x_at_start_drag, centre_y_at_start_drag);
    }

    if ((mouse_button_state_left == GLFW_PRESS)) {
        glfwGetCursorPos(window, &c_xpos, &c_ypos);
        ctx->screen_position_x = centre_x_at_start_drag + (c_xpos - start_drag_x) * ctx->zoom;
        ctx->screen_position_y = centre_y_at_start_drag - (c_ypos - start_drag_y) * ctx->zoom;
        // printf("centre: (%f, %f)\n", ctx->screen_position_x, ctx->screen_position_y);
    }

    if ((mouse_button_state_left == GLFW_RELEASE)) {
        start_drag_x = 0;
        start_drag_y = 0;
    }
}