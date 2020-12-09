

#include "Render.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <math.h>
#include <vector>
#include "Vec3.h"

double f(double p1, double p2, double p3, double p4, double t)
{
	return p1 * (1 - t) * (1 - t) * (1 - t) + 3 * p2 * t * (1 - t) * (1 - t) + 3 * p3 * t * t * (1 - t) + p4 * t * t * t; //посчитанная формула
}

double t_max = 0;

double u(double t1, double t2, double v1, double v2, double t)
{
	return t1 * (2 * t * t * t - 3 * t * t + 1) + t2*(-2 * t * t * t + 3 * t * t) + v1*(t * t * t - 2 * t * t + t) + v2*(t * t * t - t * t); //посчитанная формула
}

double CalculateAngleOX(double StartVector[3], double EndVector[3])
{
	double Vector[3];
	Vector[0] = EndVector[0] - StartVector[0];
	Vector[1] = EndVector[1] - StartVector[1];
	Vector[2] = EndVector[2] - StartVector[2];
	if (Vector[0] == 0 && Vector[1] == 0 && Vector[2] == 0)
	{
		return 0;
	}
	double Angle;
	Angle = acos(Vector[0] / (sqrt(Vector[0] * Vector[0] + Vector[1] * Vector[1] + Vector[2] * Vector[2]))) * 180 / PI;
	return Angle;
}

double CalculateAngleOY(double StartVector[3], double EndVector[3])
{
	double Vector[3];
	Vector[0] = EndVector[0] - StartVector[0];
	Vector[1] = EndVector[1] - StartVector[1];
	Vector[2] = EndVector[2] - StartVector[2];
	if (Vector[0] == 0 && Vector[1] == 0 && Vector[2] == 0)
	{
		return 0;
	}
	double Angle;
	Angle = acos(Vector[1] / (sqrt(Vector[0] * Vector[0] + Vector[1] * Vector[1] + Vector[2] * Vector[2]))) * 180 / PI;
	return Angle;
}

double CalculateAngleOZ(double StartVector[3], double EndVector[3])
{
	double Vector[3];
	Vector[0] = EndVector[0] - StartVector[0];
	Vector[1] = EndVector[1] - StartVector[1];
	Vector[2] = EndVector[2] - StartVector[2];
	if (Vector[0] == 0 && Vector[1] == 0 && Vector[2] == 0)
	{
		return 0;
	}
	double Angle;
	Angle = acos(Vector[2] / (sqrt(Vector[0] * Vector[0] + Vector[1] * Vector[1] + Vector[2] * Vector[2]))) * 180 / PI;
	return Angle;
}

Vector3 bizeWithoutDraw(double P1[3], double P2[3], double P3[3], double P4[3], double t)
{
	Vector3 Vec;
	Vec.setCoords(f(P1[0], P2[0], P3[0], P4[0], t), f(P1[1], P2[1], P3[1], P4[1], t), f(P1[2], P2[2], P3[2], P4[2], t));
	return Vec;
}

void Draw_Figure(double P1[3])
{
	glBegin(GL_QUADS);

	glColor3d(0, 0, 0.5); //дно
	glVertex3d(P1[0], P1[1], P1[2]);
	glVertex3d(P1[0], P1[1] + 1, P1[2]);
	glVertex3d(P1[0], P1[1] + 1, P1[2] + 1);
	glVertex3d(P1[0], P1[1], P1[2] + 1);

	glEnd();

	glBegin(GL_TRIANGLES); //бока
	
	glColor3d(0.1, 0.1, 0.5); 
	glVertex3d(P1[0], P1[1], P1[2]);
	glVertex3d(P1[0] + 1, P1[1] + 0.5, P1[2] + 0.5);
	glVertex3d(P1[0], P1[1] + 1, P1[2]);

	glColor3d(0.5, 0.5, 0.5);
	glVertex3d(P1[0], P1[1], P1[2] + 1);
	glVertex3d(P1[0] + 1, P1[1] + 0.5, P1[2] + 0.5);
	glVertex3d(P1[0], P1[1] + 1, P1[2] + 1);

	glColor3d(0.3, 0.3, 0.5);
	glVertex3d(P1[0], P1[1], P1[2]);
	glVertex3d(P1[0] + 1, P1[1] + 0.5, P1[2] + 0.5);
	glVertex3d(P1[0], P1[1], P1[2] + 1);

	glVertex3d(P1[0], P1[1] + 1, P1[2]);
	glVertex3d(P1[0] + 1, P1[1] + 0.5, P1[2] + 0.5);
	glVertex3d(P1[0], P1[1] + 1, P1[2] + 1);
	glEnd();

	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0);

	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);

	glColor3d(0, 0, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glEnd();
}

void bezie(double t1[3], double t2[3], double t3[3], double t4[3])
{
	glColor3d(0, 191, 255);
	glLineWidth(4);
	double P[3];
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= t_max; t += 0.01)
	{
		P[0] = f(t4[0], t3[0], t2[0], t1[0], t);
		P[1] = f(t4[1], t3[1], t2[1], t1[1], t);
		P[2] = f(t4[2], t3[2], t2[2], t1[2], t);

		glVertex3dv(P); //Рисуем точку P
	}
	glEnd();
	glLineWidth(1);
}

void bezie_s_figur(double p1[3], double p2[3], double p3[3], double p4[3], double delta_time)
{
	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
		}
	}

	bezie(p1, p2, p3, p4); //безье1

	Vector3 P_old = bizeWithoutDraw(p1, p2, p3, p4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(p1, p2, p3, p4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	double A[] = { -0.5,-0.5,-0.5 };
	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	Draw_Figure(A);
	glPopMatrix();
}

void ermit(double t1[3], double t2[3], double t3[3], double t4[3])
{
	glColor3d(0, 255, 255);
	glLineWidth(4);
	double A[3];
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= t_max; t += 0.01)
	{
		A[0] = u(t1[0], t2[0], t3[0], t4[0], t);
		A[1] = u(t1[1], t2[1], t3[1], t4[1], t);
		A[2] = u(t1[2], t2[2], t3[2], t4[2], t);

		glVertex3dv(A); //Рисуем точку A
	}
	glEnd();
	glLineWidth(1);
}

void Render(double delta_time)
{
	//t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
	//if (t_max > 1) t_max = 0; //после обнуляется
	t_max = 1.001;
	double P1[] = { 0, 0, 0 }; //Наши точки для безье
	double P2[] = { 6, -6, 12 };
	double P3[] = { 1, 3, 8 };
	double P4[] = { 10, 0, 0 };
	double P5[] = { 16, -6, -12 };
	double P6[] = { 11, 3, -8 };
	double P7[] = { 20, 0, 0 };

	double A1[] = { 0, 5, 1 }; //Наши точки для эрмита
	double A2[] = { 1, 4, 10 };
	double A3[] = { 10, 6, 0 };
	double A4[] = { 3, 3, 3 };
	double A5[] = { -3, 0, 1 };
	double A6[] = { 4, 10, -3 };
	double V1[3];
	double V2[3];
	double V3[3];
	double V4[3];
	for (int i = 0; i <= 2; i++) //вектора
	{
		V1[i] = A2[i] - A1[i];
		V2[i] = A3[i] - A4[i];
		V3[i] = A5[i] - A6[i];
		V4[i] = A4[i] - A3[i];
	}

	glBegin(GL_LINE_STRIP); //построим отрезки для безье
	glVertex3dv(P1);
	glVertex3dv(P2);
	glVertex3dv(P3);
	glVertex3dv(P4);
	glVertex3dv(P5);
	glVertex3dv(P6);
	glVertex3dv(P7);
	glEnd();
	glLineWidth(1); //ширина линии
	

	//bezie(P1, P2, P3, P4); //безье1
	bezie_s_figur(P1, P2, P3, P4, delta_time); //безье1
	//bezie(P7, P6, P5, P4); //безье2
	bezie_s_figur(P7, P6, P5, P4, delta_time); //безье2

	glColor3d(1, 0.5, 0.128);
	glBegin(GL_LINES); //отрезки для эрмита
	glVertex3dv(A1);
	glVertex3dv(A2);
	glVertex3dv(A3);
	glVertex3dv(A4);
	glVertex3dv(A5);
	glVertex3dv(A6);
	glEnd();

		
	ermit(A1, A3, V1, V2); //эрмит1
	ermit(A6, A3, V3, V2); //эрмит2
		
//	glColor3d(1, 0, 1);
//	glLineWidth(1); //возвращаем ширину линии = 1
//	//нарисуем все точки
//	glPointSize(10);
//	glBegin(GL_POINTS);
//	glVertex3dv(P);
//	glEnd();
//	glColor3d(1, 0, 0);
//	glBegin(GL_POINTS);
//	glVertex3dv(P1);
//	glVertex3dv(P2);
//	glVertex3dv(P3);
//	glEnd();
}