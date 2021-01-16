#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(RowVector &layers, std::string cost,std::string fileName):num_layers(layers.cols)
{
	
	this->activation = logistic;
	this->activation_matrix = logistic_Matrix;
	this->activation_deriv_matrix = logistic_derivative_Matrix;
	this->activation_deriv = logistic_derivative;
	
	this->layers = layers;
	this->num_layers = layers.getLength();
	
	//挂载代价函数
	if (cost == "CrossEntropy")
	{
		this->cost = &CrossEntropy;
	}
	else if (cost == "Quadratic")
	{
		this->cost = &Quadratic;
	}
	else
	{
		throw ("NeuralNetwork Init Error!\nCost Function Error!\n");
	}

	if (fileName.empty())			//没有给文件名字，则随机分配权重
	{
		srand((unsigned)time(NULL));
		//随机分配权重以及偏向
		//第一层为输入层，不分配
		
		for (int i = 1; i < num_layers; i++)//网络每层的循环 
		{
			//每一层的所有神经元的权重都是上一层所有神经元的个数，即若本层神经元为n个，上层为m个，
			//构成n*m的矩阵，通道为1
			Matrix a(layers.at<int>(0, i), layers.at<int>(0, i - 1), MAT_64FC1);
			//初始化权重矩阵，行为当前层的神经元，列为上一层的神经元
			a.InitMatData(layers.at<int>(0, i - 1));
			weights.push_back(a);
			//初始化偏向
			//每一层的偏向都是本层神经元的个数
			ColVector b(layers.at<int>(0, i), MAT_64FC1);
			b.InitMatData();
			biases.push_back(b);
			
		}
	}
	else
	{
		//LoadNetworkStructure(fileName);
	}
}

NeuralNetwork::~NeuralNetwork()
{

}

int ReverseInt(int i)
{
	unsigned char ch1, ch2, ch3, ch4;
	ch1 = i & 255;
	ch2 = (i >> 8) & 255;
	ch3 = (i >> 16) & 255;
	ch4 = (i >> 24) & 255;
	return((int)ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}

void NeuralNetwork::loadMnistData(std::vector<Matrix> &training_data, std::vector<Matrix> &validation_data)
{
	loadImageAndLabel(training_data, "train-images.idx3-ubyte", "train-labels.idx1-ubyte");
	loadImageAndLabel(validation_data, "t10k-images.idx3-ubyte", "t10k-labels.idx1-ubyte");
}

void NeuralNetwork::loadImageAndLabel(std::vector<Matrix>& data, std::string fileNameImage,std::string fileNameLabel)
{
	using namespace std;
	ifstream file_image(fileNameImage, ios::binary);
	ifstream file_label(fileNameLabel, ios::binary);
	if (file_label.is_open()&& file_image.is_open())
	{
		int label_magic_number = 0;
		int number_of_label = 0;
		file_label.read((char*)&label_magic_number, sizeof(label_magic_number));
		file_label.read((char*)&number_of_label, sizeof(number_of_label));
		label_magic_number = ReverseInt(label_magic_number);
		number_of_label = ReverseInt(number_of_label);
		cout << "label_magic_number = " << label_magic_number << endl;
		cout << "number_of_label = " << number_of_label << endl;

		int image_magic_number = 0;
		int number_of_images = 0;
		int n_rows = 0;
		int n_cols = 0;
		unsigned char label;
		file_image.read((char*)&image_magic_number, sizeof(image_magic_number));
		file_image.read((char*)&number_of_images, sizeof(number_of_images));
		file_image.read((char*)&n_rows, sizeof(n_rows));
		file_image.read((char*)&n_cols, sizeof(n_cols));
		image_magic_number = ReverseInt(image_magic_number);
		number_of_images = ReverseInt(number_of_images);
		n_rows = ReverseInt(n_rows);
		n_cols = ReverseInt(n_cols);

		cout << "image_magic_number = " << image_magic_number << endl;
		cout << "number of images = " << number_of_images << endl;
		cout << "rows = " << n_rows << endl;
		cout << "cols = " << n_cols << endl;

		for (int i = 0; i < number_of_label; i++)
		{
			unsigned char label = 0;
			file_label.read((char*)&label, sizeof(label));		//读取标签
			
			//Matrix temp(n_rows, n_cols, MAT_64FC1);				//创建矩阵
			Matrix temp(1, n_cols*n_rows, MAT_64FC1);			//创建矩阵,这里为了后续方便，选用单行的矩阵
			temp.lables = label;								//写入标签
			for (int r = 0; r < n_rows; r++)
			{
				for (int c = 0; c < n_cols; c++)
				{
					unsigned char image = 0;
					file_image.read((char*)&image, sizeof(image));//读取数据
					//tp.push_back((double)image / 255.0);
					temp.at<double>(0, r*n_cols + c) = (double)image / 255.0;
					//*(double*)(temp.data + r * n_cols*1*sizeof(double) + c * 1 * sizeof(double)) = (double)image / 255.0;//写入数据
				}
			}
			data.push_back(temp);		//加入集合
		}
	}
  else {
    cout << "读取数据文件出错，检查文件路径" << endl;
  }
}

void NeuralNetwork::SGD(std::vector<Matrix> training_data, std::vector<Matrix> evaluation_data, double epochs, double mini_batch_size, double learning_rate, double lmbda, bool monitor_evaluation_cost, bool monitor_evaluation_accuracy, bool monitor_training_cost, bool monitor_training_accuracy)
{
	using namespace std;
	unsigned int n = training_data.size();
	for (int i = 0; i < epochs; i++)
	{
		random_shuffle(training_data.begin(), training_data.end());		//打乱元素顺序
		for (int j = 0; j < n; j += mini_batch_size)
		{
			if (j + mini_batch_size < n)
			{
				vector<Matrix>::iterator i1 = training_data.begin() + j, i2 = training_data.begin() + j+ mini_batch_size; //选取部分数据进行训练
				vector<Matrix>mini_batch(i1, i2);
				update_mini_batch(mini_batch, learning_rate, lmbda, n);
			}
			printf("training %5d/%5d\r", j, n);
		}
		printf("Epoch %d training complete!\n", i);
		if (monitor_training_cost)
		{
			double cost = this->total_cost(training_data, lmbda);
			printf("Cost on training data:%lf\n", cost);
		}
		if (monitor_training_accuracy)
		{
			double accuracy = this->accuracy(training_data);
			//printf("Accuracy on training data: %.0lf / %d\n",accuracy, n);
			printf("Accuracy on training data: %2.5lf%%\n", accuracy / n * 100);
		}
		if (monitor_evaluation_cost)
		{
			double cost = this->total_cost(evaluation_data, lmbda);
			printf("Cost on evaluation data:%lf\n", cost);
		}
		if (monitor_evaluation_accuracy)
		{
			double accuracy = this->accuracy(evaluation_data);
			printf("Accuracy on evaluation data: %2.5lf%%\n", accuracy / evaluation_data.size() * 100);
		}				
		printf("\n");
	}
}

void NeuralNetwork::update_mini_batch(std::vector<Matrix>& mini_batch, double learning_rate, double lmbda, double n)
{
	std::vector<Matrix> nabla_w ;
	std::vector<Matrix> delta_nabla_w ;
	std::vector<Matrix> nabla_b;
	std::vector<Matrix> delta_nabla_b;
	//初始化为0
	for (int j = 0; j < weights.size(); j++)
	{
		Matrix t_w(weights[j], 0);
		Matrix tb_w(weights[j], 0);
		nabla_w.push_back(t_w);
		delta_nabla_w.push_back(tb_w);

		Matrix t_b(biases[j], 0);
		Matrix tb_b(biases[j], 0);
		nabla_b.push_back(t_b);
		delta_nabla_b.push_back(tb_b);
	}


	for (int i = 0; i < mini_batch.size(); i++)
	{
		backprop(mini_batch[i], delta_nabla_w, delta_nabla_b);
		for (int j = 0; j < nabla_w.size(); j++)
		{
			nabla_w[j] = nabla_w[j] + delta_nabla_w[j];		//记录每一次的偏导数的和
			nabla_b[j] = nabla_b[j] + delta_nabla_b[j];
		}
	}

	for (int j = 0; j < nabla_w.size(); j++)			//更新权重和偏向
	{
		weights[j] = ((1 - learning_rate * (lmbda / n))*weights[j]) + ((-learning_rate / mini_batch.size())*nabla_w[j]);
		biases[j] = biases[j] + ((-learning_rate / mini_batch.size())*nabla_b[j]);
	}

}


void NeuralNetwork::backprop(Matrix input, std::vector<Matrix> &nabla_w, std::vector<Matrix> &nabla_b)
{
	Matrix activation = input.transpose();	//当前要乘的a，也就是上层的输出    输入是行向量，这里改成列
	std::vector<Matrix> activations;	//所有层的a
	std::vector<Matrix> zs;				//所有层的z
	activations.push_back(activation);		//先加入一层a
	//正向传递
	for (int i = 1; i < layers.cols; i++)	//每层的运算,除去第一层
	{
		Matrix current_z;//每层有多少神经元，就有多少z和activation
		current_z = Matrix_Multiplication(weights[i - 1], activation) + biases[i - 1];	
		zs.push_back(current_z);//本层的z加入所有的z
		activation = this->activation_matrix(current_z);		//计算激活函数值
		activations.push_back(activation);//本层的a加入所有的a
	}

	//标签向量化
	Matrix y(layers.at<int>(0,layers.getLength()-1) , 1, MAT_64FC1);
	for (int i = 0; i < y.rows; i++)
	{
		y.at<double>(i, 0) = 0;
		if(i==input.lables)
			y.at<double>(i, 0) = 1;
	}

	Matrix delta = this->cost->delta(zs[zs.size() - 1], activations[activations.size() - 1], y);	//当前误差，初始化为最后一层的误差，使用交叉熵代价函数
	nabla_b[nabla_b.size() - 1] = delta;			//最后一层的偏向的偏导数就是最后一层的误差
	nabla_w[nabla_w.size() - 1] = Matrix_Multiplication(delta, activations[activations.size() - 2].transpose());//最后一层的权重是本层误差乘以上一层激活函数值

	//反向传递
	for (int i = 2; i < num_layers; i++)
	{
		Matrix current_z;
		current_z = zs[zs.size() - i];		//倒着来算  首先计算z
		Matrix sp = this->activation_deriv_matrix(current_z);	//然后根据z计算激活函数偏导
		delta = Matrix_Multiplication(weights[weights.size() - i + 1].transpose(), delta)*sp;  //下层权重和误差的乘积在和sp对应相乘就是本层权重
		nabla_b[nabla_b.size() - i] = delta;
		nabla_w[nabla_w.size() - i] = Matrix_Multiplication(delta, activations[activations.size() - i - 1].transpose());
	}

}

double NeuralNetwork::total_cost(std::vector<Matrix>& data, double lmbda)
{
	double cost = 0;
	for (int i = 0; i < data.size(); i++)
	{
		Matrix a;
		a = feedforward(data[i]);
		Matrix y(a.rows, 1, MAT_64FC1);
		for (int j = 0; j < y.rows; j++)
		{
			y.at<double>(j, 0) = 0;
			if (j == data[i].lables)
			{
				y.at<double>(j, 0) = 1;
			}
		}
		cost += this->cost->fn(a, y) / data.size();
	}
	double sum = 0;
	for (int i = 0; i < weights.size(); i++)
	{
		double num = 0;
		for (int j = 0; j < weights[i].rows; j++)
		{
			for (int k = 0; k < weights[i].cols; k++)
			{
				num += pow(weights[i].at<double>(j, k),2);
			}
		}
		num = sqrt(num);
		sum += num;
	}
	cost += sum * 0.5*(lmbda / data.size());
	return cost;
}
//前向传递
Matrix NeuralNetwork::feedforward(Matrix X)
{
	Matrix a = X.transpose();			//输入x是行向量，这要转换为列
	for (int i = 0; i < weights.size(); i++)
	{
		a = this->activation_matrix(Matrix_Multiplication(weights[i], a) + biases[i]);
	}
	return a;
}

double NeuralNetwork::accuracy(std::vector<Matrix>& data)
{
	double count = 0;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		Matrix a;
		a = feedforward(data[i]);
		double max = 0;
		int max_j = 0;
		for (int j = 0; j < a.rows; j++)
		{
			if (max < a.at<double>(j, 0))
			{
				max = a.at<double>(j, 0);
				max_j = j;
			}
		}
		if (max_j == data[i].lables)
			count++;
	}
	return count;
}




bool NeuralNetwork::SaveNetworkStructure(std::string fileName)
{
	FILE *fp;
	fp = fopen(fileName.c_str(), "wb+");
	if (fp == 0)
		return false;
	fwrite(&num_layers, sizeof(int), 1, fp);
	fwrite(layers.data, sizeof(int), num_layers, fp);
	for (int i = 0; i < num_layers - 1; i++)
	{
		fwrite(weights[i].data, sizeof(uchar), weights[i].total(), fp);
		fwrite(biases[i].data, sizeof(uchar), biases[i].total(), fp);
	}
	fclose(fp);
	return true;
}

bool NeuralNetwork::LoadNetworkStructure(std::string fileName)
{
	//FILE *fp;
	//fp = fopen(fileName.c_str(), "rb");
	//if (fp == 0)
	//	return false;
	//std::vector<std::vector<std::vector<double>>>().swap(this->weights);			//清除并释放原有内存
	//for (int i = 0; i < layers.size() - 1; i++)//每层的循环
	//{
	//	std::vector<std::vector<double>> currentLayer;
	//	//除输入层外每层每个神经元都要分配，实际分配的层从layers[1]开始
	//	for (int j = 0; j < layers[i + 1]; j++)	//当前层的每个神经元的循环
	//	{
	//		std::vector<double> currentNode;
	//		for (int k = 0; k < layers[i]; k++)//当前的权重的数量是上一层神经元的个数
	//		{
	//			double temp;
	//			fread(&temp, sizeof(double), 1, fp);
	//			currentNode.push_back(temp);			//读取权重并记录
	//		}
	//		double temp;
	//		fread(&temp, sizeof(double), 1, fp);
	//		currentNode.push_back(temp);//最后随机偏向
	//		currentLayer.push_back(currentNode);//本点加入本层
	//	}
	//	weights.push_back(currentLayer);//本层权重写入
	//}

	FILE *fp;
	fp = fopen(fileName.c_str(), "rb");
	if (fp == 0)
		return false;
	std::vector<Matrix>().swap(this->weights);			//清除并释放原有内存
	std::vector<Matrix>().swap(this->biases);			//清除并释放原有内存
	fread(&num_layers, sizeof(int), 1, fp);
	fread(layers.data, sizeof(int), num_layers, fp);
	for (int i = 1; i < num_layers; i++)
	{
		Matrix a(layers.at<int>(0, i), layers.at<int>(0, i - 1), MAT_64FC1);
		//初始化权重矩阵，行为当前层的神经元，列为上一层的神经元
		fread(a.data, sizeof(uchar), a.total(), fp);
		weights.push_back(a);
		//初始化偏向
		//每一层的偏向都是本层神经元的个数
		ColVector b(layers.at<int>(0, i), MAT_64FC1);
		fread(b.data, sizeof(uchar), b.total(), fp);
		biases.push_back(b);
	}
	fclose(fp);
	return true;
}

double logistic(double x)
{
	
	return (double)1 / (1 + exp(-x));

}
//对矩阵做非线性转换
Matrix logistic_Matrix( Matrix x)
{
	Matrix result(x.rows, x.cols, x.flag);
	for (int i = 0; i < x.rows; i++)
	{
		for (int j = 0; j < x.cols; j++)
		{
			result.at<double>(i, j) = logistic(x.at<double>(i, j));
		}
	}
	return result;
}

Matrix logistic_derivative_Matrix(Matrix x)
{
	Matrix result(x.rows, x.cols, x.flag);
	for (int i = 0; i < x.rows; i++)
	{
		for (int j = 0; j < x.cols; j++)
		{
			result.at<double>(i, j) = logistic_derivative(x.at<double>(i, j));
		}
	}
	return result;
}

double logistic_derivative(double x)
{
	double t = logistic(x);
	return t * (1 - t);
}
