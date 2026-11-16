#pragma once
#include <string>
#include <vector>

#include "raytrace.h"

#include <glm.hpp>
#include <GL/glew.h>

#define TEXTURE_SIZE 16
#define ATLAS_SIZE 32

class c_texture {
public:
    GLuint id = 0;
    bool loaded = false;

    std::string name = "";

    glm::vec2 size = glm::vec2(0.0f);
    glm::vec2 atlas_uv_min = glm::vec2(0.0f);
    glm::vec2 atlas_uv_max = glm::vec2(1.0f);

    unsigned char* raw = NULL;
    int raw_components = 3;

    c_texture()
    {

    }

    c_texture(std::string n)
    {
        name = n;
    }

    c_texture(std::string n, std::string f)
    {
        name = n;

        load_from_file(f);
    }

    bool load_from_file(std::string file);

    inline GLuint get_id() { return id; };
    inline bool is_valid() { return loaded; };
};

class c_texture_manager {
public:
    c_texture decal_atlas;
    c_texture texture_atlas;

    std::vector<c_texture*> textures;

    void initialize();

    void set_flip_texture_vertically(bool flip);

    c_texture* find_texture_by_name(std::string texture_name);

    void load_textures_from_directory(std::vector<c_texture*>& _textures, std::string path);
    void load_textures_from_directory(std::string path);

    bool create_texture_from_file(std::vector<c_texture*>& _textures, std::string texture_name, std::string texture_path);
    bool create_texture_from_file(std::string texture_name, std::string texture_path);

    void build_texture_atlas(std::vector<c_texture*>& textures, c_texture* out);
    void build_texture_atlas();
};

extern c_texture_manager* g_texture_manager;
