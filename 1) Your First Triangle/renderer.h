#pragma once
#include "../nclgl/OGLRenderer.h"

class Renderer :public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();
	virtual void RenderScene();

protected:
	Mesh* triangle;
	Shader* basic_shader;
};
