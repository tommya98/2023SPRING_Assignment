#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define TO_RADIAN		0.017453292519943296
#define PI				3.14159265358979323846264338327950

// implement(h)
typedef struct {
	float point[6][2];
	float point_center_x;
	float point_center_y;
	int state;
	int rotate;
} STAR;
STAR star_main;
STAR star_copy[100];
STAR star_copy2[8];
int star_copy_num = 0;
int star_copy_make = 0;

void make_star(STAR* s, float centerx, float centery) {
	s->point[0][0] = centerx + 0.2 * sin(0);
	s->point[0][1] = centery + 0.2 * cos(0);
	s->point[1][0] = centerx + 0.2 * sin(4 * PI / 5);
	s->point[1][1] = centery + 0.2 * cos(4 * PI / 5);
	s->point[2][0] = centerx + 0.2 * sin(8 * PI / 5);
	s->point[2][1] = centery + 0.2 * cos(8 * PI / 5);
	s->point[3][0] = centerx + 0.2 * sin(2 * PI / 5);
	s->point[3][1] = centery + 0.2 * cos(2 * PI / 5);
	s->point[4][0] = centerx + 0.2 * sin(6 * PI / 5);
	s->point[4][1] = centery + 0.2 * cos(6 * PI / 5);
	s->point_center_x = centerx;
	s->point_center_y = centery;
}

void star_shearing(int flag) {
	for (int i = 0; i < 5; i++) {
		star_main.point[i][0] -= star_main.point_center_x;
		star_main.point[i][1] -= star_main.point_center_y;
	}
	if (flag == 1) { //xÃà
		for (int i = 0; i < 5; i++) {
			star_main.point[i][0] += 0.1 * star_main.point[i][1];
		}
	}
	else if (flag == -1) {
		for (int i = 0; i < 5; i++) {
			star_main.point[i][0] -= 0.1 * star_main.point[i][1];
		}
	}
	else if(flag == 2) {
		for (int i = 0; i < 5; i++) {
			star_main.point[i][1] += 0.1 * star_main.point[i][0];
		}
	}
	else if (flag == -2) {
		for (int i = 0; i < 5; i++) {
			star_main.point[i][1] -= 0.1 * star_main.point[i][0];
		}
	}
	for (int i = 0; i < 5; i++) {
		star_main.point[i][0] += star_main.point_center_x;
		star_main.point[i][1] += star_main.point_center_y;
	}

	glutPostRedisplay();
}

void make_copy_star(void) {
	for (int i = 0; i < 5; i++) {
		star_copy[star_copy_num].point[i][0] = star_main.point[i][0];
		star_copy[star_copy_num].point[i][1] = star_main.point[i][1];
	}
	star_copy[star_copy_num].point_center_x = star_main.point_center_x;
	star_copy[star_copy_num].point_center_y = star_main.point_center_y;
	star_copy[star_copy_num].state = 2;
	star_copy[star_copy_num].rotate = 60;

	if (star_copy_num == 99) {
		star_copy_num = 0;
	}
	else {
		star_copy_num++;
	}
}

void star_copy_rotate(void){
	for (int j = 0; j < 100; j++) {
		star_copy[j].rotate--;
		if (star_copy[j].rotate <= 0) {
			star_copy[j].state = -1;
		}
		if (star_copy[j].state == 2) {
			for (int i = 0; i < 5; i++) {
				star_copy[j].point[i][0] -= star_copy[j].point_center_x;
				star_copy[j].point[i][1] -= star_copy[j].point_center_y;
			}
			for (int i = 0; i < 5; i++) {
				star_copy[j].point[i][0] = star_copy[j].point[i][0] * cos(10 * TO_RADIAN) - star_copy[j].point[i][1] * sin(10 * TO_RADIAN);
				star_copy[j].point[i][1] = star_copy[j].point[i][0] * sin(10 * TO_RADIAN) + star_copy[j].point[i][1] * cos(10 * TO_RADIAN);
			}
			for (int i = 0; i < 5; i++) {
				star_copy[j].point[i][0] *= 0.99;
				star_copy[j].point[i][1] *= 0.99;
			}
			for (int i = 0; i < 5; i++) {
				star_copy[j].point[i][0] += star_copy[j].point_center_x;
				star_copy[j].point[i][1] += star_copy[j].point_center_y;
			}
		}
	}

	for (int j = 0; j < 8; j++) {
		star_copy2[j].rotate--;
		if (star_copy2[j].rotate <= 0) {
			star_copy2[j].state = -1;
		}
		if (star_copy2[j].state == 3) {
			for (int i = 0; i < 5; i++) {
				star_copy2[j].point[i][0] -= star_copy2[j].point_center_x;
				star_copy2[j].point[i][1] -= star_copy2[j].point_center_y;
			}
			for (int i = 0; i < 5; i++) {
				star_copy2[j].point[i][0] = star_copy2[j].point[i][0] * cos(10 * TO_RADIAN) - star_copy2[j].point[i][1] * sin(10 * TO_RADIAN);
				star_copy2[j].point[i][1] = star_copy2[j].point[i][0] * sin(10 * TO_RADIAN) + star_copy2[j].point[i][1] * cos(10 * TO_RADIAN);
			}
			for (int i = 0; i < 5; i++) {
				star_copy2[j].point[i][0] *= 0.99;
				star_copy2[j].point[i][1] *= 0.99;
			}
			for (int i = 0; i < 5; i++) {
				star_copy2[j].point[i][0] += star_copy2[j].point_center_x;
				star_copy2[j].point[i][1] += star_copy2[j].point_center_y;
			}
		}
	}
	glutPostRedisplay();
}

void star_reflect(int flag) {
	float left = star_main.point[0][0], right = star_main.point[0][0], top = star_main.point[0][1], down = star_main.point[0][1];

	for (int i = 1; i < 5; i++) {
		if (star_main.point[i][0] > right) {
			right = star_main.point[i][0];
		}
		if (star_main.point[i][0] < left) {
			left = star_main.point[i][0];
		}
		if (star_main.point[i][1] > top) {
			top = star_main.point[i][1];
		}
		if (star_main.point[i][1] < down) {
			down = star_main.point[i][1];
		}
	}

	if (flag == 1 || flag == 3) {
		for (int i = 0; i < 5; i++) {
			star_copy2[0].point[i][0] = ( (- 1) * (star_main.point[i][0] - right)) + right;
			star_copy2[0].point[i][1] = star_main.point[i][1];
		}
		star_copy2[0].point_center_x = ((-1) * (star_main.point_center_x - right)) + right;
		star_copy2[0].point_center_y = star_main.point_center_y;
		star_copy2[0].state = 3;
		star_copy2[0].rotate = 60;

		for (int i = 0; i < 5; i++) {
			star_copy2[1].point[i][0] = ((-1) * (star_main.point[i][0] - left)) + left;
			star_copy2[1].point[i][1] = star_main.point[i][1];
		}
		star_copy2[1].point_center_x = ((-1) * (star_main.point_center_x - left)) + left;
		star_copy2[1].point_center_y = star_main.point_center_y;
		star_copy2[1].state = 3;
		star_copy2[1].rotate = 60;

		for (int i = 0; i < 5; i++) {
			star_copy2[2].point[i][0] = star_main.point[i][0];
			star_copy2[2].point[i][1] = ((-1) * (star_main.point[i][1] - top)) + top;
		}
		star_copy2[2].point_center_x = star_main.point_center_x;
		star_copy2[2].point_center_y = ((-1) * (star_main.point_center_y - top)) + top;
		star_copy2[2].state = 3;
		star_copy2[2].rotate = 60;

		for (int i = 0; i < 5; i++) {
			star_copy2[3].point[i][0] = star_main.point[i][0];
			star_copy2[3].point[i][1] = ((-1) * (star_main.point[i][1] - down)) + down;
		}
		star_copy2[3].point_center_x = star_main.point_center_x;
		star_copy2[3].point_center_y = ((-1) * (star_main.point_center_y - down)) + down;
		star_copy2[3].state = 3;
		star_copy2[3].rotate = 60;
	}
	if (flag == 2 || flag == 3) {
		for (int i = 0; i < 5; i++) {
			star_copy2[4].point[i][0] = star_main.point[i][1] - top + left;
			star_copy2[4].point[i][1] = star_main.point[i][0] - left + top;
		}
		star_copy2[4].point_center_x = star_main.point_center_y - top + left;
		star_copy2[4].point_center_y = star_main.point_center_x - left + top;
		star_copy2[4].state = 3;
		star_copy2[4].rotate = 60;

		for (int i = 0; i < 5; i++) {
			star_copy2[5].point[i][0] = star_main.point[i][1] - down + right;
			star_copy2[5].point[i][1] = star_main.point[i][0] - right + down;
		}
		star_copy2[5].point_center_x = star_main.point_center_y - down + right;
		star_copy2[5].point_center_y = star_main.point_center_x - right + down;
		star_copy2[5].state = 3;
		star_copy2[5].rotate = 60;

		for (int i = 0; i < 5; i++) {
			star_copy2[6].point[i][0] = (-1) * (star_main.point[i][0] - left) + left;
			star_copy2[6].point[i][1] = (-1) * (star_main.point[i][1] - down) + down;
		}
		star_copy2[6].point_center_x = (-1) * (star_main.point_center_x - left) + left;
		star_copy2[6].point_center_y = (-1) * (star_main.point_center_y - down) + down;
		star_copy2[6].state = 3;
		star_copy2[6].rotate = 60;

		for (int i = 0; i < 5; i++) {
			star_copy2[7].point[i][0] = (-1) * (star_main.point[i][0] - right) + right;
			star_copy2[7].point[i][1] = (-1) * (star_main.point[i][1] - top) + top;
		}
		star_copy2[7].point_center_x = (-1) * (star_main.point_center_x - right) + right;
		star_copy2[7].point_center_y = (-1) * (star_main.point_center_y - top) + top;
		star_copy2[7].state = 3;
		star_copy2[7].rotate = 60;
	}

	glutPostRedisplay();
}

int rightbuttonpressed = 0;
int leftbuttonpressed = 0, center_selected = 0;

float r, g, b; // Background color
float px, py, qx, qy; // Line (px, py) --> (qx, qy)
int n_object_points = 6;
float object[6][2], object_center_x, object_center_y;
float rotation_angle_in_degree;
int window_width, window_height;

void draw_axes() {
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, 0.025f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, -0.025f);
	glVertex2f(1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(-0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glEnd();
	glLineWidth(1.0f);

	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPointSize(1.0f);
}
void draw_line(float px, float py, float qx, float qy) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(px, py); 
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(px, py);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(1.0f);
}

void draw_object(void) {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(object_center_x, object_center_y);
	glEnd();
	glPointSize(1.0f);
}

// implement (h)
void draw_star(void) {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < 5; i++)
		glVertex2f(star_main.point[i][0], star_main.point[i][1]);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 5; i++)
		glVertex2f(star_main.point[i][0], star_main.point[i][1]);
	if (star_main.state == 1) {
		glColor3f(0.0f, 0.0f, 0.0f);
	}
	else {
		glColor3f(0.33f, 0.33f, 0.33f);
	}
	glVertex2f(star_main.point_center_x, star_main.point_center_y);
	glEnd();
	glPointSize(1.0f);
}

void draw_star_copy(void) {
	for (int j = 0; j < 100; j++) {
		if (star_copy[j].state == 2) {
			glBegin(GL_LINE_LOOP);
			glColor3f(0.0f, 1.0f, 0.0f);
			for (int i = 0; i < 5; i++)
				glVertex2f(star_copy[j].point[i][0], star_copy[j].point[i][1]);
			glEnd();
			glPointSize(5.0f);
			glBegin(GL_POINTS);
			glColor3f(1.0f, 1.0f, 1.0f);
			for (int i = 0; i < 5; i++)
				glVertex2f(star_copy[j].point[i][0], star_copy[j].point[i][1]);
			if (star_copy[j].state == 1) {
				glColor3f(0.0f, 0.0f, 0.0f);
			}
			else {
				glColor3f(0.33f, 0.33f, 0.33f);
			}
			glVertex2f(star_copy[j].point_center_x, star_copy[j].point_center_y);
			glEnd();
			glPointSize(1.0f);
		}
	}
}

void draw_star_copy2(void) {
	for (int j = 0; j < 8; j++) {
		if (star_copy2[j].state == 3) {
			glBegin(GL_LINE_LOOP);
			glColor3f(0.0f, 1.0f, 0.0f);
			for (int i = 0; i < 5; i++)
				glVertex2f(star_copy2[j].point[i][0], star_copy2[j].point[i][1]);
			glEnd();
			glPointSize(5.0f);
			glBegin(GL_POINTS);
			glColor3f(1.0f, 1.0f, 1.0f);
			for (int i = 0; i < 5; i++)
				glVertex2f(star_copy2[j].point[i][0], star_copy2[j].point[i][1]);
			if (star_copy2[j].state == 1) {
				glColor3f(0.0f, 0.0f, 0.0f);
			}
			else {
				glColor3f(0.33f, 0.33f, 0.33f);
			}
			glVertex2f(star_copy2[j].point_center_x, star_copy2[j].point_center_y);
			glEnd();
			glPointSize(1.0f);
		}
	}
}

void display(void) {
	glClearColor(r, g, b, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT);

	draw_axes();
	draw_line(px, py, qx, qy);
	draw_object();
	//implement (h)
	draw_star();
	draw_star_copy();	
	draw_star_copy2();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		r = 1.0f; g = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'g':
		g = 1.0f; r = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'b':
		b = 1.0f; r = g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 's':
		r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	// implement (h)
	case 'x':
		star_shearing(1);
		break;
	case 'z':
		star_shearing(-1);
		break;
	case 'y':
		star_shearing(2);
		break;
	case 't':
		star_shearing(-2);
		break;
	case '1':
		star_reflect(1);
		break;
	case '2':
		star_reflect(2);
		break;
	case '3':
		star_reflect(3);
		break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		r -= 0.1f;
		if (r < 0.0f) r = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		r += 0.1f;
		if (r > 1.0f) r = 1.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		g -= 0.1f;
		if (g < 0.0f) g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		g += 0.1f;
		if (g > 1.0f) g = 1.0f;  
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	}
}

int prevx, prevy;
void mousepress(int button, int state, int x, int y) {
	// implement (e)
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		leftbuttonpressed = 1;
		prevx = x;
		prevy = y;

		int temppx = (window_width / 2.0f) + (250 * px);
		int temppy = (window_height / 2.0f) - (250 * py);
		temppx -= x;
		temppy -= y;

		if (temppx >= -2 && temppx <= 2 && temppy >= -2 && temppy <= 2) {
			center_selected = 1;
		}
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
		leftbuttonpressed = 0;
		center_selected = 0;
	}

	// implement (f),(g)
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		rightbuttonpressed = 1;
		prevx = x;
		prevy = y;
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
		rightbuttonpressed = 0;
	}
}

void mousemove(int x, int y) {
	int specialkey = glutGetModifiers();
	float del_x = (x - prevx) / 250.0f;
	float del_y = (y - prevy) / 250.0f;

	// implement (e)
	if ((center_selected == 1) && (leftbuttonpressed == 1) && (specialkey == GLUT_ACTIVE_SHIFT)) {
		px += del_x;
		py -= del_y;
	}

	// implement (f)
	if ((rightbuttonpressed == 1) && (specialkey == GLUT_ACTIVE_ALT)) {
		for (int i = 0; i < n_object_points; i++) {
			object[i][0] += del_x;
			object[i][1] -= del_y;
		}
		object_center_x = 0.0f;
		object_center_y = 0.0f;
		for (int i = 0; i < n_object_points; i++) {
			object_center_x += object[i][0];
			object_center_y += object[i][1];
		}
		object_center_x /= n_object_points;
		object_center_y /= n_object_points;
	}

	// implement (g)
	if ((rightbuttonpressed == 1) && (specialkey == GLUT_ACTIVE_CTRL)) {
		float scale = (1 + del_x);
		for (int i = 0; i < n_object_points; i++) {
			object[i][0] -= object_center_x;
			object[i][1] -= object_center_y;
		}

		for (int i = 0; i < n_object_points; i++) {
			object[i][0] *= scale;
			object[i][1] *= scale;
		}

		for (int i = 0; i < n_object_points; i++) {
			object[i][0] += object_center_x;
			object[i][1] += object_center_y;
		}
	}

	// implement (h)
	if ((rightbuttonpressed == 1) && (specialkey == GLUT_ACTIVE_SHIFT)) {
		star_copy_make %= 3;
		if (star_copy_make == 2) {
			make_copy_star();
		}
		star_copy_make++;
		for (int i = 0; i < 5; i++) {
			star_main.point[i][0] += del_x;
			star_main.point[i][1] -= del_y;
		}
		star_main.point_center_x += del_x;
		star_main.point_center_y -= del_y;
	}

	prevx = x;
	prevy = y;

	glutPostRedisplay();
}

// implement (d)
void mousewheel(int wheel, int direction, int x, int y) {
	qx -= px;
	qy -= py;
	qx = qx * cos(direction * TO_RADIAN) - qy * sin(direction * TO_RADIAN);
	qy = qx * sin(direction * TO_RADIAN) + qy * cos(direction * TO_RADIAN);
	qx += px;
	qy += py;

	glutPostRedisplay();
}
	
void reshape(int width, int height) {
	// DO NOT MODIFY THIS FUNCTION!!!
	window_width = width, window_height = height;
	glViewport(0.0f, 0.0f, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-window_width / 500.0f, window_width / 500.0f,  -window_height / 500.0f, window_height / 500.0f, -1.0f, 1.0f);

	glutPostRedisplay();
}

// implement (h)
void timer(int param) {
	star_copy_rotate();

	glutTimerFunc(30, timer, 1);
}


void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
 	glutCloseFunc(close);

	//implement (d)
	glutMouseWheelFunc(mousewheel);

	//implement (h)
	glutTimerFunc(30, timer, 1);
}

void initialize_renderer(void) {
	register_callbacks();
	r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f; // Background color = Salmon
	// implement (b)
	px = 0.2f, py = 0.8f, qx = 0.8f, qy = 0.8f;
	rotation_angle_in_degree = 1.0f; // 1 degree
	
	// implement (c)
	float sq_cx = -0.5f, sq_cy = -0.1f, sq_side = 0.25f;
	object[0][0] = sq_cx - 2 * sq_side;
	object[0][1] = sq_cy - 0.3 * sq_side;
	object[1][0] = sq_cx - 0.6 * sq_side;
	object[1][1] = sq_cy - 0.3 * sq_side;
	object[2][0] = sq_cx - 0.1 * sq_side;
	object[2][1] = sq_cy + 0.6 * sq_side;
	object[3][0] = sq_cx + 0.5 * sq_side;
	object[3][1] = sq_cy - 0.3 * sq_side;
	object[4][0] = sq_cx + 0.5 * sq_side;
	object[4][1] = sq_cy - 1.5 * sq_side;
	object[5][0] = sq_cx - 2 * sq_side;
	object[5][1] = sq_cy - 1.5 * sq_side;
	object_center_x = object_center_y = 0.0f;
	for (int i = 0; i < n_object_points; i++) {
		object_center_x += object[i][0];
		object_center_y += object[i][1];
	}
	object_center_x /= n_object_points;
	object_center_y /= n_object_points;

	// implement (h)
	make_star(&star_main, 1, 1);
	star_main.state = 1;
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
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

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple 2D Transformations";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'r', 'g', 'b', 's', 'q'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: SHIFT/L-click and move, ALT/R-click and move, CTRL/R-click and move",
		"    - Wheel used: up and down scroll"
		"    - Other operations: window size change"
	};

	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
 //	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	// implement (a)
	glutInitWindowSize(750, 750);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

   // glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}