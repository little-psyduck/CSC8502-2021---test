#include "HeightMap.h"

#include <iostream>

HeightMap::HeightMap(const std::string& name,
	const float& stretch_fac, const float& height_fac, const float& tex_fac) {

	int channels;

	GLubyte* data = SOIL_load_image(name.c_str(), &width, &height, &channels, 1);

	if (!data) {
		std::cout << "Heightmap  can't load  file!\n";
		return;
	}

	numVertices = width * height;
	numIndices = (width - 1) * (height - 1) * 6;

	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector3 vertex_scale = Vector3(stretch_fac, height_fac, stretch_fac);
	Vector2 texture_scale = Vector2(1.0f / tex_fac, 1.0f / tex_fac);

	for (unsigned int z = 0; z < height; ++z) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int offset = z * width + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertex_scale;
			textureCoords[offset] = Vector2(x, z) * texture_scale;
		}
	}

	SOIL_free_image_data(data);

	int i = 0;

	for (int z = 0; z < height - 1; ++z) {
		for (int x = 0; x < width - 1; ++x) {
			int a = (z * width) + x;
			int b = (z * width) + (x + 1);
			int c = ((z + 1) * width) + (x + 1);
			int d = ((z + 1) * width) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();

	BufferData();

	height_map_size.x = vertex_scale.x * (width - 1);
	height_map_size.y = vertex_scale.y * 255.0f;
	height_map_size.z = vertex_scale.z * (height - 1);
}

Vector3 HeightMap::GetHeightMapSize() const{
	return height_map_size;
}

float HeightMap::AreaHeight(const unsigned int& x, const unsigned int& z) const {
	unsigned int index = z * width + x;
	return vertices[index].y;
}

void HeightMap::SetWorldPos(const Vector3& pos) {
	world_position.SetPositionVector(pos);
}
Matrix4 HeightMap::GetWorldPos() const {
	return world_position;
}