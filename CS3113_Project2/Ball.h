#pragma once
#include "Shared.h"

constexpr float BALL_SPEED = 1.0f;

constexpr float BALL_WIDTH = 0.5f, BALL_HEIGHT = 0.5f;

constexpr glm::vec3 INIT_BALL_MOVEMENT = glm::vec3(-1.0f, -1.0f, 0.0f);


class Ball{
public:
	Ball(glm::vec3 initPos);
	void update(float deltaTime);

	const GLuint getTextureID();
	void setTextureID(GLuint newTextureID);
	
	const glm::mat4 getModelMatrix();

	const glm::vec3 getPosition();
	void setPosition(glm::vec3 position);

	const glm::vec3 getMovement();
	void setMovement(glm::vec3 movement);


private:
	glm::mat4 m_modelMatrix;
	glm::vec3 m_movement, m_position;
	float m_speed;
	GLuint m_textureID;
};