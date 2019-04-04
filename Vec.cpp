#include "Vec.hpp"

RowVector::RowVector()
{
}

RowVector::RowVector(const RowVector & m):Matrix(m)
{

}

RowVector::RowVector(int cols, uint type):Matrix(1,cols,type)
{
}

RowVector::RowVector(int cols, uint type, void * data, size_t step):Matrix(1,cols,type,data,step)
{
}

int RowVector::getLength()
{
	return cols;
}

double & RowVector::operator[](int cols)
{
	switch (MAT_DEPTH(flag))
	{
	case MAT_64F:
		break;
	default:
		throw("MAT_DEPTH ERROR!");
		break;
	}
	return at<double>(0, cols);
}


RowVector::~RowVector()
{
}









ColVector::ColVector()
{
}

ColVector::ColVector(const ColVector & m) :Matrix(m)
{
}

ColVector::ColVector(int rows, uint type):Matrix(rows,1,type)
{
}

ColVector::ColVector(int rows, uint type, void * data, size_t step):Matrix(rows,1,type,data,step)
{
}

int ColVector::getLength()
{
	return rows;
}

double & ColVector::operator[](int rows)
{
	switch (MAT_DEPTH(flag))
	{
	case MAT_64F:
		break;
	default:
		throw("MAT_DEPTH ERROR!");
		break;
	}
	return at<double>(rows, 0);
}

ColVector::~ColVector()
{
}
