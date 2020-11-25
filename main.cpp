/*
*        Computer Graphics Course - Shenzhen University
*       Week 7 - Shadow Projection Matrix Skeleton Code
* ============================================================
*
* - ����������ǲο����룬����Ҫ����ο���ҵ˵��������˳������ɡ�
* - ��������OpenGL�����������������У���ο���һ��ʵ��γ�����ĵ���
*
* ============================================================
* ���̣��ⲩ��(Bojian Wu)
* ���䣺bj.wu@siat.ac.cn
* ���������˵������ҵ���κ����⣬��ӭ���ʼ���ѯ��
*/

#include "Angel.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <iostream>

using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint modelMatrixID;
GLuint viewMatrixID;
GLuint rotationMatrixID;
GLuint modelViewMatrixID;
GLuint projMatrixID;
GLuint fColorID;

// ����ӽǲ���
float l = -3.0, r = 3.0;    // ���Ҳü�ƽ��
float b = -3.0, t = 3.0;    // ���²ü�ƽ��
float n = -3.0, f = 3.0;    // Զ���ü�ƽ��
float rotationAngle = 0.0; // ��ת�Ƕ�

vec4 red(1.0, 0.0, 0.0, 1.0);
vec4 black(0.0, 0.0, 0.0, 1.0);

float lightPos[3] = { -0.5, 2.0, 0.5 };

vec3 points[3] = {
	vec3(-0.5, 0.5, 0.5),
	vec3(0.5, 0.5, 0.5),
	vec3(0.0, 0.75, 0.0)
};

//////////////////////////////////////////////////////////////////////////
// �����������

namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;

	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{
		return mat4(2 / (right - left), 0, 0, -(right + left) / (right - left),
			0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
			0, 0, -2 / (zFar - zNear), -(zFar + zNear) / (zFar - zNear),
			0, 0, 0, 1);
	}


	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		vec4  n;
		vec4  u;
		vec4  v;

		n = eye - at;
		n = normalize(n);
		u = vec4(cross(up, n), 0.0);
		u = normalize(u);
		v = vec4(cross(n, u), 0.0);
		v = normalize(v);

		return mat4(u, v, n, vec4(0.0, 0.0, 0.0, 1.0)) *
			mat4(1.0, 0, 0, 0,
				0, 1.0, 0, 0,
				0, 0, 1.0, 0,
				-eye.x, -eye.y, -eye.z, 1.0);
	}
}

//////////////////////////////////////////////////////////////////////////
// OpenGL ��ʼ��

void init()
{
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	programID = InitShader("vshader.glsl", "fshader.glsl");

	// �Ӷ�����ɫ���л�ȡ��Ӧ������λ��
	vPositionID = glGetAttribLocation(programID, "vPosition");

	modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
	viewMatrixID = glGetUniformLocation(programID, "viewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");
	fColorID = glGetUniformLocation(programID, "fColor");

	// ����VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// OpenGL��Ӧ״̬����
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// ��Ⱦ

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO ����ģ-�ӱ任����ͶӰ����
	// ����ʹ��Ĭ���������ʱ��������ķ�����ͶӰƽ��ƽ�У���ˣ�����һ������ķ����λ��
	vec4 eye(0.5, 2.0, -0.5, 1.0); // ��Դ����y-zƽ��ĶԳƷ���
	vec4 at(0, 0, 0, 1);   // ԭ��
	vec4 up(0, 1, 0, 0);      // Ĭ�Ϸ���

	// ����Ϊ�����ص㲻����ͶӰ�任�����Խ�ͶӰ��������Ϊ����ͶӰ���ɣ�
	Camera::modelMatrix = mat4(1.0);
	Camera::viewMatrix = Camera::lookAt(eye, at, up);
	Camera::projMatrix = Camera::ortho(-3, 3, -3, 3, -3, 3);

	glEnableVertexAttribArray(vPositionID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		vPositionID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	Camera::modelMatrix = Translate(vec3(0.0, 0.0, 0.0)) * RotateZ(0.0) * RotateY(rotationAngle) * RotateX(0.0) * Scale(vec3(1.0, 1.0, 1.0));
	Camera::modelMatrix *= RotateY(rotationAngle);
	// TODO ��������ͶӰ�����»���ԭʼ�������Σ��ú�ɫ��ʾ��


	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &Camera::viewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);
	glUniform4fv(fColorID, 1, red);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// TODO ������ӰͶӰ���󣬻���ͶӰ֮��������Σ��ú�ɫ��ʾ��
	mat4 shadowProjMatrix = mat4(-lightPos[1], lightPos[0], 0, 0,
		0, 0, 0, 0,
		0, lightPos[2], -lightPos[1], 0,
		0, 1, 0, -lightPos[1]);

	Camera::modelMatrix *= shadowProjMatrix;
	// ���Ķ�������ɫ�������ݣ�����ʵ��1����ͬ
	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &Camera::viewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);

	glUniform4fv(fColorID, 1, black);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);

	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// �������ô���

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// �����Ӧ����

void mouse(int button, int state, int x, int y)
{
	GLfloat px = x;
	GLfloat py = y;
	px = (px - 250)/250;
	py = (250-py)/250;
	lightPos[0] = px;
	lightPos[2] = py;

	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////
// ������Ӧ����

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit(EXIT_SUCCESS);
		break;
	case 'x':		// 'x' ��ʹ����������ת-1.0��
		rotationAngle -= 1.0;
		break;
	case 'X':		// 'X' ��ʹ����������ת1.0��
		rotationAngle += 1.0;
		break;
	}
	//modelMatrix *= RotateX(rotationAngle);
	glutPostRedisplay();
}

void printHelp()
{
	printf("%s\n\n", "Camera");
	printf("Keyboard options:\n");
	printf("y: Decrease rotation angle (aroud Y axis)\n");
	printf("Y: Increase rotation angle (aroud Y axis)\n");
}

//////////////////////////////////////////////////////////////////////////

void idle(void)
{
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////

void clean()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL-Tutorial");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	printHelp();
	glutMainLoop();

	clean();

	return 0;
}