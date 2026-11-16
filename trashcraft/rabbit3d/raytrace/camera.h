#pragma once
#include "raytrace.h"
#include "entity/entity_list.h"

#include <glm.hpp>
#include <mutex>

class c_camera
{
public:
    enum e_mode
    {
        FIRST_PERSON,
        THIRD_PERSON_BACK,
        THIRD_PERSON_FORWARD
    };
private:
	struct owner_data_t
	{
		glm::vec3 origin = glm::vec3(0.0f);
		glm::vec3 eye_position = glm::vec3(0.0f);
	};

	int mode = 0;

	c_entity* owner = nullptr;
	owner_data_t owner_data = { };

	float field_of_view = 68.0f, aspect_ratio = 1.0f;

	glm::vec3 origin = glm::vec3(0.0f), transform_origin = glm::vec3(0.0f);
	glm::vec2 angles = glm::vec2(0.0f);

	glm::vec3 forward = glm::vec3(0.0f);
	glm::vec3 right = glm::vec3(0.0f);
	glm::vec3 up = glm::vec3(0.0f);

    float near_plane = 0.1f, far_plane = 256.0f;
public:
	void update();
	void update_mode();
	void update_transform();
	void update_owner();

	void process_input(glm::vec2 view_angle);

	inline void set_owner(c_entity* owner) { this->owner = owner; };

	inline float get_field_of_view() { return this->field_of_view; };
	inline void set_field_of_view(float fov) { this->field_of_view = fov; };

	inline float get_aspect_ratio() { return this->aspect_ratio; };
	inline void set_aspect_ratio(float aspect) { this->aspect_ratio = aspect; };

	inline glm::vec3 get_origin() { return this->origin + this->transform_origin; };
	inline void set_origin(const glm::vec3& origin) { this->origin = origin; };

	inline glm::vec2 get_angles() { return angles; };
	inline void set_angles(const glm::vec2& angle) { this->angles = angle; };

	inline float get_near_plane() { return this->near_plane; };
	inline float get_far_plane() { return this->far_plane; };

	inline void set_near_plane(float near_plane) { this->near_plane = near_plane; };
	inline void set_far_plane(float far_plane) { this->far_plane = far_plane; };

	inline int get_mode() { return this->mode; };
	inline void set_mode(int mode) { this->mode = mode; };

	inline glm::vec3 get_forward() { return this->forward; };
	inline glm::vec3 get_right() { return this->right; };
	inline glm::vec3 get_up() { return this->up; };

	inline glm::mat4 get_view_matrix() { return glm::lookAt(get_origin(), get_origin() + this->forward, this->up); };
	inline glm::mat4 get_projection_matrix() { return glm::perspective(glm::radians(this->field_of_view), this->aspect_ratio, this->near_plane, this->far_plane); };
};
