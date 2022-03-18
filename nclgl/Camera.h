#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include <vector>
class Camera {
public:
	Camera() { yaw = 0.0f; pitch = 0.0f; speed_coe = 100.0f; point_data.resize(0); }
	Camera(const float& yaw_a, const float& pitch_a, const Vector3& pos_a) {
		yaw = yaw_a;
		pitch = pitch_a;
		position = pos_a;
		speed_coe = 100.0f;

		point_data.resize(0);
	}
	~Camera() = default;

	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	float GetYaw() { return yaw; }
	void SetYaw(const float& val) { yaw = val; }

	float GetPitch() { return pitch; }
	void SetPitch(const float& val) { pitch = val; }

	Vector3 GetPosition() { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	void SetSpeed(const float& v) { speed_coe = v; }

	void FixPoint();

protected:
	float yaw;
	float pitch;
	float speed_coe;
	Vector3 position;

	std::vector<Vector3> point_data;
};