#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"

class Renderer :public OGLRenderer {
protected:
	HeightMap* height_map;
	Shader* shader;
	Camera* camera;
	GLuint terrain_tex;

public:
	Renderer::Renderer(Window& parent);
	virtual ~Renderer();
	
	virtual void RenderScene();
	
	virtual void UpdateScene(const float &dt);
};