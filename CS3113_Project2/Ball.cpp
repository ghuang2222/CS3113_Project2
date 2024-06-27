#include "Ball.h"

Ball::Ball(glm::vec3 initPos) : m_modelMatrix(glm::mat4(1.0f)),
								m_movement(INIT_BALL_MOVEMENT),
								m_position(initPos),
								m_speed(BALL_SPEED)
{}

void Ball::update(float deltaTime) {
	m_modelMatrix = glm::mat4(1.0f);
	// Collision detection
	if (fabs(m_position.y) >= Y_LIMIT) {
		m_position.y += -1.0f * m_movement.y * 0.025f;
		m_movement.y *= -1.0f;
	}


	//Translation
	m_position += m_movement * m_speed * deltaTime;
	m_modelMatrix = glm::translate(m_modelMatrix, m_position);

	//Scale
	m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(0.5f, 0.5f, 0.0f));
}


const GLuint Ball::getTextureID() {
	return m_textureID;
}

void Ball::setTextureID(GLuint newTextureID) {
	m_textureID = newTextureID;
}

const glm::mat4 Ball::getModelMatrix() {
	return m_modelMatrix;
}

const glm::vec3 Ball::getPosition() {
	return m_position;
}

void Ball::setPosition(glm::vec3 position) {
	m_position = position;
}

const glm::vec3 Ball::getMovement() {
	return m_movement;
}

void Ball::setMovement(glm::vec3 movement) {
	m_movement = movement;
}