#pragma once

#include "Shared.h"
constexpr float PADDLE_SPEED = 3.0f;

constexpr float PADDLE_WIDTH = 0.5f, PADDLE_HEIGHT = 1.0f;

class Paddle {
public:
	Paddle(glm::vec3 initPos);
	virtual void processInput(const SDL_Event& event) = 0; // is this even correct?
	
	void update(const float deltaTime);

	const GLuint getTextureID();
	void setTextureID(GLuint newTextureID);

	const glm::mat4 getModelMatrix();
	
	const glm::vec3 getPosition();

	void toggleAutopilot();

	virtual ~Paddle();

protected:
	glm::mat4 m_modelMatrix;
	glm::vec3 m_position, m_movement;
	float m_speed;
	GLuint m_textureID;
	bool m_autopilotOn;
};

class RightPaddle : public Paddle {
public:
	RightPaddle(glm::vec3 initPos);
	void processInput(const SDL_Event& event) override;
};

class LeftPaddle : public Paddle {
public:
	LeftPaddle(glm::vec3 initPos);
	void processInput(const SDL_Event& event) override;
};
