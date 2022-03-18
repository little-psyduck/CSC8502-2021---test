#include "cw_Renderer.h"
#include "../nclgl/HeightMap.h"
#include <iostream>
#include <string>

float water_rotate;
float water_cycle;

using std::cout;

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	//initiate succeed
	init = true;
	//loading
	LoadSkyBox();
	LoadLights();
	LoadTerrain();
	LoadCamera();

	//open tests
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

Renderer::~Renderer() {
	//delete camera
	delete camera;
	camera = nullptr;

	//delete light
	
	delete[] point_lights;
	glDeleteTextures(1, &buffer_colour_tex);
	glDeleteTextures(1, &buffer_normal_tex);
	glDeleteTextures(1, &buffer_depth_tex);
	glDeleteTextures(1, &light_diffuse_tex);
	glDeleteTextures(1, &light_specular_tex);

	glDeleteFramebuffers(1, &buffer_FBO);
	glDeleteFramebuffers(1, &point_light_FBO);
	
	//delete skybox
	delete skybox_mesh;
	skybox_mesh = nullptr;
	delete skybox_shader;
	skybox_shader = nullptr;
	glDeleteTextures(1, &skybox_texture);

	//delete terrain
	delete terrain_height;
	terrain_height = nullptr;
	delete terrain_shader;
	terrain_shader = nullptr;
	glDeleteTextures(1, &terrain_texture_ground);
	glDeleteTextures(1, &terrain_texture_weed);
	glDeleteTextures(1, &terrain_bump_weed);
	glDeleteTextures(1, &terrain_bump_ground);
	
	//delete water
	delete water_shader;
	water_shader = nullptr;
	glDeleteTextures(1, &water_texture);
	glDeleteTextures(1, &water_bump);

	delete root;
}

void Renderer::RenderScene() {
	//clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	FillBuffer();

	PointLightRender();
	CombineBuffer();
}

void Renderer::UpdateScene(const float& dt) {
	//refresh view
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	UpdateWater(dt);
	//UpdateNodes(dt);

}

#pragma region Camera
void Renderer::LoadCamera() {
	//initiate camera
	camera = new Camera
	(-130.0f, -20.0f, Vector3(terrain_height->GetHeightMapSize() / 2));
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera->SetSpeed(1000);
}

#pragma endregion


#pragma region Lights
void Renderer::LoadLights() {
	//initiate point lights
	point_lights = new Light[LIGHTS_NUMBER];
	light_obj = Mesh::LoadFromMeshFile("Sphere.msh");

	screen = Mesh::GenerateQuad();
	
	for (int i = 0; i < LIGHTS_NUMBER; ++i) {
		point_lights[i].SetPosition(LIGHT_POS[i]);
		point_lights[i].SetColour(Vector4(1.0f, 0.6f, 0.8f, 1.0f));
		point_lights[i].SetRadius(1000.0);
	}

	light_shader = new Shader(LIGHTSHADER"PointLightVertex.glsl", LIGHTSHADER"PointLightFragment.glsl");
	if (!light_shader->LoadSuccess()) {
		cout << "Light Shader Load Fail.\n";
		init = false;
		return;
	}

	combine_shader = new Shader(LIGHTSHADER"CombineVertex.glsl", LIGHTSHADER"CombineFragment.glsl");
	if (!combine_shader->LoadSuccess()) {
		cout << "Combine Shader Load Fail.\n";
		init = false;
		return;
	}

	GenFBO();
}

void Renderer::GenFBO() {
	glGenFramebuffers(1, &buffer_FBO);
	glGenFramebuffers(1, &skyboxFBO);
	glGenFramebuffers(1, &point_light_FBO);
	//glGenFramebuffers(1, &water_FBO);

	glGenFramebuffers(1, &depth_FBO);
	//glGenFramebuffers(1, &water_DEP);

	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1 };
	GLenum buffer_two[1] = { GL_COLOR_ATTACHMENT0  };
	
	//
	GenScreenTexture(buffer_depth_tex, true);
	GenScreenTexture(buffer_colour_tex);
	GenScreenTexture(buffer_normal_tex);

	GenScreenTexture(light_diffuse_tex);
	GenScreenTexture(light_specular_tex);
	
	GenScreenTexture(skybox_tex);

	GenScreenTexture(buffer_colour_depth_tex);

	//Generate terrain FBO
	glBindFramebuffer(GL_FRAMEBUFFER, buffer_FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_colour_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, buffer_normal_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer_depth_tex, 0);

	glDrawBuffers(2, buffers);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Generate Frame Buffer Fail.\n";
		init = false;
		return;
	}

	//generate depth FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_colour_depth_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, buffer_normal_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer_depth_tex, 0);

	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Generate Frame Buffer Fail.\n";
		init = false;
		return;
	}

	//water depth

	//generate point light FBO
	glBindFramebuffer(GL_FRAMEBUFFER, point_light_FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, light_diffuse_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, light_specular_tex, 0);

	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Generate Frame Buffer Fail.\n";
		init = false;
		return;
	}

	//generate skybox FBO
	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skybox_tex, 0);

	glDrawBuffers(1, buffer_two);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Generate Frame Buffer Fail.\n";
		init = false;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GenScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//try linear later

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::FillBuffer() {
	//draw terrain
	glBindFramebuffer(GL_FRAMEBUFFER, buffer_FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	RenderTerrain(0.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//draw depth
	glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	RenderTerrain(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderSkyBox();
}


void Renderer::PointLightRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, point_light_FBO);
	BindShader(light_shader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(light_shader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buffer_depth_tex);

	glUniform1i(glGetUniformLocation(light_shader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, buffer_normal_tex);

	glUniform3fv(glGetUniformLocation
	(light_shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform2f(glGetUniformLocation
	(light_shader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();

	glUniformMatrix4fv(glGetUniformLocation
	(light_shader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();

	for (int i = 0; i < LIGHTS_NUMBER; ++i) {
		SetShaderLight(point_lights[i]);
		light_obj->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffer() {
	BindShader(combine_shader);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();

	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combine_shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buffer_colour_tex);

	glUniform1i(glGetUniformLocation(combine_shader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, light_diffuse_tex);
	
	glUniform1i(glGetUniformLocation(combine_shader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, light_specular_tex);

	glUniform1i(glGetUniformLocation(combine_shader->GetProgram(), "skyboxTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, skybox_tex);

	glUniform1i(glGetUniformLocation(combine_shader->GetProgram(), "dt_terrain"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, buffer_colour_depth_tex);

	screen->Draw();
}

#pragma endregion


#pragma region Skybox

void Renderer::LoadSkyBox() {
	//load shader
	skybox_mesh = Mesh::GenerateQuad();

	/*Shader*/
	skybox_shader = new Shader(SKYBOX"skyboxVertex.glsl", SKYBOX"skyboxFragment.glsl");	//load shader
	//test if shader load succeed
	if (!skybox_shader->LoadSuccess()) {
		cout << "Skybox Shader Fail!\n";
		init = false;
		return;
	}

	/*TEXTURE*/
	//create texture name string and axis
	std::string name[6] = { SKYBOX"Sky Right.jpg",SKYBOX"Sky Left.jpg", SKYBOX"Sky Up.jpg" ,
	SKYBOX"Sky Down.jpg", SKYBOX"Sky Back.jpg", SKYBOX"Sky Front.jpg" };

	GLuint axis[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

	//generate and bind texture
	glGenTextures(1, &skybox_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	//load skybox's textures information
	GLubyte* imagedata = nullptr;
	int pic_width, pic_height, channels;
	//process every texture
	for (int i = 0; i < 6; ++i) {
		//load image information
		imagedata = SOIL_load_image(name[i].c_str(), &pic_width, &pic_height, &channels, 0);
		//test if load succeed
		if (!imagedata) {
			cout << "Load Image Fail!\n";
			init = false;
			return;
		}
		glTexImage2D(axis[i], 0, GL_RGB, pic_width, pic_height, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);

		//release data
		SOIL_free_image_data(imagedata);
	}

	//set texture attributes
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::RenderSkyBox() {
	glDepthMask(GL_FALSE);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(skybox_shader);

	UpdateShaderMatrices();
	skybox_mesh->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_TRUE);
}

#pragma endregion


#pragma region Terrain

void Renderer::LoadTerrain() {
	//load terrain height map
	terrain_height = new HeightMap(TERRAIN"island_height.jpg", 2.0f, 2.0f, 64.0f);

	//load terrain texture
	terrain_texture_level = 0;

	LoadTextureTerrain(TERRAIN"ground_tex.tga", terrain_texture_ground);
	LoadTextureTerrain(TERRAIN"ground_nor.tga", terrain_bump_ground);

	LoadTextureTerrain(TERRAIN"weed_tex.tga", terrain_texture_weed);
	LoadTextureTerrain(TERRAIN"weed_nor.tga", terrain_bump_weed);

	//load terrain shader
	terrain_shader = new Shader(TERRAIN"LightTerrain_vertex.glsl", TERRAIN"LightTerrain_fragment.glsl");
	if (!terrain_shader->LoadSuccess()) {
		cout << "Load Terrain Shader Fail\n";
		init = false;
		return;
	}
}

void Renderer::RenderTerrain(float isDepth) {
	//bind shader
	BindShader(terrain_shader);

	//bind texture
	BindTextureTerrain("diffuseTex_ground", terrain_texture_ground);
	BindTextureTerrain("bumpTex_ground", terrain_bump_ground);

	BindTextureTerrain("diffuseTex_weed", terrain_texture_weed);
	BindTextureTerrain("bumpTex_weed", terrain_bump_weed);


	glUniform1f(glGetUniformLocation
	(terrain_shader->GetProgram(), "isDepth"), isDepth);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,(float)width / (float)height, 45.0f);

	UpdateShaderMatrices();

	terrain_height->Draw();

	//release texture
	glBindTexture(GL_TEXTURE_2D, 0);
	terrain_texture_level = 0;
}

void Renderer::LoadTextureTerrain(const std::string filename, GLuint& target) {
	target = SOIL_load_OGL_texture
	(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!target) {
		cout << "Load Texture Fail\n";
		init = false;
		return;
	}
	SetTextureRepeating(target, true);
}

void Renderer::BindTextureTerrain(const std::string& shader_name, GLuint& target) {
	glUniform1i(glGetUniformLocation
	(terrain_shader->GetProgram(), shader_name.c_str()), terrain_texture_level);
	glActiveTexture(GL_TEXTURE0 + terrain_texture_level);
	glBindTexture(GL_TEXTURE_2D, target);
	++terrain_texture_level;
}

#pragma endregion


#pragma region SceneManagement

void Renderer::LoadObjects() {
	//load root
	root = new SceneNode();
	//load tree
	Plants* saku = new Plants(PLANTS"beutTree", PLANTS"Plants");
	for (int i = 0; i < 16; ++i) {
		Plants* saku_copy = new Plants(*saku);
		sakuras.emplace_back(saku_copy);
	}
	for (int i = 0; i < 16; ++i)
		SettleDownModel(sakuras[i], Vector3(15.0f, 15.0f, 15.0f), SAKURA_POS[i], i * 30);
}

void Renderer::SettleDownModel(SceneNode* obj, const Vector3& scale, const Vector3& pos, const float &degree) {
	obj->SetTransform(Matrix4::Translation(pos) * Matrix4::Rotation(degree, Vector3(0.0f, 1.0f, 0.0f)));
	obj->SetModelScale(scale);
	obj->SetGroundPosition(terrain_height);
	root->AddChild(obj);
}


void Renderer::UpdateNodes(const float& dt) {
	frustum_frame.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
}

void Renderer::BuildNodeLists(SceneNode *view) {
	if (frustum_frame.InsideFrustum(view)) {
		Vector3 dir = view->GetWroldTransform().GetPositionVector() - camera->GetPosition();
		view->SetCameraDistance(Vector3::Dot(dir, dir));
		if (view->GetColour().w < 1.0f)
			object_transparent.emplace_back(view);
		else
			object_opaque.emplace_back(view);
	}

	for (auto iter = view->GetChildIteratorStart(); iter != view->GetChildIteratorEnd(); ++iter)
		BuildNodeLists(*iter);
}

void Renderer::SortNodeLists() {
	std::sort(object_transparent.rbegin(), object_transparent.rend(), SceneNode::CompareByCameraDistance);
	std::sort(object_opaque.begin(), object_opaque.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (int i = 0; i < object_opaque.size(); ++i)
		object_opaque[i]->RendObject(*this, camera);

	for (int i = 0; i < object_transparent.size(); ++i)
		object_transparent[i]->RendObject(*this, camera);
}

void Renderer::ClearNodeLists() {
	object_transparent.clear();
	object_opaque.clear();
}

void Renderer::RenderObjects() {
	BuildNodeLists(root);
	SortNodeLists();
	DrawNodes();
	ClearNodeLists();
}

#pragma endregion


#pragma region Water
void Renderer::LoadWater() {
	water_height = new HeightMap(WATER"water_height.png", 16.0f, 0.5f, 16.0f);
	// 
	//load file
	LoadTextureTerrain(WATER"water_tex.png", water_texture);
	LoadTextureTerrain(WATER"water_nor.png", water_bump);

	water_shader = new Shader(WATER"Water_vertex.glsl", WATER"Water_fragment.glsl");
	if (!water_shader) {
		cout << "Load Shader Fail.\n";
		init = false;
		return;
	}

	//set position
	water_height->SetWorldPos(Vector3(0, 100.0f, 0));

	//initiate water flow varible
	water_rotate = 0.0f;
	water_cycle = 0.0f;
}

void Renderer::UpdateWater(const float &dt) {
	water_rotate += dt * 2;
	water_cycle += dt * 0.25;
	if (water_rotate == FLT_MAX)
		water_rotate = 0;
	if (water_cycle == FLT_MAX)
		water_cycle = 0;
}

void Renderer::RenderWater(float isDepth) {
	BindShader(water_shader);
	//transport camera position

	glUniform1i(glGetUniformLocation(water_shader->GetProgram(), "water_diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, water_texture);

	glUniform1i(glGetUniformLocation(water_shader->GetProgram(), "water_bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, water_bump);

	glUniform1f(glGetUniformLocation
	(water_shader->GetProgram(), "isDepth"), isDepth);


	//set shader matrix arguments
	water_move = Matrix4::Translation(Vector3(water_cycle, 0.0f, water_cycle))
		* Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f))
		* Matrix4::Rotation(water_rotate, Vector3(0.0f, 0.0f, 1.0f));

	water_height->GetWorldPos().ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation
	(water_shader->GetProgram(), "water_modelMatrix"), 1, false, water_height->GetWorldPos().values);

	glUniformMatrix4fv(glGetUniformLocation
	(water_shader->GetProgram(), "water_textureMatrix"), 1, false, water_move.values);

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	//update matrix
	UpdateShaderMatrices();
	water_height->Draw();

	//
	glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma endregion
