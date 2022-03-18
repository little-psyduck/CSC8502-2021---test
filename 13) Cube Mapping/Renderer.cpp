#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	quad = Mesh::GenerateQuad();

	height_map = new HeightMap(TEXTUREDIR"noise.png", 16, 1, 16);
	//
	water_tex = SOIL_load_OGL_texture
	(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!water_tex)
		return;
	SetTextureRepeating(water_tex, true);

	earth_tex = SOIL_load_OGL_texture
	(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!earth_tex)
		return;
	SetTextureRepeating(earth_tex, true);

	earth_bump = SOIL_load_OGL_texture
	(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!earth_bump)
		return;
	SetTextureRepeating(earth_bump, true);
	//
	cube_map = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"sky.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!cube_map)
		return;
	//
	reflect_shader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	if (!reflect_shader->LoadSuccess())
		return;

	skybox_shader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	if (!skybox_shader->LoadSuccess())
		return;

	light_shader = new Shader("BumpVertex.glsl", "BumpFragment.glsl");
	if (!light_shader->LoadSuccess())
		return;
	//
	Vector3 heightmapSize = height_map->GetHeightMapSize();
	camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.5f, 5.0f, 0.5f));
	light = new Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	water_cycle = 0.0f;
	water_rotate = 0.0f;

	init = true;
}

Renderer ::~Renderer() {
	delete height_map;
	delete camera;
	delete light_shader;
	delete reflect_shader;
	delete skybox_shader;
	delete light;
	delete quad;
	glDeleteTextures(1, &water_tex);
	glDeleteTextures(1, &earth_tex);
	glDeleteTextures(1, &earth_bump);
	camera = nullptr;
	height_map = nullptr;
	light_shader = nullptr;
	reflect_shader = nullptr;
	skybox_shader = nullptr;
	light = nullptr;
	quad = nullptr;
}

void Renderer::UpdateScene(const float &dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();

	water_rotate += dt * 2;
	water_cycle += dt * 0.25;

	if (water_rotate == 10000)
		water_rotate = 0;
	if (water_cycle == 10000)
		water_cycle = 0;
		
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox(); 
	DrawHeightmap(); 
	DrawWater();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	
	BindShader(skybox_shader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	BindShader(light_shader);

	SetShaderLight(*light);
	glUniform3fv
	(glGetUniformLocation(light_shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	//
	glUniform1i(glGetUniformLocation(light_shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earth_tex);

	glUniform1i(glGetUniformLocation(light_shader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earth_bump);
	//
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	height_map->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflect_shader);

	glUniform3fv
	(glGetUniformLocation(reflect_shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	//
	glUniform1i(glGetUniformLocation(reflect_shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, water_tex);

	glUniform1i(glGetUniformLocation(reflect_shader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
	//
	Vector3 height_size = height_map->GetHeightMapSize();

	modelMatrix = Matrix4::Translation(height_size * 0.5f) 
		* Matrix4::Scale(height_size * 0.5f) 
		* Matrix4::Rotation(90, Vector3(1.0f, 1.0f, 0.0f));

	textureMatrix = Matrix4::Translation(Vector3(water_cycle,0.0f,water_cycle))
		* Matrix4::Scale(Vector3(10.0f,10.0f,10.0f))
		* Matrix4::Rotation(water_rotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();
}

