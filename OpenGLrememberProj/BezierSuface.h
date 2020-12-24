#pragma once

//вычисление факториала 
int Factorial(int n)
{
	if (n < 0)
		return 0;
	if (n == 0)
		return 1;
	else
		return n * Factorial(n - 1);
}

//вычисление многочленов Бернштейна
double BernsteinPolynomial(int n, int i, double t)
{
	double res = (Factorial(n) / (Factorial(n - i) * Factorial(i))) * pow(t, i) * pow(1 - t, n - i);

	return res;
}

//вычисление точек поверхности
Point BezierSurfaceFinction(double u, double v, std::vector<std::vector<Point>> points)
{
	Point Res(0, 0, 0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			double PolyBeri = BernsteinPolynomial(3, i, u);
			double PolyBerj = BernsteinPolynomial(3, j, v);
			Res.x += PolyBeri * PolyBerj * points[i][j].x;
			Res.y += PolyBeri * PolyBerj * points[i][j].y;
			Res.z += PolyBeri * PolyBerj * points[i][j].z;
		}
	}
	return Res;
}

void BezierSuface(std::vector<std::vector<Point>> points, GLuint texId)
{
	//точки 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			points[i][j].PrintPoints();
		}
	}
	//линии
	glColor4f(1, 1, 0, 0);
	for (int i = 0; i < 4; i++)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < 4; j++)
		{
			points[i][j].Vertex3dv();
		}
		glEnd();
	}
	for (int i = 0; i < 4; i++)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < 4; j++)
		{
			points[j][i].Vertex3dv();
		}
		glEnd();
	}
	//поверхность 
	std::vector<Point> p;
	std::vector<std::vector<Point>> Massive;
	//нахождение точек поверхности 
	for (double u = 0; u <= 1.0001; u += 0.1)
	{
		for (double v = 0; v <= 1.0001; v += 0.1)
		{
			p.push_back(BezierSurfaceFinction(u, v,points));
		}
		Massive.push_back(p);
		p.clear();
	}
	//построение поверхности, вычисление нормалей и наложение на него текстур
	glBindTexture(GL_TEXTURE_2D, texId);

	glBegin(GL_TEXTURE_2D);

	for (int i = 0; i < Massive.size() - 1; i++)
	{
		for (int j = 0; j < Massive[i].size() - 1; j++)
		{
			Point A = Massive[i][j];
			Point B = Massive[i][j + 1];
			Point C = Massive[i + 1][j + 1];
			Point D = Massive[i + 1][j];

			double row = 1. / Massive.size();
			double col = 1. / Massive[i].size();

			double a_tex[2] = { row * i, col * j };
			double b_tex[2] = { row * i,col * (j + 1) };
			double c_tex[2] = { row * (i + 1),col * (j + 1) };
			double d_tex[2] = { row * (i + 1),col * j };

			double a[3];
			double b[3];
			double n[3];

			double A_copy[3] = { A.x,A.y,A.z };
			double B_copy[3] = { B.x,B.y,B.z };
			double C_copy[3] = { C.x,C.y,C.z };


			glBegin(GL_QUADS);
			glColor4f(1, 1, 1, 0);
			FindingAVectorByPoints(B_copy, A_copy, a);
			FindingAVectorByPoints(B_copy, C_copy, b);
			FindingTheNormalVectorForTheTwoVectors(n, a, b);
			glNormal3dv(n);
			glTexCoord2dv(a_tex);
			A.Vertex3dv();
			glTexCoord2dv(b_tex);
			B.Vertex3dv();
			glTexCoord2dv(c_tex);
			C.Vertex3dv();
			glTexCoord2dv(d_tex);
			D.Vertex3dv();
			glEnd();
		}

	}
	glDisable(GL_TEXTURE_2D);
}
