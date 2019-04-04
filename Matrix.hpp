#ifndef MATRIX_HPP
#define MATRIX_HPP
#include "macro.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef  unsigned char uchar;
typedef  unsigned int uint;
#ifndef _WIN64
	typedef  unsigned long size_t;
#endif // !_WIN64

//typedef  unsigned long size_t;
//flag 0-2 bit:������� Ҳ�Ǿ����������� 000 MAT_8U
//						    			 001 MAT_8S
//										 011 MAT_16U
//										 100 MAT_16S
//										 101 MAT_32S
//										 110 MAT_32F
//										 111 MAT_64F
//	   3-11 bit:ͨ���� ���512

class Matrix
{
public:
	Matrix() :flag(0), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), CountOfQuote(0)
	{
		step[0] = 0;
		step[1] = 0;
	}
	Matrix(const Matrix& m);		//�������캯��  ����Ϊ����ʱҲҪ����  ��������Ϊcosnt ������ܱ�����û�к��ʵĿ������캯��
	Matrix(const Matrix& m, int value);//��ʼ��һ����mһ���ľ��󣬳�ʼ����ֵΪvalue
	Matrix(int rows, int cols, uint type );	//����һ��Matrix��������ռ�
	//�˹��캯�����¿����ڴ沢������data�����ݣ�����dataӦ���ͷ�
	Matrix(int rows, int cols, uint type, void* data, size_t step = 0);
	virtual ~Matrix();
	uint flag;		//��־����ʶͨ���������͵�
	uchar dims;		//ά��   �������Ϊ2
	int rows;		//����
	int cols;		//����
	uchar *data;	//������			��Ϊ�Ӿ���ʱ����ָ��ָ���Ӿ������ʼ��ַ
	uchar *datastart;//ԭʼ������       ��Ϊ�Ӿ���ʱ����ָ��ָ��ԭ�о������ʼ��ַ
	uchar *dataend; //ָ��ԭ�о��������ַ	ָ�������Ч���ݿռ����һ����ַ����Ϊ��������ָ�����ü�����
	uchar *datalimit;//ָ���Ӿ��������ַ
	int step[2];	//���Ϊ��ά�ľ�����˲�������������
	unsigned int *CountOfQuote;//ָ�����ü�����
	int lables;		//��ʶ��ǰ���ݶ�Ӧ�ı�ǩ����û�п��Զ���Ϊ-1.

	//��ʼ��Ȩ�أ�����Ϊ��һ����Ԫ������������ĸ�����1/sqrt(Previous)��Ϊ��׼�Ĭ��1Ϊ��׼��,����ʼ��Ϊ
	//��׼��Ϊ1����ֵΪ0�ĸ�˹���
	virtual void InitMatData(double Previous=0);
	static double gaussrand_NORMAL();
    int getSizeofElement();//���Ԫ����ռ���ֽ���
	uint channels();//ͨ����
	size_t total(); //��þ�����ռ�ܿռ� ��λ�ֽ�
	void create();	//ʵ�ʴ����µ��ڴ�ʱ��ʹ��
	void destory();	//�ͷ��ڴ�
	void t();		//����ת�ã����·����ڴ�
	Matrix transpose();	//����ת�ã������¾���
	Matrix& operator=(const Matrix& m);//�Ⱥ�����
	//������������ֻ�ʺ�MAT_8U����������
	uchar GetPixelRGB(int rows, int cols, uint channel=0);	//���ĳ�㴦��ֵ������BGR��ʽ���,Ĭ�϶�ȡ��ͨ��Ϊ0
	uchar* PixelRGBPointer(int rows, int cols, uint channel=0);//����ĳ�㴦��ָ�룬����д�����ص㣬Ĭ�϶�ȡ��ͨ��Ϊ0
	template<typename T>
	T& at(int rows, int cols, uint channel = 0); //��ȡ����ĳ����ֵ
	bool IsEmpty();		//�ж��Ƿ�Ϊ�գ����������Ƿ����ռ�
	Matrix row(int x);	//��ø��е����ݣ�ʵ��Ϊ�Ӿ���
	Matrix col(int y);	//��ø��е����ݣ�ʵ��Ϊ�Ӿ���
	Matrix reshape(int cn, int row = 0);	//�޸ĵ�ǰ��������У�ʵ�ʾ����޸�step��row��channel��col��ֵ����������ľ�����֮ǰ��row*col*channel��ͬ������rowĬ�ϲ��޸� 
	double norm();//����
};

extern Matrix Matrix_Multiplication(Matrix a, Matrix b);
extern Matrix operator+(Matrix a, Matrix b);
//�����ӦԪ�����
extern Matrix operator*(Matrix a, Matrix b);
//һ�������Ծ���
extern Matrix operator*(double a, Matrix b);
template<typename T>
inline T& Matrix::at(int rows, int cols, uint channel)
{
	return *(T*)PixelRGBPointer(rows, cols, channel);
}

#endif // !MATRIX_H
