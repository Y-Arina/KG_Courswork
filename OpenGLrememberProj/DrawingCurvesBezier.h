#pragma once
#include<corecrt_math.h>

//нахождение точки кривой Безье
void PointBezierCurve(double P1[3], double P2[3], double P3[3], double P4[3], double t, double X[3])
{
	X[0] = pow(1 - t, 3) * P1[0] + 3 * t * pow(1 - t, 2) * P2[0] + 3 * pow(t, 2) * (1 - t) * P3[0] + pow(t, 3) * P4[0];
	X[1] = pow(1 - t, 3) * P1[1] + 3 * t * pow(1 - t, 2) * P2[1] + 3 * pow(t, 2) * (1 - t) * P3[1] + pow(t, 3) * P4[1];
	X[2] = pow(1 - t, 3) * P1[2] + 3 * t * pow(1 - t, 2) * P2[2] + 3 * pow(t, 2) * (1 - t) * P3[2] + pow(t, 3) * P4[2];
}

//рисование кривой Безье
void DrawingBezierCurve(double P1[3], double P2[3], double P3[3], double P4[3], int k)
{
	double X[3];

	glBegin(GL_LINE_STRIP);
	glColor3f(0.5f, 1.0f, 1.0f);
	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		PointBezierCurve(P1, P2, P3, P4, t, X);
		glVertex3dv(X);
	}
	glEnd();

	glColor3f(2.0f, 0.5f, 1.0f);
	if (k == 1)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3dv(P1);
		glVertex3dv(P2);
		glVertex3dv(P4);
		glVertex3dv(P3);
		glEnd();
	}
	else
	{
		glBegin(GL_LINE_STRIP);
		glVertex3dv(P1);
		glVertex3dv(P2);
		glVertex3dv(P3);
		glVertex3dv(P4);
		glEnd();
	}
}
//вызов ф-ий для кривой Безье
void BezierCurves(double P1[3], double P2[3], double P3[3], double P4[3], int k)
{
	DrawingBezierCurve(P1, P2, P3, P4, k);
}
