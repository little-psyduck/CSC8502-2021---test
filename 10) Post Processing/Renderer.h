#pragma once

#include "../nclgl/OGLRenderer.h"

class HeightMap;
class Camera;

class Renderer :public OGLRenderer {
protected:
	Shader* scene_shader;
	Shader* process_shader;

	Camera* camera;

	Mesh* quad;
	HeightMap* height_map;

	GLuint height_texture;

	GLuint buffer_FBO;
	GLuint process_FBO;

	GLuint buffer_color_texture[2];
	GLuint buffer_depth_texture;

	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void UpdateScene(float dt);
	virtual void RenderScene();
};
