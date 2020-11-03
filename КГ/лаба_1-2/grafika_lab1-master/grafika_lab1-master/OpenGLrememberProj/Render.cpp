#include "Render.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

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

void DrawVogCircle(double x,double y,double kor) //вогнутая стана
{
	int n = 60;
	glBegin(GL_QUAD_STRIP);
	for (float i = 0; i <= 6.2; i=i+0.05)
	{	
		glColor3d(1, 0, 0);
		glVertex3d(x + kor * sin(i * 2 * M_PI / n), y + kor * cos(i * 2 * M_PI / n), 0);
		glColor3d(0.4, 1.5, 1);
		glVertex3d(x + kor * sin(i * 2 * M_PI / n), y + kor * cos(i * 2 * M_PI / n), 2);
	}
	glEnd();
}

void DrawNiz(int Urr) //веер у вогнутой стены
{
	int n = 60;
	glBegin(GL_TRIANGLE_STRIP);
	for (float i = 3.05; i <= 9.3; i=i+0.05)
	{
		glColor3d(0.4, 1.5, 1);
		glVertex3d(6, 6, Urr);
		glVertex3d(x - kor * sin(i * 2 * M_PI / n), y - kor * cos(i * 2 * M_PI / n), Urr);
	}
	glEnd();
}

void NizV(int Ur) //низ ноги крыша
{
	DrawNiz(Ur);
	glBegin(GL_TRIANGLES);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B1);
	glVertex3dv(B2);
	glVertex3dv(B10);
	glEnd();
}

void NizN(int Ur) //низ ноги основа
{
	DrawNiz(Ur);
	glBegin(GL_TRIANGLES);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(A1);
	glVertex3dv(A2);
	glVertex3dv(A10);
	glEnd();
}

void DrawPolCircle(void) //полукруглая стена
{	
	int n = 60;
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n/2; i++)
	{
		glColor3d(1, 0, 0);
		glVertex3d(3.5 + (sqrt(41) / 2) * sin(i * 2 * M_PI / n), 3 + (sqrt(41) / 2) * cos(i * 2 * M_PI / n), 0);
		glColor3d(0.4, 1.5, 1);
		glVertex3d(3.5 + (sqrt(41) / 2) * sin(i * 2 * M_PI / n), 3 + (sqrt(41) / 2) * cos(i * 2 * M_PI / n), 2);
	}
	glEnd();
}

void DrawVeer(int Urr) //веера у полукруглой стены
{
	int n = 60;
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= n / 2; i++)
	{
		glColor3d(0.4, 1.5, 1);
		glVertex3d(3.5 + (sqrt(41) / 2) * sin(i * 2 * M_PI / n), 3 + (sqrt(41) / 2) * cos(i * 2 * M_PI / n), Urr);
		glVertex3d(3.5, 3, Urr);
	}
	glEnd();
}

void Veer(int Ur) //поворот веера
{
	glPushMatrix();
	glTranslated(3.5, 3, 0);
	glRotated(231.34, 0, 0, 5);
	glTranslated(-3.5, -3, 0);
	DrawVeer(Ur);
	glPopMatrix();
}

void Render(double delta_time)
{	glBegin(GL_QUADS);
	glColor3d(0.4, 1.5, 1);

	//glVertex3dv(A1); //нога1 низ
	//glVertex3dv(A2);
	//glVertex3dv(A9);
	//glVertex3dv(A10);

	glVertex3dv(A2); //нога2 низ
	glVertex3dv(A9);
	glVertex3dv(A8);
	glVertex3dv(A3);

	//glVertex3dv(B1); //нога1 верх
	//glVertex3dv(B2);
	//glVertex3dv(B9);
	//glVertex3dv(B10);

	glVertex3dv(B2); //нога2 верх
	glVertex3dv(B9);
	glVertex3dv(B8);
	glVertex3dv(B3);

	glEnd();
	glBegin(GL_TRIANGLE_STRIP); //шляпа низ
	glVertex3dv(A7);
	glVertex3dv(A6);
	glVertex3dv(A8);
	glVertex3dv(A5);
	glVertex3dv(A3);
	glVertex3dv(A4);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP); //шляпа верх
	glVertex3dv(B7);
	glVertex3dv(B6);
	glVertex3dv(B8);
	glVertex3dv(B5);
	glVertex3dv(B3);
	glVertex3dv(B4);
	glEnd();

	glBegin(GL_QUADS); //везде боковина фигуры
	glColor3d(1, 0, 0);
	glVertex3dv(A1);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B1);
	glVertex3dv(B2);
	glColor3d(1, 0, 0);
	glVertex3dv(A2);

	glVertex3dv(A2);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B2);
	glVertex3dv(B3);
	glColor3d(1, 0, 0);
	glVertex3dv(A3);

	glVertex3dv(A3);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B3);
	glVertex3dv(B4);
	glColor3d(1, 0, 0);
	glVertex3dv(A4);

	//стена где появидась выпуклость
	/*glVertex3dv(A4);   
	glVertex3dv(B4);
	glVertex3dv(B5);
	glVertex3dv(A5);*/

	glVertex3dv(A5);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B5);
	glVertex3dv(B6);
	glColor3d(1, 0, 0);
	glVertex3dv(A6);

	glVertex3dv(A6);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B6);
	glVertex3dv(B7);
	glColor3d(1, 0, 0);
	glVertex3dv(A7);

	glVertex3dv(A7);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B7);
	glVertex3dv(B8);
	glColor3d(1, 0, 0);
	glVertex3dv(A8);

	glVertex3dv(A8);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B8);
	glVertex3dv(B9);
	glColor3d(1, 0, 0);
	glVertex3dv(A9);

	//стена где появидась вогнутость
	/*glVertex3dv(A9);       
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B9);
	glVertex3dv(B10);
	glColor3d(1, 0, 0);
	glVertex3dv(A10);*/

	glVertex3dv(A10);
	glColor3d(0.4, 1.5, 1);
	glVertex3dv(B10);
	glVertex3dv(B1);
	glColor3d(1, 0, 0);
	glVertex3dv(A1);
	glEnd();
	
	//сторона фигуры - выпуклость
	glPushMatrix(); 
	glTranslated(3.5, 3, 0);
	glRotated(231.34, 0, 0, 5);
	glTranslated(-3.5, -3, 0);
	DrawPolCircle();
	glPopMatrix();

	Veer(0); //веер низ
	Veer(2); //веер верх

	//сторона фигуры - вогнутость
	glPushMatrix(); 
	glTranslated(x, y, 0);
	glRotated(161.565, 0, 0, 5);
	glTranslated(-x, -y, 0);
	DrawVogCircle(x, y, kor);
	glPopMatrix();

	NizV(2);
	NizN(0);

	glBegin(GL_LINES); //ось
	glColor3d(0, 0, 0);
	glVertex3dv(Mn);
	glVertex3dv(Mv);
	glEnd();
}
//----------------------------------------------------
//double A1[] = { 2,2, 0 };
//double A2[] = { -2,2, 0 };
//double A3[] = { -2,-2, 0 };
//double A4[] = { 2,-2, 0 };
//
//double B1[] = { 2,2, 2 };
//double B2[] = { -2,2,2 };
//double B3[] = { -2,-2,2 };
//double B4[] = { 2,-2,2 };
//
//double C[] = { 0,0,8 };

//void Render(double delta_time)
//{glBegin(GL_QUADS);
//
//glColor3d(0.4, 1.5, 1);
//
//glVertex3dv(A1);
//glVertex3dv(A2);
//glVertex3dv(A3);
//glVertex3dv(A4);
//
//glVertex3dv(A1);
//glVertex3dv(B1);
//glVertex3dv(B2);
//glVertex3dv(A2);
//
//glVertex3dv(A2);
//glVertex3dv(B2);
//glVertex3dv(B3);
//glVertex3dv(A3);
//
//glVertex3dv(A3);
//glVertex3dv(B3);
//glVertex3dv(B4);
//glVertex3dv(A4);
//
//glVertex3dv(A4);
//glVertex3dv(B4);
//glVertex3dv(B1);
//glVertex3dv(A1);
//
//glEnd();
//
//glBegin(GL_TRIANGLES);
//glColor3d(1.4, 1.5, 0);
//
//glVertex3dv(B1);
//glVertex3dv(C);
//glVertex3dv(B2);
//
//glVertex3dv(B2);
//glVertex3dv(C);
//glVertex3dv(B3);
//
//glVertex3dv(B3);
//glVertex3dv(C);
//glVertex3dv(B4);
//
//glVertex3dv(B4);
//glVertex3dv(C);
//glVertex3dv(B1);
//glEnd();
//}
