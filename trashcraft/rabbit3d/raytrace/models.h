#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "scene.h"
#include <GL/glew.h>

class c_item;
class c_block;

class c_model_manager
{
public:
    std::unordered_map<std::string, c_model*> models;

    void initialize();
    void initialize_items();

    c_model* find_model_by_name(const std::string& model_name);
    c_model* allocate_model_by_name(const std::string& model_name);

    c_model* get_model_instance(const std::string& model_name);
    c_model* get_model_instance(c_model* model);

    bool create_model_from_file(const std::string& model_name, const std::string& model_path);
    bool create_model_from_texture(const std::string& model_name, c_texture* texture);
    bool create_model_from_block(c_block* block);
    bool create_model_from_item(c_item* item);

    bool create_player_model(const std::string& model_name, const std::string& texture_id, bool second_layer = true);
};

extern c_model_manager* g_model_manager;
