#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer :public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer();
	virtual void RenderScene();

	void ToggleObject(); 
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);
protected:
	GLuint textures[2];
	Mesh* mesh[2];
	Shader* shader;
	Vector3 positions[2];

	bool modify_object;
	bool using_depth;
	bool using_alpha;
	int blend_mode;
};
