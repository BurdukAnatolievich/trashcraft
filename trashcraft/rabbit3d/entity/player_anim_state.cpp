#include "entity_list.h"
#include <time/time.h>

void c_player_animation_state::reset()
{
    firstperson_hand_swing = 0.0f;
}

void c_player_animation_state::update(float eye_yaw, float eye_pitch)
{
    if (!owner) {
        return;
    }

    if (!initialized) {
        reset();
        initialized = true;
    }

    if (owner->attack_progress > 0.0f) {
        if (!firstperson_swinging) {
            firstperson_hand_swing = 0.08f;
            firstperson_last_hand_swing = 0.08f;
        }
        firstperson_swinging = true;
    }

    if (firstperson_swinging) {
        firstperson_last_hand_swing = firstperson_hand_swing;
        firstperson_hand_swing += g_time->get_frame_time() / 0.25f;

        if (firstperson_hand_swing >= 1.0f) {
            if (owner->attack_progress <= 0.0f) {
                firstperson_hand_swing = 0.0f;
                firstperson_last_hand_swing = 0.0f;
                firstperson_swinging = false;
            }
            else {
                firstperson_hand_swing -= 1.0f;
                firstperson_last_hand_swing = 0.0f;
            }
        }
    }
    //if (current_move_speed < 0.03f)
    //{
    //    float head_yaw_delta = fabs(g_math->NormalizeYaw(head_yaw - last_head_yaw));
    //    float body_yaw_delta = fabs(g_math->NormalizeYaw(head_yaw - body_yaw));

    //    if (body_yaw_delta >= body_yaw_rotation_threshold || head_yaw_delta >= body_yaw_per_tick_rotation_threshold)
    //        body_yaw = head_yaw;
    //}
    //else
    //{
    //    float body_yaw_add = std::clamp(owner->move_angle.yaw - head_yaw, -body_yaw_rotation_threshold, body_yaw_rotation_threshold);
    //    body_yaw = head_yaw + body_yaw_add;
    //}

    //c_model* model = (c_model*)owner->get_model();
    //if (!model)
    //    return;

    //for (c_mesh& mesh : model->meshes)
    //{
    //    if (mesh.name == "head")
    //    {
    //        mesh.rotation.Init(0.0f, head_yaw, 0.0f);

    //        mesh.pivot_point.Init(0.0f, 0.0f, -1.45f);
    //        mesh.pivot_point_rotation.Init(head_pitch, 0.0f, 0.0f);
    //    }
    //    else if (mesh.name == "body")
    //    {
    //        mesh.rotation.Init(0.0f, body_yaw, 0.0f);

    //        mesh.pivot_point.Init(0.0f, 0.0f, -1.0f);
    //        mesh.pivot_point_rotation.Init(0.0f, 0.0f, 0.0f);
    //    }
    //    else if (mesh.name == "left_arm")
    //    {
    //        mesh.rotation.Init(0.0f, body_yaw, 0.0f);

    //        mesh.pivot_point.Init(0.0f, 0.0f, -1.4f);
    //        mesh.pivot_point_rotation.Init(left_arm_swing, 0.0f, 0.0f);
    //    }
    //    else if (mesh.name == "right_arm")
    //    {
    //        mesh.rotation.Init(0.0f, body_yaw, 0.0f);

    //        mesh.pivot_point.Init(0.0f, 0.0f, -1.4f);
    //        mesh.pivot_point_rotation.Init(right_arm_swing, 0.0f, 0.0f);
    //    }
    //    else if (mesh.name == "left_leg")
    //    {
    //        mesh.rotation.Init(0.0f, body_yaw, 0.0f);

    //        mesh.pivot_point.Init(0.0f, 0.0f, -0.74f);
    //        mesh.pivot_point_rotation.Init(left_leg_swing, 0.0f, 0.0f);
    //    }
    //    else if (mesh.name == "right_leg")
    //    {
    //        mesh.rotation.Init(0.0f, body_yaw, 0.0f);

    //        mesh.pivot_point.Init(0.0f, 0.0f, -0.74f);
    //        mesh.pivot_point_rotation.Init(right_leg_swing, 0.0f, 0.0f);
    //    }
    //}
}
