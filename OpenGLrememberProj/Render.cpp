#include "Render.h"

#include <windows.h>
#include <vector>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "MyShaders.h"

#include "ObjLoader.h"
#include "GUItextRectangle.h"

#include "Texture.h"
#include "Point.h"
#include "DrawingCurvesHermite.h"
#include "DrawingCurvesBezier.h"
#include "WorkingWithVector.h"
#include "BezierSuface.h"
#include "MovementObjectAlongCurves.h"
#include "DrawingCircle.h"

GuiTextRectangle rec;

bool textureMode = true;
bool lightMode = true;
bool Counterclockwise = true;
bool FlagAlpha = true;
bool AnimationFir = false;
bool JampSnowmen2 = true;


//небольшой дефайн для упрощения кода
#define POP glPopMatrix()
#define PUSH glPushMatrix()


ObjFile *model;
ObjFile Snowman, Ground, MainFir, Fir, Snowman2;
Texture MainFirTex, GroundTex, SnowmanTex, Snowman2Tex, FirTex;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[10];  //массивчик для десяти шейдеров
Shader frac;
Shader cassini;




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



}  camera;   //создаем объект камеры


//класс недоделан!
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




float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;

//обработчик движения мыши
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


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}


	
	//двигаем свет по плоскости, в точку где мышь
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

//обработчик вращения колеса  мыши
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

//обработчик нажатия кнопок клавиатуры
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

	if (key == 'S')
	{
		frac.LoadShaderFromFile();
		frac.Compile();

		s[0].LoadShaderFromFile();
		s[0].Compile();

		cassini.LoadShaderFromFile();
		cassini.Compile();
	}

	if (key == 'Q')
		Time = 0;

	if (OpenGL::isKeyPressed('O'))
	{
		Counterclockwise = !Counterclockwise;
	}
	if (OpenGL::isKeyPressed('U'))
	{
		AnimationFir = !AnimationFir;
	}
	if (OpenGL::isKeyPressed('I'))
	{
		FlagAlpha = !FlagAlpha;
	}
	if (OpenGL::isKeyPressed('P'))
	{
		JampSnowmen2 = !JampSnowmen2;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{

}


void DrawQuad()
{
	double A[] = { 0,0 };
	double B[] = { 1,0 };
	double C[] = { 1,1 };
	double D[] = { 0,1 };
	glBegin(GL_QUADS);
	glColor3d(.5, 0, 0);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);
	glEnd();
}


ObjFile objModel,monkey;

Texture monkeyTex;

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
	
	


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
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

	/*
	//texture1.loadTextureFromFile("textures\\texture.bmp");   загрузка текстуры из файла
	*/


	frac.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	frac.FshaderFileName = "shaders\\frac.frag"; //имя файла фрагментного шейдера
	frac.LoadShaderFromFile(); //загружаем шейдеры из файла
	frac.Compile(); //компилируем

	cassini.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	cassini.FshaderFileName = "shaders\\cassini.frag"; //имя файла фрагментного шейдера
	cassini.LoadShaderFromFile(); //загружаем шейдеры из файла
	cassini.Compile(); //компилируем
	

	s[0].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[0].FshaderFileName = "shaders\\light.frag"; //имя файла фрагментного шейдера
	s[0].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[0].Compile(); //компилируем

	s[1].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //имя файла фрагментного шейдера
	s[1].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[1].Compile(); //компилируем

	

	 //так как гит игнорит модели *.obj файлы, так как они совпадают по расширению с объектными файлами, 
	 // создающимися во время компиляции, я переименовал модели в *.obj_m
	loadModel("models\\lpgun6.obj_m", &objModel);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\SS.obj_m", &Snowman);
	SnowmanTex.loadTextureFromFile("textures\\SS.bmp");
	SnowmanTex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\SS.obj_m", &Snowman2);
	Snowman2Tex.loadTextureFromFile("textures\\SS.bmp");
	Snowman2Tex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\2.obj_m", &Ground);
	GroundTex.loadTextureFromFile("textures\\2.bmp");
	GroundTex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\1.obj_m", &MainFir);
	MainFirTex.loadTextureFromFile("textures\\el1.bmp");
	MainFirTex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\er.obj_m", &Fir);
	FirTex.loadTextureFromFile("textures\\ek.bmp");
	FirTex.bindTexture();

	


	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\monkey.obj_m", &monkey);
	monkeyTex.loadTextureFromFile("textures//tex.bmp");
	monkeyTex.bindTexture();


	tick_n = GetTickCount();
	tick_o = tick_n;

	rec.setSize(300, 100);
	rec.setPosition(10, ogl->getHeight() - 100-10);
	rec.setText("O - по/против часовой движение снеговика\nU-вкл/выкл вращение ели\nI-по/против часовой вращение ели\nP-вкл/выкл режима прыгания снеговика\nWASD-управление снеговиком\n",0,0,0);

	
}

Point PointMainFir(0, 0, 1);
Point PointSnowman(0, 0, 5.05);
Point PointSnowman2(20, 20, 5.05);
void DrawForest()
{
	glPushMatrix();
	glScaled(0.5, 0.5, 0.5);
	glTranslated(0, 35, 2);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-40, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glScaled(0.5, 0.5, 0.5);
	glTranslated(0, -35, 2);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-40, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();

	glTranslated(-8, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();
	glPopMatrix();
}
//рисование статических объектов
void DrawTheObject()
{
	glPushMatrix();
	glScaled(1, 1, 0.3);
	GroundTex.bindTexture();
	Ground.DrawObj();
	glPopMatrix();
}

double h = 0;
bool Jamp = true;
void JampingSnowman()
{
	if (h <= 0)
	{
		h = 0;
		Jamp = true;
	}
	if (h >= 3)
	{
		h = 3;
		Jamp = false;
	}
	if (Jamp == true)
	{
		h += 0.10;
		glPushMatrix();
		glScaled(0.4, 0.4, 0.4);
		PointSnowman2.Transleted();
		glTranslated(0, 0, h);
		Snowman2Tex.bindTexture();
		Snowman2.DrawObj();
		glPopMatrix();
	}
	else
	{
		h -= 0.10;
		glPushMatrix();
		glScaled(0.4, 0.4, 0.4);
		PointSnowman2.Transleted();
		glTranslated(0, 0, h);
		Snowman2Tex.bindTexture();
		Snowman2.DrawObj();
		glPopMatrix();
	}
}

double alpha = 0;
double Alpha = 0;
//вращение ели вокруг оси 
double AnimationFirClockwiseDirection(double Alpha)
{
	if (alpha > 360) alpha = 0;

	alpha += 1;
	glPushMatrix();
	PointMainFir.Transleted();
	glRotated(alpha, 0, 0, 1);
	MainFirTex.bindTexture();
	MainFir.DrawObj();
	glPopMatrix();

	return alpha;
}
//вращение ели против оси
double AnimationFirCounterclockwise(double Alpha)
{
	if (alpha < 0) alpha = 360;

	alpha -= 1;
	glPushMatrix();
	PointMainFir.Transleted();
	glRotated(alpha, 0, 0, 1);
	MainFirTex.bindTexture();
	MainFir.DrawObj();
	glPopMatrix();

	return alpha;
}

double T = 0;
double t_max = 0;
double t_min = 2 * PI;
double delta_time = 0;
bool Checkingt_max = true;

void FindingAnAngleWithAxes(double point[3], double next_point[3])
{
	double x_object[3];
	double X_Space[3] = { -1,0,0 };
	double Vec[3] = { 0,0,0 };
	FindingAVectorByPoints(point, next_point, Vec);
	NormalizationOfTheVector(Vec, FIndingTheLenghtOfAVector(Vec));
	x_object[0] = Vec[0]; x_object[1] = Vec[1]; x_object[2] = 0;

	NormalizationOfTheVector(x_object, FIndingTheLenghtOfAVector(x_object));

	double cos = ScalarProductOfTwoVectors(X_Space, x_object);
	double VecProduct[3] = { 0,0,0 };
	VectorProductOfTwoVectors(X_Space, x_object, VecProduct);

	double sin = VecProduct[2] / abs(VecProduct[2]);
	double alpha = acos(cos) * 180.0 / M_PI * sin;
	double betta = acos(Vec[2]) * 180.0 / M_PI - 90;

	glRotated(alpha, 0, 0, 1);
	glRotated(betta, 0, 1, 0);
}
//движение снеговика против часовой стрелки
void MoveSnowmanCounterclockwise(double t_min, double r)
{
	double X[3];
	double X_next[3];

	FindingPointCircle(r, t_min, X);
	FindingPointCircle(r, t_min - 0.01, X_next);

	glPushMatrix();
	glTranslated(X[0], X[1], X[2]);
	FindingAnAngleWithAxes(X, X_next);
	glScaled(0.4, 0.4, 0.4);
	PointSnowman.Transleted();
	SnowmanTex.bindTexture();
	Snowman.DrawObj();
	glPopMatrix();
}
//Движение снеговика по часовой стрелке 
void MoveSnowmanClockwiseDirection(double t_max, double r)
{
	double X[3];
	double X_next[3];

	FindingPointCircle(r, t_max, X);
	FindingPointCircle(r, t_max + 0.01, X_next);

	glPushMatrix();
	glTranslated(X[0],X[1],X[2]);
	FindingAnAngleWithAxes(X, X_next);
	glScaled(0.4, 0.4, 0.4);
	PointSnowman.Transleted();
	SnowmanTex.bindTexture();
	Snowman.DrawObj();
	glPopMatrix();
}
//функции вызывающая ф-ии для осуществления движения по кривым
double AnimationSnowmanClockwiseDirection(double T)
{
	t_max = T;
	delta_time = SearchDeltaTime();
	double r = 7;
	if (t_max >= 2*PI)
	{
		t_max = 0;
	}

	t_max += delta_time / 5;
	MoveSnowmanClockwiseDirection(t_max, r);
	return t_max;
}
double AnimationSnowmanCounterclockwise(double T)
{
	t_min = T;
	delta_time = SearchDeltaTime();
	double r = 7;

	if (t_min <= 0)
	{
		t_min = 2 * PI;
	}
	t_min -= delta_time / 5;
	MoveSnowmanCounterclockwise(t_min, r);
	return t_min;
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
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//Прогать тут  


	//





	s[0].UseShader();

	//передача параметров в шейдер.  Шаг один - ищем адрес uniform переменной по ее имени. 
	int location = glGetUniformLocationARB(s[0].program, "light_pos");
	//Шаг 2 - передаем ей значение
	glUniform3fARB(location, light.pos.X(), light.pos.Y(),light.pos.Z());

	location = glGetUniformLocationARB(s[0].program, "Ia");
	glUniform3fARB(location, 0.2, 0.2, 0.2);

	location = glGetUniformLocationARB(s[0].program, "Id");
	glUniform3fARB(location, 1.0, 1.0, 1.0);

	location = glGetUniformLocationARB(s[0].program, "Is");
	glUniform3fARB(location, .7, .7, .7);


	location = glGetUniformLocationARB(s[0].program, "ma");
	glUniform3fARB(location, 0.2, 0.2, 0.1);

	location = glGetUniformLocationARB(s[0].program, "md");
	glUniform3fARB(location, 0.4, 0.65, 0.5);

	location = glGetUniformLocationARB(s[0].program, "ms");
	glUniform4fARB(location, 0.9, 0.8, 0.3, 25.6);

	location = glGetUniformLocationARB(s[0].program, "camera");
	glUniform3fARB(location, camera.pos.X(), camera.pos.Y(), camera.pos.Z());

	//первый пистолет
	/*objModel.DrawObj();


	Shader::DontUseShaders();
	
	//второй, без шейдеров
	glPushMatrix();
		glTranslated(-5,15,0);
		//glScaled(-1.0,1.0,1.0);
		objModel.DrawObj();
	glPopMatrix();



	//обезьяна

	s[1].UseShader();
	int l = glGetUniformLocationARB(s[1].program,"tex"); 
	glUniform1iARB(l, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0
	glPushMatrix();
	glRotated(-90, 0, 0, 1);
	monkeyTex.bindTexture();
	monkey.DrawObj();
	glPopMatrix();*/

	s[1].UseShader();
	
	

	/*glPushMatrix();
	glTranslated(7, 0, 0);
	FirTex.bindTexture();
	Fir.DrawObj();
	glPopMatrix();*/

	DrawTheObject();
	DrawForest();

	if (JampSnowmen2 == true)
	{
		JampingSnowman();
	}
	else
	{
		if (h > 0)
		{
			h -= 0.10;
			glPushMatrix();
			glScaled(0.4, 0.4, 0.4);
			PointSnowman2.Transleted();
			glTranslated(0, 0, h);
			Snowman2Tex.bindTexture();
			Snowman2.DrawObj();
			glPopMatrix();
		}
		else
		{
			bool flag = false;
			if (OpenGL::isKeyPressed('W'))
			{
				flag = true;
				PointSnowman2.x += 0.1;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (OpenGL::isKeyPressed('W') && OpenGL::isKeyPressed('D'))
			{
				flag = true;
				PointSnowman2.x += 0.1;
				PointSnowman2.y += 0.2;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (OpenGL::isKeyPressed('W') && OpenGL::isKeyPressed('A'))
			{
				flag = true;
				PointSnowman2.x += 0.1;
				PointSnowman2.y -= 0.2;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (OpenGL::isKeyPressed('D'))
			{
				flag = true;
				PointSnowman2.y += 0.1;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (OpenGL::isKeyPressed('A'))
			{
				flag = true;
				PointSnowman2.y -= 0.1;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (OpenGL::isKeyPressed('S'))
			{
				flag = true;
				PointSnowman2.x -= 0.1;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (OpenGL::isKeyPressed('S') && OpenGL::isKeyPressed('D'))
			{
				flag = true;
				PointSnowman2.x -= 0.1;
				PointSnowman2.y += 0.2;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (OpenGL::isKeyPressed('S') && OpenGL::isKeyPressed('A'))
			{
				flag = true;
				PointSnowman2.x -= 0.1;
				PointSnowman2.y -= 0.2;
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
			if (flag == false)
			{
				glPushMatrix();
				glScaled(0.4, 0.4, 0.4);
				PointSnowman2.Transleted();
				Snowman2Tex.bindTexture();
				Snowman2.DrawObj();
				glPopMatrix();
			}
		}
		
	}

	if (AnimationFir == false)
	{
		glPushMatrix();
		PointMainFir.Transleted();
		glRotated(Alpha, 0, 0, 1);
		MainFirTex.bindTexture();
		MainFir.DrawObj();
		glPopMatrix();
	}
	else
	{
		if (FlagAlpha == true)
		{
			Alpha = AnimationFirClockwiseDirection(Alpha);
		}
		else
		{
			Alpha = AnimationFirCounterclockwise(Alpha);
		}
	}

	if (Counterclockwise == true)
	{
		T = AnimationSnowmanClockwiseDirection(T);
	}
	else
	{
		T = AnimationSnowmanCounterclockwise(T);
	}
	//////Рисование фрактала

	
	/*
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		frac.UseShader();

		int location = glGetUniformLocationARB(frac.program, "size");
		glUniform2fARB(location, (GLfloat)ogl->getWidth(), (GLfloat)ogl->getHeight());

		location = glGetUniformLocationARB(frac.program, "uOffset");
		glUniform2fARB(location, offsetX, offsetY);

		location = glGetUniformLocationARB(frac.program, "uZoom");
		glUniform1fARB(location, zoom);

		location = glGetUniformLocationARB(frac.program, "Time");
		glUniform1fARB(location, Time);

		DrawQuad();

	}
	*/
	
	
	//////Овал Кассини
	
	/*
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		cassini.UseShader();

		int location = glGetUniformLocationARB(cassini.program, "size");
		glUniform2fARB(location, (GLfloat)ogl->getWidth(), (GLfloat)ogl->getHeight());


		location = glGetUniformLocationARB(cassini.program, "Time");
		glUniform1fARB(location, Time);

		DrawQuad();
	}

	*/

	
	
	

	
	Shader::DontUseShaders();

	
	
}   //конец тела функции


bool gui_init = false;

//рисует интерфейс, вызывется после обычного рендера
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	

	glActiveTexture(GL_TEXTURE0);
	rec.Draw();


		
	Shader::DontUseShaders(); 



	
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

