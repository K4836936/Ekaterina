#include "Render.h"
#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"

#include <sstream>
#include <iostream>

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "MyShaders.h"

#include "ObjLoader.h"
#include "GUItextRectangle.h"

#include "Texture.h"

#include<vector>
#include "PlayBall.h"

using namespace std;

GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.

bool textureMode = true;
bool lightMode = true;


//��������� ������ ��� ��������� ����
#define POP glPopMatrix()
#define PUSH glPushMatrix()


ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

//������ ��� �������� (������ ����� ���������� � ������������ ��������
vector<Shader> shader = { Shader("shaders\\v.vert", "shaders\\light.frag"),
						  Shader("shaders\\v.vert", "shaders\\textureShader.frag")
};


ObjFile BigBallObj, cubeMaxObj, cubeMinObj, manObj, planetObj, platformObj, towerObj;
Texture manTex, planetTex, towerTex, grassTex;

//����� ���������� �����
double TimeLastFrame = 0;
//�������� ���, ���� ����� ������ ����
bool showCubeMax = false;
//�������� ������ �������� ����
bool showActiveZone = false;
//�������� ������� ���
bool showCubeMin = false;
//�������� ����� � ��������
bool changeColor = false;
//�������������
bool Restart = false;

//�������� 
//����� �������
int TotalBall = 20;
//���� �������� �������
int BallReleased = TotalBall;
//����� �������
int BallDestroyed = 0;
//���������� ����������� ������� ��� ������ ��� ����
int BallMissed = 0;

using namespace std;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	virtual void SetUpCamera()
	{

		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ���������!
class WASDcamera :public CustomCamera
{
public:
		
	float camSpeed;

	WASDcamera()
	{
		camSpeed = 0.4;
		pos.setCoords(5, 5, 5);
		lookPoint.setCoords(0, 0, 0);
		normal.setCoords(0, 0, 1);
	}

	virtual void SetUpCamera()
	{

		if (OpenGL::isKeyPressed('W'))
		{
			Vector3 forward = (lookPoint - pos).normolize()*camSpeed;
			pos = pos + forward;
			lookPoint = lookPoint + forward;
			
		}
		if (OpenGL::isKeyPressed('S'))
		{
			Vector3 forward = (lookPoint - pos).normolize()*(-camSpeed);
			pos = pos + forward;
			lookPoint = lookPoint + forward;
			
		}

		LookAt();
	}

} WASDcam;


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
				glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}
		/*
		if (f1)
			glEnable(GL_LIGHTING);
		if (f2)
			glEnable(GL_TEXTURE_2D);
		if (f3)
			glEnable(GL_DEPTH_TEST);
			*/
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����



//������ ���������� ����
int mouseX = 0, mouseY = 0;




float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;

//���������� �������� ����
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}


	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

//���������� �������� ������  ����
void mouseWheelEvent(OpenGL *ogl, int delta)
{


	float _tmpZ = delta*0.003;
	if (ogl->isKeyPressed('Z'))
		_tmpZ *= 10;
	zoom += 0.2*zoom*_tmpZ;


	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;
}

//���������� ������� ������ ����������
void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}	   

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'Q')
		Time = 0;

	//�������� ���, ���� ����� ����
	if (key == 'S') {
		if (!showCubeMax) {
			showCubeMax = true;
		}
		else {
			showCubeMax = false;
		}
	}
	//�������� ��� ��� �������� ���
	if (key == 'A') {
		if (!showCubeMin) {
			showCubeMin = true;
		}
		else {
			showCubeMin = false;
		}
	}
	//�������� �������� ����
	if (key == 'D') {
		if (!showActiveZone) {
			showActiveZone = true;
		}
		else {
			showActiveZone = false;
		}
	}
	//�������� �����
	if (key == 'E') {
		if (!changeColor) {
			changeColor = true;
		}
	}
	//����������
	if (key == 'P') {
		if (!Restart) {
			Restart = true;
		}
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{

}


//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{

	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	
	


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	 //��� ��� ��� ������� ������ *.obj �����, ��� ��� ��� ��������� �� ���������� � ���������� �������, 
	 // ������������ �� ����� ����������, � ������������ ������ � *.obj_m

	for (int i = 0; i < (int)shader.size(); i++) {
		// ��������� ������� �� �����
		shader[i].LoadShaderFromFile();
		//�����������
		shader[i].Compile();
	}

	loadModel("models\\BigBall.obj_m", &BigBallObj);
	loadModel("models\\cubeMax.obj_m", &cubeMaxObj);
	loadModel("models\\cubeMin.obj_m", &cubeMinObj);
	loadModel("models\\man.obj_m", &manObj);
	loadModel("models\\planet.obj_m", &planetObj);
	loadModel("models\\platform.obj_m", &platformObj);
	loadModel("models\\tower.obj_m", &towerObj);
	
	manTex.loadTextureFromFile("textures//man.bmp");
	manTex.bindTexture();

	planetTex.loadTextureFromFile("textures//planet.bmp");
	planetTex.bindTexture();

	towerTex.loadTextureFromFile("textures//tower.bmp");
	towerTex.bindTexture();

	grassTex.loadTextureFromFile("textures//grass.bmp");
	grassTex.bindTexture();

}




void Render(OpenGL *ogl)
{   
	
	tick_o = tick_n;
	tick_n = GetTickCount();
	Time += (tick_n - tick_o) / 1000.0;

	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//��������� ���������
	GLfloat amb[] = { 0.6, 0.6, 0.6, 1. };
	GLfloat dif[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat spec[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat sh = 0.1f * 256;

	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//������� ��� 

	// --- ������� ---


	glPushMatrix();
	glRotated(180, 0, 0, 1);
	manTex.bindTexture();
	manObj.DrawObj();
	glPopMatrix();

	// --- ������ ����� ---
	
	DrawTower();

	// -- ������� ������ � ��������� ������� ( --

	Play();

	// --- ������� ����, ���� ����� ��� ���� ---

	if (showCubeMax) {
		DrawCubeMax();
	}

	// --- ������� ���� ��� �������� ��� ---

	if (showCubeMin) {
		DrawCubeMin();
	}

	// --- ������� �������� ��� ---

	if (showActiveZone) {
		DrawActiveZone();
	}
	// --- ��������� ---


	//��������� ����
	glDisable(GL_LIGHTING);
	//��������� ��������
	glDisable(GL_TEXTURE_2D);
	//�������� ����� ����������
	glEnable(GL_BLEND);
	////������ ����� ��� ������������� ��������� � ���������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glColor3d(0.75, 0.34, 0.67);
	glColor4d(0.34, 0.76, 0.32, 0.44);
	platformObj.DrawObj();
	glPopMatrix();

	//��������� ����� ����������
	glDisable(GL_BLEND);
	//�������� ����
	glEnable(GL_LIGHTING);
	//�������� ��������
	glEnable(GL_TEXTURE_2D);

	if (changeColor) {
		changeColor = false;
	}

	if (Restart) {
		Restart = false;
	}
}   //����� ���� �������


bool gui_init = false;

//������ ���������, ��������� ����� �������� �������
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 250);
	rec.setPosition(10, ogl->getHeight() - 250 - 10);
		 

	std::stringstream ss;
	ss << "����� ����� - " << TotalBall << std::endl;
	ss << "���� �������� ����� - " << BallReleased << std::endl;
	ss << "����� ����� - " << BallDestroyed << std::endl;
	//ss << "���-�� ����������� ������� ��� ������ ��� ���� - " << BallMissed << std::endl;
	ss << "A - ��������/������ ��� ��� �������� ���" << std::endl;
	ss << "S - ��������/������ ���, ���� ����� ������ ����" << std::endl;
	ss << "D - ��������/������ �������� ����" << std::endl;
	ss << "E - �������� ����� � �������� � alpha-����������" << std::endl;
	ss << "P - ������������� ����" << std::endl;
	ss << "B + (1...9) - ��������� �������" << std::endl;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;


	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	Shader::DontUseShaders();

}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 250 - 10);
}

