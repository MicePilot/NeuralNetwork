#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include "Matrix.hpp"
#include "Vec.hpp"
#include "CostFunction.h"
#include <vector>
#include <string>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm> 

static double logistic(double x);
static Matrix logistic_Matrix(Matrix x);
static Matrix logistic_derivative_Matrix(Matrix x);
static double logistic_derivative(double x);

class NeuralNetwork
{
public:
	NeuralNetwork(RowVector &layers, std::string cost="CrossEntropy", std::string fileName="");
	virtual ~NeuralNetwork();
	//读取MNIST数据 这里为了简化，验证集就是测试集
	void loadMnistData(std::vector<Matrix> &training_data, std::vector<Matrix> &validation_data);
	void loadImageAndLabel(std::vector<Matrix>& data, std::string fileNameImage, std::string fileNameLabel);
	//随机梯度下降算法
	void SGD(std::vector<Matrix> training_data, std::vector<Matrix> evaluation_data,
		double epochs = 10000, double mini_batch_size = 10,
		double learning_rate = 0.2, double lmbda = 0.0,
		bool monitor_evaluation_cost = false,
		bool monitor_evaluation_accuracy = false,
		bool monitor_training_cost = false,
		bool monitor_training_accuracy = false);
	//训练小样本集
	void update_mini_batch(std::vector<Matrix> &mini_batch, double learning_rate, double lmbda, double n);

	//BackPropagation算法 返回计算得到w和b的偏导数，以便更新权重
	void backprop(Matrix input, std::vector<Matrix> &nabla_w, std::vector<Matrix> &nabla_b);

	double total_cost(std::vector<Matrix> &data, double lmbda);

	Matrix feedforward(Matrix X);

	double accuracy(std::vector<Matrix> &data);
	
	std::vector<Matrix> weights;	//每层的权重是矩阵，所有的权重组成vector
	std::vector<Matrix> biases;	//每层的偏向是列向量，所有偏向组成vector
	RowVector layers;				//网络结构 行向量
	int num_layers;					//网络层数
	CostFunction *cost;				//代价函数的统一接口，虚基类
	CrossEntropyCost CrossEntropy;	//交叉熵代价函数
	QuadraticCost 	Quadratic;		//平方误差代价函数

	double (*activation)(double);			//指向本类的函数，这里是统一的调用接口
	Matrix (*activation_matrix)(Matrix);
	Matrix (*activation_deriv_matrix)(Matrix);
	double (*activation_deriv)(double);

	
	bool SaveNetworkStructure(std::string fileName = "cx.cxnn");
	bool LoadNetworkStructure(std::string fileName = "cx.cxnn");
	double interval_mapping(double O, double O_Max, double O_Min, double N_Max, double N_Min)	//区间映射
	{
		double N;
		N = (N_Max - N_Min) / (O_Max - O_Min);
		N *= (O - O_Min);
		N += N_Min;
		return N;
	}
	
};



#endif // !NEURALNETWORK_H


