#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(Window &parent):OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, (Vector3(0, 100, 750.0f)));

	quad = Mesh::GenerateQuad();
	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	CubeRobot *robot = new CubeRobot(cube);

	shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	if (!shader->LoadSuccess())
		return;

	texture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!texture)
		return;

	root = new SceneNode();

	for (int i = 0; i < 5; ++i) {
		SceneNode* s = new SceneNode();
		s->SetTransform(Matrix4::Translation(Vector3(i*-50, 100.0f, (100 * i) - 200.0f)));
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetMesh(quad);
		s->SetBoundingRadius(100.0f);
		root->AddChild(s);
	}
	robot->SetTransform(Matrix4::Translation(Vector3(-500.0f, 0, 0)));
	robot->SetModelScale(Vector3(3.0f, 3.0f, 3.0f));

	root->AddChild(robot);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	init = true;
}

Renderer::~Renderer() {
	delete camera;
	delete root;
	delete cube;
	delete quad;
	delete shader;
	glDeleteTextures(1, &texture);
	camera = nullptr;
	root = nullptr;
	cube = nullptr;
	quad = nullptr;
	shader = nullptr;
}

void Renderer::UpdateScene(const float& dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frustum_frame.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
}

void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

	DrawNodes();
	ClearNodeLists();
}

void Renderer::ClearNodeLists() {
	transparent_node_list.clear();
	opaque_node_list.clear();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frustum_frame.InsideFrustum(from)) {
		Vector3 dir = from->GetWroldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		if (from->GetColour().w < 1.0f)
			transparent_node_list.emplace_back(from);
		else
			opaque_node_list.emplace_back(from);
	}
	
	for (auto iter = from->GetChildIteratorStart(); iter != from->GetChildIteratorEnd(); ++iter)
		BuildNodeLists(*iter);
}

void Renderer::SortNodeLists() {
	std::sort(transparent_node_list.rbegin(), transparent_node_list.rend(), SceneNode::CompareByCameraDistance);
	std::sort(opaque_node_list.begin(), opaque_node_list.end(), SceneNode::CompareByCameraDistance);
}
void Renderer::DrawNodes() {
	for (int i = 0; i < opaque_node_list.size(); ++i)
		DrawNode(opaque_node_list[i]);

	for (int i = 0; i < transparent_node_list.size(); ++i)
		DrawNode(transparent_node_list[i]);
}
void Renderer::DrawNode(SceneNode* node) {
	if (node->GetMesh()) {
		Matrix4 model = node->GetWroldTransform() * Matrix4::Scale(node->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false,model.values);

		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&node->GetColour());

		texture = node->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), texture);

		node->Draw(*this);
	}
}