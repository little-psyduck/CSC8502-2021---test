#pragma once

#include "../nclgl/OGLRenderer.h"

class Mesh;
class Camera;
class MeshAnimation;
class MeshMaterial;

class Renderer :public OGLRenderer {
protected:
	Camera* camera;
	Mesh* mesh;
	Shader* shader;
	MeshAnimation* anime;
	MeshMaterial* material;

	vector<GLuint> mat_textures;

	int current_frame;
	float frame_time;

public:
	Renderer(Window& parent);
	virtual ~Renderer();
	
	void UpdateScene(const float &dt) override;
	void RenderScene() override;
};