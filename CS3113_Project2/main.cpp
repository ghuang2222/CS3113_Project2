/**
* Author: Garvin Huang
* Assignment: Pong Clone
* Date due: 2024-06-29, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Shared.h"
#include "ShaderProgram.h"
#include "stb_image.h"

#include "PaddleClasses.h"
#include "Ball.h"

enum AppStatus { RUNNING, TERMINATED };

constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1, // to be generated, that is
LEVEL_OF_DETAIL = 0, // mipmap reduction image level
TEXTURE_BORDER = 0; // this value MUST be zero


SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_projection_matrix;

float g_previous_ticks = 0.0f;

//Setup
constexpr char PADDLE_SPRITE_PATH[] = "spatula.png";
glm::vec3 RIGHT_PADDLE_INIT_POS = glm::vec3(3.0f, 0.0f, 0.0f),
          LEFT_PADDLE_INIT_POS = glm::vec3(-3.0f, 0.0f, 0.0f);

RightPaddle g_RightPaddle = RightPaddle(RIGHT_PADDLE_INIT_POS);
LeftPaddle g_LeftPaddle = LeftPaddle(LEFT_PADDLE_INIT_POS);
bool g_autopilotOn = false;

constexpr char BALL_SPRITE_PATH[] = "burger.png";
glm::vec3 BALL1_INIT_POS = ZERO_VEC,
          BALL2_INIT_POS = glm::vec3(0.0f, 0.25f, 0.0f),
          BALL3_INIT_POS = glm::vec3(0.0f, -0.25f, 0.0f);

int g_numBalls = 1;
Ball g_Ball1 = Ball(BALL1_INIT_POS),
     g_Ball2 = Ball(BALL2_INIT_POS),
     g_Ball3 = Ball(BALL3_INIT_POS);

constexpr char PLAYER1_WIN_PATH[] = "player1_winscreen.png";
constexpr char PLAYER2_WIN_PATH[] = "player2_winscreen.png";
glm::mat4 g_winscreen_matrix = glm::mat4(1.0f);

const glm::vec3 WINSCREEN_SCALE = glm::vec3(5.0f, 5.0f * 225.0f / 1061.0f , 0.0f);
GLuint g_winscreen_tid; 
bool g_gameOver = false;
//

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Burger Pong",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    //loading textures
    g_RightPaddle.setTextureID(load_texture(PADDLE_SPRITE_PATH));
    g_LeftPaddle.setTextureID(load_texture(PADDLE_SPRITE_PATH));
    g_Ball1.setTextureID(load_texture(BALL_SPRITE_PATH));
    g_Ball2.setTextureID(load_texture(BALL_SPRITE_PATH));
    g_Ball3.setTextureID(load_texture(BALL_SPRITE_PATH));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_t: // toggle 1-player mode
                        g_autopilotOn = !g_autopilotOn;
                        g_RightPaddle.toggleAutopilot();
                        break;
                    case SDLK_1:
                        g_numBalls = 1;
                        break;
                    case SDLK_2:
                        g_numBalls = 2;
                        break;
                    case SDLK_3:
                        g_numBalls = 3;
                        break;
                    default:
                        break;
                    }

            default:
                break;
        }
        if (!g_autopilotOn) { g_RightPaddle.processInput(event); }
        g_LeftPaddle.processInput(event);
    }
}

void BallPaddleCollide(Ball& ball,  Paddle& paddle)  //handles ball-paddle collisions
{ 
    glm::vec3 ballPos = ball.getPosition();
    float ball_x = ballPos.x, ball_y = ballPos.y;

    glm::vec3 paddlePos = paddle.getPosition();
    float paddle_x = paddlePos.x, paddle_y= paddlePos.y;

    float x_distance = fabs(ball_x - paddle_x) - ((BALL_WIDTH + PADDLE_WIDTH) / 2.0f);
    float y_distance = fabs(ball_y - paddle_y) - ((BALL_HEIGHT + PADDLE_HEIGHT) / 2.0f);

    if (x_distance < 0 && y_distance < 0)
    {   
        glm::vec3 ballMovement = ball.getMovement();
        ballPos.x += -1.0f * ballMovement.x * fabs(x_distance);
        ball.setPosition(ballPos);
        ballMovement.x *= -1.0f;
        ball.setMovement(ballMovement);
    }
}


void updateBall(Ball& ball, const float& delta_time) {
    ball.update(delta_time);
    BallPaddleCollide(ball, g_RightPaddle);
    BallPaddleCollide(ball, g_LeftPaddle);

}


void update()
{         
    /* Delta time calculations */
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    g_RightPaddle.update(delta_time);
    g_LeftPaddle.update(delta_time);
    updateBall(g_Ball1, delta_time);
    if (g_numBalls >= 2) { updateBall(g_Ball2, delta_time);}
    if (g_numBalls == 3) { updateBall(g_Ball3, delta_time);}
    
    float ball1_x = g_Ball1.getPosition().x,
          ball2_x = g_Ball2.getPosition().x,
          ball3_x = g_Ball3.getPosition().x;

    if (fabs(ball1_x) >= 5 || fabs(ball2_x) >= 5 || fabs(ball3_x) >= 5) {
        g_gameOver = true; 
        if (ball1_x >= 5 || ball2_x >= 5 || ball3_x >= 5) {
            g_winscreen_tid = load_texture(PLAYER1_WIN_PATH);
        }
        else { g_winscreen_tid = load_texture(PLAYER2_WIN_PATH); }
        g_winscreen_matrix = glm::scale(g_winscreen_matrix, WINSCREEN_SCALE);
    }
}


void draw_object(const glm::mat4& object_g_model_matrix, const GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
        0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_RightPaddle.getModelMatrix(), g_RightPaddle.getTextureID());
    draw_object(g_LeftPaddle.getModelMatrix(), g_LeftPaddle.getTextureID());
    draw_object(g_Ball1.getModelMatrix(), g_Ball1.getTextureID());
    if (g_numBalls >= 2) { draw_object(g_Ball2.getModelMatrix(), g_Ball2.getTextureID()); }
    if (g_numBalls == 3) { draw_object(g_Ball3.getModelMatrix(), g_Ball3.getTextureID()); }
    if (g_gameOver) { draw_object(g_winscreen_matrix, g_winscreen_tid);}

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{

    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        if (!g_gameOver) { update(); }
        render();
    }

    shutdown();
    return 0;
}




