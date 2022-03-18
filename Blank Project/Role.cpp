#include "Role.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/OGLRenderer.h"
#include <iostream>

Role::Role(const std::string& obj_name, const std::string& shader_name)
	:SceneNode(), frame_time(0.0f), current_frame(0) {
	//load shader
	role_shader = new Shader(shader_name + "Vertex.glsl", shader_name + "Fragment.glsl");
	//load model information
	mesh = Mesh::LoadFromMeshFile("../Role/" + obj_name + ".msh");
	role_anime = new MeshAnimation("../Role/" + obj_name + ".anm");
	role_material = new MeshMaterial("../Role/" + obj_name + ".mat");

	//load model texture
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* mat_entry = role_material->GetMaterialForLayer(i);
		const string* filename = nullptr;

		//load diffuse texture
		mat_entry->GetEntry("Diffuse", &filename);
		string path = *filename;
		GLuint texID = SOIL_load_OGL_texture
		(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		if (!texID) {
			std::cout << "Model Diffuse Texture Load Fail.\n";
			return;
		}
		diffuse_textures.emplace_back(texID);
		
		//load bump texture
		mat_entry->GetEntry("Bump", &filename);
		path = *filename;
		texID = SOIL_load_OGL_texture
		(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		if (!texID) {
			std::cout << "Model Bump Texture Load Fail.\n";
			return;
		}
		bump_textures.emplace_back(texID);
		
	}
}

void Role::Update(const float& dt) {
	frame_time -= dt;
	while (frame_time < 0.0f) {
		current_frame = (current_frame + 1) % role_anime->GetFrameCount();
		frame_time += 1.0f / role_anime->GetFrameRate();
	}
	
	SceneNode::Update(dt);
}

void Role::RendObject(OGLRenderer& renderer, Camera* camera) {
	vector<Matrix4> frame_matrices;
	Matrix4 role_model = world_transform * Matrix4::Scale(model_scale);

	renderer.BindShader(role_shader);
	glUniform1i(glGetUniformLocation(role_shader->GetProgram(), "diffuseTex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(role_shader->GetProgram(), "role_model"), 1, false, role_model.values);
	renderer.UpdateShaderMatrices();

	const Matrix4* inv_bind_pose = mesh->GetInverseBindPose();
	const Matrix4* frame_data = role_anime->GetJointData(current_frame);

	for (int i = 0; i < mesh->GetJointCount(); ++i)
		frame_matrices.emplace_back(frame_data[i] * inv_bind_pose[i]);

	glUniformMatrix4fv
	(glGetUniformLocation(role_shader->GetProgram(), "joints")
		, frame_matrices.size(), false, (float*)frame_matrices.data());

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_textures[i]);

		mesh->DrawSubMesh(i);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}




Role::~Role() {
	delete role_anime;
	role_anime = nullptr;
	delete role_material;
	role_material = nullptr;
	delete role_shader;
	role_shader = nullptr;
	for (int i = 0; i < diffuse_textures.size(); ++i)
		glDeleteTextures(1, &diffuse_textures[i]);
	for (int i = 0; i < bump_textures.size(); ++i)
		glDeleteTextures(1, &bump_textures[i]);
}

