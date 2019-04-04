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
	//��ȡMNIST���� ����Ϊ�˼򻯣���֤�����ǲ��Լ�
	void loadMnistData(std::vector<Matrix> &training_data, std::vector<Matrix> &validation_data);
	void loadImageAndLabel(std::vector<Matrix>& data, std::string fileNameImage, std::string fileNameLabel);
	//����ݶ��½��㷨
	void SGD(std::vector<Matrix> training_data, std::vector<Matrix> evaluation_data,
		double epochs = 10000, double mini_batch_size = 10,
		double learning_rate = 0.2, double lmbda = 0.0,
		bool monitor_evaluation_cost = false,
		bool monitor_evaluation_accuracy = false,
		bool monitor_training_cost = false,
		bool monitor_training_accuracy = false);
	//ѵ��С������
	void update_mini_batch(std::vector<Matrix> &mini_batch, double learning_rate, double lmbda, double n);

	//BackPropagation�㷨 ���ؼ���õ�w��b��ƫ�������Ա����Ȩ��
	void backprop(Matrix input, std::vector<Matrix> &nabla_w, std::vector<Matrix> &nabla_b);

	double total_cost(std::vector<Matrix> &data, double lmbda);

	Matrix feedforward(Matrix X);

	double accuracy(std::vector<Matrix> &data);
	
	std::vector<Matrix> weights;	//ÿ���Ȩ���Ǿ������е�Ȩ�����vector
	std::vector<Matrix> biases;	//ÿ���ƫ����������������ƫ�����vector
	RowVector layers;				//����ṹ ������
	int num_layers;					//�������
	CostFunction *cost;				//���ۺ�����ͳһ�ӿڣ������
	CrossEntropyCost CrossEntropy;	//�����ش��ۺ���
	QuadraticCost 	Quadratic;		//ƽ�������ۺ���

	double (*activation)(double);			//ָ����ĺ�����������ͳһ�ĵ��ýӿ�
	Matrix (*activation_matrix)(Matrix);
	Matrix (*activation_deriv_matrix)(Matrix);
	double (*activation_deriv)(double);

	
	bool SaveNetworkStructure(std::string fileName = "cx.cxnn");
	bool LoadNetworkStructure(std::string fileName = "cx.cxnn");
	double interval_mapping(double O, double O_Max, double O_Min, double N_Max, double N_Min)	//����ӳ��
	{
		double N;
		N = (N_Max - N_Min) / (O_Max - O_Min);
		N *= (O - O_Min);
		N += N_Min;
		return N;
	}
	
};



#endif // !NEURALNETWORK_H


