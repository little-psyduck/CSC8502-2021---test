#pragma once

#include "../nclgl/OGLRenderer.h"

class Camera;
class HeightMap;
class Shader;
class Light;

class Renderer :public OGLRenderer {
protected:
	HeightMap* height_map;
	Camera* camera;
	Shader* shader;
	Light* light;
	GLuint texture;
	GLuint bumpmap;

public:
	Renderer(Window& parent);
	virtual ~Renderer();
	virtual void UpdateScene(float dt);
	virtual void RenderScene();
};