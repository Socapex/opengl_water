#pragma once
#include "engine/window.h"
#include "engine/component.h"
#include "engine/globals.h"
#include "components/transform.h"
#include "components/camera.h"

using namespace std::placeholders;

namespace {
	inline bool key_down(int key) {
		auto w = Window::main->window;
		int state = glfwGetKey(w, key);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}
} // namespace anonymous

struct EditorCamera : public Component {
	void init() override {
		_camera = entity->add_component<Camera>();
		_transform = entity->get_component<Transform>();
		_transform->position = { 0.f, 2.f, 5.f };

		Window::main->set_on_mouse_button(std::bind(&EditorCamera::on_mouse_click,
				this, _1, _2, _3));
		Window::main->set_on_mouse_pos(std::bind(&EditorCamera::on_mouse_move,
				this, _1, _2));
		Window::main->set_on_mouse_scroll(std::bind(&EditorCamera::on_mouse_scroll,
				this, _1, _2));
//		Window::main->set_on_key(std::bind(&EditorCamera::on_key_down,
//				this, _1, _2, _3, _4));

		_transform->look_at({0.f, 0.f, 0.f});
	}

	void update(float dt) override {
		/* Shitty but whatever. */
		if (_right_click) {
			if (key_down(GLFW_KEY_W)) {
				_transform->position += _transform->forward() * dt * move_speed;
			}
			if (key_down(GLFW_KEY_S)) {
				_transform->position += _transform->forward() * dt * -move_speed;
			}
			if (key_down(GLFW_KEY_D)) {
				_transform->position += _transform->right() * dt * move_speed;
			}
			if (key_down(GLFW_KEY_A)) {
				_transform->position += _transform->right() * dt * -move_speed;
			}
			if (key_down(GLFW_KEY_Q)) {
				_transform->position.y += dt * -move_speed;
			}
			if (key_down(GLFW_KEY_E)) {
				_transform->position.y += dt * move_speed;
			}
		}

		_last_dt = dt;
	}

private:
//	void on_key_down(int, int, int action, int) {
//		if (action == GLFW_RELEASE)
//			return;
//
////		switch (key) {
////			case GLFW_KEY_W: {
////				_transform->position += _transform->forward() * _last_dt
////						* move_speed;
////			} break;
////			case GLFW_KEY_S: {
////				_transform->position += _transform->forward() * _last_dt
////						* -move_speed;
////			} break;
////		}
//	}

	void on_mouse_move(double x, double y) {
		glm::dvec2 mouse_pos{ x, y };
		glm::dvec2 mouse_delta = mouse_pos - _last_mouse_pos;

		if (_left_click && !_right_click) {
			glm::quat horiz = glm::angleAxis(
					glm::radians(-(float)mouse_delta.x * _last_dt * mouse_speed),
					glm::vec3{0.f, 1.f, 0.f});
			_transform->position = horiz * _transform->position;
			_transform->rotation = horiz * _transform->rotation;

			glm::quat vert = glm::angleAxis(
					glm::radians(-(float)mouse_delta.y * _last_dt * mouse_speed),
					_transform->right());
			_transform->position = vert * _transform->position;
			_transform->rotation = vert * _transform->rotation;
		}

		if (_right_click) {
			glm::quat horiz = glm::angleAxis(
					glm::radians(-(float)mouse_delta.x * _last_dt * mouse_speed),
					glm::vec3{0.f, 1.f, 0.f});
			_transform->rotation = horiz * _transform->rotation;

			glm::quat vert = glm::angleAxis(
					glm::radians(-(float)mouse_delta.y * _last_dt * mouse_speed),
					_transform->right());
			_transform->rotation = vert * _transform->rotation;
		}

		if (_middle_click) {
			_transform->position += _transform->right() * _last_dt
					* -mouse_delta.x;
			_transform->position += _transform->up() * _last_dt
					* mouse_delta.y;
		}

		_last_mouse_pos = mouse_pos;
	}

	void on_mouse_scroll(double, double y) {
		if (!_right_click) {
			_transform->position += _transform->forward() * _last_dt
					* move_speed * y;
		}

		if (_right_click) {
			move_speed += y;
		}
	}

	void on_mouse_click(int button, int action, int) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				glfwGetCursorPos(Window::main->window, &_last_mouse_pos.x,
						&_last_mouse_pos.y);
				glfwSetInputMode(Window::main->window, GLFW_CURSOR,
						GLFW_CURSOR_HIDDEN);
				_left_click = true;
			} else if (action == GLFW_RELEASE) {
				glfwSetInputMode(Window::main->window, GLFW_CURSOR,
						GLFW_CURSOR_NORMAL);
				_left_click = false;
			}
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (action == GLFW_PRESS) {
				glfwGetCursorPos(Window::main->window, &_last_mouse_pos.x,
						&_last_mouse_pos.y);
				glfwSetInputMode(Window::main->window, GLFW_CURSOR,
						GLFW_CURSOR_HIDDEN);
				_right_click = true;
			} else if (action == GLFW_RELEASE) {
				glfwSetInputMode(Window::main->window, GLFW_CURSOR,
						GLFW_CURSOR_NORMAL);
				_right_click = false;
			}
		}

		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (action == GLFW_PRESS) {
				glfwGetCursorPos(Window::main->window, &_last_mouse_pos.x,
						&_last_mouse_pos.y);
				glfwSetInputMode(Window::main->window, GLFW_CURSOR,
						GLFW_CURSOR_HIDDEN);
				_middle_click = true;
			} else if (action == GLFW_RELEASE) {
				glfwSetInputMode(Window::main->window, GLFW_CURSOR,
						GLFW_CURSOR_NORMAL);
				_middle_click = false;
			}
		}
	}

public:
	float mouse_speed = 15.f;
	float move_speed = 15.f;

private:
	Transform* _transform;
	Camera* _camera;

	bool _left_click = false;
	bool _right_click = false;
	bool _middle_click = false;
	float _last_dt = 0.f;
	glm::dvec2 _last_mouse_pos = { 0.f, 0.f };
};
