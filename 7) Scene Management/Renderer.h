#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/CubeRobot.h"

class Renderer :public OGLRenderer {
protected:
	SceneNode* root;
	Camera* camera;
	Mesh* quad;
	Mesh* cube;
	Shader* shader;
	GLuint texture;

	Frustum frustum_frame;

	vector<SceneNode*> transparent_node_list;
	vector<SceneNode*> opaque_node_list;

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* node);

public:
	Renderer(Window &parent);
	virtual ~Renderer();

	virtual void UpdateScene(const float& msec);
	virtual void RenderScene();
};
