#include "camera.h"

Camera::Camera(glm::vec3 iposition, float width,float height)
{
	position = iposition; // initial position
	theta = 0; // rotation Y around  axis. Starts with forward direction as ( 0, 0, -1 )
	phi = M_PI/4; // rotation around X axis. Starts with up direction as ( 0, 1, 0 )
	moveSpeed = 15;
	turnSpeed = 0.3; // radians/sec

	// dont need to change these
	negativeMovement = glm::vec3(0, 0, 0);
	positiveMovement = glm::vec3(0, 0, 0);
	negativeTurn = glm::vec2(0, 0); // .x for theta, .y for phi
	positiveTurn = glm::vec2(0, 0);
	fovy = M_PI / 4.f;
	aspectRatio = width / (float)height;
	nearPlane = 0.1;
	farPlane = 10000;
	proj = glm::mat4();
	view = glm::mat4();
}

void Camera::updateCamera(float dt, float width, float height)
{
	theta += turnSpeed * (negativeTurn.x + positiveTurn.x) * dt;

	// cap the rotation about the X axis to be less than 90 degrees to avoid gimble lock
	float maxAngleInRadians = 85.f * M_PI / 180.f;
	phi = std::min(maxAngleInRadians, std::max(-maxAngleInRadians, phi + turnSpeed * (negativeTurn.y + positiveTurn.y) * dt));

	// re-orienting the angles to match the wikipedia formulas: https://en.wikipedia.org/wiki/Spherical_coordinate_system
	// except that their theta and phi are named opposite
	float t = theta + M_PI / 2.f;
	float p = phi + M_PI / 2.f;
	glm::vec3 forwardDir = glm::vec3(sin(p) * cos(t), cos(p), -sin(p) * sin(t));
	glm::vec3 upDir = glm::vec3(sin(phi) * cos(t), cos(phi), -sin(t) * sin(phi));
	glm::vec3 rightDir = glm::vec3(cos(theta), 0, -sin(theta));
	glm::vec3 velocity = glm::vec3(negativeMovement.x + positiveMovement.x, negativeMovement.y + positiveMovement.y, negativeMovement.z + positiveMovement.z);
	position += forwardDir * (moveSpeed * velocity.z * dt);
	position += upDir * (moveSpeed * velocity.y * dt);
	position += rightDir * (moveSpeed * velocity.x * dt);

	aspectRatio = width / (float)height;
	/*
	perspective(fovy, aspectRatio, nearPlane, farPlane);
	camera(position.x, position.y, position.z,
		position.x + forwardDir.x, position.y + forwardDir.y, position.z + forwardDir.z,
		upDir.x, upDir.y, upDir.z);
	*/
	//Set the Camera view paramters (FVO, aspect ratio, etc.)
	proj = glm::perspective(fovy, aspectRatio, nearPlane, farPlane); //FOV, aspect, near, far

	//Set the Camera Position and Orientation
	view = glm::lookAt(
		glm::vec3(position.x, position.y, position.z),  //Cam Position
		glm::vec3(position.x + forwardDir.x, position.y + forwardDir.y, position.z + forwardDir.z),  //Look at point
		glm::vec3(upDir.x, upDir.y, upDir.z)); //Up

	//std::cout << "position.x = " << position.x << "   position.y = " << position.y << "   position.z = " << position.z << "\n\n";
	//std::cout << "lookat.x = " << position.x + forwardDir.x << "   lookat.y = " << position.y + forwardDir.y << "   lookat.z = " << position.z + forwardDir.z << "\n\n";
}
