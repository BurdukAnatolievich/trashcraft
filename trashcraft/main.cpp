#include "imgui_internal.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GL/glew.h"
#pragma comment(lib, "glew32s.lib")

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <mutex>
#include <shared_mutex>

#include <stdio.h>
#include <Windows.h>
#include <rabbit3d.h>
#include <game.h>
#include <ext/matrix_transform.hpp>
#include <save.h>
#include <multithread.h>


//void game_logic_thread()
//{
//    while (true) {
//        g_game->update();
//    }
//}

// Main code
int main(int, char**)
{
    Threading::InitThreads();

    g_config->initialize();

    //std::thread(game_logic_thread).detach();

    if (!glfwInit())
    {
        MessageBoxA(0, "Failed to initialize GLFW.", "Fatal error.", 0);
        return 1;
    }

    const char* glsl_version = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(g_config->get_screen_resolution().x, g_config->get_screen_resolution().y, "Trashcraft Alpha", nullptr, nullptr);
    if (!window)
    {
        MessageBoxA(0, "Failed to create GLFW window.", "Fatal error.", 0);
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        MessageBoxA(0, "Failed to initialize GLEW.", "Fatal error.", 0);
        return 1;
    }

    g_game->initialize();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        static bool show_world_generator_debug = true;

        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_F3))
            show_world_generator_debug = !show_world_generator_debug;

        g_input->set_mouse_scroll(ImGui::GetIO().MouseWheel);

        g_input->set_mouse_delta(glm::vec2(ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y));
        g_input->set_mouse_position(glm::vec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y));

        g_input->update();
        g_game->update();

        if (g_global_vars->local_player) {
            g_global_vars->local_player->animation_state->update(0, 0);
        }

        g_ui_manager->render();

        if (show_world_generator_debug)
        {
            int tickrate = int(1.0f / g_time->get_interval_per_tick());

            std::stringstream debug_info;
            debug_info << "Trashcraft Alpha v";
            debug_info << TRASHCRAFT_VERSION_STRING << "\n";
            /*debug_info << "Powered by Rabbit3D\n\n"; */

            if (g_global_vars->in_game && g_global_vars->local_player)
            {
                debug_info << "Updated ticks (updated / wanted): " << g_time->get_updated_ticks() << " / " << tickrate << "\n";
            }

            debug_info << ("FPS: " + std::to_string((int)ImGui::GetIO().Framerate)) << " / " << int(1.0 / g_time->get_frame_time()) << "\n";

            if (g_global_vars->in_game && g_global_vars->local_player)
            {
                glm::vec3 position = g_global_vars->local_player->get_origin();
                debug_info << "\nPosition: x = " << (int)position.x << ", y = " << (int)position.y << ", z = " << (int)position.z;
                debug_info << "\nChunk position: x = " << g_global_vars->local_player->chunk.x << ", y = " << g_global_vars->local_player->chunk.y;
                debug_info << "\nInterpolation Amount: " << g_time->get_interpolation_amount() << std::endl;

                debug_info << "\nAngles: Pitch = " << g_global_vars->local_player->view_angle.x << ", Yaw = " << g_global_vars->local_player->view_angle.y << "\n";
                debug_info << "\nHealth: " << std::dec << g_global_vars->local_player->health << "\n";
            }

            ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(18.0f, FLT_MAX, 0.0f, debug_info.str().c_str());
            ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0, 0), text_size + ImVec2(20, 10), ImColor(20, 20, 20, 120));
            ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(10.0f, 5.0f), ImColor(255, 255, 255), debug_info.str().c_str());

            static float noise[1024][1024];
            static siv::PerlinNoise noi;

            ImGui::Begin("DEBUG", (bool*)0);
            {

                //static float ff = 0.02f;
                //static float pers = 0.5f;
                //ImGui::SliderFloat("Noise frequency", &ff, 0.0f, 1.0f);
                //ImGui::SliderFloat("Persistance", &pers, 0.0f, 2.0f);

                //static int oct = 0;
                //ImGui::SliderInt("Octaves", &oct, 1, 32);

                //if (ImGui::Button("Generate noise"))
                //{
                //    for (int x = 15360; x < 16384; x++)
                //    {
                //        for (int y = 15360; y < 16384; y++)
                //            noise[x - 15360][y - 15360] = noi.octave2D_01((double)x * (double)ff, (double)y * (double)ff, oct, (double)pers);
                //    }
                //}

                //int x1 = 0, y1 = 0;
                //for (int x = 0; x < 1024; x++)
                //{
                //    for (int y = 0; y < 1024; y++)
                //    {
                //        float l = noise[x][y];
                //        ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(10 + x, 10 + y), ImVec2(10 + x + 1, 10 + y + 1), ImColor(l, l, l));

                //        if (ImGui::IsMouseHoveringRect(ImVec2(10 + x, 10 + y), ImVec2(10 + x + 1, 10 + y + 1), false))
                //        {
                //            x1 = x;
                //            y1 = y;
                //        }
                //    }
                //}

                //ImGui::GetForegroundDrawList()->AddText(ImVec2(10 + x1, 10 + y1), ImColor(255, 0, 0), std::to_string(noise[x1][y1]).c_str());
            }
            ImGui::End();
        }

        if (GetAsyncKeyState(VK_XBUTTON2) & 1)
        {
            if (g_input->get_is_locked())
                g_input->unlock();
            else
                g_input->lock();
        }

        g_render->run();
        g_time->ran_update(true);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
