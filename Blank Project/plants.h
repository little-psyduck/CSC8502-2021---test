#pragma once
#include <vector>
#include <string>

#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/SceneNode.h"

class Mesh;
class MeshMaterial;
class Renderer;

class Plants :public SceneNode {
protected:
	Shader* plants_shader;
	MeshMaterial* plants_material;

	std::vector<GLuint> diffuse_textures;
	std::vector<GLuint> bump_textures;
public:
	Plants();
	Plants(const std::string& obj_name, const std::string& shader);
	Plants(const Plants &copy);
	void Update(const float& dt) override;
	void RendObject(OGLRenderer& renderer, Camera* camera) override;

	~Plants();
};
