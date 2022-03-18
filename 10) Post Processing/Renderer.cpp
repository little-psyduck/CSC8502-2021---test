#include "Renderer.h"
#include "../nclgl/HeightMap.h"

const int POSE_PASSES = 10;

Renderer::Renderer(Window &parent) :OGLRenderer(parent) {
	camera = new Camera(0, 0, Vector3(150, 300, -100));
	quad = Mesh::GenerateQuad();

	height_map = new HeightMap(TEXTUREDIR"noise.png");
	height_texture = SOIL_load_OGL_texture
	(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!height_texture)
		return;

	SetTextureRepeating(height_texture, true);

	scene_shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	if (!scene_shader->LoadSuccess())
		return;
	process_shader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	if (!process_shader->LoadSuccess())
		return;
	//
	glGenFramebuffers(1, &buffer_FBO);
	glGenFramebuffers(1, &process_FBO);

	glBindFramebuffer(GL_FRAMEBUFFER, buffer_FBO);
	//
	glGenTextures(1, &buffer_depth_texture);
	glBindTexture(GL_TEXTURE_2D,buffer_depth_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D
	(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	if (!buffer_depth_texture)
		return;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer_depth_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, buffer_depth_texture, 0);
	//

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &buffer_color_texture[i]);
		glBindTexture(GL_TEXTURE_2D, buffer_color_texture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D
		(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	if (!buffer_color_texture)
		return;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_color_texture[0], 0);
	
	//
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;
	//
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);

	init = true;
}

Renderer::~Renderer() {
   delete  scene_shader;
   delete  process_shader;
   delete  height_map;
   delete  quad; 
   delete  camera;

   glDeleteTextures(2, buffer_color_texture);
   glDeleteTextures(1, &buffer_depth_texture);
   glDeleteFramebuffers(1, &process_FBO);
   glDeleteFramebuffers(1, &buffer_FBO);
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	DrawScene();
	DrawPostProcess();
	PresentScene();
}

void Renderer::DrawScene() { 
	glBindFramebuffer(GL_FRAMEBUFFER, buffer_FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	BindShader(scene_shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(scene_shader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, height_texture);
	height_map->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, process_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_color_texture[1], 0);
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(process_shader);
	modelMatrix.ToIdentity(); 
	viewMatrix.ToIdentity(); 
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(process_shader->GetProgram(), "SceneTex"), 0);//question

	for (int i = 0; i < POSE_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_color_texture[1], 0);
		glUniform1i(glGetUniformLocation(process_shader->GetProgram(), "isVertical"), 0);
		
		glBindTexture(GL_TEXTURE_2D, buffer_color_texture[0]);
		quad->Draw();
		//
		glUniform1i(glGetUniformLocation(process_shader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_color_texture[0], 0);

		glBindTexture(GL_TEXTURE_2D, buffer_color_texture[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(scene_shader);

	modelMatrix.ToIdentity(); 
	viewMatrix.ToIdentity(); 
	projMatrix.ToIdentity();

	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buffer_color_texture[0]);
	glUniform1i(glGetUniformLocation(scene_shader->GetProgram(), "diffuseTex"), 0);
	
	quad->Draw();
}