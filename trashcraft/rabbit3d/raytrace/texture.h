#pragma once
#include <string>
#include <vector>

#include "raytrace.h"

#include <glm.hpp>
#include <GL/glew.h>

#define TEXTURE_SIZE 16
#define ATLAS_SIZE 32

enum e_texture_category : uint8_t
{
    TEXTURE_CATEGORY_BLOCK,
    TEXTURE_CATEGORY_ITEM,
    TEXTURE_CATEGORY_ENTITY,
    TEXTURE_CATEGORY_GUI,
    TEXTURE_CATEGORY_MISC,
    TEXTURE_CATEGORY_INTERNAL,
};;

class c_texture {
public:
    e_texture_category category = TEXTURE_CATEGORY_MISC;

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

    c_texture(const std::string& name, e_texture_category category)
    {
        this->name = name;
		this->category = category;
    }

    bool load_from_file(const std::string& file);

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

    c_texture* find_texture_by_name(const std::string& texture_name);

    void load_textures_from_directory(std::vector<c_texture*>& _textures, const std::string& path, e_texture_category category = TEXTURE_CATEGORY_MISC);
    void load_textures_from_directory(const std::string& path, e_texture_category category = TEXTURE_CATEGORY_MISC);

    bool create_texture_from_file(std::vector<c_texture*>& _textures, const std::string& texture_name, const std::string& texture_path, e_texture_category category = TEXTURE_CATEGORY_MISC);
    bool create_texture_from_file(const std::string& texture_name, const std::string& texture_path, e_texture_category category = TEXTURE_CATEGORY_MISC);

    void build_texture_atlas(std::vector<c_texture*>& textures, c_texture* out);
    void build_texture_atlas();
};

extern c_texture_manager* g_texture_manager;
