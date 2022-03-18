#include "Window.h"
#include "Camera.h"

#include <algorithm>

void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::max(pitch, -90.0f);
	pitch = std::min(pitch, 90.0f);

	if (yaw > 360)
		yaw -= 360.0f;
	if (yaw < 0)
		yaw += 360.0f;

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0.0f, 1.0f, 0.0f));
	Vector3 forward = rotation * Vector3(0.0f, 0.0f, -1.0f);
	Vector3 right = rotation * Vector3(1.0f, 0.0f, 0.0f);
	
	float speed = speed_coe * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
		position += (forward * speed);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
		position -= (forward * speed);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
		position -= (right * speed);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
		position += (right * speed);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		position.y -= speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
		position.y += speed;
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1.0f, 0.0f, 0.0f))
		* Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f))
		* Matrix4::Translation(-position);
}

void Camera::FixPoint(){
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_N)) {
		if (point_data.empty())
			point_data.emplace_back(position);

		else if (position != point_data.back())
			point_data.emplace_back(position);
	}
}