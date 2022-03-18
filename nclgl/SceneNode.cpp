#include "SceneNode.h"
#include "HeightMap.h"

SceneNode::SceneNode() :
	mesh(nullptr), colour(Vector4(1.0, 1.0, 1.0, 1.0)), parent(nullptr), model_scale(Vector3(1.0, 1.0, 1.0))
	, bounding_radius(1.0f), distance_from_camera(0.0f) {}

SceneNode::SceneNode(Mesh* m, Vector4 c) :
	mesh(m), colour(c), parent(nullptr), model_scale(Vector3(1.0, 1.0, 1.0))
	, bounding_radius(1.0f), distance_from_camera(0.0f) {}

SceneNode::SceneNode(const SceneNode& copy) :
	mesh(copy.mesh), colour(copy.colour), parent(nullptr), model_scale(Vector3(1.0, 1.0, 1.0))
	, bounding_radius(1.0f), distance_from_camera(0.0f) {}


SceneNode::~SceneNode() {
	for (int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::SetTransform(const Matrix4& matrix) {
	transform = matrix;
}

const Matrix4& SceneNode::GetTransform() const {
	return transform;
}

Matrix4 SceneNode::GetWroldTransform() const {
	return world_transform;
}

void SceneNode::SetColour(const Vector4& vec) {
	colour = vec;
}

const Vector4& SceneNode::GetColour() const {
	return colour;
}

void SceneNode::SetModelScale(const Vector3& vec) {
	model_scale = vec;
	bounding_radius = 10.0f;
}

const Vector3& SceneNode::GetModelScale() const {
	return model_scale;
}

Mesh* SceneNode::GetMesh() const {
	return mesh;
}

void SceneNode::SetMesh(Mesh* m) {
	mesh = m;
}



void SceneNode::AddChild(SceneNode* s) {
	children.emplace_back(s);
	s->parent = this;
}

void SceneNode::Update(const float& dt) {
	if (parent)
		world_transform = parent->world_transform * transform;
	else
		world_transform = transform;

	for (int i = 0; i < children.size(); ++i)
		children[i]->Update(dt);
}

std::vector<SceneNode*>::const_iterator SceneNode::GetChildIteratorStart() const {
	return children.begin();
}

std::vector<SceneNode*>::const_iterator SceneNode::GetChildIteratorEnd() const {
	return children.end();
}


float SceneNode::GetBoundingRadius() const {
	return bounding_radius;
}

void SceneNode::SetBoundingRadius(const float& val) {
	bounding_radius = val;
}

float SceneNode::GetCameraDistance() const {
	return distance_from_camera;
}

void SceneNode::SetCameraDistance(const float& val) {
	distance_from_camera = val;
}

bool SceneNode::CompareByCameraDistance(SceneNode* a, SceneNode* b) {
	return a->distance_from_camera < b->distance_from_camera ? true : false;
}

void SceneNode::RendObject(OGLRenderer& renderer, Camera* camera) {
	if (mesh == nullptr)
		return;
}

void SceneNode::SetY(const float& val) {
	transform.values[13] = val;
}

void SceneNode::SetX(const float& val) {
	transform.values[12] = val;
}

void SceneNode::SetZ(const float& val) {
	transform.values[14] = val;
}

void SceneNode::MoveY(const float& val) {
	transform.values[13] += val;
}

void SceneNode::MoveX(const float& val) {
	transform.values[12] += val;
}

void SceneNode::MoveZ(const float& val) {
	transform.values[14] = val;
}

void SceneNode::SetGroundPosition(const HeightMap* const terrain) {
	transform.values[13]
		= terrain->AreaHeight(transform.values[12] / 2, transform.values[14] / 2);
}