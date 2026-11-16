#include <config.h>
#include <world.h>
#include <rabbit3d.h>

#include <multithreading/multithread.h>

#include "GL/glew.h"
#pragma comment(lib, "glew32s.lib")

#include <glm.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <global_vars.h>

c_render* g_render = new c_render;

void c_render::render_cube_outline()
{
    glBindVertexArray(cube_outline_vao);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
}

void c_render::render_cube_textured()
{
    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void c_render::prepare_chunk_transform(glm::ivec2 index)
{
    if (!current_shader) {
        return;
    }

    glm::vec3 local_origin = glm::vec3(index.x * CHUNK_SIZE, 0, index.y * CHUNK_SIZE);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, local_origin);

    current_shader->set_mat4("model", model);
}

void c_render::prepare_block_transform(const glm::ivec3& block_pos)
{
    if (!current_shader) {
        return;
    }

    float offset = 0.01f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(block_pos) - glm::vec3(offset * 0.5f));
    model = glm::scale(model, glm::vec3(1.0f + offset));

    current_shader->set_mat4("model", model);
}

void c_render::prepare_mesh_transform(c_mesh* mesh)
{
    
}

void c_render::render_chunks(bool process_translucent)
{
    if (!current_shader) {
        return;
    }

    struct render_job_t {
        int cx, cz;
        world_chunk_t* chunk;
    };

    int render_distance = g_config->render.render_distance;

    glm::mat4 model = glm::mat4(1.0f);
    current_shader->set_mat4("model", model);

    std::vector<render_job_t> visible_chunks;
    visible_chunks.reserve((render_distance * 2 + 1) * (render_distance * 2 + 1));

    {
        //std::lock_guard<std::mutex> lock(g_world->chunk_filler_mutex);

        for (int cz = -render_distance; cz <= render_distance; cz++) {
            for (int cx = -render_distance; cx <= render_distance; cx++) {
                world_chunk_t* chunk = g_world->rendered_chunk({ cx, cz });
                if (chunk) {
                    visible_chunks.push_back({ cx, cz, chunk });
                }
            }
        }
    } 

    glDepthMask(GL_TRUE);

    if (visible_chunks.empty())
        return;

    // job, job, job
    for (auto& job : visible_chunks) {
        if (job.chunk->opaque_mesh) {
            prepare_chunk_transform({ job.cx, job.cz });
            job.chunk->opaque_mesh->draw();
        }
    }

    if (process_translucent) {
        glDepthMask(GL_FALSE);

        for (auto& job : visible_chunks) {
            if (job.chunk->translucent_mesh) {
                prepare_chunk_transform({ job.cx, job.cz });
                job.chunk->translucent_mesh->draw();
            }
        }
    }

    glDepthMask(GL_TRUE);
}

void c_render::render_entities(bool process_local)
{
    if (!current_shader) {
        return;
    }

    if (!g_entity_list->get_entity_count()) {
        return;
    }

    struct entity_render_snapshot_t
    {
        c_scene_object* model_ptr = nullptr;

        glm::vec3 origin = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
    };

    std::vector<entity_render_snapshot_t> render_jobs;

    {
        //std::lock_guard<std::mutex> lock(g_entity_list->entity_mutex);

        int count = g_entity_list->get_entity_count();
        render_jobs.reserve(count);

        float interp_amt = g_time->get_interpolation_amount();

        for (int i = 0; i < count; i++) {
            c_entity* entity = g_entity_list->get_entity(i);
            if (!entity || entity->should_be_destroyed)
                continue;

            if (entity == g_global_vars->local_player) {
                c_camera* bound_camera = static_cast<c_player*>(entity)->get_bound_camera();
                if (bound_camera && bound_camera->get_mode() == c_camera::e_mode::FIRST_PERSON) {
                    continue;
                }
            }

            c_scene_object* root_obj = entity->get_model();
            if (!root_obj)
                continue;

            if (root_obj->type == c_scene_object::e_type::MODEL) {
                c_model* model = static_cast<c_model*>(root_obj);

                for (auto& mesh : model->meshes) {
                    entity_render_snapshot_t job;
                    job.model_ptr = &mesh;
                    job.origin = entity->get_interpolated_abs_origin(interp_amt);
                    job.rotation = glm::vec3(0.0f, entity->get_interpolated_yaw(interp_amt), 0.0f);
                    job.scale = model->scale;

                    render_jobs.push_back(job);
                }
            }
            else if (root_obj->type == c_scene_object::e_type::BOX || root_obj->type == c_scene_object::e_type::MESH) {
                entity_render_snapshot_t job;
                job.model_ptr = root_obj;
                job.origin = entity->get_interpolated_abs_origin(interp_amt);
                job.rotation = glm::vec3(0.0f, entity->get_interpolated_yaw(interp_amt), 0.0f);
                job.scale = root_obj->scale;

                render_jobs.push_back(job);
            }
        }
    }

    for (auto& job : render_jobs) {
        if (!job.model_ptr)
            continue;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, job.origin);
        model = glm::rotate(model, -glm::radians(job.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, job.scale);

        current_shader->set_mat4("model", model);

        if (job.model_ptr->type == c_scene_object::e_type::MESH) {
            static_cast<c_mesh*>(job.model_ptr)->draw();
        }
        else if (job.model_ptr->type == c_scene_object::e_type::BOX) {
            //static_cast<c_box*>(job.model_ptr)->draw();
        }
    }
}

void c_render::prepare_cube_vbo()
{
    static const float cube_vertices[] = {
        0.0f, 0.0f, 1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

        1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,

        0.0f, 0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        1.0f, 0.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        0.0f, 1.0f, 1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
        0.0f, 1.0f, 1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,

        0.0f, 0.0f, 0.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f
    };

    static const float outline_vertices[] = {
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f
    };

    if (cube_vao == 0) {
        glGenVertexArrays(1, &cube_vao);
        glGenBuffers(1, &cube_vbo);

        glBindVertexArray(cube_vao);
        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(c_mesh::vertex_t), (void*)offsetof(c_mesh::vertex_t, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(c_mesh::vertex_t), (void*)offsetof(c_mesh::vertex_t, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(c_mesh::vertex_t), (void*)offsetof(c_mesh::vertex_t, uv));
        glEnableVertexAttribArray(2);
    }

    if (cube_outline_vao == 0) {
        glGenVertexArrays(1, &cube_outline_vao);
        glGenBuffers(1, &cube_outline_vbo);

        glBindVertexArray(cube_outline_vao);
        glBindBuffer(GL_ARRAY_BUFFER, cube_outline_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(outline_vertices), outline_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
}

void c_render::render_block_outline()
{
    if (!current_shader) {
        return;
    }

    current_shader->set_vec3("color", glm::vec3(0.0f, 0.0f, 0.0f));

    glLineWidth(2.5f);
    render_cube_outline();
    glLineWidth(1.0f);
}

void c_render::render_block_decals()
{
    if (!current_shader) {
        return;
    }

    current_shader->set_int("texture_atlas", 0);
    current_shader->set_float("shift", static_cast<int>(g_global_vars->local_player->attack_progress * 10.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texture_manager->decal_atlas.id);

    glDepthMask(GL_FALSE);
    render_cube_textured();
    glDepthMask(GL_TRUE);
}

void c_render::render_scene(bool depth, bool process_translucent)
{
    prepare_cube_vbo();

    render_chunks(process_translucent);
    render_entities(true);

    if (depth) {
        return;
    }

    c_player* local = g_global_vars->local_player;
    if (local) {
        if (local->looking_at.block) {
            set_shader(outline_shader);

            prepare_block_transform(local->looking_at.block_pos);
            render_block_outline();

            if (local->attack_progress > 0.0f && local->interaction_object.block) {
                set_shader(decal_shader);

                prepare_block_transform(local->interaction_object.block_pos);
                render_block_decals();
            }
        }
    }
}

void c_render::compile_shaders()
{
    depth_buffer_shader = new c_shader("shaders/depth_buffer.vs", "shaders/depth_buffer.fs");
    direct_lighting_shader = new c_shader("shaders/direct_lighting.vs", "shaders/direct_lighting.fs");
    outline_shader = new c_shader("shaders/outline.vs", "shaders/outline.fs");
    decal_shader = new c_shader("shaders/decal.vs", "shaders/decal.fs");

    depth_buffer_shader->clean();
    direct_lighting_shader->clean();
    outline_shader->clean();
    decal_shader->clean();
}

void c_render::initialize()
{
    compile_shaders();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec2 screen_res = g_config->get_screen_resolution();
    main_viewport_width = screen_res.x;
    main_viewport_height = screen_res.y;

    depth_map_width = depth_map_height = 3072;

    initialize_depth_map();

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void c_render::initialize_depth_map()
{
    glGenFramebuffers(1, &depth_map_fbo);

    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depth_map_width, depth_map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
}

void c_render::render_main()
{
    glm::vec3 sun_position = scene->get_skybox()->get_sun_position();
    glm::vec3 camera_position = camera->get_origin();

    // render shadow map first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, depth_map_width, depth_map_height);

    const float near_plane = 0.1f, far_plane = 512.0f;
    glm::mat4 light_space_matrix = glm::ortho(-128.0f, 128.0f, -128.0f, 128.0f, near_plane, far_plane) * glm::lookAt(sun_position, glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    set_shader(depth_buffer_shader);
    current_shader->set_mat4("lightSpaceMatrix", light_space_matrix);

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    render_scene(true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render actual scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, main_viewport_width, main_viewport_height);

    set_shader(direct_lighting_shader);
    current_shader->set_int("texture_atlas", 0);
    current_shader->set_int("shadow_depth_map", 1);

    current_shader->set_vec3("viewPos", camera_position);
    current_shader->set_vec3("lightPos", sun_position);
    current_shader->set_mat4("lightSpaceMatrix", light_space_matrix);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    render_scene();

    glClear(GL_DEPTH_BUFFER_BIT);

    render_viewmodel();
}

void c_render::render_viewmodel()
{
    if (!current_shader || !camera || !g_global_vars->local_player) {
        return;
    }

    c_player* local = g_global_vars->local_player;

    c_model* held_model = local->held_item_model;
    if (!local->held_item) {
        return;
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    set_shader(direct_lighting_shader);
    current_shader->set_int("texture_atlas", 0);

    glm::mat4 inv_view = glm::inverse(camera->get_view_matrix());
    glm::mat4 hand = glm::mat4(1.0f);

    hand = glm::translate(hand, glm::vec3(0.56f, -0.52f, -0.72f));

    c_player_animation_state* anim_state = local->get_anim_state();
    if (anim_state && anim_state->firstperson_hand_swing > 0.0f) {
        float swing = glm::clamp(anim_state->firstperson_hand_swing, 0.0f, 1.0f);

        float f = std::sin(swing * glm::pi<float>());
        float f1 = std::sin(std::sqrt(swing) * glm::pi<float>());
        float f2 = std::sin(swing * swing * glm::pi<float>());

        hand = glm::translate(hand, glm::vec3(
            -0.4f * f1,
            0.2f * std::sin(std::sqrt(swing) * glm::pi<float>() * 2.0f),
            -0.2f * f
        ));

        hand = glm::rotate(hand, glm::radians(45.0f * f1), glm::vec3(0.0f, 1.0f, 0.0f));
        hand = glm::rotate(hand, glm::radians(-20.0f * f2), glm::vec3(0.0f, 0.0f, 1.0f));
        hand = glm::rotate(hand, glm::radians(-80.0f * f1), glm::vec3(1.0f, 0.0f, 0.0f));
        hand = glm::rotate(hand, glm::radians(-45.0f * f1), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    if (local->held_item->type == c_item::e_type::BLOCK)
    {
        hand = glm::rotate(hand, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        hand = glm::scale(hand, glm::vec3(0.4f));
        hand = glm::translate(hand, glm::vec3(-0.5f, -0.5f, -0.5f));
    }
    else
    {
        hand = glm::translate(hand, glm::vec3(0.07f, 0.20f, 0.07f));
        hand = glm::rotate(hand, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        hand = glm::rotate(hand, glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        hand = glm::scale(hand, glm::vec3(0.68f));

        current_shader->set_mat4("model", inv_view * hand);

        for (auto& mesh : held_model->meshes) {
            mesh.draw();
        }

    }

    glEnable(GL_CULL_FACE);
}

void c_render::run()
{
    if (!camera || !scene)
        return;

    std::vector<c_scene_object*> b_free_queue;
    {
        //std::lock_guard<std::mutex> lock(free_mutex);
        if (!free_queue.empty()) {
            b_free_queue.swap(free_queue);
        }
    }

    for (c_scene_object* to_free : b_free_queue) {
        if (to_free) {
            switch (to_free->type) {
            case c_scene_object::e_type::MESH: {
                c_mesh* mesh = static_cast<c_mesh*>(to_free);
                mesh->free();
                delete mesh;
                break;
            }
            case c_scene_object::e_type::MODEL: {
                c_model* model = static_cast<c_model*>(to_free);
                for (auto& mesh : model->meshes) {
                    mesh.free();
                }
                delete model;
                break;
            }
            default:
                delete to_free;
                break;
            };
        }
    }

    glClearColor(0.2f, 0.2f, 0.7f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!active)
        return;

    camera->update();

    render_main();
}
