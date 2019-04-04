#include "Matrix.hpp"


Matrix::Matrix(const Matrix & m)
{
	flag = m.flag;
	lables = m.lables;
	dims = m.dims;
	rows = m.rows;
	cols = m.cols;
	data = m.data;
	dataend = m.dataend;
	datalimit = m.datalimit;
	datastart = m.datastart;
	step[0] = m.step[0];
	step[1] = m.step[1];
	CountOfQuote = m.CountOfQuote;
	MAT_XADD(CountOfQuote, 1);		//引用计数器加1	
}

Matrix::Matrix(const Matrix & m, int value)
{
	this->rows = m.rows;
	this->cols = m.cols;
	this->flag = m.flag;
	this->dims = 2;
	this->data = 0;					//先设置为0，否则create函数会判定已存在data区域，将不再分配空间
	this->create();

	step[1] = channels() * getSizeofElement();	//第二维步长
	step[0] = step[1] * cols;		//第一维步长

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < channels(); k++)
			{
				switch (MAT_DEPTH(flag))
				{
				case MAT_8U:
					at<uchar>(i, j, k) = value;
					break;
				case MAT_8S:
					at<char>(i, j, k) = value;
					break;
				case MAT_16U:
					at<uint>(i, j, k) = value;
					break;
				case MAT_16S:
					at<int>(i, j, k) = value;
					break;
				case MAT_32S:
					at<int>(i, j, k) = value;
					break;
				case MAT_32F:
					at<float>(i, j, k) = value;
					break;
				case MAT_64F:
					at<double>(i, j, k) = value;
					break;
				default:
					break;
				}
			}
		}
	}
	
}

Matrix::Matrix(int rows, int cols, uint type) :lables(-1)
{
	this->rows = rows;
	this->cols = cols;
	this->flag = type;
	this->dims = 2;
	this->data = 0;					//先设置为0，否则create函数会判定已存在data区域，将不再分配空间
	this->create();

	step[1] = channels() * getSizeofElement();	//第二维步长
	step[0] = step[1] * cols;		//第一维步长
}

Matrix::Matrix(int rows, int cols, uint type, void * data, size_t step):dims(0), lables(-1)
{
	this->rows = rows;
	this->cols = cols;
	this->flag = type;
	this->data = 0;					//先设置为0，否则create函数会判定已存在data区域，将不再分配空间
	this->create();

	this->step[1] = channels() * getSizeofElement();	//第二维步长
	this->step[0] = this->step[1] * this->cols;		//第一维步长
	memcpy(this->data, data, this->total());		//该data单元需要外部释放
}

Matrix::~Matrix()
{
	if (this->CountOfQuote != 0)
	{
		if ((MAT_XADD(this->CountOfQuote,-1)) == 0)		//引用计数器减一后判定是否为最后一个引用，若是
		{
			delete[] this->datastart;			//释放空间    datastart一定是原有的空间，避免了子矩阵是最后一个引用而释放空间不完整
			this->data = 0;
			this->CountOfQuote = 0;
		}
	}
}

void Matrix::InitMatData(double Previous)
{
	if (Previous == 0)
		Previous = 1;
	for (int i = 0; i < this->rows; i++)
	{
		for (int j = 0; j < this->cols; j++)
		{
			for (int k = 0; k < this->channels(); k++)
			{
				double num;
				
				num = this->gaussrand_NORMAL() / sqrt(Previous);
			
				switch(MAT_DEPTH(flag))
				{
					case MAT_8U:
						*(uchar*)(data + i * step[0] + j * step[1] + k * getSizeofElement()) = num;
						break;
					case MAT_8S:
						*(char*)(data + i * step[0] + j * step[1] + k * getSizeofElement()) = num;
						break;
					case MAT_16U:
						*(uint*)(data + i * step[0] + j * step[1] + k * getSizeofElement()) = num;
						break;
					case MAT_16S:
						*(int*)(data + i * step[0] + j * step[1] + k * getSizeofElement()) = num;
						break;
					case MAT_32S:
						*(int*)(data + i * step[0] + j * step[1] + k * getSizeofElement()) = num;
						break;
					case MAT_32F:
						*(float*)(data + i * step[0] + j * step[1] + k * getSizeofElement()) = num;
						break;
					case MAT_64F:
						*(double*)(data + i * step[0] + j * step[1] + k * getSizeofElement()) = num;
						break;
					default:
						break;

				}
				
			}
		}
	}
}

double Matrix::gaussrand_NORMAL()
{
		static double V1, V2, S;
		static int phase = 0;
		double X;


		if (phase == 0) {
			do {
				double U1 = (double)rand() / RAND_MAX;   //0-1
				double U2 = (double)rand() / RAND_MAX;


				V1 = 2 * U1 - 1;	//-1-1
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;	//0-2
			} while (S >= 1 || S == 0);	//0-1


			X = V1 * sqrt(-2 * log(S) / S);
		}
		else
			X = V2 * sqrt(-2 * log(S) / S);


		phase = 1 - phase;

		return X/6.0*0.25;// / 6.0;		//经过一亿次实验，分布在-6~6之间，这里映射到-1~1

}

int Matrix::getSizeofElement()
{
	int num = MAT_DEPTH(this->flag);
	//注意此处int的位数可能和float一样
	switch (num)
	{
	case MAT_8U:
		num = sizeof(uchar);
		break;
	case MAT_8S:
		num = sizeof(char);
		break;
	case MAT_16U:
		num = sizeof(uint);
		break;
	case MAT_16S:
		num = sizeof(int);
		break;
	case MAT_32S:
		num = sizeof(int);
		break;
	case MAT_32F:
		num = sizeof(float);
		break;
	case MAT_64F:
		num = sizeof(double);
		break;
	default:
		break;
	}
	
	return num;
}

uint Matrix::channels()
{
	return MAT_CHANNELS(flag);
}

size_t Matrix::total()
{
	return this->rows*this->cols*this->channels()*this->getSizeofElement();
}

void Matrix::create()
{
	if (this->data != 0)	//指向了存储区
	{

	}
	else					//需要重新分配空间的，已经不是子矩阵了
	{
		this->datastart = this->data = new uchar[total() + 4];		//分配空间，最后的4为计数区
		this->CountOfQuote = (unsigned int*)(this->data + total());//使得计数指针指向计数区
		this->dataend = this->datalimit = (uchar*)(this->CountOfQuote); //结束地址为有效数据空间的下一个地址
		*(this->CountOfQuote) = 1;					//首次创建，设置计数器为一
	}
}

void Matrix::destory()
{
	delete[] this->datastart;
}

Matrix Matrix::transpose()
{
	Matrix new_matrix(this->cols, this->rows, this->flag);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < channels(); k++)
			{
				switch (MAT_DEPTH(flag))
				{
				case MAT_8U:
					new_matrix.at<uchar>(j, i, k) = at<uchar>(i, j, k);
					break;
				case MAT_8S:
					new_matrix.at<char>(j, i, k) = at<char>(i, j, k);
					break;
				case MAT_16U:
					new_matrix.at<uint>(j, i, k) = at<uint>(i, j, k);
					break;
				case MAT_16S:
					new_matrix.at<int>(j, i, k) = at<int>(i, j, k);
					break;
				case MAT_32S:
					new_matrix.at<int>(j, i, k) = at<int>(i, j, k);
					break;
				case MAT_32F:
					new_matrix.at<float>(j, i, k) = at<float>(i, j, k);
					break;
				case MAT_64F:
					new_matrix.at<double>(j, i, k) = at<double>(i, j, k);
					break;
				default:
					break;
				}
			}
		}
	}
	return new_matrix;
}

Matrix & Matrix::operator=(const Matrix & m)
{
	if (this->CountOfQuote != 0)		//已存在数据    这里主要是为了重新调整roi数据区使用
	{
		if ((MAT_XADD(CountOfQuote,-1)) == 0)		//引用计数器减一后判定是否为最后一个引用，若是
		{
			destory();					//释放空间    
		}
	}
	flag = m.flag;
	lables = m.lables;
	dims = m.dims;
	rows = m.rows;
	cols = m.cols;
	data = m.data;
	dataend = m.dataend;
	datastart = m.datastart;
	datalimit = m.datalimit;
	step[0] = m.step[0];
	step[1] = m.step[1];
	CountOfQuote = m.CountOfQuote;
	MAT_XADD(CountOfQuote, 1);			//引用计数器加1
	return *this;
}

uchar Matrix::GetPixelRGB(int rows, int cols, uint channel)
{
	return *(data + step[0] * rows + step[1] * cols + channel * 1);
}

uchar * Matrix::PixelRGBPointer(int rows, int cols, uint channel)
{
	if (((rows < (this->rows)) && (rows >= 0)) && ((cols < (this->cols)) && (cols >= 0)))
	{

		return (data + step[0] * rows + step[1] * cols + channel * 1);
	}
	else
		return nullptr;
}

bool Matrix::IsEmpty()
{
	return (data == 0) ? true : false;
}

double Matrix::norm()
{
	double sum = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < channels(); k++)
			{
				switch (MAT_DEPTH(flag))
				{
				case MAT_8U:
					sum += pow(at<uchar>(i, j, k), 2);
						break;
				case MAT_8S:
					sum += pow(at<char>(i, j, k), 2);
					break;
				case MAT_16U:
					sum += pow(at<uint>(i, j, k), 2);
					break;
				case MAT_16S:
					sum += pow(at<int>(i, j, k), 2);
					break;
				case MAT_32S:
					sum += pow(at<int>(i, j, k), 2);
					break;
				case MAT_32F:
					sum += pow(at<float>(i, j, k), 2);
					break;
				case MAT_64F:
					sum += pow(at<double>(i, j, k), 2);
					break;
				default:
					break;
				}
			}
			
		}
	}
	return sqrt(sum);
}
//矩阵乘法
Matrix Matrix_Multiplication(Matrix a, Matrix b)
{

	if (a.cols != b.rows)		//矩阵是否可以相乘的检测   a的列数必须等于b的行数
		return Matrix();
	Matrix result(a.rows, b.cols, MAT_DEPTH(a.flag));

	for (int i = 0; i < a.rows; i++)
	{
		for (int j = 0; j < b.cols; j++)
		{
			double sum = 0;
			for (int k = 0; k < a.cols; k++)
			{
				//sum += (*((T*)a.PixelRGBPointer(i, k, 0)))*(*((T*)b.PixelRGBPointer(k, j, 0)));

				switch (MAT_DEPTH(a.flag))
				{
				case MAT_8U:
					sum += a.at<uchar>(i, k)*b.at <uchar>(k, j);
					break;
				case MAT_8S:
					sum += a.at<char>(i, k)*b.at <char>(k, j);
					break;
				case MAT_16U:
					sum += a.at<uint>(i, k)*b.at <uint>(k, j);
					break;
				case MAT_16S:
					sum += a.at<int>(i, k)*b.at <int>(k, j);
					break;
				case MAT_32S:
					sum += a.at<int>(i, k)*b.at <int>(k, j);
					break;
				case MAT_32F:
					sum += a.at<float>(i, k)*b.at <float>(k, j);
					break;
				case MAT_64F:
					sum += a.at<double>(i, k)*b.at <double>(k, j);
					break;
				default:
					break;
				}

			}
			switch (MAT_DEPTH(a.flag))
			{
			case MAT_8U:
				result.at<uchar>(i, j) = sum;
				break;
			case MAT_8S:
				result.at<char>(i, j) = sum;
				break;
			case MAT_16U:
				result.at<uint>(i, j) = sum;
				break;
			case MAT_16S:
				result.at<int>(i, j) = sum;
				break;
			case MAT_32S:
				result.at<int>(i, j) = sum;
				break;
			case MAT_32F:
				result.at<float>(i, j) = sum;
				break;
			case MAT_64F:
				result.at<double>(i, j) = sum;
				break;
			default:
				break;
			}
		}
	}
	
	return result;
}
//矩阵相加
Matrix operator+(Matrix a, Matrix b)
{
	
		Matrix output(a.rows, a.cols, MAT_DEPTH(a.flag));
		for (int i = 0; i < output.rows; i++)
		{
			for (int j = 0; j < output.cols; j++)
			{
				for (int k = 0; k < output.channels(); k++)
				{

					switch (MAT_DEPTH(a.flag))
					{
					case MAT_8U:
						output.at<uchar>(i, j, k) = a.at<uchar>(i, j, k) + b.at<uchar>(i, j, k);
						break;
					case MAT_8S:
						output.at<char>(i, j, k) = a.at<char>(i, j, k) + b.at<char>(i, j, k);
						break;
					case MAT_16U:
						output.at<uint>(i, j, k) = a.at<uint>(i, j, k) + b.at<uint>(i, j, k);
						break;
					case MAT_16S:
						output.at<int>(i, j, k) = a.at<int>(i, j, k) + b.at<int>(i, j, k);
						break;
					case MAT_32S:
						output.at<int>(i, j, k) = a.at<int>(i, j, k) + b.at<int>(i, j, k);
						break;
					case MAT_32F:
						output.at<float>(i, j, k) = a.at<float>(i, j, k) + b.at<float>(i, j, k);
						break;
					case MAT_64F:
						output.at<double>(i, j, k) = a.at<double>(i, j, k) + b.at<double>(i, j, k);
						break;
					default:
						break;
					}
				}
			}
		}
		return output;

	
}
//矩阵对应元素相乘
Matrix operator*(Matrix a, Matrix b)
{
	if ((b.rows != a.rows) || (b.cols != a.cols))
	{
		Matrix nu;
		return nu;
	}
	Matrix output(a.rows, a.cols, a.flag);
	for (int i = 0; i < a.rows; i++)
	{
		for (int j = 0; j < a.cols; j++)
		{
			for (int k = 0; k < a.channels(); k++)
			{
				switch (MAT_DEPTH(a.flag))
				{
				case MAT_8U:
					output.at<uchar>(i, j, k) = a.at<uchar>(i, j, k) * b.at<uchar>(i, j, k);
					break;
				case MAT_8S:
					output.at<char>(i, j, k) = a.at<char>(i, j, k) * b.at<char>(i, j, k);
					break;
				case MAT_16U:
					output.at<uint>(i, j, k) = a.at<uint>(i, j, k) * b.at<uint>(i, j, k);
					break;
				case MAT_16S:
					output.at<int>(i, j, k) = a.at<int>(i, j, k) * b.at<int>(i, j, k);
					break;
				case MAT_32S:
					output.at<int>(i, j, k) = a.at<int>(i, j, k) * b.at<int>(i, j, k);
					break;
				case MAT_32F:
					output.at<float>(i, j, k) = a.at<float>(i, j, k) * b.at<float>(i, j, k);
					break;
				case MAT_64F:
					output.at<double>(i, j, k) = a.at<double>(i, j, k) * b.at<double>(i, j, k);
					break;
				default:
					break;
				}
			}
		}
	}
	return output;
}
//一个数乘矩阵
Matrix operator*(double a, Matrix b)
{
	Matrix output(b.rows, b.cols, b.flag);
	for (int i = 0; i < b.rows; i++)
	{
		for (int j = 0; j < b.cols; j++)
		{
			for (int k = 0; k < b.channels(); k++)
			{
				switch (MAT_DEPTH(b.flag))
				{
				case MAT_8U:
					output.at<uchar>(i, j, k) = b.at<uchar>(i, j, k) * a;
					break;
				case MAT_8S:
					output.at<char>(i, j, k) = b.at<char>(i, j, k) * a;
					break;
				case MAT_16U:
					output.at<uint>(i, j, k) = b.at<uint>(i, j, k) * a;
					break;
				case MAT_16S:
					output.at<int>(i, j, k) = b.at<int>(i, j, k) * a;
					break;
				case MAT_32S:
					output.at<int>(i, j, k) = b.at<int>(i, j, k) * a;
					break;
				case MAT_32F:
					output.at<float>(i, j, k) = b.at<float>(i, j, k) * a;
					break;
				case MAT_64F:
					output.at<double>(i, j, k) = b.at<double>(i, j, k) * a;
					break;
				default:
					break;
				}
			}
		}
	}
	return output;
}
