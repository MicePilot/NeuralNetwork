#include "CostFunction.h"



CostFunction::CostFunction()
{
}


CostFunction::~CostFunction()
{
}







CrossEntropyCost::CrossEntropyCost()
{
}

//���㽻������ʧ
double CrossEntropyCost::fn(Matrix a, Matrix y)
{
	double sum = 0;
	for (int i = 0; i < y.rows; i++)
	{
		double a1 = -1;
		double a2 = -1;
		if (y.at<double>(i, 0) == 0)
			a1 = 0;
		if (y.at<double>(i, 0) == 1 && a.at<double>(i, 0) == 1)
		{
			a2 = 0;
		}
		if (a1 == -1)
		{
			a1 = -y.at<double>(i, 0) * log(a.at<double>(i, 0));
		}
		if (a2 == -1)
		{
			a2 = -(1 - y.at<double>(i, 0))*log(1 - a.at<double>(i, 0));
		}
		sum += a1 + a2;
	}
	return sum;
}

//��������
Matrix CrossEntropyCost::delta(Matrix z, Matrix a, Matrix y)
{
	//ÿ�β����ľ������һ��
	Matrix result( a.rows,1, MAT_64FC1);
	for (int i = 0; i < a.rows; i++)
	{
		result.at<double>(i, 0) = a.at<double>(i, 0) - y.at<double>(i, 0);
	}
	return result;
}

CrossEntropyCost::~CrossEntropyCost()
{
}






QuadraticCost::QuadraticCost()
{
}

double QuadraticCost::fn(Matrix a, Matrix y)
{
	return 0;
}

Matrix QuadraticCost::delta(Matrix z, Matrix a, Matrix y)
{
	return Matrix();
}

QuadraticCost::~QuadraticCost()
{
}
