#pragma once
#include "scene.h"
#include "camera.h"
#include "shader.h"

#include <glm.hpp>

#include <mutex>

class c_shader;

class c_render
{
    c_scene* scene = nullptr;
    c_camera* camera = nullptr;

    unsigned int cube_vao = NULL;
    unsigned int cube_vbo = NULL;

    unsigned int cube_outline_vao = NULL;
    unsigned int cube_outline_vbo = NULL;

    int depth_map_width = 1024, depth_map_height = 1024;
    int main_viewport_width = 1, main_viewport_height = 1;

    unsigned int depth_map = NULL;
    unsigned int depth_map_fbo = NULL;

    c_shader* depth_buffer_shader = NULL;
    c_shader* direct_lighting_shader = NULL;
    c_shader* outline_shader = NULL;
    c_shader* decal_shader = NULL;

    c_shader* current_shader = NULL;

    std::mutex free_mutex;
    std::vector<c_scene_object*> free_queue;

    bool active = false;
public:
    bool get_active() { return this->active; };
    void set_active(bool active) { this->active = active; };

    c_scene* get_scene() { return this->scene; };
    void set_scene(c_scene* scene) { this->scene = scene; };

    c_camera* get_camera() { return this->camera; };
    void set_camera(c_camera* camera) { this->camera = camera; };

    c_shader* get_shader() { return this->current_shader; };
    void set_shader(c_shader* shader) { this->current_shader = shader; if (shader) { shader->use(); shader->set_mat4("view", camera->get_view_matrix()); shader->set_mat4("projection", camera->get_projection_matrix()); } };

    void request_free(c_scene_object* object) { /*std::lock_guard<std::mutex> free_guard(free_mutex);*/ this->free_queue.push_back(object); };

    void compile_shaders();

    GLuint bake_model_icon(c_model* model, int size = 64);

    void render_cube_outline();
    void render_cube_textured();

    void prepare_chunk_transform(glm::ivec2 index);
    void prepare_block_transform(const glm::ivec3& block_pos);
    void prepare_mesh_transform(c_mesh* mesh);

    void render_chunks(bool process_translucent);
    void render_entities(bool process_local);

    void prepare_cube_vbo();
    void render_block_outline();
    void render_block_decals();

    void render_scene(bool depth = false, bool process_translucent = true);
    void render_main();

    void render_viewmodel();

    void initialize();
    void initialize_depth_map();

    void run();
};

extern c_render* g_render;
