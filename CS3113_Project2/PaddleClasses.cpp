#include "PaddleClasses.h"

Paddle::Paddle(glm::vec3 initPos): m_modelMatrix(glm::mat4(1.0f)),
                                  m_movement(ZERO_VEC),
                                  m_position(initPos),
                                  m_speed(PADDLE_SPEED),
                                  m_autopilotOn(false)
{}

void Paddle::update(const float delta_time) {
    //reset matrix
    m_modelMatrix = glm::mat4(1.0f);
    
    //Logic
    if (m_autopilotOn) {
        if (m_movement == ZERO_VEC) { m_movement.y = 1.0f; }
    }
    //Boundary detection
    if (fabs(m_position.y) < Y_LIMIT){ 
        m_position += m_movement * m_speed * delta_time; 
    }
    else { 
        m_position.y += -1.0f * m_movement.y * 0.05f; 
        if (m_autopilotOn) { m_movement.y *= -1.0f; }
    }
    
    //Translate
    m_modelMatrix = glm::translate(m_modelMatrix, m_position);
    

}

const glm::mat4 Paddle::getModelMatrix() {
    return m_modelMatrix;
}

const GLuint Paddle::getTextureID() {
    return m_textureID;
}

void Paddle::setTextureID(GLuint newTextureID) {
    m_textureID = newTextureID;
}

const glm::vec3 Paddle::getPosition() {
    return m_position;
}

void Paddle::toggleAutopilot() {
    m_autopilotOn = !(m_autopilotOn);
}

Paddle::~Paddle() {}



RightPaddle::RightPaddle(glm::vec3 initPos) : Paddle(initPos){}

void RightPaddle::processInput(const SDL_Event& event) {
	m_movement = ZERO_VEC;
    
    switch (event.type)
    {

    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_UP:
            // Move paddle up
            m_movement.y = 1.0f;
            break;

        case SDLK_DOWN:
            // Move paddle down
            m_movement.y = -1.0f;
            break;

        default:
            break;
        }

    default:
        break;
    }
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_UP])
    {
        m_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        m_movement.y = -1.0f;
    }
}



LeftPaddle::LeftPaddle(glm::vec3 initPos) : Paddle(initPos) {}

void LeftPaddle::processInput(const SDL_Event& event) {
    m_movement = ZERO_VEC;

    switch (event.type)
    {

    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
            // Move paddle up
            m_movement.y = 1.0f;
            break;

        case SDLK_s:
            // Move paddle down
            m_movement.y = -1.0f;
            break;

        default:
            break;
        }

    default:
        break;
    }
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_W])
    {
        m_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        m_movement.y = -1.0f;
    }
}
