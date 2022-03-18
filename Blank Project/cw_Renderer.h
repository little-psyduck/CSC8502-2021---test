#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Frustum.h"
#include "Role.h"
#include "plants.h"

class HeightMap;

#define SKYBOX "../Skybox/"
#define	TERRAIN "../Terrain/"
#define ROLE "../Role/"
#define WATER "../Water/"
#define PLANTS "../Plants/"
#define LIGHTSHADER "../LightShader/"

const Vector3 SAKURA_POS[17] = {
	Vector3(3694.05469 ,538.107727 ,736.385254),
	Vector3(3096.00757 ,538.107727 ,396.403564),
	Vector3(2375.11841  ,538.107727 ,1095.35999),
	Vector3(1635.24390  ,538.107727 ,1123.38110),
	Vector3(2247.07324  ,538.107727 ,1806.20056),
	Vector3(2859.52588  ,538.107727 ,2272.68921),
	Vector3(3642.14746  ,538.107727 ,2594.82739),
	Vector3(3899.19727  ,538.107727 ,3258.18384),
	Vector3(3186.04077  ,538.107727 ,3734.17383),
	Vector3(1638.35645  ,538.107727 ,3378.81396),
	Vector3(429.884827  ,538.107727 ,3480.95947),
	Vector3(558.945007  ,538.107727 ,3082.46289),
	Vector3(322.495300   ,538.107727 ,2263.93774),
	Vector3(404.260437   ,538.107727 ,1537.45947),
	Vector3(1031.05408   ,538.107727 ,1374.33411),
	Vector3(596.525696   ,538.107727 ,683.163635),
	Vector3(1454.18164   ,538.107727 ,520.695923),
};

const Vector3 LIGHT_POS[8] = {
	Vector3(500,700,500),
	Vector3(2000,700,2000),
	Vector3(500,700,2000),
	Vector3(2000,700,500),

	Vector3(1000,700,500),
	Vector3(500,700,1000),
	Vector3(1000,700,3000),
	Vector3(3000,700,1000),
};

const int LIGHTS_NUMBER = 8;

class Renderer : public OGLRenderer {
	friend class Role;
	friend class Plants;
public:
	Renderer(Window& parent);
	~Renderer();
	void RenderScene() override;
	void UpdateScene(const float& dt) override;
protected:
#pragma region Camera
	Camera* camera;

	void LoadCamera();
#pragma endregion


#pragma region Lights
	Light* point_lights;

	Shader* light_shader;
	Shader* combine_shader;

	GLuint buffer_FBO;
	GLuint depth_FBO;
	GLuint point_light_FBO;

	GLuint buffer_colour_depth_tex;

	GLuint buffer_colour_tex;
	GLuint buffer_normal_tex;
	GLuint buffer_depth_tex;

	GLuint light_diffuse_tex;
	GLuint light_specular_tex;

	GLuint skyboxFBO;
	GLuint skybox_tex;

	Mesh* screen;
	Mesh* light_obj;

	void LoadLights();
	void GenFBO();
	void GenScreenTexture(GLuint& into, bool depth = false);

	void FillBuffer();
	void PointLightRender();
	void CombineBuffer();
#pragma endregion

#pragma region Skybox
	Mesh* skybox_mesh;
	Shader* skybox_shader;
	GLuint skybox_texture;
	void LoadSkyBox();
	void RenderSkyBox();
#pragma endregion

#pragma region Terrain
	HeightMap* terrain_height;

	GLuint terrain_texture_ground;
	GLuint terrain_texture_weed;

	GLuint terrain_bump_ground;
	GLuint terrain_bump_weed;


	Shader* terrain_shader;
	void LoadTerrain();
	void RenderTerrain(float isDepth = 0.0);
	//sub function of above
	void LoadTextureTerrain(const std::string filename, GLuint& target);
	void BindTextureTerrain(const std::string& shader_name, GLuint& target);
	int terrain_texture_level;

#pragma endregion

#pragma region Water
	GLuint water_texture;
	GLuint water_bump;
	Shader* water_shader;
	HeightMap* water_height;
	Matrix4 water_move;

	void LoadWater();
	void RenderWater(float isDepth = 0.0);
	void UpdateWater(const float &dt);

	//GLuint water_FBO;
	//GLuint water_DEP;

	//GLuint water_buffer_colour_tex;
	//GLuint water_buffer_normal_tex;
	//GLuint water_buffer_depth_tex;
	//GLuint water_buffer_colour_depth_tex;
#pragma endregion


#pragma region SceneManagement
	SceneNode* root;
	Role* DogKnight;

	vector<Plants*> sakuras;

	Frustum frustum_frame;
	vector<SceneNode*> object_transparent;
	vector<SceneNode*> object_opaque;

	void LoadObjects();
	void UpdateNodes(const float& dt);

	void RenderObjects();
	// sub functions of above
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();

	void SettleDownModel(SceneNode* obj, const Vector3& scale, const Vector3& pos, const float &rotate_degree);

#pragma endregion

};

