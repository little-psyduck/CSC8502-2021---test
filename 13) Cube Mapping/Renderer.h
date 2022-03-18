#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;
class Shader;
class HeightMap;
class Light;

class Renderer :public OGLRenderer {
protected:
	Shader* light_shader;
	Shader* reflect_shader;
	Shader* skybox_shader;

	HeightMap* height_map;

	Mesh* quad;

	Camera* camera;

	Light* light;

	GLuint cube_map;
	GLuint water_tex;
	GLuint earth_tex;
	GLuint earth_bump;

	float water_rotate;
	float water_cycle;

	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();

public:
	Renderer(Window & parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(const float &dt) override;
};