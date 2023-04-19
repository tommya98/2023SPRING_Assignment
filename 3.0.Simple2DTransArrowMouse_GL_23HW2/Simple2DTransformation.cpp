#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <time.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

#define CARD_NUM 30
int animation_mode = 1;
unsigned int timestamp = 0;
typedef struct {
	float x, y, vx, vy, sizex, sizey, rotate;
	int dir;
}card_t;
card_t cards[CARD_NUM + 1];
int card_flag = 0;

void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	glutPostRedisplay();
	if (animation_mode)
		glutTimerFunc(10, timer, 0);
}


void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.5f; axes[1][0] = win_width / 2.5f;
	axes[2][1] = -win_height / 2.5f;
	axes[3][1] = win_height / 2.5f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
	// y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

//airplane
#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 }, { -20.0, 15.0 }, { -20.0, 20.0 }, { 0.0, 23.0 }, { 20.0, 20.0 }, { 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 }, { -11.0, -12.0 }, { -12.0, -7.0 }, { 0.0, -10.0 }, { 12.0, -7.0 }, { 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 }, { -6.0, 0.0 }, { -6.0, 22.0 }, { 6.0, 22.0 }, { 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 }, { -7.0, 24.0 }, { -7.0, 21.0 }, { 7.0, 21.0 }, { 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 }, { -18.0, 3.0 }, { -16.0, 10.0 }, { -18.0, 20.0 }, { -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 }, { 18.0, 3.0 }, { 16.0, 10.0 }, { 18.0, 20.0 }, { 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//draw cocktail
#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 },{ -6.0, -11.0 },{ -1.0, 0.0 },{ 1.0, 0.0 },{ 6.0, -11.0 },{ 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 },{ -9.0, 4.0 },{ -12.0, 7.0 },{ 12.0, 7.0 },{ 9.0, 4.0 },{ 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 },{ -12.0, 10.0 },{ 12.0, 10.0 },{ 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 },{ 12.0, 12.0 },{ 14.0, 12.0 },{ 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 },{ 10.0, 14.0 },{ 10.0, 16.0 },{ 12.0, 18.0 },{ 14.0, 18.0 },{ 16.0, 16.0 },{ 16.0, 14.0 },{ 14.0, 12.0 } };

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};

GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw)
		+ sizeof(deco);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw),
		sizeof(deco), deco);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glBindVertexArray(VAO_cocktail);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 },{ -18.0, 0.0 },{ -13.0, 0.0 },{ -10.0, 8.0 },{ 10.0, 8.0 },{ 13.0, 0.0 },{ 18.0, 0.0 },{ 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 },{ -8.0, 6.0 },{ -2.0, 6.0 },{ -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 },{ 0.0, 6.0 },{ 8.0, 6.0 },{ 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 },{ -13.0, -8.0 },{ -13.0, -7.0 },{ -11.0, -4.0 },{ -7.0, -4.0 },{ -5.0, -7.0 },{ -5.0, -8.0 },{ -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 },{ 5.0, -8.0 },{ 5.0, -7.0 },{ 7.0, -4.0 },{ 11.0, -4.0 },{ 13.0, -7.0 },{ 13.0, -8.0 },{ 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 },{ -17.0, -2.0 },{ -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 },{ -17.0, -5.0 },{ -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1) + sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

// sword
#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

// ojbect
#define OBJECT_BODY1 0
#define OBJECT_BODY2 1
#define OBJECT_DIAMOND1 2
#define OBJECT_DIAMOND2 3
#define OBJECT_CROWN 4
#define OBJECT_IHS 5

GLfloat object_body1[6][2] = { { -5.0, 3.5 }, { -4.0, 5.0 }, { 4.0, 5.0 }, { 5.0, 3.5 }, { -5.0, 3.5 }, { -5.0, 3.5 } };
GLfloat object_body2[9][2] = { { -5.0, 3.5 }, { -5.0, -7.0 }, { 5.0, 3.5 }, { 5.0, -7.0 }, { -5.0, -7.0 }, { 5.0, 3.5 }, { -5, -7.0 }, { 0.0, -10.0 }, { 5.0, -7.0 } };
GLfloat object_diamond1[6][2] = { { -5.0, -7.0 }, { -5, -6.0 }, { -0.72, 0.0 }, { 0.0, 0.0 }, { -5.0, -7.0 }, { -0.72, 0.0 } };
GLfloat object_diamond2[6][2] = { { 5.0, -7.0 }, { 5, -6.0 }, { 0.72, 0.0 }, { 0.0, 0.0 }, { 5.0, -7.0 }, { 0.72, 0.0 } };
GLfloat object_crwon[30][2] = { { -3.0, -8.0 }, { -3.0, -6.0 }, { -1.75, -6.0 }, { -1.75, -8.0 }, { -3.0, -8.0 }, { -1.75, -6.0 }, { -1.75, -8.0 }, { -1.75, -7.0 }, { -0.5, -7.0 }, { -0.5, -8.0 }, { -1.75, -8.0 }, { -0.5, -7.0 }, { -0.5, -8.0 }, { -0.5, -6.0 }, { 0.5, -6.0 }, { 0.5, -8.0 }, { -0.5, -8.0 }, { 0.5, -6.0 }, { 0.5, -8.0 }, { 0.5, -7.0 }, { 1.75, -7.0 }, { 1.75, -8.0 }, { 0.5, -8.0 }, { 1.75, -7.0 }, { 3.0, -8.0 }, { 3.0, -6.0 }, { 1.75, -6.0 }, { 1.75, -8.0 }, { 3.0, -8.0 }, { 1.75, -6.0 } };
GLfloat object_ihs[18][2] = { { -4.0, 3.5 }, { -4.0, 2.5 }, { -3.5, 3.5 }, { -3.5, 2.5 }, { -3.0, 3.5 }, { -3.0, 2.5 }, { -3.5, 3.0 }, { -3.0, 3.0 }, { -2.5, 3.5 }, { -2.0, 3.5 }, { -2.5, 3.5 }, { -2.5, 3.0 }, { -2.5, 3.0 }, { -2.0, 3.0 }, { -2.0, 3.0 }, { -2.0, 2.5 }, { -2.0, 2.5 }, { -2.5, 2.5 } };

GLfloat object_color[6][3] = {
	{ 179 / 255.0f, 46 / 255.0f, 49 / 255.0f },
	{ 179 / 255.0f, 46 / 255.0f, 49 / 255.0f },
	{ 182 / 255.0f, 182 / 255.0f, 182 / 255.0f },
	{ 182 / 255.0f, 182 / 255.0f, 182 / 255.0f },
	{ 182 / 255.0f, 182 / 255.0f, 182 / 255.0f },
	{ 182 / 255.0f, 182 / 255.0f, 182 / 255.0f }
};

GLuint VBO_object, VAO_object;

void prepare_object() {
	GLsizeiptr buffer_size = sizeof(object_body1) + sizeof(object_body2) + sizeof(object_diamond1) + sizeof(object_diamond2) + sizeof(object_crwon) + sizeof(object_ihs);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_object);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_object);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(object_body1), object_body1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(object_body1), sizeof(object_body2), object_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(object_body1) + sizeof(object_body2), sizeof(object_diamond1), object_diamond1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(object_body1) + sizeof(object_body2) + sizeof(object_diamond1), sizeof(object_diamond2), object_diamond2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(object_body1) + sizeof(object_body2) + sizeof(object_diamond1) + sizeof(object_diamond2), sizeof(object_crwon), object_crwon);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(object_body1) + sizeof(object_body2) + sizeof(object_diamond1) + sizeof(object_diamond2) + sizeof(object_crwon), sizeof(object_ihs), object_ihs);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_object);
	glBindVertexArray(VAO_object);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_object);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_object() {
	glLineWidth(2);
	glBindVertexArray(VAO_object);

	glUniform3fv(loc_primitive_color, 1, object_color[OBJECT_BODY1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, object_color[OBJECT_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 9);

	glUniform3fv(loc_primitive_color, 1, object_color[OBJECT_DIAMOND1]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 6);

	glUniform3fv(loc_primitive_color, 1, object_color[OBJECT_DIAMOND2]);
	glDrawArrays(GL_TRIANGLE_FAN, 21, 6);

	glUniform3fv(loc_primitive_color, 1, object_color[OBJECT_CROWN]);
	glDrawArrays(GL_TRIANGLES, 27, 30);

	glUniform3fv(loc_primitive_color, 1, object_color[OBJECT_IHS]);
	glDrawArrays(GL_LINES, 57, 18);

	glBindVertexArray(0);
}

void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	int airplane_clock, cocktail_clock, car_clock, car_flag = 0, sword_clock, house_clock, i, object_clock;
	static float house_scale1 = 1, house_scale2 = 1;
	float rand1, rand2;

	//ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//draw_axes();

	switch (animation_mode) {
	case 1:
		airplane_clock = timestamp % 1440;
		rand1 = sin(airplane_clock * TO_RADIAN);

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - ((float)airplane_clock * 1200 / 1440) + 36 * rand1, -400.0f + ((float)airplane_clock * 800 / 1440) + 24 * rand1, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock / 360.0f, airplane_clock / 360.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 236 * TO_RADIAN + rand1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + ((float)airplane_clock * 1200 / 1440) + 36 * rand1, -400.0f + ((float)airplane_clock * 800 / 1440) + 24 * rand1, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock / 360.0f, airplane_clock / 360.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 124 * TO_RADIAN + rand1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - ((float)airplane_clock * 1200 / 1440) + 36 * rand1, 400.0f - ((float)airplane_clock * 800 / 1440) + 24 * rand1, 0.0f));
		if (airplane_clock < 720) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock / 360.0f, airplane_clock / 360.0f, 0.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4 - airplane_clock / 360.0f, 4 - airplane_clock / 360.0f, 0.0f));
		}
		ModelMatrix = glm::rotate(ModelMatrix, 304 * TO_RADIAN + rand1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + ((float)airplane_clock * 1200 / 1440) + 36 * rand1, 400.0f - ((float)airplane_clock * 800 / 1440) + 24 * rand1, 0.0f));
		if (airplane_clock < 720) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock / 360.0f, airplane_clock / 360.0f, 0.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4 - airplane_clock / 360.0f, 4 - airplane_clock / 360.0f, 0.0f));
		}
		ModelMatrix = glm::rotate(ModelMatrix, 34 * TO_RADIAN + rand1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		if (timestamp == 1440) {
			animation_mode = 2;
			timestamp = 0;
		}

		break;
	case 2:
		cocktail_clock = timestamp % 1440;
		rand1 = sin(cocktail_clock * TO_RADIAN);

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((-500.0f + (rand1 + 1) * 500) * (1440 - cocktail_clock) / 1500, -300.0f + cocktail_clock / 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (cocktail_clock + 180), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((-250.0f + (rand1 + 1) * 250) * (1440 - cocktail_clock) / 1500, -250.0f + cocktail_clock / 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (cocktail_clock + 180), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -200.0f + cocktail_clock / 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (cocktail_clock + 180), glm::vec3(0.0f, -1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -200.0f + cocktail_clock / 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * cocktail_clock, glm::vec3(0.0f, -1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((250.0f - (rand1 + 1) * 250) * (1440 - cocktail_clock) / 1500, -250.0f + cocktail_clock / 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * cocktail_clock, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((500.0f - (rand1 + 1) * 500) * (1440 - cocktail_clock) / 1500, -300.0f + cocktail_clock / 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * cocktail_clock, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();


		if (timestamp == 1440) {
			animation_mode = 3;
			timestamp = 0;
		}
		break;
	case 3:
		car_clock = timestamp % 1440;

		if (car_clock >= 1200) {
			car_flag = 2;
		}

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f + (float)car_clock / 45, 1.0f + (float)car_clock / 45, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();

		if (car_clock < 200) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();
		}
		else if (car_clock < 400) {
			car_clock -= 200;
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();
		}
		else if (car_clock < 800) {
			car_clock -= 400;
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			if (car_clock < 50) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car2();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car2();
			}
			else {
				car_flag = 1;
			}
		}

		else if (car_clock < 1200) {
			car_flag = 1;
			car_clock -= 800;

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			if (car_clock < 40) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -200.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car2();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car2();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 100.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car2();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 200.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_car2();
			}
			else {
				car_flag = 2;
			}
		}

		if (car_flag >= 1) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, -100.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * timestamp, glm::vec3(1.0f, 1.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * timestamp, glm::vec3(1.0f, 1.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();
		}
		if (car_flag >= 2) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, -100.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * timestamp, glm::vec3(1.0f, 1.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 100.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * timestamp, glm::vec3(1.0f, 1.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, -200.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * timestamp, glm::vec3(1.0f, 1.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 200.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * timestamp, glm::vec3(1.0f, 1.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();
		}

		if (timestamp == 1440) {
			animation_mode = 4;
			timestamp = 0;
			car_flag = 0;
		}
		break;
	case 4:
		sword_clock = timestamp % 1440;
		rand1 = sin(sword_clock * TO_RADIAN);
		rand2 = cos(sword_clock * TO_RADIAN);

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.00f - 150 * rand2, -350.0f + 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 3 * TO_RADIAN * (270 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.00f - 150.0f * (1 + rand1), -200.0f - 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 3 * TO_RADIAN * (0 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.00f + 150 * rand2, -50.0f - 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 3 * TO_RADIAN * (90 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-450.00f + 150.0f * (1 + rand1), -200.0f + 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 3 * TO_RADIAN * (180 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.00f - 150 * rand2, -350.0f + 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 4 * TO_RADIAN * (270 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(450.00f - 150.0f * (1 + rand1), -200.0f - 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 4 * TO_RADIAN * (0 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.00f + 150 * rand2, -50.0f - 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 4 * TO_RADIAN * (90 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.00f + 150.0f * (1 + rand1), -200.0f + 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 4 * TO_RADIAN * (180 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.00f - 150 * rand2, 50.0f + 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 2 * TO_RADIAN * (270 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(450.00f - 150.0f * (1 + rand1), 200.0f - 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 2 * TO_RADIAN * (0 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.00f + 150 * rand2, 350.0f - 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 2 * TO_RADIAN * (90 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.00f + 150.0f * (1 + rand1), 200.0f + 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 2 * TO_RADIAN * (180 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.00f - 150 * rand2, 50.0f + 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (270 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.00f - 150.0f * (1 + rand1), 200.0f - 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (0 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.00f + 150 * rand2, 350.0f - 150.0f * (1 + rand1), 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (90 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-450.00f + 150.0f * (1 + rand1), 200.0f + 150.0f * rand2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (180 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		if (timestamp == 1440) {
			animation_mode = 5;
			timestamp = 0;
		}
		break;
	case 5:
		house_clock = timestamp % 1440;

		for (i = 1; i < house_clock; i++) {
			if (house_clock > 80 * (2 * i - 2) && house_clock < 80 * (2 * i)) {
				house_scale1 *= 1.05;
				ModelMatrix = glm::rotate(glm::mat4(1.0f), TO_RADIAN * house_clock, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-6.0f * house_scale1, 3.0f * house_scale1, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * house_scale1, 1.0f * house_scale1, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();
			}
			if (house_clock > 80 * (2 * i - 1) && house_clock < 80 * (2 * i + 1)) {
				house_scale2 *= 1.05;
				ModelMatrix = glm::rotate(glm::mat4(1.0f), TO_RADIAN * (180 + house_clock), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-6.0f * house_scale2, 3.0f * house_scale2, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * house_scale2, 1.0f * house_scale2, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();
			}
			if (house_clock == 160 * i) {
				house_scale1 = 1;
			}
			if (house_clock == 80 + 160 * i) {
				house_scale2 = 1;
			}
		}

		if (timestamp == 1440) {
			animation_mode = 6;
			timestamp = 0;
			house_scale1 = 1;
			house_scale2 = 1;
		}
		break;
	case 6:
		airplane_clock = timestamp % 1440;
		cocktail_clock = timestamp % 400;
		car_clock = timestamp % 500;
		sword_clock = timestamp % 1080;
		house_clock = timestamp;
		object_clock = timestamp % 200;
		rand1 = sin(timestamp * TO_RADIAN);
		rand2 = cos(timestamp * TO_RADIAN);

		for (i = 1; i < house_clock; i++) {
			if (house_clock > 80 * (2 * i - 2) && house_clock < 80 * (2 * i)) {
				house_scale1 *= 1.05;
				ModelMatrix = glm::rotate(glm::mat4(1.0f), TO_RADIAN * house_clock, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-6.0f * house_scale1, 3.0f * house_scale1, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * house_scale1, 1.0f * house_scale1, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();
			}
			if (house_clock > 80 * (2 * i - 1) && house_clock < 80 * (2 * i + 1)) {
				house_scale2 *= 1.05;
				ModelMatrix = glm::rotate(glm::mat4(1.0f), TO_RADIAN * (180 + house_clock), glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-6.0f * house_scale2, 3.0f * house_scale2, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f * house_scale2, 1.0f * house_scale2, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();
			}
			if (house_clock == 160 * i) {
				house_scale1 = 1;
			}
			if (house_clock == 80 + 160 * i) {
				house_scale2 = 1;
			}
		}


		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - ((float)airplane_clock * 1200 / 1440) + 36 * rand1, -400.0f + ((float)airplane_clock * 800 / 1440) + 24 * rand1, 0.0f));
		if (airplane_clock < 720) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock / 360.0f, airplane_clock / 360.0f, 0.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4 - airplane_clock / 360.0f, 4 - airplane_clock / 360.0f, 0.0f));
		}
		ModelMatrix = glm::rotate(ModelMatrix, 236 * TO_RADIAN + rand1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + ((float)airplane_clock * 1200 / 1440) + 36 * rand1, -400.0f + ((float)airplane_clock * 800 / 1440) + 24 * rand1, 0.0f));
		if (airplane_clock < 720) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock / 360.0f, airplane_clock / 360.0f, 0.0f));
		}
		else {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4 - airplane_clock / 360.0f, 4 - airplane_clock / 360.0f, 0.0f));
		}
		ModelMatrix = glm::rotate(ModelMatrix, 124 * TO_RADIAN + rand1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();


		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((-500.0f + (rand1 + 1) * 500) * (1440 - cocktail_clock) / 1500, -450.0f + cocktail_clock * 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (cocktail_clock + 180), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((-250.0f + (rand1 + 1) * 250) * (1440 - cocktail_clock) / 1500, -400.0f + cocktail_clock * 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (cocktail_clock + 180), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -350.0f + cocktail_clock * 2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * (cocktail_clock + 180), glm::vec3(0.0f, -1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -350.0f + cocktail_clock * 2, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * cocktail_clock, glm::vec3(0.0f, -1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((250.0f - (rand1 + 1) * 250) * (1440 - cocktail_clock) / 1500, -400.0f + cocktail_clock * 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * cocktail_clock, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((500.0f - (rand1 + 1) * 500) * (1440 - cocktail_clock) / 1500, -450.0f + cocktail_clock * 2.5, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, TO_RADIAN * cocktail_clock, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();


		if (car_clock < 200) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();
		}
		else if (car_clock < 400) {
			car_clock -= 200;
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, -200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, -100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 100.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(600.0f - 10 * car_clock, 200.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + 10 * car_clock, 300.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car2();
		}


		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f * rand2 + rand2 * 300, -150.0f + 150.0f * (1 + rand1) + rand1 * 200, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (2 * rand1 + 3) * TO_RADIAN * (270 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.00f - 150.0f * (1 + rand1) + rand2 * 300, -150.0f * rand2 + rand1 * 200, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (2 * rand1 + 3) * TO_RADIAN * (0 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f * rand2 + rand2 * 300, 150.0f - 150.0f * (1 + rand1) + rand1 * 200, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (2 * rand1 + 3) * TO_RADIAN * (90 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand1), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-150.00f + 150.0f * (1 + rand1) + rand2 * 300, 150.0f * rand2 + rand1 * 200, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (2 * rand1 + 3) * TO_RADIAN * (180 - sword_clock), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f + 2 * (1 + rand2), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		for (int i = 0; i <= CARD_NUM; i++) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cards[i].x, cards[i].y, 0.0f));
			if (cards[i].dir == 0) {
				ModelMatrix = glm::rotate(ModelMatrix, object_clock * cards[i].rotate * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else if (cards[i].dir == 1) {
				ModelMatrix = glm::rotate(ModelMatrix, object_clock * cards[i].rotate * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else if (cards[i].dir == 2) {
				ModelMatrix = glm::rotate(ModelMatrix, object_clock * cards[i].rotate * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else if (cards[i].dir == 3) {
				ModelMatrix = glm::rotate(ModelMatrix, object_clock * cards[i].rotate * TO_RADIAN, glm::vec3(1.0f, 0.0f, 1.0f));
			}
			else if (cards[i].dir == 4) {
				ModelMatrix = glm::rotate(ModelMatrix, object_clock * cards[i].rotate * TO_RADIAN, glm::vec3(0.0f, 1.0f, 1.0f));
			}
			else if (cards[i].dir == 5) {
				ModelMatrix = glm::rotate(ModelMatrix, object_clock * cards[i].rotate * TO_RADIAN, glm::vec3(1.0f, 1.0f, 0.0f));
			}
			else if (cards[i].dir == 6) {
				ModelMatrix = glm::rotate(ModelMatrix, object_clock * cards[i].rotate * TO_RADIAN, glm::vec3(1.0f, 1.0f, 1.0f));
			}
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cards[i].sizex, cards[i].sizey, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_object();

			cards[i].x += cards[i].vx;
			cards[i].y += cards[i].vy;
			if (cards[i].x > win_width / 2) {
				cards[i].vx = (rand() % 5) * -1;
				cards[i].sizex = rand() % 2 + 2;
				cards[i].sizey = rand() % 2 + 2;
				cards[i].rotate = rand() % 50;
				cards[i].dir = rand() % 7;
			}
			else if (cards[i].x < -1 * win_width / 2) {
				cards[i].vx = (rand() % 5);
				cards[i].sizex = rand() % 2 + 2;
				cards[i].sizey = rand() % 2 + 2;
				cards[i].rotate = rand() % 50;
				cards[i].dir = rand() % 7;
			}
			else if (cards[i].y < -1 * win_height / 2) {
				cards[i].vy = (rand() % 5);
				cards[i].sizex = rand() % 2 + 2;
				cards[i].sizey = rand() % 2 + 2;
				cards[i].rotate = rand() % 50;
				cards[i].dir = rand() % 7;
			}
			else if (cards[i].y > win_height / 2) {
				cards[i].vy = (rand() % 5) * -1;
				cards[i].sizex = rand() % 2 + 2;
				cards[i].sizey = rand() % 2 + 2;
				cards[i].rotate = rand() % 50;
				cards[i].dir = rand() % 7;
			}
			if (card_flag) {
				card_flag--;
				cards[CARD_NUM].vx = 0;
				cards[CARD_NUM].vy = 0;
				cards[CARD_NUM].rotate = 0;
				cards[CARD_NUM].dir = 0;
				cards[CARD_NUM].sizex = (float)(2000 - card_flag) / 100;
				cards[CARD_NUM].sizey = (float)(2000 - card_flag) / 100;
				if (card_flag == 0) {
					cards[CARD_NUM].x = 100000;
					cards[CARD_NUM].y = 100000;
				}
			}
			for (int j = 0; j < CARD_NUM; j++) {
				if (i != j && cards[i].x - cards[j].x <= 1 && cards[j].x - cards[i].x <= 1 && cards[i].y - cards[j].y <= 1 && cards[j].y - cards[i].y <= 1) {
					card_flag = 2000;
					cards[CARD_NUM].x = cards[i].x;
					cards[CARD_NUM].y = cards[i].x;
				}
			}
		}
		break;
	default:
		printf("test mode %d\n", animation_mode);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(10.0f, 10.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object();
	}

	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void special(int key, int x, int y) {
	/*
#define SENSITIVITY 2.0
	switch (key) {
	case GLUT_KEY_LEFT:
		centerx -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		centerx += SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		centery -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		centery += SENSITIVITY;
		glutPostRedisplay();
		break;
	}
	*/
}

int leftbuttonpressed = 0;
void mouse(int button, int state, int x, int y) {

	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;

}

void motion(int x, int y) {
	/*
	static int delay = 0;
	static float tmpx = 0.0, tmpy = 0.0;
	float dx, dy;
	if (leftbuttonpressed) {
		centerx =  x - win_width/2.0f, centery = (win_height - y) - win_height/2.0f;
		if (delay == 8) {
			dx = centerx - tmpx;
			dy = centery - tmpy;

			if (dx > 0.0) {
				rotate_angle = atan(dy / dx) + 90.0f*TO_RADIAN;
			}
			else if (dx < 0.0) {
				rotate_angle = atan(dy / dx) - 90.0f*TO_RADIAN;
			}
			else if (dx == 0.0) {
				if (dy > 0.0) rotate_angle = 180.0f*TO_RADIAN;
				else  rotate_angle = 0.0f;
			}
			tmpx = centerx, tmpy = centery;
			delay = 0;
		}
		glutPostRedisplay();
		delay++;
	}
	*/
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);
	glutTimerFunc(10, timer, 0);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(44 / 255.0f, 180 / 255.0f, 49 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void init_object(void) {
	srand(time(NULL));

	for (int i = 0; i < CARD_NUM; i++) {
		cards[i].x = rand() % 1000 - 500;
		cards[i].y = rand() % 700 - 350;
		cards[i].vx = rand() % 10 - 5;
		cards[i].vy = rand() % 10 - 5;
		cards[i].sizex = rand() % 2 + 2;
		cards[i].sizey = rand() % 2 + 2;
		cards[i].rotate = rand() % 50;
		cards[i].dir = rand() % 7;
	}
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_house();
	prepare_car();
	prepare_cocktail();
	prepare_car2();
	prepare_sword();
	prepare_object();
	init_object();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple2DTransformation_GLSL_3.0";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC', four arrows",
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}