#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"

#include <vector>

class HeightMap;

class SceneNode {
public:
	SceneNode();
	SceneNode(Mesh* m, Vector4 c);
	SceneNode(const SceneNode &copy);

	~SceneNode();

	void SetTransform(const Matrix4 &matrix);
	const Matrix4& GetTransform() const;
	Matrix4 GetWroldTransform() const;

	void SetColour(const Vector4& vec);
	const Vector4& GetColour() const;

	void SetModelScale(const Vector3& vec);
	const Vector3& GetModelScale() const;

	Mesh* GetMesh() const;
	void SetMesh(Mesh *m);

	void AddChild(SceneNode* s);

	virtual void RendObject(OGLRenderer& renderer, Camera* camera);
	virtual void Update(const float& dt);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() const;
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() const;

	float GetBoundingRadius() const;
	void SetBoundingRadius(const float& val);

	float GetCameraDistance() const;
	void SetCameraDistance(const float& val);

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b);

	void SetY(const float& val);
	void SetX(const float& val);
	void SetZ(const float& val);

	void MoveY(const float& val);
	void MoveX(const float& val);
	void MoveZ(const float& val);

	void SetGroundPosition(const HeightMap* const terrain);

protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 world_transform;
	Matrix4 transform;
	Vector3 model_scale;
	Vector4 colour;
	std::vector<SceneNode*> children;

	float distance_from_camera;
	float bounding_radius;
};
