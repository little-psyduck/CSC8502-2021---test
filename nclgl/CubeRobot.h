#pragma once
#include "../nclgl/SceneNode.h"

class CubeRobot :public SceneNode {
public:
	CubeRobot(Mesh* cube);
	virtual ~CubeRobot() = default;
	virtual void Update(const float& dt);

private:
	SceneNode* head;
	SceneNode* left_arm;
	SceneNode* right_arm;
	SceneNode* left_leg;
	SceneNode* right_leg;
};