#pragma once

#include "Mesh.h"
#include <string>

class HeightMap :public Mesh {
protected:
	Vector3 height_map_size;
	int width, height;

	Matrix4 world_position;

public:
	HeightMap(const std::string& name,
		const float& stretch_fac, const float& height_fac, const float& tex_fac);
	~HeightMap() = default;

	Vector3 GetHeightMapSize() const;

	void SetWorldPos(const Vector3 &pos);
	Matrix4 GetWorldPos() const;

	float AreaHeight(const unsigned int& x, const unsigned int& z) const;
};