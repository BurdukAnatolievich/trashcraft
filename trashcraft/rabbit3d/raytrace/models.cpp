#include "models.h"
#include "render.h"

#include <item.h>
#include <block.h>

#include <global_vars.h>

c_model_manager* g_model_manager = new c_model_manager;

void c_model_manager::initialize()
{
    create_player_model("local_mdl", "steve", true);
    //create_model_from_file("hatsune_miku", "models/hatsune_miku.obj");

    create_model_from_file("cherry_door_block", "models/cherry_door.obj");
}

void c_model_manager::initialize_items()
{
    for (auto& item_ : g_global_vars->items)
    {
        c_item* item = item_.second;
        if (item->type != c_item::e_type::BLOCK)
            continue;

        create_model_from_item(item);


        item->item_model = find_model_by_name(item->name);
    }
}

c_model* c_model_manager::find_model_by_name(const std::string& model_name)
{
    auto it = models.find(model_name);
    if (it == models.end())
        return nullptr;

    return it->second;
}

c_model* c_model_manager::allocate_model_by_name(const std::string& model_name)
{
    return nullptr;
}

c_model* c_model_manager::get_model_instance(const std::string& model_name)
{
    c_model* found_model = find_model_by_name(model_name);
    return get_model_instance(found_model);
}

c_model* c_model_manager::get_model_instance(c_model* model)
{
    if (!model)
        return nullptr;

    c_model* new_model = new c_model(model->name, glm::vec3(0.0f));
    new_model->meshes = model->meshes;

    return new_model;
}

bool c_model_manager::create_model_from_file(const std::string& model_name, const std::string& model_path)
{
    if (find_model_by_name(model_name))
        return true;

    c_model* model = new c_model(model_name, glm::vec3(0.0f));
    if (!model->load_from_file(model_path))
    {
        delete model;
        return false;
    }

    models.emplace(model_name, model);
    return true;
}

bool c_model_manager::create_model_from_texture(const std::string& model_name, c_texture* texture)
{
    if (!texture || !texture->raw)
        return false;

    if (models.find(model_name) != models.end()) {
        return false;
	}

    c_model* model = new c_model(model_name, glm::vec3(0.0f));

    c_mesh* mesh = &model->meshes.emplace_back();
    mesh->texture = texture;

    constexpr int width = 16;
    constexpr int height = 16;
    constexpr float step_x = 1.0f / static_cast<float>(width);
    constexpr float step_y = 1.0f / static_cast<float>(height);

    constexpr float half_depth = (1.0f / 16.0f) * 0.5f;
    constexpr float z_min = -half_depth;
    constexpr float z_max = half_depth;
    constexpr float offset_x = -0.5f;
    constexpr float offset_y = -0.5f;

    auto is_opaque = [&](int px, int py) -> bool {
        if (px < 0 || px >= width || py < 0 || py >= height)
            return false;
        if (texture->raw_components < 4)
            return true;

        const unsigned char* p = texture->raw + (py * width + px) * texture->raw_components;
        return p[3] > 10;
        };

    auto push_quad = [&](
        const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
        const glm::vec3& normal,
        const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)
        {
            mesh->vertices.push_back({ p0, normal, uv0 });
            mesh->vertices.push_back({ p1, normal, uv1 });
            mesh->vertices.push_back({ p2, normal, uv2 });

            mesh->vertices.push_back({ p2, normal, uv2 });
            mesh->vertices.push_back({ p3, normal, uv3 });
            mesh->vertices.push_back({ p0, normal, uv0 });
        };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!is_opaque(x, y))
                continue;

            int inv_y = (height - 1) - y;

            float x0 = offset_x + x * step_x;
            float x1 = offset_x + (x + 1) * step_x;
            float y0 = offset_y + inv_y * step_y;
            float y1 = offset_y + (inv_y + 1) * step_y;

            float u0 = x * step_x;
            float u1 = (x + 1) * step_x;
            float v0 = (y + 1) * step_y; 
            float v1 = y * step_y; 

            glm::vec2 uv_center((x + 0.5f) * step_x, (y + 0.5f) * step_y);

            push_quad(
                { x0, y0, z_max }, { x1, y0, z_max }, { x1, y1, z_max }, { x0, y1, z_max },
                { 0.0f, 0.0f, 1.0f },
                { u0, v0 }, { u1, v0 }, { u1, v1 }, { u0, v1 }
            );

            push_quad(
                { x1, y0, z_min }, { x0, y0, z_min }, { x0, y1, z_min }, { x1, y1, z_min },
                { 0.0f, 0.0f, -1.0f },
                { u1, v0 }, { u0, v0 }, { u0, v1 }, { u1, v1 }
            );

            if (!is_opaque(x - 1, y)) {
                push_quad(
                    { x0, y0, z_min }, { x0, y0, z_max }, { x0, y1, z_max }, { x0, y1, z_min },
                    { -1.0f, 0.0f, 0.0f },
                    uv_center, uv_center, uv_center, uv_center
                );
            }

            if (!is_opaque(x + 1, y)) {
                push_quad(
                    { x1, y0, z_max }, { x1, y0, z_min }, { x1, y1, z_min }, { x1, y1, z_max },
                    { 1.0f, 0.0f, 0.0f },
                    uv_center, uv_center, uv_center, uv_center
                );
            }

            if (!is_opaque(x, y - 1)) {
                push_quad(
                    { x0, y1, z_max }, { x1, y1, z_max }, { x1, y1, z_min }, { x0, y1, z_min },
                    { 0.0f, 1.0f, 0.0f },
                    uv_center, uv_center, uv_center, uv_center
                );
            }

            if (!is_opaque(x, y + 1)) {
                push_quad(
                    { x0, y0, z_min }, { x1, y0, z_min }, { x1, y0, z_max }, { x0, y0, z_max },
                    { 0.0f, -1.0f, 0.0f },
                    uv_center, uv_center, uv_center, uv_center
                );
            }
        }
    }

    mesh->should_reassemble_vb = true;

    models[model_name] = model;
    return true;
}

bool c_model_manager::create_model_from_block(c_block* block)
{
    if (!block)
        return false;

    if (models.find(block->name) != models.end())
        return false;

    c_model* model = new c_model(block->name, glm::vec3(0.0f));

    const glm::vec3 p0 = block->min - glm::vec3(0.5f);
    const glm::vec3 p1 = block->max - glm::vec3(0.5f);

    const float u0 = block->min.x, u1 = block->max.x;
    const float v0 = 1.0f - block->min.y, v1 = 1.0f - block->max.y;
    const float w0 = block->min.z, w1 = block->max.z;

    model->meshes.reserve(6);
    auto add_face = [&](c_block::e_material mat,
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d,
        const glm::vec3& normal,
        const glm::vec2& t0, const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3)
        {
            c_texture* tex = block->materials[mat] ? block->materials[mat] : block->materials[c_block::e_material::SIDE];
            if (!tex)
                return;

            c_mesh* mesh = &model->meshes.emplace_back(c_mesh(tex->name, { 0.0f, 0.0f, 0.0f }, tex));
            mesh->should_reassemble_vb = true;

            mesh->vertices.push_back({ a, normal, t0 });
            mesh->vertices.push_back({ b, normal, t1 });
            mesh->vertices.push_back({ c, normal, t2 });

            mesh->vertices.push_back({ c, normal, t2 });
            mesh->vertices.push_back({ d, normal, t3 });
            mesh->vertices.push_back({ a, normal, t0 });
        };

    add_face(c_block::e_material::SIDE, { p1.x, p0.y, p1.z }, { p1.x, p0.y, p0.z }, { p1.x, p1.y, p0.z }, { p1.x, p1.y, p1.z }, { 1.0f, 0.0f, 0.0f }, { 1.0f - w1, v0 }, { 1.0f - w0, v0 }, { 1.0f - w0, v1 }, { 1.0f - w1, v1 });
    add_face(c_block::e_material::SIDE, { p0.x, p0.y, p0.z }, { p0.x, p0.y, p1.z }, { p0.x, p1.y, p1.z }, { p0.x, p1.y, p0.z }, { -1.0f, 0.0f, 0.0f }, { w0, v0 }, { w1, v0 }, { w1, v1 }, { w0, v1 });

    add_face(c_block::e_material::UPPER, { p0.x, p1.y, p0.z }, { p1.x, p1.y, p0.z }, { p1.x, p1.y, p1.z }, { p0.x, p1.y, p1.z }, { 0.0f, 1.0f, 0.0f }, { u0, 1.0f - w0 }, { u1, 1.0f - w0 }, { u1, 1.0f - w1 }, { u0, 1.0f - w1 });
    add_face(c_block::e_material::BOTTOM, { p0.x, p0.y, p1.z }, { p1.x, p0.y, p1.z }, { p1.x, p0.y, p0.z }, { p0.x, p0.y, p0.z }, { 0.0f, -1.0f, 0.0f }, { u0, w1 }, { u1, w1 }, { u1, w0 }, { u0, w0 });

    add_face(c_block::e_material::SIDE, { p0.x, p0.y, p1.z }, { p1.x, p0.y, p1.z }, { p1.x, p1.y, p1.z }, { p0.x, p1.y, p1.z }, { 0.0f, 0.0f, 1.0f }, { u0, v0 }, { u1, v0 }, { u1, v1 }, { u0, v1 });
    add_face(c_block::e_material::SIDE, { p1.x, p0.y, p0.z }, { p0.x, p0.y, p0.z }, { p0.x, p1.y, p0.z }, { p1.x, p1.y, p0.z }, { 0.0f, 0.0f, -1.0f }, { 1.0f - u1, v0 }, { 1.0f - u0, v0 }, { 1.0f - u0, v1 }, { 1.0f - u1, v1 });

    if (model->meshes.empty()) {
        delete model;
        return false;
    }

    models[block->name] = model;
    return true;
}

bool c_model_manager::create_model_from_item(c_item* item)
{
    if (!item) {
        return false;
    }

    if (item->type != c_item::e_type::BLOCK) {
        return create_model_from_texture(item->name, item->item_material);
    }

    return create_model_from_block((c_block*)item);
}

bool c_model_manager::create_player_model(const std::string& model_name, const std::string& texture_id, bool second_layer)
{
    if (models.find(model_name) != models.end()) {
        return false;
    }

    c_texture* texture = g_texture_manager->find_texture_by_name(texture_id);
    if (!texture) {
        return false;
    }

    c_model* model = new c_model(model_name, glm::vec3(0.0f));
    model->meshes.reserve(6);

    const float pixel_scale = (1.0f / 16.0f) * 0.9375f;
    const float tex_size = texture->size.x;

    auto push_quad = [&](
        c_mesh& mesh,
        const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
        const glm::vec3& normal,
        float u0, float v0, float u1, float v1)
        {
            glm::vec2 uv0(u0 / tex_size, v1 / tex_size);
            glm::vec2 uv1(u1 / tex_size, v1 / tex_size);
            glm::vec2 uv2(u1 / tex_size, v0 / tex_size);
            glm::vec2 uv3(u0 / tex_size, v0 / tex_size);

            mesh.vertices.push_back({ p0, normal, uv0 });
            mesh.vertices.push_back({ p1, normal, uv1 });
            mesh.vertices.push_back({ p2, normal, uv2 });

            mesh.vertices.push_back({ p2, normal, uv2 });
            mesh.vertices.push_back({ p3, normal, uv3 });
            mesh.vertices.push_back({ p0, normal, uv0 });
        };

    auto push_box = [&](c_mesh& mesh, const glm::vec3& min_px, const glm::vec3& size_px, float u, float v, float inflate = 0.0f)
        {
            float x0 = (min_px.x - inflate) * pixel_scale;
            float y0 = (min_px.y - inflate) * pixel_scale;
            float z0 = (min_px.z - inflate) * pixel_scale;

            float x1 = (min_px.x + size_px.x + inflate) * pixel_scale;
            float y1 = (min_px.y + size_px.y + inflate) * pixel_scale;
            float z1 = (min_px.z + size_px.z + inflate) * pixel_scale;

            float w = size_px.x;
            float h = size_px.y;
            float d = size_px.z;

            glm::vec3 v000(x0, y0, z0);
            glm::vec3 v100(x1, y0, z0);
            glm::vec3 v110(x1, y1, z0);
            glm::vec3 v010(x0, y1, z0);

            glm::vec3 v001(x0, y0, z1);
            glm::vec3 v101(x1, y0, z1);
            glm::vec3 v111(x1, y1, z1);
            glm::vec3 v011(x0, y1, z1);

            push_quad(mesh, v001, v101, v111, v011, { 0.0f,  0.0f,  1.0f }, u + d, v + d, u + d + w, v + d + h);
            push_quad(mesh, v100, v000, v010, v110, { 0.0f,  0.0f, -1.0f }, u + 2 * d + w, v + d, u + 2 * d + 2 * w, v + d + h);
            push_quad(mesh, v000, v001, v011, v010, { -1.0f,  0.0f,  0.0f }, u, v + d, u + d, v + d + h);
            push_quad(mesh, v101, v100, v110, v111, { 1.0f,  0.0f,  0.0f }, u + d + w, v + d, u + 2 * d + w, v + d + h);
            push_quad(mesh, v011, v111, v110, v010, { 0.0f,  1.0f,  0.0f }, u + d, v, u + d + w, v + d);
            push_quad(mesh, v000, v100, v101, v001, { 0.0f, -1.0f,  0.0f }, u + d + w, v, u + d + 2 * w, v + d);
        };

    auto add_part = [&](std::string part_name, const glm::vec3& pivot_px) -> c_mesh&
        {
            c_mesh& mesh = model->meshes.emplace_back(part_name, glm::vec3(0.0f), texture);
            mesh.use_pivot = true;
            mesh.pivot_point = pivot_px * pixel_scale;
            mesh.pivot_point_rotation = glm::vec3(0.0f);
            mesh.should_reassemble_vb = true;
            return mesh;
        };

    c_mesh& head = add_part("head", { 0.0f, 24.0f, 0.0f });
    push_box(head, { -4.0f, 24.0f, -4.0f }, { 8.0f, 8.0f, 8.0f }, 0.0f, 0.0f);
    if (second_layer)
        push_box(head, { -4.0f, 24.0f, -4.0f }, { 8.0f, 8.0f, 8.0f }, 32.0f, 0.0f, 0.5f);

    c_mesh& body = add_part("body", { 0.0f, 24.0f, 0.0f });
    push_box(body, { -4.0f, 12.0f, -2.0f }, { 8.0f, 12.0f, 4.0f }, 16.0f, 16.0f);
    if (second_layer)
        push_box(body, { -4.0f, 12.0f, -2.0f }, { 8.0f, 12.0f, 4.0f }, 16.0f, 32.0f, 0.25f);

    c_mesh& right_arm = add_part("right_arm", { -5.0f, 22.0f, 0.0f });
    push_box(right_arm, { -8.0f, 12.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 40.0f, 16.0f);
    if (second_layer)
        push_box(right_arm, { -8.0f, 12.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 40.0f, 32.0f, 0.25f);

    c_mesh& left_arm = add_part("left_arm", { 5.0f, 22.0f, 0.0f });
    push_box(left_arm, { 4.0f, 12.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 32.0f, 48.0f);
    if (second_layer)
        push_box(left_arm, { 4.0f, 12.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 48.0f, 48.0f, 0.25f);
    c_mesh& right_leg = add_part("right_leg", { -1.9f, 12.0f, 0.0f });
    push_box(right_leg, { -3.9f, 0.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 0.0f, 16.0f);
    if (second_layer)
        push_box(right_leg, { -3.9f, 0.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 0.0f, 32.0f, 0.25f);
    c_mesh& left_leg = add_part("left_leg", { 1.9f, 12.0f, 0.0f });
    push_box(left_leg, { -0.1f, 0.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 16.0f, 48.0f);
    if (second_layer)
        push_box(left_leg, { -0.1f, 0.0f, -2.0f }, { 4.0f, 12.0f, 4.0f }, 0.0f, 48.0f, 0.25f);

	models[model_name] = model;
    return true;
}