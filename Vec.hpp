#ifndef VEC_HPP
#define VEC_HPP

#include "Matrix.hpp"
//������
class RowVector :public Matrix
{
public:
	RowVector();
	RowVector(const RowVector& m);		//�������캯��  ����Ϊ����ʱҲҪ����  ��������Ϊcosnt ������ܱ�����û�к��ʵĿ������캯��
	RowVector(int cols, uint type);	//����һ����������������ռ�
	RowVector(int cols, uint type, void* data, size_t step = 0);
	int getLength();
	double& operator[](int cols);	//��������double
	virtual ~RowVector();
};





//������
class ColVector:public Matrix
{
public:
	ColVector();
	ColVector(const ColVector& m);		//�������캯��  ����Ϊ����ʱҲҪ����  ��������Ϊcosnt ������ܱ�����û�к��ʵĿ������캯��
	ColVector(int rows, uint type);	//����һ����������������ռ�
	ColVector(int rows, uint type, void* data, size_t step = 0);
	int getLength();
	double & operator[](int rows);	//��������double
	virtual ~ColVector();

private:

};



#endif // !VEC_H

