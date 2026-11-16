#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm.hpp>
#include <GL/glew.h>

class c_shader
{
    GLuint id = 0;

    std::string vertex_shader_code = "";
    std::string fragment_shader_code = "";
    std::string geometry_shader_code = "";
public:
    c_shader()
    {
        id = 0;

        vertex_shader_code = "";
        fragment_shader_code = "";
        geometry_shader_code = "";
    }

    c_shader(std::string vertex_shader, std::string fragment_shader, std::string geometry_shader = "")
    {
        load(vertex_shader, fragment_shader, geometry_shader);
        compile();
    }

    void use();
    void load(std::string vertex_shader, std::string fragment_shader, std::string geometry_shader = "");
    void compile();

    void clean();
    void free();

    void set_bool(const std::string& name, bool value);
    void set_int(const std::string& name, int value);
    void set_float(const std::string& name, float value);
    void set_vec2(const std::string& name, const glm::vec2& value);
    void set_vec2(const std::string& name, float x, float y);
    void set_vec3(const std::string& name, const glm::vec3& value);
    void set_vec3(const std::string& name, float x, float y, float z);
    void set_vec4(const std::string& name, const glm::vec4& value);
    void set_vec4(const std::string& name, float x, float y, float z, float w);
    void set_mat2(const std::string& name, const glm::mat2& mat);
    void set_mat3(const std::string& name, const glm::mat3& mat);
    void set_mat4(const std::string& name, const glm::mat4& mat);
private:
    void check_compile_errors(GLuint shader, std::string type);
};
