#include "camera.h"
#include <input.h>
#include <imgui.h>
#include <Windows.h>
#include <time/time.h>

void c_camera::update()
{
    //std::lock_guard<std::mutex> owner_guard(g_entity_list->entity_mutex);
    update_owner();
    update_transform();
    update_mode();
}

void c_camera::update_mode()
{
    if (g_input->is_key_pressed(ImGuiKey_F5)) {
        mode++;
        mode %= 3;
    }

    float thirdperson_distance = 4.0f;

    switch (mode) {
    case FIRST_PERSON:
        transform_origin = glm::vec3(0.0f);
        break;
    case THIRD_PERSON_BACK:
        transform_origin = -(forward * thirdperson_distance);
        break;
    case THIRD_PERSON_FORWARD:
        transform_origin = forward * thirdperson_distance;

        forward = -forward; 
        right = -right;
        break;
    }
}

void c_camera::update_owner() 
{
    if (!owner) {
        return;
    }

    owner_data.origin = owner->get_interpolated_abs_origin(g_time->get_interpolation_amount());
    owner_data.eye_position = owner->eye_position;

    set_origin(owner_data.origin + owner_data.eye_position);
}

void c_camera::update_transform()
{
    glm::vec3 fwd;
    fwd.x = -sin(glm::radians(angles.y)) * cos(glm::radians(angles.x));
    fwd.y = -sin(glm::radians(angles.x));
    fwd.z = cos(glm::radians(angles.y)) * cos(glm::radians(angles.x));

    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    forward = glm::normalize(fwd);
    right = glm::normalize(glm::cross(forward, world_up));
    up = glm::normalize(glm::cross(right, forward));
}

void c_camera::process_input(glm::vec2 view_angle)
{
    angles += view_angle;
    angles = g_input->clamp_angles(angles);
}