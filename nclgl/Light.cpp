#include "Light.h"

Light::Light(const Vector3& position_, const Vector4& colour_, const float& radius_):
	position(position_), color(colour_), radius(radius_) { }

Vector3 Light::GetPosition() const {
	return position;
}

void Light::SetPosition(const Vector3& val) {
	position = val;
}

float Light::GetRadius() const {
	return radius;
}

void Light::SetRadius(const float& val) {
	radius = val;
}

Vector4 Light::GetColour() const {
	return color;
}

void Light::SetColour(const Vector4& val) {
	color = val;
}
