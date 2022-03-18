#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Shader.h"

enum {SHADOWSIZE = 2048};

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0, -25, Vector3(-8.0f, 5.0f, 8.0f));
	
	light = new Light(Vector3(-20.0f, 10.0f, -20.0f), Vector4(1, 1, 1, 1), 250.0f);
	//
	scene_shader = new Shader("shadowscenevert.glsl", "shadowscenefrag.glsl");
	if (!scene_shader->LoadSuccess())
		return;

	shadow_shader = new Shader("shadowVert.glsl", "shadowFrag.glsl");
	if (!shadow_shader->LoadSuccess())
		return;
	//
	glGenTextures(1, &shadow_tex);
	glBindTexture(GL_TEXTURE_2D, shadow_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	glGenFramebuffers(1, &shadow_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_tex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//
	scene_meshes.emplace_back(Mesh::GenerateQuad());
	scene_meshes.emplace_back(Mesh::LoadFromMeshFile("Sphere.msh"));
	scene_meshes.emplace_back(Mesh::LoadFromMeshFile("Cylinder.msh"));	
	scene_meshes.emplace_back(Mesh::LoadFromMeshFile("Cone.msh"));

	scene_diffuse = SOIL_load_OGL_texture
	(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!scene_diffuse)
		return;
	SetTextureRepeating(scene_diffuse, true);
	
	scene_bump = SOIL_load_OGL_texture
	(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!scene_bump)
		return;
	SetTextureRepeating(scene_bump, true);
	//
	glEnable(GL_DEPTH_TEST);

	scene_transform.resize(4);
	scene_transform[0] = Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(10, 10, 1));
	scene_time = 0.0f;

	init = true;
}

Renderer::~Renderer() {
	glDeleteFramebuffers(1, &shadow_FBO);
	glDeleteTextures(1, &shadow_tex);

	delete camera;
	delete scene_shader;
	delete shadow_shader;
	camera = nullptr;
	scene_shader = nullptr;
	shadow_shader = nullptr;

	for (auto& i : scene_meshes) {
		delete i;
		i = nullptr;
	}
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	scene_time += dt;

	for (int i = 1; i < 4; ++i) {
		Vector3 t = Vector3(-10 + (5 * i), 2.0f + sin(scene_time * i), 0);
		scene_transform[i] = Matrix4::Translation(t) * Matrix4::Rotation(scene_time * 10 * i, Vector3(1, 0, 0));
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawShadowScene();
	DrawMainScene();
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_FBO);
	
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadow_shader);
	//
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1.0f, 100.0f, 1.0f, 45.0f);
	shadowMatrix = projMatrix * viewMatrix;

	for (int i = 0; i < 4; ++i) {
		modelMatrix = scene_transform[i];
		UpdateShaderMatrices();
		scene_meshes[i]->Draw();
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawMainScene() {
	BindShader(scene_shader);
	SetShaderLight(*light);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//
	glUniform1i(glGetUniformLocation(scene_shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, scene_diffuse);

	glUniform1i(glGetUniformLocation(scene_shader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, scene_bump);

	glUniform1i(glGetUniformLocation(scene_shader->GetProgram(), "shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadow_tex);
	//
	glUniform3fv(glGetUniformLocation(scene_shader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	for (int i = 0; i < 4; ++i) {
		modelMatrix = scene_transform[i];
		UpdateShaderMatrices();
		scene_meshes[i]->Draw();
	}
}