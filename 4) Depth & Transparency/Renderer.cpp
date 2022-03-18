#include "Renderer.h"

Renderer::Renderer(Window &parent):OGLRenderer(parent) {
	mesh[0] = Mesh::GenerateQuad();
	mesh[1] = Mesh::GenerateTriangle();

	textures[0] = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[1] = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!(textures[0] || textures[1]))
		return;

	positions[0] = Vector3(0.0f, 0.0f, -5.0f);
	positions[1] = Vector3(0.0f, 0.0f, -5.0f);

	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	if (!shader->LoadSuccess())
		return;

	modify_object = true;
	using_depth = false;
	using_alpha = false;
	blend_mode = 0;

	projMatrix = Matrix4::Perspective(1.0f, 100.0f, (float)width / (float)height, 45.0f);
	init = true;
}

Renderer::~Renderer() {
	delete mesh[0];
	delete mesh[1];
	delete shader;

	mesh[0] = nullptr;
	mesh[1] = nullptr;
	shader = nullptr;

	glDeleteTextures(2, textures);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	
	for (int i = 0; i < 2; ++i) {
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false,
			(float*)&Matrix4::Translation(positions[i]));

		glBindTexture(GL_TEXTURE_2D, textures[i]);

		mesh[i]->Draw();
	}
}

void Renderer::ToggleObject() {
	modify_object = !modify_object;
}

void Renderer::MoveObject(float by) {
	positions[(int)modify_object].z += by;
}

void Renderer::ToggleBlendMode() {
	blend_mode = (blend_mode + 1) % 4;
	switch (blend_mode) {
	case 0:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case 1:
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
		break;
	case 2:
		glBlendFunc(GL_ONE, GL_ZERO);
		break;
	case 3:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	};
}

void Renderer::ToggleAlphaBlend() {
	using_alpha = !using_alpha;
	using_alpha ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void Renderer::ToggleDepth(){
	using_depth = !using_depth;
	using_depth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}