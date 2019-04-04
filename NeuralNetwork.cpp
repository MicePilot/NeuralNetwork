#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(RowVector &layers, std::string cost,std::string fileName):num_layers(layers.cols)
{
	
	this->activation = logistic;
	this->activation_matrix = logistic_Matrix;
	this->activation_deriv_matrix = logistic_derivative_Matrix;
	this->activation_deriv = logistic_derivative;
	
	this->layers = layers;
	this->num_layers = layers.getLength();
	
	//���ش��ۺ���
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

	if (fileName.empty())			//û�и��ļ����֣����������Ȩ��
	{
		srand((unsigned)time(NULL));
		//�������Ȩ���Լ�ƫ��
		//��һ��Ϊ����㣬������
		
		for (int i = 1; i < num_layers; i++)//����ÿ���ѭ�� 
		{
			//ÿһ���������Ԫ��Ȩ�ض�����һ��������Ԫ�ĸ���������������ԪΪn�����ϲ�Ϊm����
			//����n*m�ľ���ͨ��Ϊ1
			Matrix a(layers.at<int>(0, i), layers.at<int>(0, i - 1), MAT_64FC1);
			//��ʼ��Ȩ�ؾ�����Ϊ��ǰ�����Ԫ����Ϊ��һ�����Ԫ
			a.InitMatData(layers.at<int>(0, i - 1));
			weights.push_back(a);
			//��ʼ��ƫ��
			//ÿһ���ƫ���Ǳ�����Ԫ�ĸ���
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
			file_label.read((char*)&label, sizeof(label));		//��ȡ��ǩ
			
			//Matrix temp(n_rows, n_cols, MAT_64FC1);				//��������
			Matrix temp(1, n_cols*n_rows, MAT_64FC1);			//��������,����Ϊ�˺������㣬ѡ�õ��еľ���
			temp.lables = label;								//д���ǩ
			for (int r = 0; r < n_rows; r++)
			{
				for (int c = 0; c < n_cols; c++)
				{
					unsigned char image = 0;
					file_image.read((char*)&image, sizeof(image));//��ȡ����
					//tp.push_back((double)image / 255.0);
					temp.at<double>(0, r*n_cols + c) = (double)image / 255.0;
					//*(double*)(temp.data + r * n_cols*1*sizeof(double) + c * 1 * sizeof(double)) = (double)image / 255.0;//д������
				}
			}
			data.push_back(temp);		//���뼯��
		}
	}

}

void NeuralNetwork::SGD(std::vector<Matrix> training_data, std::vector<Matrix> evaluation_data, double epochs, double mini_batch_size, double learning_rate, double lmbda, bool monitor_evaluation_cost, bool monitor_evaluation_accuracy, bool monitor_training_cost, bool monitor_training_accuracy)
{
	using namespace std;
	unsigned int n = training_data.size();
	for (int i = 0; i < epochs; i++)
	{
		random_shuffle(training_data.begin(), training_data.end());		//����Ԫ��˳��
		for (int j = 0; j < n; j += mini_batch_size)
		{
			if (j + mini_batch_size < n)
			{
				vector<Matrix>::iterator i1 = training_data.begin() + j, i2 = training_data.begin() + j+ mini_batch_size; //ѡȡ�������ݽ���ѵ��
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
	//��ʼ��Ϊ0
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
			nabla_w[j] = nabla_w[j] + delta_nabla_w[j];		//��¼ÿһ�ε�ƫ�����ĺ�
			nabla_b[j] = nabla_b[j] + delta_nabla_b[j];
		}
	}

	for (int j = 0; j < nabla_w.size(); j++)			//����Ȩ�غ�ƫ��
	{
		weights[j] = ((1 - learning_rate * (lmbda / n))*weights[j]) + ((-learning_rate / mini_batch.size())*nabla_w[j]);
		biases[j] = biases[j] + ((-learning_rate / mini_batch.size())*nabla_b[j]);
	}

}


void NeuralNetwork::backprop(Matrix input, std::vector<Matrix> &nabla_w, std::vector<Matrix> &nabla_b)
{
	Matrix activation = input.transpose();	//��ǰҪ�˵�a��Ҳ�����ϲ�����    ������������������ĳ���
	std::vector<Matrix> activations;	//���в��a
	std::vector<Matrix> zs;				//���в��z
	activations.push_back(activation);		//�ȼ���һ��a
	//���򴫵�
	for (int i = 1; i < layers.cols; i++)	//ÿ�������,��ȥ��һ��
	{
		Matrix current_z;//ÿ���ж�����Ԫ�����ж���z��activation
		current_z = Matrix_Multiplication(weights[i - 1], activation) + biases[i - 1];	
		zs.push_back(current_z);//�����z�������е�z
		activation = this->activation_matrix(current_z);		//���㼤���ֵ
		activations.push_back(activation);//�����a�������е�a
	}

	//��ǩ������
	Matrix y(layers.at<int>(0,layers.getLength()-1) , 1, MAT_64FC1);
	for (int i = 0; i < y.rows; i++)
	{
		y.at<double>(i, 0) = 0;
		if(i==input.lables)
			y.at<double>(i, 0) = 1;
	}

	Matrix delta = this->cost->delta(zs[zs.size() - 1], activations[activations.size() - 1], y);	//��ǰ����ʼ��Ϊ���һ�����ʹ�ý����ش��ۺ���
	nabla_b[nabla_b.size() - 1] = delta;			//���һ���ƫ���ƫ�����������һ������
	nabla_w[nabla_w.size() - 1] = Matrix_Multiplication(delta, activations[activations.size() - 2].transpose());//���һ���Ȩ���Ǳ�����������һ�㼤���ֵ

	//���򴫵�
	for (int i = 2; i < num_layers; i++)
	{
		Matrix current_z;
		current_z = zs[zs.size() - i];		//��������  ���ȼ���z
		Matrix sp = this->activation_deriv_matrix(current_z);	//Ȼ�����z���㼤���ƫ��
		delta = Matrix_Multiplication(weights[weights.size() - i + 1].transpose(), delta)*sp;  //�²�Ȩ�غ����ĳ˻��ں�sp��Ӧ��˾��Ǳ���Ȩ��
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
//ǰ�򴫵�
Matrix NeuralNetwork::feedforward(Matrix X)
{
	Matrix a = X.transpose();			//����x������������Ҫת��Ϊ��
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
	//std::vector<std::vector<std::vector<double>>>().swap(this->weights);			//������ͷ�ԭ���ڴ�
	//for (int i = 0; i < layers.size() - 1; i++)//ÿ���ѭ��
	//{
	//	std::vector<std::vector<double>> currentLayer;
	//	//���������ÿ��ÿ����Ԫ��Ҫ���䣬ʵ�ʷ���Ĳ��layers[1]��ʼ
	//	for (int j = 0; j < layers[i + 1]; j++)	//��ǰ���ÿ����Ԫ��ѭ��
	//	{
	//		std::vector<double> currentNode;
	//		for (int k = 0; k < layers[i]; k++)//��ǰ��Ȩ�ص���������һ����Ԫ�ĸ���
	//		{
	//			double temp;
	//			fread(&temp, sizeof(double), 1, fp);
	//			currentNode.push_back(temp);			//��ȡȨ�ز���¼
	//		}
	//		double temp;
	//		fread(&temp, sizeof(double), 1, fp);
	//		currentNode.push_back(temp);//������ƫ��
	//		currentLayer.push_back(currentNode);//������뱾��
	//	}
	//	weights.push_back(currentLayer);//����Ȩ��д��
	//}

	FILE *fp;
	fp = fopen(fileName.c_str(), "rb");
	if (fp == 0)
		return false;
	std::vector<Matrix>().swap(this->weights);			//������ͷ�ԭ���ڴ�
	std::vector<Matrix>().swap(this->biases);			//������ͷ�ԭ���ڴ�
	fread(&num_layers, sizeof(int), 1, fp);
	fread(layers.data, sizeof(int), num_layers, fp);
	for (int i = 1; i < num_layers; i++)
	{
		Matrix a(layers.at<int>(0, i), layers.at<int>(0, i - 1), MAT_64FC1);
		//��ʼ��Ȩ�ؾ�����Ϊ��ǰ�����Ԫ����Ϊ��һ�����Ԫ
		fread(a.data, sizeof(uchar), a.total(), fp);
		weights.push_back(a);
		//��ʼ��ƫ��
		//ÿһ���ƫ���Ǳ�����Ԫ�ĸ���
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
//�Ծ�����������ת��
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
