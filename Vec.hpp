#ifndef VEC_HPP
#define VEC_HPP

#include "Matrix.hpp"
//行向量
class RowVector :public Matrix
{
public:
	RowVector();
	RowVector(const RowVector& m);		//拷贝构造函数  类作为参数时也要调用  参数必须为cosnt 否则可能报错类没有合适的拷贝构造函数
	RowVector(int cols, uint type);	//创建一个行向量，并分配空间
	RowVector(int cols, uint type, void* data, size_t step = 0);
	int getLength();
	double& operator[](int cols);	//仅适用于double
	virtual ~RowVector();
};





//列向量
class ColVector:public Matrix
{
public:
	ColVector();
	ColVector(const ColVector& m);		//拷贝构造函数  类作为参数时也要调用  参数必须为cosnt 否则可能报错类没有合适的拷贝构造函数
	ColVector(int rows, uint type);	//创建一个列向量，并分配空间
	ColVector(int rows, uint type, void* data, size_t step = 0);
	int getLength();
	double & operator[](int rows);	//仅适用于double
	virtual ~ColVector();

private:

};



#endif // !VEC_H

