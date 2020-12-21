#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>

#include <random>
#include <iostream>
#include <vector>
#include <math.h>

#include "ObjLoader.h"
#include "Texture.h"
#include "Point.h"
#include "DrawBezierAndHermit.h"
#include "MyOGL.h"

using namespace std;

extern ObjFile BigBallObj, cubeMaxObj, cubeMinObj, manObj, planetObj, platformObj, towerObj;
extern Texture manTex, planetTex, towerTex, grassTex;

//����� ���������� �����
extern double TimeLastFrame;
//�������� ���, ���� ����� ������ ����
extern bool showCubeMax;
//�������� ������ �������� ����
extern bool showActiveZone;
//�������� ������� ���
extern bool showCubeMin;
//����� �������
extern int TotalBall;
//���� �������� �������
extern int BallReleased;
//����� �������
extern int BallDestroyed;
//���������� ����������� ������� ��� ������ ��� ����
extern int BallMissed;
//�������� ����� � ��������
extern bool changeColor;
//�������������
extern bool Restart;

//�������� �������� ����
void Color_4d(float color[4]) {
	color[0] = (float)((rand() % 999) + 1.0) / 1000;
	color[1] = (float)((rand() % 999) + 1.0) / 1000;
	color[2] = (float)((rand() % 999) + 1.0) / 1000;
	color[3] = (float)((rand() % 999) + 1.0) / 1000;
}

//�������� �������� ����
void Color_3d(float color[3]) {
	color[0] = (float)((rand() % 999) + 1.0) / 1000;
	color[1] = (float)((rand() % 999) + 1.0) / 1000;
	color[2] = (float)((rand() % 999) + 1.0) / 1000;
}

//�������� �������� ����
vector<float> Color_4d() {
	vector<float> color = {
		(float)((rand() % 999) + 1.0) / 1000,
		(float)((rand() % 999) + 1.0) / 1000,
		(float)((rand() % 999) + 1.0) / 1000,
		(float)((rand() % 999) + 1.0) / 1000
	};
	return color;
}

//�������� �������� ����
vector<float> Color_3d() {
	vector<float> color = {
		(float)((rand() % 999) + 1.0) / 1000,
		(float)((rand() % 999) + 1.0) / 1000,
		(float)((rand() % 999) + 1.0) / 1000
	};
	return color;
}

//������
double random(int a, int b)
{
	std::random_device random_device; // �������� ��������.
	std::mt19937 generator(random_device()); // ��������� ��������� �����.
	// (����� ������� ���� ���������������� ��������, ����� ����� ������)

	std::uniform_int_distribution<> distribution(a, b); // ����������� ������������� [a, b]

	return distribution(generator); // ��������� �����.
}

//������
double random(double a, double b)
{
	std::random_device random_device; // �������� ��������.
	std::mt19937 generator(random_device()); // ��������� ��������� �����.
	// (����� ������� ���� ���������������� ��������, ����� ����� ������)

	std::uniform_int_distribution<> distribution(a, b); // ����������� ������������� [a, b]

	return distribution(generator); // ��������� �����.
}

//���������� �������� ���� (� ���� ������ �������� �������)
double CubeMax_a = 5;
Point CubeMax_O = Point(20, 0, 10);

//���������� ���� �������� (������� ����� ����, � ����� ����� ������ ����� ����������� ��� �������� ���)
double CubeMin_a = 3;
Point CubeMin_O = Point(CubeMax_O);

Point A1 = Point(CubeMax_O.x + CubeMin_a, CubeMax_O.y - CubeMin_a, CubeMax_O.z - CubeMin_a);
Point A2 = Point(CubeMax_O.x - CubeMin_a, CubeMax_O.y + CubeMin_a, CubeMax_O.z - CubeMin_a);
Point A3 = Point(CubeMax_O.x - CubeMin_a, CubeMax_O.y - CubeMin_a, CubeMax_O.z - CubeMin_a);
Point A4 = Point(CubeMax_O.x + CubeMin_a, CubeMax_O.y + CubeMin_a, CubeMax_O.z - CubeMin_a);

Point A5 = Point(CubeMax_O.x + CubeMin_a, CubeMax_O.y - CubeMin_a, CubeMax_O.z + CubeMin_a);
Point A6 = Point(CubeMax_O.x - CubeMin_a, CubeMax_O.y + CubeMin_a, CubeMax_O.z + CubeMin_a);
Point A7 = Point(CubeMax_O.x - CubeMin_a, CubeMax_O.y - CubeMin_a, CubeMax_O.z + CubeMin_a);
Point A8 = Point(CubeMax_O.x + CubeMin_a, CubeMax_O.y + CubeMin_a, CubeMax_O.z + CubeMin_a);

vector<Point> All_Vertex = { A1, A2, A3, A4, A5, A6, A7, A8, CubeMin_O };

//������� ������� ����� 3d
void SetColor3d(vector<float> color) {
	glColor3d(color[0], color[1], color[2]);
}

//������� ������� ����� 4d
void SetColor4d(vector<float> color) {
	glColor4d(color[0], color[1], color[2], color[3]);
}

//������ ���, ���� ����� ������ ��� ����
void DrawCubeMax() {
	//��������� ����
	glDisable(GL_LIGHTING);
	//��������� ��������
	glDisable(GL_TEXTURE_2D);
	//�������� ����� ����������
	glEnable(GL_BLEND);
	//������ ����� ��� ������������� ��������� � ���������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- ������ ����� ---
	static vector<float> color3d_CubeMax = Color_3d();
	static vector<float> color4d_CubeMax = Color_4d();

	if (changeColor) {
		color3d_CubeMax = Color_3d();
		color4d_CubeMax = Color_4d();
	}

	glPushMatrix();

	CubeMax_O.Translated();

	SetColor3d(color3d_CubeMax);
	SetColor4d(color4d_CubeMax);
	cubeMaxObj.DrawObj();

	glPopMatrix();

	//��������� ����� ����������
	glDisable(GL_BLEND);
	//�������� ����
	glEnable(GL_LIGHTING);
	//�������� ��������
	glEnable(GL_TEXTURE_2D);
}

//������ ��� ��� �������� ���
void DrawCubeMin() {
	//��������� ����
	glDisable(GL_LIGHTING);
	//��������� ��������
	glDisable(GL_TEXTURE_2D);
	//�������� ����� ����������
	glEnable(GL_BLEND);
	//������ ����� ��� ������������� ��������� � ���������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- ������ ����� ---
	static vector<float> color3d_CubeMin = Color_3d();
	static vector<float> color4d_CubeMin = Color_4d();

	if (changeColor) {
		color3d_CubeMin = Color_3d();
		color4d_CubeMin = Color_4d();
	}

	glPushMatrix();

	CubeMax_O.Translated();

	SetColor3d(color3d_CubeMin);
	SetColor4d(color4d_CubeMin);
	cubeMinObj.DrawObj();

	glPopMatrix();

	//��������� ����� ����������
	glDisable(GL_BLEND);
	//�������� ����
	glEnable(GL_LIGHTING);
	//�������� ��������
	glEnable(GL_TEXTURE_2D);
}

//������ �������� ����
void DrawActiveZone() {
	//��������� ����
	glDisable(GL_LIGHTING);
	//��������� ��������
	glDisable(GL_TEXTURE_2D);
	//�������� ����� ����������
	glEnable(GL_BLEND);
	//������ ����� ��� ������������� ��������� � ���������
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- ������ ����� �������� ����� ---
	static vector<vector<float>> color3d_ActiveZones = vector<vector<float>>();
	static vector<vector<float>> color4d_ActiveZones = vector<vector<float>>();
	static bool Color_ActiveZone = true;

	if (Color_ActiveZone || changeColor) {
		color3d_ActiveZones.clear();
		color4d_ActiveZones.clear();
		for (int i = 0; i < 9; i++) {
			color3d_ActiveZones.push_back(Color_3d());
			color4d_ActiveZones.push_back(Color_4d());
		}
		Color_ActiveZone = false;
	}

	// --- ������ �������� ���� ---
	for (int i = 0; i < (int)All_Vertex.size(); i++) {
		glPushMatrix();
		All_Vertex[i].Translated();
		SetColor3d(color3d_ActiveZones[i]);
		SetColor4d(color4d_ActiveZones[i]);
		BigBallObj.DrawObj();
		glPopMatrix();
	}

	//��������� ����� ����������
	glDisable(GL_BLEND);
	//�������� ����
	glEnable(GL_LIGHTING);
	//�������� ��������
	glEnable(GL_TEXTURE_2D);
}

//���������� �����
double tower_a = 15;
double tower_b = 15;
Point H1 = Point(CubeMax_O.x - tower_a, CubeMax_O.y - tower_b, 0);
Point H2 = Point(CubeMax_O.x - tower_a, CubeMax_O.y + tower_b, 0);
Point H3 = Point(CubeMax_O.x + tower_a, CubeMax_O.y - tower_b, 0);
Point H4 = Point(CubeMax_O.x + tower_a, CubeMax_O.y + tower_b, 0);

vector<Point> PositionTower = { H1, H2, H3, H4 };

//������ �����
void DrawTower() {
	for (int i = 0; i < (int)PositionTower.size(); i++) {

		// --- ������ �����---

		glPushMatrix();

		PositionTower[i].Translated();
		towerTex.bindTexture();
		towerObj.DrawObj();

		glPopMatrix();

		// --- ������ �����---

		glPushMatrix();

		PositionTower[i].Translated();
		glScaled(3, 3, 3);
		grassTex.bindTexture();
		platformObj.DrawObj();

		glPopMatrix();

	}
}


//������ � ��� ���������
int random_Tower(int i) {
	int tower = 0;
	while (true) {
		tower = random(1, 4);
		if (tower != i) {
			return tower;
		}
	}
}

vector<vector<Point>> PointsBezier = vector<vector<Point>>();
vector<bool> StartBall = vector<bool>();
vector<bool> DestroyedBall = vector<bool>();

//����� �������, ��� ��� ���� �����
int number_active = 0;
//���� �����, ��� ��� ���� �����
vector<float> color3d_active = { 1.0, 0., 0. };
vector<float> color4d_active = { 1.0, 0., 0., 1 };

//�����������, ���� ���� ������ ������ ���� ������
bool listener_one() {
	int i = 0;

	if (OpenGL::isKeyPressed('1')) i++;
	if (OpenGL::isKeyPressed('2')) i++;
	if (OpenGL::isKeyPressed('3')) i++;
	if (OpenGL::isKeyPressed('4')) i++;
	if (OpenGL::isKeyPressed('5')) i++;
	if (OpenGL::isKeyPressed('6')) i++;
	if (OpenGL::isKeyPressed('7')) i++; 
	if (OpenGL::isKeyPressed('8')) i++;
	if (OpenGL::isKeyPressed('9')) i++;

	if (i == 1 && !OpenGL::isKeyPressed('B')) {
		return true;
	}
	return false;
}

//������ ����
double Sphere_R = 3;
//������ ������
double ball_R = 0.2;

//�������� ����� �� ����� ������ �����
bool PointInsideSphere(Point& O, Point& A) {
	static double r = pow(Sphere_R, 2);
	double d = pow(A.x - O.x, 2) + pow(A.y - O.y, 2) + pow(A.z - O.z, 2);

	//����� ����� ������ �����
	if (r >= d) {
		return true;
	}
	else {
		vector<Point> Q = vector<Point>();
		Q.push_back(Point(A.x, A.y, A.z + ball_R));
		Q.push_back(Point(A.x, A.y, A.z - ball_R));
		Q.push_back(Point(A.x + ball_R, A.y, A.z));
		Q.push_back(Point(A.x - ball_R, A.y, A.z));
		Q.push_back(Point(A.x, A.y + ball_R, A.z));
		Q.push_back(Point(A.x, A.y - ball_R, A.z));

		for (int i = 0; i < (int)Q.size(); i++) {
			d = pow(Q[i].x - O.x, 2) + pow(Q[i].y - O.y, 2) + pow(Q[i].z - O.z, 2);
			if (r >= d) {
				return true;
			}
		}
	}

	//����� ����� ��� �����
	return false;
}

//������� ������
int listener(Point point) {
	if (OpenGL::isKeyPressed('1')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A1))
			{
				return 1;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('2')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A2))
			{
				return 2;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('3')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A3))
			{
				return 3;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('4')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A4))
			{
				return 4;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('5')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A5))
			{
				return 5;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('6')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A6))
			{
				return 6;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('7')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A7))
			{
				return 7;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('8')) {
		if (listener_one()) {
			if (PointInsideSphere(point, A8))
			{
				return 8;
			}
			return 0;
		}
	}

	if (OpenGL::isKeyPressed('9')) {
		if (listener_one()) {
			if (PointInsideSphere(point, CubeMin_O))
			{
				return 9;
			}
			return 0;
		}
	}
	return 0;
}

//�������� ������
int Vertex�hecking() {
	if (OpenGL::isKeyPressed('1') && OpenGL::isKeyPressed('B')) return 1;
	if (OpenGL::isKeyPressed('2') && OpenGL::isKeyPressed('B')) return 2;
	if (OpenGL::isKeyPressed('3') && OpenGL::isKeyPressed('B')) return 3;
	if (OpenGL::isKeyPressed('4') && OpenGL::isKeyPressed('B')) return 4;
	if (OpenGL::isKeyPressed('5') && OpenGL::isKeyPressed('B')) return 5;
	if (OpenGL::isKeyPressed('6') && OpenGL::isKeyPressed('B')) return 6;
	if (OpenGL::isKeyPressed('7') && OpenGL::isKeyPressed('B')) return 7;
	if (OpenGL::isKeyPressed('8') && OpenGL::isKeyPressed('B')) return 8;
	if (OpenGL::isKeyPressed('9') && OpenGL::isKeyPressed('B')) return 9;

	return 0;
}


//��� �����������
double myorg = 0.2;

//������� �������
void Play() {
	static double t_max = 0;
	static double t_max_next = 0;
	static bool flag_tmax = true;

	//�������������� ��������� ���������� �������
	static bool start = true;
	if (start || Restart) {
		//������� ������ ������
		PointsBezier.clear();
		StartBall.clear();
		DestroyedBall.clear();

		//���� �������� �������
		BallReleased = TotalBall;
		//����� �������
		BallDestroyed = 0;
		//���������� ����������� ������� ��� ������ ��� ����
		BallMissed = 0;

		for (int i = 0; i < TotalBall; i++) {
			int a = random(1, 4);
			int b = random_Tower(a);
			int min_z = 0;
			int max_z = 20;
			vector<Point> PointBezier = vector<Point>();
			Point P1, P2, P3, P4;
			switch (a) {
			case 1:
				P1 = { H1.x, H1.y, random(min_z, max_z) };
				break;
			case 2:
				P1 = { H2.x, H2.y, random(min_z, max_z) };
				break;
			case 3:
				P1 = { H3.x, H3.y, random(min_z, max_z) };
				break;
			case 4:
				P1 = { H4.x, H4.y, random(min_z, max_z) };
				break;
			}
			P2 = {
					random(CubeMax_O.x - CubeMax_a + myorg, CubeMax_O.x + CubeMax_a - myorg),
					random(CubeMax_O.y - CubeMax_a + myorg, CubeMax_O.y + CubeMax_a - myorg),
					random(CubeMax_O.z - CubeMax_a + myorg, CubeMax_O.z + CubeMax_a - myorg)
			};
			P3 = {
					random(CubeMax_O.x - CubeMax_a + myorg, CubeMax_O.x + CubeMax_a - myorg),
					random(CubeMax_O.y - CubeMax_a + myorg, CubeMax_O.y + CubeMax_a - myorg),
					random(CubeMax_O.z - CubeMax_a + myorg, CubeMax_O.z + CubeMax_a - myorg)
			};
			P4 = { PositionTower[b - 1].x, PositionTower[b - 1].y, random(min_z, max_z) };

			PointBezier.push_back(P1);
			PointBezier.push_back(P2);
			PointBezier.push_back(P3);
			PointBezier.push_back(P4);

			PointsBezier.push_back(PointBezier);
			StartBall.push_back(false);
			DestroyedBall.push_back(false);
		}

		start = false;
	}

	//��������� �������
	double delta_time = Search_delta_time();
	double go = delta_time / 5; //t_max ���������� = 1 �� 5 ������
	//t_max ���� �� ���� ���������� �� 0 �� 1 ���������� �� ����� � �����
	if (flag_tmax) {
		t_max += go;
		t_max_next = t_max + go;
		if (t_max > 1) {
			t_max = 1;
			flag_tmax = false;
		}
		if (t_max_next > 1) {
			t_max_next = 1;
		}
	}
	else {
		//t_max = 0;
		//t_max_next = 0;
		//flag_tmax = true;
		t_max -= go;
		t_max_next = t_max - go;
		if (t_max < 0) {
			t_max = 0;
			flag_tmax = true;
		}
		if (t_max_next < 0) {
			t_max_next = 0;
		}
	}

	//�� ���� ��� ����� ����� ������ ���� ���
	bool active = false;
	int MainTop = 0;

	//������ �������� �������
	for (int i = 0; i < TotalBall; i++) {
		//���� ��� �� ���������
		if (!DestroyedBall[i]) {
			glPushMatrix();

			Point point = BezierCurve3(PointsBezier[i][0], PointsBezier[i][1], PointsBezier[i][2], PointsBezier[i][3], t_max);
			Point next_point = BezierCurve3(PointsBezier[i][0], PointsBezier[i][1], PointsBezier[i][2], PointsBezier[i][3], t_max_next);
			point.Translated();
			planetTex.bindTexture();
			planetObj.DrawObj();

			glPopMatrix();

			int top = listener(point);
			if (top != 0 && !active) {
				//����� ���������
				DestroyedBall[i] = true;
				active = true;
				MainTop = top;

				//����������� ������� ������ �������
				BallDestroyed++;
			}
		}
	}

	//���������� ���� ������� ����
	if (MainTop != 0) {

		//��������� ����
		glDisable(GL_LIGHTING);
		//��������� ��������
		glDisable(GL_TEXTURE_2D);
		//�������� ����� ����������
		glEnable(GL_BLEND);
		//������ ����� ��� ������������� ��������� � ���������
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		SetColor3d(color3d_active);
		SetColor4d(color4d_active);
		All_Vertex[MainTop - 1].Translated();

		BigBallObj.DrawObj();

		glPopMatrix();

		//��������� ����� ����������
		glDisable(GL_BLEND);
		//�������� ����
		glEnable(GL_LIGHTING);
		//�������� ��������
		glEnable(GL_TEXTURE_2D);

	}

	int check = Vertex�hecking();
	if (check != 0) {
		//��������� ����
		glDisable(GL_LIGHTING);
		//��������� ��������
		glDisable(GL_TEXTURE_2D);
		//�������� ����� ����������
		glEnable(GL_BLEND);
		//������ ����� ��� ������������� ��������� � ���������
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		SetColor3d(color3d_active);
		SetColor4d(color4d_active);
		All_Vertex[check - 1].Translated();

		BigBallObj.DrawObj();

		glPopMatrix();

		//��������� ����� ����������
		glDisable(GL_BLEND);
		//�������� ����
		glEnable(GL_LIGHTING);
		//�������� ��������
		glEnable(GL_TEXTURE_2D);
	}
}