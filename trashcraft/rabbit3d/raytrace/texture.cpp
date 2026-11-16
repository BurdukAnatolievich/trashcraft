#include "texture.h"
#include "scene.h"
#include "models.h"

#include <menu.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm.hpp>

#include <algorithm>
#include <filesystem>

c_texture_manager* g_texture_manager = new c_texture_manager;

bool c_texture::load_from_file(std::string file)
{
    int width, height, components;

    unsigned char* data = stbi_load(file.c_str(), &width, &height, &components, 0);
    if (data) {
        raw = new unsigned char[width * height * components];
        raw_components = components;

        size = glm::vec2(width, height);

        std::memcpy(raw, data, width * height * raw_components);

        glGenTextures(1, &id);

        GLenum format;
        if (raw_components == 1)
            format = GL_RED;
        else if (raw_components == 3)
            format = GL_RGB;
        else if (raw_components == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);

        loaded = true;
        return true;
    }

    //g_menu->get_console()->add_log("Failed to load texture: " + file);
    return false;
}

void c_texture_manager::initialize()
{
    // BLOCKS

    load_textures_from_directory("blocks/");

    // ATLAS

    build_texture_atlas();

    // DECALS
    std::vector<c_texture*> _textures;
    load_textures_from_directory(_textures, "blocks/destroy/");

    build_texture_atlas(_textures, &decal_atlas);
    
    // ITEMS

    load_textures_from_directory("items/");

    // GUI

    load_textures_from_directory("gui/");

    // ENTITIES

    load_textures_from_directory("entities/");
}

void c_texture_manager::set_flip_texture_vertically(bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
}

void c_texture_manager::load_textures_from_directory(std::vector<c_texture*>& _textures, std::string path)
{
    for (auto& iterator : std::filesystem::directory_iterator("./textures/" + path)) {
        if (iterator.is_directory()) {
            continue;
        }

        const std::filesystem::path& path = iterator.path();
        create_texture_from_file(_textures, path.stem().string(), path.string());
    }
}

void c_texture_manager::load_textures_from_directory(std::string path)
{
    for (auto& iterator : std::filesystem::directory_iterator("./textures/" + path)) {
        if (iterator.is_directory()) {
            continue;
        }

        const std::filesystem::path& path = iterator.path();
        create_texture_from_file(path.stem().string(), path.string());
    }
}

c_texture* c_texture_manager::find_texture_by_name(std::string texture_name)
{
    for (auto& texture : textures) {
        if (texture->name == texture_name)
            return texture;
    }

    return nullptr;
}

bool c_texture_manager::create_texture_from_file(std::vector<c_texture*>& _textures, std::string texture_name, std::string texture_path)
{
    c_texture* texture = new c_texture(texture_name);
    if (!texture->load_from_file(texture_path)) {
        delete texture;
        return false;
    }

    _textures.push_back(texture);
    return true;
}

bool c_texture_manager::create_texture_from_file(std::string texture_name, std::string texture_path)
{
    c_texture* texture = new c_texture(texture_name);
    if (!texture->load_from_file(texture_path)) {
        delete texture;
        return false;
    }

    textures.push_back(texture);
    return true;
}

void c_texture_manager::build_texture_atlas(std::vector<c_texture*>& _textures, c_texture* out)
{
    if (!out) {
        return;
    }

    int width = TEXTURE_SIZE * ATLAS_SIZE;
    int height = TEXTURE_SIZE * ATLAS_SIZE;

    out->loaded = true;
    out->size = glm::vec2(width, height);

    unsigned char* data = new unsigned char[width * height * 4];
    int current_texture_x = 0, current_texture_y = 0;

    for (auto& texture : _textures) {
        if (texture->size.x > TEXTURE_SIZE || texture->size.y > TEXTURE_SIZE)
            continue;

        for (int y = 0; y < TEXTURE_SIZE; ++y) {
            for (int x = 0; x < TEXTURE_SIZE; ++x) {
                unsigned char* pixel_offset = data + ((current_texture_y * TEXTURE_SIZE + y) * width + (current_texture_x * TEXTURE_SIZE + x)) * 4;
                unsigned char* pixel_offset_copy = texture->raw + (y * TEXTURE_SIZE + x) * texture->raw_components;

                pixel_offset[0] = pixel_offset_copy[0];
                pixel_offset[1] = texture->raw_components >= 3 ? pixel_offset_copy[1] : pixel_offset_copy[0];
                pixel_offset[2] = texture->raw_components >= 3 ? pixel_offset_copy[2] : pixel_offset_copy[0];
                pixel_offset[3] = texture->raw_components == 4 ? pixel_offset_copy[3] : 255;
            }
        }

        texture->atlas_uv_min = glm::vec2(current_texture_x * TEXTURE_SIZE, current_texture_y * TEXTURE_SIZE) / out->size;
        texture->atlas_uv_max = glm::vec2(current_texture_x * TEXTURE_SIZE + TEXTURE_SIZE, current_texture_y * TEXTURE_SIZE + TEXTURE_SIZE) / out->size;

        current_texture_x++;
        if (current_texture_x >= ATLAS_SIZE) {
            current_texture_x = 0;
            current_texture_y++;
        }
    }

    glGenTextures(1, &out->id);

    glBindTexture(GL_TEXTURE_2D, out->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    delete[] data;

    // mipmaps for texture atlas cause weird artifacts
    //glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void c_texture_manager::build_texture_atlas()
{
    texture_atlas = c_texture("texture_atlas");

    build_texture_atlas(textures, &texture_atlas);
}
