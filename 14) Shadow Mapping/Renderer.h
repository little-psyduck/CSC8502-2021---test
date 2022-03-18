#pragma once

#include "../nclgl/OGLRenderer.h"

class Light;
class Camera;
class Mesh;

class Renderer :public OGLRenderer {
protected:
	Camera* camera;
	Light* light;
	//
	Shader* scene_shader;
	Shader* shadow_shader;
	
	GLuint shadow_tex;
	GLuint shadow_FBO;
	//
	GLuint scene_diffuse;
	GLuint scene_bump;
	float scene_time;

	vector<Mesh*> scene_meshes;
	vector<Matrix4> scene_transform;
	//
	void DrawShadowScene();
	void DrawMainScene();
public:
	Renderer(Window& parent);
	virtual ~Renderer();
	virtual void UpdateScene(float dt);
	virtual void RenderScene();
};
