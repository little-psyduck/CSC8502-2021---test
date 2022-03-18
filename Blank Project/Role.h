#pragma once
#include <vector>
#include <string>

#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/SceneNode.h"

class Mesh;
class MeshAnimation;
class MeshMaterial;
class Renderer;

class Role :public SceneNode {
protected:
	int current_frame;
	float frame_time;

	Shader* role_shader;
	MeshAnimation* role_anime;
	MeshMaterial* role_material;

	std::vector<GLuint> diffuse_textures;
	std::vector<GLuint> bump_textures;
public:
	Role(const std::string& obj_name, const std::string& shader);
	void Update(const float& dt) override;
	void RendObject(OGLRenderer& renderer, Camera* camera) override;

	~Role();
};
