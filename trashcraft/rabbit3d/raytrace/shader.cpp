#include "shader.h"

#include "../glm/glm.hpp"
#include "../../game/menu.h"

void c_shader::use()
{
    glUseProgram(id);
}

void c_shader::load(std::string vertex_shader, std::string fragment_shader, std::string geometry_shader)
{
    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;
    std::ifstream geometry_shader_file;

    vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    geometry_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertex_shader_file.open(vertex_shader);
        fragment_shader_file.open(fragment_shader);

        std::stringstream vertex_shader_stream, fragment_shader_stream;
        vertex_shader_stream << vertex_shader_file.rdbuf();
        fragment_shader_stream << fragment_shader_file.rdbuf();

        vertex_shader_file.close();
        fragment_shader_file.close();

        vertex_shader_code = vertex_shader_stream.str();
        fragment_shader_code = fragment_shader_stream.str();

        if (geometry_shader != "")
        {
            geometry_shader_file.open(geometry_shader);

            std::stringstream geometry_shader_stream;
            geometry_shader_stream << geometry_shader_file.rdbuf();
            geometry_shader_file.close();

            geometry_shader_code = geometry_shader_stream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        //g_menu->get_console()->add_log(std::string("Failed to load shader: ") + e.what());
    }
}

void c_shader::compile()
{
    bool has_geometry_shader = geometry_shader_code != "";

    const char* vertex_shader_code_ = vertex_shader_code.c_str();
    const char* fragment_shader_code_ = fragment_shader_code.c_str();
    const char* geometry_shader_code_ = geometry_shader_code.c_str();

    GLuint vertex_shader, fragment_shader, geometry_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code_, NULL);
    glCompileShader(vertex_shader);
    check_compile_errors(vertex_shader, "VERTEX");

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code_, NULL);
    glCompileShader(fragment_shader);
    check_compile_errors(fragment_shader, "FRAGMENT");

    if (has_geometry_shader)
    {
        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_shader, 1, &geometry_shader_code_, NULL);
        glCompileShader(geometry_shader);
        check_compile_errors(geometry_shader, "GEOMETRY");
    }

    id = glCreateProgram();

    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    if (has_geometry_shader)
        glAttachShader(id, geometry_shader);

    glLinkProgram(id);

    check_compile_errors(id, "PROGRAM");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (has_geometry_shader)
        glDeleteShader(geometry_shader);
}

void c_shader::check_compile_errors(GLuint shader, std::string type)
{
    GLint success;
    GLchar info_log[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        //if (!success)
       //     g_menu->get_console()->add_log("Failed to compile shader: " + std::string(info_log));
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        //if (!success)
        //    g_menu->get_console()->add_log("Failed to compile shader: " + std::string(info_log));
    }
}

void c_shader::clean()
{
    vertex_shader_code.clear();
    fragment_shader_code.clear();
    geometry_shader_code.clear();
}

void c_shader::free()
{
    glDeleteProgram(id);
}

void c_shader::set_bool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void c_shader::set_int(const std::string& name, int value) 
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void c_shader::set_float(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void c_shader::set_vec2(const std::string& name, const glm::vec2& value) 
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void c_shader::set_vec2(const std::string& name, float x, float y)
{
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void c_shader::set_vec3(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void c_shader::set_vec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void c_shader::set_vec4(const std::string& name, const glm::vec4& value)
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void c_shader::set_vec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void c_shader::set_mat2(const std::string& name, const glm::mat2& mat)
{
    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void c_shader::set_mat3(const std::string& name, const glm::mat3& mat)
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void c_shader::set_mat4(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
