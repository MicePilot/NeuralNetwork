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
//flag 0-2 bit:数据深度 也是就是数据类型 000 MAT_8U
//						    			 001 MAT_8S
//										 011 MAT_16U
//										 100 MAT_16S
//										 101 MAT_32S
//										 110 MAT_32F
//										 111 MAT_64F
//	   3-11 bit:通道数 最高512

class Matrix
{
public:
	Matrix() :flag(0), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), CountOfQuote(0)
	{
		step[0] = 0;
		step[1] = 0;
	}
	Matrix(const Matrix& m);		//拷贝构造函数  类作为参数时也要调用  参数必须为cosnt 否则可能报错类没有合适的拷贝构造函数
	Matrix(const Matrix& m, int value);//初始化一个和m一样的矩阵，初始化的值为value
	Matrix(int rows, int cols, uint type );	//创建一个Matrix，并分配空间
	//此构造函数是新开辟内存并复制了data的内容，后续data应当释放
	Matrix(int rows, int cols, uint type, void* data, size_t step = 0);
	virtual ~Matrix();
	uint flag;		//标志，标识通道数据类型等
	uchar dims;		//维度   这里最大为2
	int rows;		//行数
	int cols;		//列数
	uchar *data;	//数据区			当为子矩阵时，该指针指向子矩阵的起始地址
	uchar *datastart;//原始数据区       当为子矩阵时，该指针指向原有矩阵的起始地址
	uchar *dataend; //指向原有矩阵结束地址	指向的是有效数据空间的下一个地址，若为正常矩阵，指向引用计数区
	uchar *datalimit;//指向子矩阵结束地址
	int step[2];	//最多为二维的矩阵，因此步长仅定义两个
	unsigned int *CountOfQuote;//指向引用计数区
	int lables;		//标识当前数据对应的标签，若没有可以定义为-1.

	//初始化权重，参数为上一层神经元个数，即输入的个数，1/sqrt(Previous)作为标准差，默认1为标准差,即初始化为
	//标准差为1，均值为0的高斯随机
	virtual void InitMatData(double Previous=0);
	static double gaussrand_NORMAL();
    int getSizeofElement();//获得元素所占的字节数
	uint channels();//通道数
	size_t total(); //获得矩阵所占总空间 单位字节
	void create();	//实际创建新的内存时候使用
	void destory();	//释放内存
	void t();		//矩阵转置，重新分配内存
	Matrix transpose();	//矩阵转置，返回新矩阵
	Matrix& operator=(const Matrix& m);//等号重载
	//以下两个函数只适合MAT_8U的数据类型
	uchar GetPixelRGB(int rows, int cols, uint channel=0);	//获得某点处的值，按照BGR方式存放,默认读取的通道为0
	uchar* PixelRGBPointer(int rows, int cols, uint channel=0);//或者某点处的指针，方便写入像素点，默认读取的通道为0
	template<typename T>
	T& at(int rows, int cols, uint channel = 0); //获取矩阵某处的值
	bool IsEmpty();		//判断是否为空，这里仅检测是否分配空间
	Matrix row(int x);	//获得该行的数据，实际为子矩阵
	Matrix col(int y);	//获得该列的数据，实际为子矩阵
	Matrix reshape(int cn, int row = 0);	//修改当前矩阵的行列，实际就是修改step、row、channel和col的值，最后调整后的矩阵与之前的row*col*channel相同，参数row默认不修改 
	double norm();//求范数
};

extern Matrix Matrix_Multiplication(Matrix a, Matrix b);
extern Matrix operator+(Matrix a, Matrix b);
//矩阵对应元素相乘
extern Matrix operator*(Matrix a, Matrix b);
//一个数乘以矩阵
extern Matrix operator*(double a, Matrix b);
template<typename T>
inline T& Matrix::at(int rows, int cols, uint channel)
{
	return *(T*)PixelRGBPointer(rows, cols, channel);
}

#endif // !MATRIX_H
