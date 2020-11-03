#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
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
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

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

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

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
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

GLuint texId[2];

void Texture(const char* name, const int number)
{
	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE* texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP(name, &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//генерируем ИД для текстуры
	glGenTextures(1, &texId[number]);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[number]);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);



	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

//GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	
	Texture("texture1.bmp", 0);
	Texture("texture2.bmp", 1);
	

	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void normal(double a[3], double b[3], double c[3], double n[])
{
	double x1 = a[0] - b[0];
	double y1 = a[1] - b[1];
	double z1 = a[2] - b[2];

	double x2 = c[0] - b[0];
	double y2 = c[1] - b[1];
	double z2 = c[2] - b[2];

	n[0] = - y1 * z2 + y2 * z1;
	n[1] = x1 * z2 - x2 * z1;
	n[2] = - x1 * y2 + x2 * y1;
}

#define _USE_MATH_DEFINES
#include <cmath>

double A1[] = { 10, 3, 0 };
double A2[] = { 6, 6, 0 };
double A3[] = { 3, 5, 0 };
double A4[] = { 6, 1, 0 };
double A5[] = { 1, 5, 0 };
double A6[] = { 1, 8, 0 };
double A7[] = { 6, 11, 0 };
double A8[] = { 3, 7, 0 };
double A9[] = { 6, 8, 0 };
double A10[] = { 10, 5, 0 };

double B1[] = { 10, 3, 2 };
double B2[] = { 6, 6, 2 };
double B3[] = { 3, 5, 2 };
double B4[] = { 6, 1, 2 };
double B5[] = { 1, 5, 2 };
double B6[] = { 1, 8, 2 };
double B7[] = { 6, 11, 2 };
double B8[] = { 3, 7, 2 };
double B9[] = { 6, 8, 2 };
double B10[] = { 10, 5, 2 };

double Mv[] = { 8, 6, 4 };
double Mn[] = { 8, 6, -2 };

double k1 = (8 - 6) / (6 - 8);
double k2 = (10 - 8) / (5 - 6);
double x = (k1 * k2 * (6 - 10) + k2 * (8 + 6) - k1 * (6 + 5)) / (2 * (k2 - k1)); //координаты центра окр вогнутой станы
double y = (-1 / k1) * (x - (8 + 6) / 2) + (6 + 8) / 2;
double kor = sqrt((x - 8) * (x - 8) + (y - 6) * (y - 6)); // радиус окр вогнутой станы

void DrawVogCircle(double x, double y, double kor) //вогнутая стана
{
	int p = 60;
	glBegin(GL_QUAD_STRIP);
	for (float i = 0; i <= 6.2; i = i + 0.05)
	{
		double A[] = { x + kor * sin(i * 2 * M_PI / p), y + kor * cos(i * 2 * M_PI / p), 0 };
		double B[] = { x + kor * sin(i * 2 * M_PI / p), y + kor * cos(i * 2 * M_PI / p), 2 };
		double C[] = { x + kor * sin((i+0.05) * 2 * M_PI / p), y + kor * cos((i+0.05) * 2 * M_PI / p), 0 };
		double n[3] = { 0,0,0 };
		normal(A, B, C, n);
		glNormal3d(-n[0], -n[1], -n[2]);
		glColor3d(1, 0, 0);
		glVertex3dv(A);
		glColor3d(0.4, 1.5, 1);
		glVertex3dv(B);
	}
	glEnd();
}

void DrawNiz(int Urr, int alf) //веер у вогнутой стены
{
	int n = 60;
	glBegin(GL_TRIANGLE_STRIP);
	for (float i = 3.05; i <= 9.3; i = i + 0.05)
	{
		if(alf==0){glColor3d(0.4, 1.5, 1); }
		if (Urr == 0) { glNormal3d(0, 0, -1); }
		else { glNormal3d(0, 0, 1); }
		glTexCoord2d(0.5, 0.5);    //не корректно натянута текстура, но без нее плох видно освещение этого куска, при выкл текстуре освещение норм
		glVertex3d(6, 6, Urr);
		glTexCoord2d((x - kor * sin(i * 2 * M_PI / n))*0.1,(y - kor * cos(i * 2 * M_PI / n))*0.1);   //не корректно натянута текстура, но без нее плох видно освещение этого куска
		glVertex3d(x - kor * sin(i * 2 * M_PI / n), y - kor * cos(i * 2 * M_PI / n), Urr);
	}
	glEnd();
}

void NizV(int Ur, int a) //низ ноги верхний уровень
{
	DrawNiz(Ur, a);
	glBegin(GL_TRIANGLES);
	if (a == 0) { glColor3d(0.4, 1.5, 1); }
	glNormal3d(0, 0, 1);
	glTexCoord2d(0.1, 0.8); glVertex3dv(B1);
	glTexCoord2d(0.5, 0.5); glVertex3dv(B2);
	glTexCoord2d(0.1, 0.6); glVertex3dv(B10);
	glEnd();
}

void NizN(int Ur, int a) //низ ноги нижний уровень
{
	DrawNiz(Ur, a);
	glBegin(GL_TRIANGLES);
	glColor3d(0.4, 1.5, 1);
	glNormal3d(0, 0, -1);
	glTexCoord2d(0.1, 0.8); glVertex3dv(A1);
	glTexCoord2d(0.5, 0.5); glVertex3dv(A2);
	glTexCoord2d(0.1, 0.6); glVertex3dv(A10);
	glEnd();
}

void DrawPolCircle(void) //полукруглая стена
{
	int n = 120;
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n / 2; i++)
	{
		double A[] = { 3.5 + (sqrt(41) / 2) * sin(i * 2 * M_PI / n), 3 + (sqrt(41) / 2) * cos(i * 2 * M_PI / n), 0 };
		double B[] = { 3.5 + (sqrt(41) / 2) * sin(i * 2 * M_PI / n), 3 + (sqrt(41) / 2) * cos(i * 2 * M_PI / n), 2 };
		double C[] = { 3.5 + (sqrt(41) / 2) * sin((i++) * 2 * M_PI / n), 3 + (sqrt(41) / 2) * cos((i++) * 2 * M_PI / n), 0 };
		double n[3] = { 0,0,0 };
		normal(A, B, C, n);
		glNormal3d(n[0], n[1], n[2]);
		glColor3d(1, 0, 0);
		glVertex3dv(A);
		glColor3d(0.4, 1.5, 1);
		glVertex3dv(B);
	}
	glEnd();
}

void DrawVeer(int Urr, int alf) //веер у полукруглой стены
{
	int n = 60;
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= n / 2; i++)
	{
		if (alf == 0) { glColor3d(0.4, 1.5, 1); }
		if (Urr > 0) { glNormal3d(0, 0, 1); }
		else { glNormal3d(0, 0, -1); }
		glVertex3d(3.5 + (sqrt(41) / 2) * sin(i * 2 * M_PI / n), 3 + (sqrt(41) / 2) * cos(i * 2 * M_PI / n), Urr);
		glVertex3d(3.5, 3, Urr);
	}
	glEnd();
}

void Veer(int Ur, int a) //поворот веера
{
	glPushMatrix();
	glTranslated(3.5, 3, 0);
	glRotated(231.34, 0, 0, 5);
	glTranslated(-3.5, -3, 0);
	DrawVeer(Ur, a);
	glPopMatrix();
}



void Render(OpenGL *ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  
	int vkl = 0;  //проверка на включенное альфаналожение
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	double n[3] = { 0,0,0 };

	glBegin(GL_QUADS);
	glColor3d(0.4, 1.5, 1);

	//glVertex3dv(A1); //нога1 низ
	//glVertex3dv(A2);
	//glVertex3dv(A9);
	//glVertex3dv(A10);

	glNormal3d(0, 0, -1);
	glTexCoord2d(0.5, 0.5); glVertex3dv(A2); //нога2 низ
	glTexCoord2d(0.5, 0.3); glVertex3dv(A9);
	glTexCoord2d(0.8, 0.4); glVertex3dv(A8);
	glTexCoord2d(0.8, 0.6); glVertex3dv(A3);

	//glVertex3dv(B1); //нога1 верх
	//glVertex3dv(B2);
	//glVertex3dv(B9);
	//glVertex3dv(B10);

	glEnd();

	glBegin(GL_QUADS); //боковина фигуры, нога

	normal(A1, B1, B2, n);
	glNormal3d(n[0], n[1], n[2]);
	glColor3d(1, 0, 0);
	glTexCoord2d(0, 0); glVertex3dv(A1);
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0, 0.2); glVertex3dv(B1);
	glTexCoord2d(0.5, 0.2); glVertex3dv(B2);
	glColor3d(1, 0, 0);
	glTexCoord2d(0.5, 0); glVertex3dv(A2);

	normal(A2, B2, B3, n);
	glNormal3d(n[0], n[1], n[2]);
	glTexCoord2d(0.5, 0); glVertex3dv(A2);   //3.1622776601683795 длинна отрезка
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0.5, 0.2); glVertex3dv(B2);
	glTexCoord2d(0.8162278, 0.2); glVertex3dv(B3);
	glColor3d(1, 0, 0);
	glTexCoord2d(0.8162278, 0); glVertex3dv(A3);

	normal(A8, B8, B9, n);
	glNormal3d(n[0], n[1], n[2]); //3.1622776601683795 длинна отрезка
	glTexCoord2d(0.5, 0); glVertex3dv(A8);
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0.5, 0.2); glVertex3dv(B8);
	glTexCoord2d(0.8162278, 0.2); glVertex3dv(B9);
	glColor3d(1, 0, 0);
	glTexCoord2d(0.8162278, 0); glVertex3dv(A9);

	//стена где появидась вогнутость
	/*glVertex3dv(A9);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B9);
	glVertex3dv(B10);
	glColor3d(1, 0, 0);
	glVertex3dv(A10);*/

	normal(A10, B10, B1, n);
	glNormal3d(n[0], n[1], n[2]);
	glTexCoord2d(0.3, 0); glVertex3dv(A10);
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0.3, 0.2); glVertex3dv(B10);
	glTexCoord2d(0, 0.2); glVertex3dv(B1);
	glColor3d(1, 0, 0);
	glTexCoord2d(0, 0); glVertex3dv(A1);
	glEnd();

	//сторона фигуры - вогнутость
	glPushMatrix();
	glTranslated(x, y, 0);
	glRotated(161.565, 0, 0, 5);
	glTranslated(-x, -y, 0);
	DrawVogCircle(x, y, kor);
	glPopMatrix();

	NizN(0, vkl);

	glBindTexture(GL_TEXTURE_2D, texId[1]);

	glNormal3d(0, 0, -1);
	glBegin(GL_TRIANGLE_STRIP); //шляпа низ
	glTexCoord2d(0.5, 0); glVertex3dv(A7);
	glTexCoord2d(1, 0.3); glVertex3dv(A6);
	glTexCoord2d(0.8, 0.4); glVertex3dv(A8);
	glTexCoord2d(1, 0.6); glVertex3dv(A5);
	glTexCoord2d(0.8, 0.6); glVertex3dv(A3);
	glTexCoord2d(0.5, 1); glVertex3dv(A4);
	glEnd();

	
	glBegin(GL_QUADS); //боковина фигуры, шляпка

	normal(A3, B3, B4, n);
	glNormal3d(n[0], n[1], n[2]);
	glTexCoord2d(0, 0); glVertex3dv(A3);
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0, 0.2); glVertex3dv(B3);
	glTexCoord2d(0.5, 0.2); glVertex3dv(B4);
	glColor3d(1, 0, 0);
	glTexCoord2d(0.5, 0); glVertex3dv(A4);

	//стена где появидась выпуклость
	/*glVertex3dv(A4);
	glVertex3dv(B4);
	glVertex3dv(B5);
	glVertex3dv(A5);*/

	normal(A5, B5, B6, n);
	glNormal3d(n[0], n[1], n[2]);
	glTexCoord2d(0, 0); glVertex3dv(A5);
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0, 0.2); glVertex3dv(B5);
	glTexCoord2d(0.3, 0.2); glVertex3dv(B6);
	glColor3d(1, 0, 0);
	glTexCoord2d(0.3, 0); glVertex3dv(A6);

	normal(A6, B6, B7, n);
	glNormal3d(n[0], n[1], n[2]);   //5.830951894845301
	glTexCoord2d(0.3, 0); glVertex3dv(A6);
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0.3, 0.2); glVertex3dv(B6);
	glTexCoord2d(0.8830952, 0.2); glVertex3dv(B7);
	glColor3d(1, 0, 0);
	glTexCoord2d(0.8830952, 0); glVertex3dv(A7);

	normal(A7, B7, B8, n);
	glNormal3d(n[0], n[1], n[2]);
	glTexCoord2d(0, 0); glVertex3dv(A7);
	glColor3d(0.4, 1.5, 1);
	glTexCoord2d(0, 0.2); glVertex3dv(B7);
	glTexCoord2d(0.5, 0.2); glVertex3dv(B8);
	glColor3d(1, 0, 0);
	glTexCoord2d(0.5, 0); glVertex3dv(A8);

	glEnd();
	
	//сторона фигуры - выпуклость
	glPushMatrix();
	glTranslated(3.5, 3, 0);
	glRotated(231.34, 0, 0, 5);
	glTranslated(-3.5, -3, 0);
	DrawPolCircle();
	glPopMatrix();

	Veer(0, vkl); //веер низ
	

	glBegin(GL_LINES); //ось
	glColor3d(0, 0, 0);
	glVertex3dv(Mn);
	glVertex3dv(Mv);
	glEnd();

	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  vkl = 1;

	if (vkl == 1) { glColor4d(1, 0, 0, 0.7); }
	else glColor3d(0.4, 1.5, 1);

	glBindTexture(GL_TEXTURE_2D, texId[1]);

	glNormal3d(0, 0, 1);
	glBegin(GL_TRIANGLE_STRIP); //шляпа верх
	glTexCoord2d(0.5, 0); glVertex3dv(B7);
	glTexCoord2d(1, 0.3); glVertex3dv(B6);
	glTexCoord2d(0.8, 0.4); glVertex3dv(B8);
	glTexCoord2d(1, 0.6); glVertex3dv(B5);
	glTexCoord2d(0.8, 0.6); glVertex3dv(B3);
	glTexCoord2d(0.5, 1); glVertex3dv(B4);
	glEnd();

	Veer(2, vkl); //веер верх

	glBindTexture(GL_TEXTURE_2D, texId[0]);
	
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0.5, 0.5); glVertex3dv(B2); //нога2 верх
	glTexCoord2d(0.5, 0.3); glVertex3dv(B9);
	glTexCoord2d(0.8, 0.4); glVertex3dv(B8);
	glTexCoord2d(0.8, 0.6); glVertex3dv(B3);
	glEnd();
		
	NizV(2,vkl);



   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}