#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	height_map = new HeightMap(TEXTUREDIR"noise.png");
	//
	texture = SOIL_load_OGL_texture
	(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!texture)
		return;
	SetTextureRepeating(texture, true);

	bumpmap = SOIL_load_OGL_texture
	(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!bumpmap) 
		return;
	SetTextureRepeating(bumpmap, true);
	//
	Vector3 height_map_size = height_map->GetHeightMapSize();
	camera = new Camera(-45.0f, 0.0f, height_map_size * Vector3(0.5f, 1.0f, 0.5f));

	light = new Light
	(height_map_size * Vector3(0.5f, 1.0f, 0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), height_map_size.x * 0.5f);

	shader = new Shader("BumpVertex.glsl", "BumpFragment.glsl");
	if (!shader->LoadSuccess())
		return;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);

	init = true;
}

Renderer ::~Renderer() {
	delete height_map;
	delete camera;
	delete shader;
	delete light;
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &bumpmap);
	camera = nullptr;
	height_map = nullptr;
	shader = nullptr;
	light = nullptr;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shader);
	//
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumpmap);
	//
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	UpdateShaderMatrices();
	SetShaderLight(*light);

	height_map->Draw();
}