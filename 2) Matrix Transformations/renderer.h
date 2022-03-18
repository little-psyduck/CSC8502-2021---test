#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer :public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer();

	virtual void RenderScene();

	virtual void UpdateScene(float dt);

	void SwitchToPerspective();
	void SwitchToOrthographic();

	inline void SetScale(const float& s) { scale = s; }
	inline void SetRotation(const float& r) { rotation = r; }
	inline void SetPosition(const Vector3& p) { position = p; }

private:
	float scale;
	float rotation;
	Vector3 position;

	Mesh* triangle;
	Shader* matrix_shader;
	Camera* camera;
};