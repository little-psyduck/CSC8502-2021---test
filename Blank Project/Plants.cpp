#include "Plants.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/OGLRenderer.h"
#include <iostream>

Plants::Plants() :plants_shader(nullptr), plants_material(nullptr) {  }

Plants::Plants(const std::string& obj_name, const std::string& shader_name) :SceneNode() {
	//load shader
	plants_shader = new Shader(shader_name + "Vertex.glsl", shader_name + "Fragment.glsl");
	//load model information
	mesh = Mesh::LoadFromMeshFile("../Plants/" + obj_name + ".msh");
	plants_material = new MeshMaterial("../Plants/" + obj_name + ".mat");

	//load model texture
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* mat_entry = plants_material->GetMaterialForLayer(i);
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
		mesh->GenerateNormals();

		//load bump texture
		if (mat_entry->GetEntry("Bump", &filename)) {
			path = *filename;
			texID = SOIL_load_OGL_texture
			(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
			if (!texID) {
				std::cout << "Model Bump Texture Load Fail.\n";
				return;
			}
			bump_textures.emplace_back(texID);
			mesh->GenerateTangents();
		}
		else
			std::cout << "There is no bump texture.\n";
	}
}

Plants::Plants(const Plants& copy) :SceneNode(copy) {
	plants_shader = copy.plants_shader;
	diffuse_textures = copy.diffuse_textures;

	if (!bump_textures.empty())
		bump_textures = copy.bump_textures;
}

void Plants::Update(const float& dt) {
	SceneNode::Update(dt);
}

void Plants::RendObject(OGLRenderer& renderer, Camera* camera) {
	Matrix4 plants_model = world_transform * Matrix4::Scale(model_scale);

	renderer.BindShader(plants_shader);

	glUniform1i(glGetUniformLocation(plants_shader->GetProgram(), "diffuseTex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(plants_shader->GetProgram(), "plants_model"),
		1, false, plants_model.values);
	glUniform3fv
	(glGetUniformLocation(plants_shader->GetProgram(), "camera_position"), 1, (float*)&camera->GetPosition());
	renderer.UpdateShaderMatrices();	

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_textures[i]);

		mesh->DrawSubMesh(i);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

Plants::~Plants() {
	delete plants_shader;
	plants_shader = nullptr;
	if (plants_material != nullptr) {
		delete plants_material;
		plants_material = nullptr;
	}
	for (int i = 0; i < diffuse_textures.size(); ++i)
		glDeleteTextures(1, &diffuse_textures[i]);
	if (!bump_textures.empty()) {
		for (int i = 0; i < bump_textures.size(); ++i)
			glDeleteTextures(1, &bump_textures[i]);
	}

}