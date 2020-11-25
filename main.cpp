/*
*        Computer Graphics Course - Shenzhen University
*       Week 7 - Shadow Projection Matrix Skeleton Code
* ============================================================
*
* - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
* - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
*
* ============================================================
* 助教：吴博剑(Bojian Wu)
* 邮箱：bj.wu@siat.ac.cn
* 如果对上述说明或作业有任何问题，欢迎发邮件咨询。
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

// 相机视角参数
float l = -3.0, r = 3.0;    // 左右裁剪平面
float b = -3.0, t = 3.0;    // 上下裁剪平面
float n = -3.0, f = 3.0;    // 远近裁剪平面
float rotationAngle = 0.0; // 旋转角度

vec4 red(1.0, 0.0, 0.0, 1.0);
vec4 black(0.0, 0.0, 0.0, 1.0);

float lightPos[3] = { -0.5, 2.0, 0.5 };

vec3 points[3] = {
	vec3(-0.5, 0.5, 0.5),
	vec3(0.5, 0.5, 0.5),
	vec3(0.0, 0.75, 0.0)
};

//////////////////////////////////////////////////////////////////////////
// 相机参数控制

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
		// TODO 请按照实验课内容补全相机观察矩阵的计算
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
// OpenGL 初始化

void init()
{
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	programID = InitShader("vshader.glsl", "fshader.glsl");

	// 从顶点着色器中获取相应变量的位置
	vPositionID = glGetAttribLocation(programID, "vPosition");

	modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
	viewMatrixID = glGetUniformLocation(programID, "viewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");
	fColorID = glGetUniformLocation(programID, "fColor");

	// 生成VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// 生成VBO，并绑定顶点坐标
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// OpenGL相应状态设置
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// 渲染

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO 设置模-视变换矩阵，投影矩阵
	// 由于使用默认相机参数时，相机看的方向与投影平面平行，因此，调整一下相机的方向和位置
	vec4 eye(0.5, 2.0, -0.5, 1.0); // 光源关于y-z平面的对称方向
	vec4 at(0, 0, 0, 1);   // 原点
	vec4 up(0, 1, 0, 0);      // 默认方向

	// （因为本节重点不在于投影变换，所以将投影矩阵设置为正交投影即可）
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
	// TODO 在正常的投影矩阵下绘制原始的三角形（用红色表示）


	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &Camera::viewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);
	glUniform4fv(fColorID, 1, red);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// TODO 计算阴影投影矩阵，绘制投影之后的三角形（用黑色表示）
	mat4 shadowProjMatrix = mat4(-lightPos[1], lightPos[0], 0, 0,
		0, 0, 0, 0,
		0, lightPos[2], -lightPos[1], 0,
		0, 1, 0, -lightPos[1]);

	Camera::modelMatrix *= shadowProjMatrix;
	// 请阅读顶点着色器的内容，对照实验1的异同
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
// 重新设置窗口

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// 鼠标响应函数

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
// 键盘响应函数

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
	case 'x':		// 'x' 键使得三角形旋转-1.0度
		rotationAngle -= 1.0;
		break;
	case 'X':		// 'X' 键使得三角形旋转1.0度
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