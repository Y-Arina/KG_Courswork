#pragma once
#include<corecrt_math.h>

//нахождение точки кривой Эрмита
void PointHermiteCurves(double P1[3], double P4[3], double R1[3], double R4[3], double t, double X[3])
{
	X[0] = P1[0] * (2 * pow(t, 3) - 3 * pow(t, 2) + 1) + P4[0] * (-2 * pow(t, 3) + 3 * pow(t, 2)) + R1[0] * (pow(t, 3) - 2 * pow(t, 2) + t) + R4[0] * (pow(t, 3) - pow(t, 2));
	X[1] = P1[1] * (2 * pow(t, 3) - 3 * pow(t, 2) + 1) + P4[1] * (-2 * pow(t, 3) + 3 * pow(t, 2)) + R1[1] * (pow(t, 3) - 2 * pow(t, 2) + t) + R4[1] * (pow(t, 3) - pow(t, 2));
	X[2] = P1[2] * (2 * pow(t, 3) - 3 * pow(t, 2) + 1) + P4[2] * (-2 * pow(t, 3) + 3 * pow(t, 2)) + R1[2] * (pow(t, 3) - 2 * pow(t, 2) + t) + R4[2] * (pow(t, 3) - pow(t, 2));
}

//нахождение векторов касательных для кривой Эрмита
void TangentVectorHermite(double P1[3], double P2[3], double P3[3], double P4[3], double R1[3], double R4[3])
{
	//нахождение касательной в первой точке (при т=0)
	R1[0] = 3 * (P2[0] - P1[0]);
	R1[1] = 3 * (P2[1] - P1[1]);
	R1[2] = 3 * (P2[2] - P1[2]);

	//нахождение касательной в последней точке (при т=1)
	R4[0] = 3 * (P4[0] - P3[0]);
	R4[1] = 3 * (P4[1] - P3[1]);
	R4[2] = 3 * (P4[2] - P3[2]);
}

//рисование кривой Эрмита и векторов в точках т=0 и т=1
void DrawingHermiteCurve(double P1[3], double P4[3], double R1[3], double R4[3])
{
	double X[3];

	glBegin(GL_LINE_STRIP);
	glColor3f(0.5f, 1.0f, 1.0f);
	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		PointHermiteCurves(P1, P4, R1, R4, t, X);
		glVertex3dv(X);
	}
	glEnd();

	glColor3f(1.0f, 0.5f, 0.0f);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3dv(P1);
	glVertex3dv(P4);
	glEnd();

	glColor3f(2.0f, 0.5f, 1.0f);
	glBegin(GL_LINES);
	glVertex3dv(P1);
	glVertex3dv(R1);

	glVertex3dv(P4);
	glVertex3dv(R4);
	glEnd();
}

//вызов ф-ий для кривой Эрмита
void HermiteCurves(double P1[3], double P2[3], double P3[3], double P4[3])
{
	//касательные кривой Эрмита
	double R1[3];
	double R4[3];

	TangentVectorHermite(P1, P2, P3, P4, R1, R4);
	DrawingHermiteCurve(P1, P4, R1, R4);
}