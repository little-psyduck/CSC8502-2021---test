#include "renderer.h"

Renderer::Renderer(Window& parent):OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	basic_shader = new Shader("vertex.glsl", "fragment.glsl");

	if (!basic_shader->LoadSuccess())
		return;
	init = true;
}

Renderer::~Renderer() {
	delete triangle;
	delete basic_shader;
	triangle = nullptr;
	basic_shader = nullptr;
}

void Renderer::RenderScene() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(basic_shader);
	triangle->Draw();
}