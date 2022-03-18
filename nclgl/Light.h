#pragma once

#include "Vector3.h"
#include "Vector4.h"

class Light {
protected:
	float radius;
	Vector4 color;
	Vector3 position;

public:
	Light() = default;
	Light(const Vector3& position, const Vector4& colour, const float& radius);
	~Light() = default;

	Vector3 GetPosition() const;
	void SetPosition(const Vector3& val);

	float GetRadius() const;
	void SetRadius(const float &val);

	Vector4 GetColour() const;
	void SetColour(const Vector4& val);
};