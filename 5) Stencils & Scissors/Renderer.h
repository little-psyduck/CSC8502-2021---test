#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer :public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();

	void ToggleScissor() {
		using_scissor = !using_scissor;
	}
	void ToggleStencil() {
		using_stencil = !using_stencil;
	}
protected:
	Mesh* meshes[2];
	Shader* shader;
	GLuint textures[2];

	bool using_scissor;
	bool using_stencil;
};