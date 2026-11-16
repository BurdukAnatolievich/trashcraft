#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <istream>

#include "raytrace.h"
#include "skybox.h"
#include "texture.h"

#include <glm.hpp>

struct aabb_t {
    glm::vec3 min, max;
};

class c_scene_object
{
public:
	enum e_type
	{
		BOX,
        MESH,
        MODEL
	};

	int type = -1;
	std::string name = "None";

    glm::vec3 origin = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    bool transparency = false, ignore_depth = false, use_pivot = false;
    glm::vec3 pivot_point = glm::vec3(0.0f);
    glm::vec3 pivot_point_rotation = glm::vec3(0.0f);

    bool is_light = false;
    float light_intensity = 1.0f, light_range = 4.0f;
};

class c_box : public c_scene_object
{
public:
    enum e_texture_side
    {
        BOX_SIDE,
        BOX_TOP,
        BOX_BOTTOM,
        BOX_MAX
    };

	c_box()
	{
        this->type = c_scene_object::e_type::BOX;

        this->bb_min = glm::vec3(0.0f);
        this->bb_max = glm::vec3(1.0f);
	}

	c_box(std::string name, const glm::vec3& origin, const glm::vec3& min, const glm::vec3& max, c_texture* texture_side = nullptr, c_texture* texture_top = nullptr, c_texture* texture_bottom = nullptr, bool transparency = false)
	{
        this->type = c_scene_object::e_type::BOX;
        this->name = name;

        this->origin = origin;

		this->bb_min = min;
        this->bb_max = max;

        this->textures[BOX_SIDE] = texture_side;
        this->textures[BOX_TOP] = texture_top;
        this->textures[BOX_BOTTOM] = texture_bottom;

        this->transparency = transparency;
	}

    glm::vec3 bb_min = glm::vec3(0.0f), bb_max = glm::vec3(1.0f);
    c_texture* textures[BOX_MAX] = { nullptr, nullptr, nullptr };
};

class c_mesh : public c_scene_object
{
public:
    struct vertex_t
    {
        glm::vec3 position, normal;
        glm::vec2 uv;
    };

    std::vector<vertex_t> vertices;

    size_t vb_size = 0;
    unsigned int vao = 0, vbo = 0;

    bool should_reassemble_vb = false;

    c_texture* texture = nullptr;

    c_mesh();
    c_mesh(std::string name, const glm::vec3& origin, c_texture* texture = nullptr);

    void create();
    void setup();

    void draw();

    void clear();
    void free();
};

class c_model : public c_scene_object
{
public:
    std::vector<c_mesh> meshes;

    c_model(std::string name, const glm::vec3& origin)
    {
        this->type = c_scene_object::e_type::MODEL;

        this->name = name;
        this->origin = origin;
    }

    bool load_from_file(std::string file);

    void draw();
    c_mesh* get_mesh_by_name(std::string name);
};

class c_camera;

class c_scene
{
    c_skybox* skybox;
public:
    c_skybox* get_skybox();
    void set_skybox(c_skybox* sky);
};
