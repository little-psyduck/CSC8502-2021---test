#pragma once

#include "Plane.h"
#include "SceneNode.h"
#include "Matrix4.h"

class Frustum {
protected:
	Plane plane[6];
public:
	Frustum() = default;
	~Frustum() = default;

	void FromMatrix(const Matrix4& mat);
	bool InsideFrustum(SceneNode* node);
};