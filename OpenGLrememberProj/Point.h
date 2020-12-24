#pragma once

//����� ��� �������� � ������ � �������
class Point {
public:double x, y, z;
	  Point(double X, double Y, double Z) {
		  x = X; y = Y; z = Z;
	  }
	  void PrintPoints();
	  void Vertex3dv();
	  void Transleted();
};

//��������� �����
void Point::PrintPoints()
{
	glColor4f(1, 0, 0, 0);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3d(x, y, z);
	glEnd();
}

//�������� ��������� � ������� ��� ���������
void Point::Vertex3dv()
{
	glVertex3d(x, y, z);
}

void Point::Transleted()
{
	glTranslated(x, y, z);
}
