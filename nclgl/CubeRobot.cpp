#include "CubeRobot.h"

CubeRobot::CubeRobot(Mesh* cube) {
	SceneNode* body = new SceneNode(cube, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	body->SetModelScale(Vector3(10.0f, 15.0f, 5.0f));
	body->SetTransform(Matrix4::Translation(Vector3(0.0f, 35.0f, 0.0f)));
	body->SetBoundingRadius(15.0f);
	AddChild(body);

	head = new SceneNode(cube, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	head->SetModelScale(Vector3(5.0f, 5.0f, 5.0f));
	head->SetTransform(Matrix4::Translation(Vector3(0.0f, 30.0f, 0.0f)));
	head->SetBoundingRadius(5.0f);
	body->AddChild(head);

	left_arm = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	left_arm->SetModelScale(Vector3(3.0f, -18.0f, 3.0f));
	left_arm->SetTransform(Matrix4::Translation(Vector3(-12.0f, 30.0f, -1.0f)));
	left_arm->SetBoundingRadius(18.0f);
	body->AddChild(left_arm);

	right_arm = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	right_arm->SetModelScale(Vector3(3.0f, -18.0f, 3.0f));
	right_arm->SetTransform(Matrix4::Translation(Vector3(12.0f, 30.0f, -1.0f)));
	right_arm->SetBoundingRadius(18.0f);
	body->AddChild(right_arm);

	left_leg = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	left_leg->SetModelScale(Vector3(3.0f, -17.5f, 3.0f));
	left_leg->SetTransform(Matrix4::Translation(Vector3(-8.0f, 0.0f, 0.0f)));
	left_leg->SetBoundingRadius(18.0f);
	body->AddChild(left_leg);

	right_leg = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	right_leg->SetModelScale(Vector3(3.0f, -17.5f, 3.0f));
	right_leg->SetTransform(Matrix4::Translation(Vector3(8.0f, 0.0f, 0.0f)));
	right_leg->SetBoundingRadius(18.0f);
	body->AddChild(right_leg);
}

void CubeRobot::Update(const float& dt) {
	transform = transform * Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));

	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-100.0f * dt, Vector3(0, 1, 0)));

	left_arm->SetTransform(left_arm->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));

	right_arm->SetTransform(right_arm->GetTransform() * Matrix4::Rotation(30.0f * dt, Vector3(1, 0, 0)));

	SceneNode::Update(dt);
}