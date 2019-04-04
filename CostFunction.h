#ifndef COSTFUNCTION_H
#define COSTFUNCTION_H
#include "Matrix.hpp"
class CostFunction
{
public:
	CostFunction();
	virtual double fn(Matrix a, Matrix y) = 0;
	virtual Matrix delta(Matrix z, Matrix a, Matrix y) = 0;
	virtual ~CostFunction();
};

class CrossEntropyCost :public CostFunction
{
public:
	CrossEntropyCost();
	virtual double fn(Matrix a, Matrix y);
	virtual Matrix delta(Matrix z, Matrix a, Matrix y);
	virtual ~CrossEntropyCost();
};

class QuadraticCost :public CostFunction
{
public:
	QuadraticCost();
	virtual double fn(Matrix a, Matrix y);
	virtual Matrix delta(Matrix z, Matrix a, Matrix y);
	virtual ~QuadraticCost();
};

#endif // !CROSSFUNCTION_H



