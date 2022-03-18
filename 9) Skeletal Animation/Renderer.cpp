#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"


Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	camera = new Camera(-3.0f, 0.0f, Vector3(0, 1.4f, 4.0f));

	shader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	if (!shader->LoadSuccess())
		return;

	mesh = Mesh::LoadFromMeshFile("../Role/DogPBR.msh");
	anime = new MeshAnimation("../Role/DogPBR.anm");
	material = new MeshMaterial("../Role/DogPBR.mat");

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* mat_entry = material->GetMaterialForLayer(i);
		const string* filename = nullptr;
		mat_entry->GetEntry("Diffuse", &filename);
		string path = *filename;

		GLuint texID = SOIL_load_OGL_texture
		(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		if (!texID)
			return;

		mat_textures.emplace_back(texID);
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	current_frame = 0;
	frame_time = 0.0f;

	init = true;
}

Renderer::~Renderer() {
	delete camera;
	delete shader;
	delete mesh;
	delete material;
	delete anime;

	for (int i = 0; i < mat_textures.size(); ++i)
		glDeleteTextures(1, &mat_textures[i]);

	camera = nullptr;
	shader = nullptr;
	mesh = nullptr;
	material = nullptr;
	anime = nullptr;
}

void Renderer::UpdateScene(const float &dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();

	frame_time -= dt;
	while (frame_time < 0.0f) {
		current_frame = (current_frame + 1) % anime->GetFrameCount();
		frame_time += 1.0f / anime->GetFrameRate();
	}
}

void Renderer::RenderScene() {
	vector<Matrix4> frame_matrices;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shader);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	UpdateShaderMatrices();

	const Matrix4* inv_bind_pose = mesh->GetInverseBindPose();
	const Matrix4* frame_data = anime->GetJointData(current_frame);

	for (int i = 0; i < mesh->GetJointCount(); ++i)
		frame_matrices.emplace_back(frame_data[i] * inv_bind_pose[i]);

	glUniformMatrix4fv
	(glGetUniformLocation(shader->GetProgram(), "joints")
		, frame_matrices.size(), false, (float*)frame_matrices.data());

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mat_textures[i]);

		mesh->DrawSubMesh(i);
	}
}
