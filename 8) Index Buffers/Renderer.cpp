#include "Renderer.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	height_map = new HeightMap(TEXTUREDIR"cliff_height.png");

	Vector3 dimensions = height_map->GetHeightMapSize();
	camera = new Camera(0, 0, dimensions * Vector3(0.5, 2, 0.5));

	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	if (!shader->LoadSuccess())
		return;

	terrain_tex = SOIL_load_OGL_texture
	(TEXTUREDIR"shore_sand_albedo.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!terrain_tex)
		return;
	SetTextureRepeating(terrain_tex, true);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;
}

Renderer::~Renderer() {
	delete camera;
	delete shader;
	delete height_map;
	
	glDeleteTextures(1, &terrain_tex);

	camera = nullptr;
	shader = nullptr;
	height_map = nullptr;
}

void Renderer::UpdateScene(const float &dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrain_tex);

	height_map->Draw();
}