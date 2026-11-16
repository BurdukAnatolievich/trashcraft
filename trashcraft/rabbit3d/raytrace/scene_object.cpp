#include "scene.h"
#include <OBJ_Loader.h>

bool c_model::load_from_file(std::string file)
{
    objl::Loader loader;
    if (!loader.LoadFile(file))
        return false;

    for (auto& loaded_mesh : loader.LoadedMeshes) {
        c_mesh* mesh = &meshes.emplace_back(c_mesh(loaded_mesh.MeshName, glm::vec3(0.0f), nullptr));

        mesh->vertices.resize(loaded_mesh.Vertices.size());
        std::memcpy(mesh->vertices.data(), loaded_mesh.Vertices.data(), sizeof(c_mesh::vertex_t) * loaded_mesh.Vertices.size());

        if (!g_texture_manager->find_texture_by_name(loaded_mesh.MeshMaterial.name)) {
            g_texture_manager->set_flip_texture_vertically(true);
            g_texture_manager->create_texture_from_file(loaded_mesh.MeshMaterial.name, "textures/" + loaded_mesh.MeshMaterial.map_Kd);
            g_texture_manager->set_flip_texture_vertically(false);
        }

        mesh->texture = g_texture_manager->find_texture_by_name(loaded_mesh.MeshMaterial.name);

        mesh->create();
        mesh->setup();
    }

    return true;
}

void c_model::draw()
{
    for (auto& mesh : meshes)
        mesh.draw();
}

c_mesh* c_model::get_mesh_by_name(std::string name)
{
    for (auto& mesh : meshes)
    {
        if (mesh.name == name)
            return &mesh;
    }

    return nullptr;
}

c_mesh::c_mesh()
{
    this->type = c_scene_object::e_type::MESH;
}

c_mesh::c_mesh(std::string name, const glm::vec3& origin, c_texture* texture)
{
    this->type = c_scene_object::e_type::MESH;
    this->name = name;

    this->origin = origin;
    this->rotation = glm::vec3(0.0f);

    this->texture = texture;
}

void c_mesh::create()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
}

void c_mesh::setup()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    vb_size = vertices.size();

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, position));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, uv));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void c_mesh::draw()
{
    if (should_reassemble_vb) {
        create();
        setup();
        should_reassemble_vb = false;
    }

    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->get_id());
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vb_size);
    glBindVertexArray(0);
}

void c_mesh::clear()
{
    vertices.clear();
}

void c_mesh::free()
{
    if (vao != 0 || vbo != 0) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);

        vb_size = 0;
        vao = vbo = 0;
    }
}
