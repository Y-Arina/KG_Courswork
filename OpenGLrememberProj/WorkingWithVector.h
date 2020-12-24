#pragma once
//нахождение вектора по 2 точкам 
void FindingAVectorByPoints(double point[3], double nextpoint[3], double* Vector)
{
	Vector[0] = nextpoint[0] - point[0];
	Vector[1] = nextpoint[1] - point[1];
	Vector[2] = nextpoint[2] - point[2];
}

//Нахождение вектора нормали по двум векторам 
void FindingTheNormalVectorForTheTwoVectors(double* n, double a[3], double b[3])
{
	n[0] = a[1] * b[2] - b[1] * a[2];
	n[1] = (-1) * a[0] * b[2] + b[0] * a[2];
	n[2] = a[0] * b[1] - b[0] * a[1];
}

//нахождение длины вектора
double FIndingTheLenghtOfAVector(double* Vector)
{
	double Lenght = sqrt(pow(Vector[0], 2) + pow(Vector[1], 2) + pow(Vector[2], 2));

	return Lenght;
}

//нормализация вектора
void NormalizationOfTheVector(double* Vector, double Lenght)
{
	Vector[0] /= Lenght;
	Vector[1] /= Lenght;
	Vector[2] /= Lenght;
}

//скалярное произведение двух векторов
double ScalarProductOfTwoVectors(double* a, double* b)
{
	double ScalarProduct = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return ScalarProduct;
}

//векторное произведение двух векторов
void VectorProductOfTwoVectors(double* a, double* b, double* result)
{
	result[0] = a[1] * b[2] - a[2] * b[1];
	result[1] = (-1) * a[0] * b[2] + b[0] * a[2];
	result[2] = a[0] * b[1] - a[1] * b[0];
}
