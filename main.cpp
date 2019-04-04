#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "NeuralNetwork.h"
using namespace std;


int main()
{

	RowVector lay(3, MAT_32SC1);
	lay.at<int>(0, 0) = 784;
	lay.at<int>(0, 1) = 30;
	lay.at<int>(0, 2) = 10;

	NeuralNetwork NN(lay);
	std::vector<Matrix> training_data, validation_data;
	NN.loadMnistData(training_data, validation_data);
	NN.LoadNetworkStructure();
	/*NN.SGD(training_data, validation_data, 30, 10, 0.5, 5, true, true, true, true);

	if (NN.SaveNetworkStructure())
	{
		cout << "save success!" << endl;
	}*/

	while (1)
	{
		cout << "预测第几个图片：" << endl;
		int i = 0;
		cin >> i;
		Matrix a = NN.feedforward(validation_data[i]);
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
		cout << "预测为：" << max_j << endl;
		cout << "实际为：" << validation_data[i].lables << endl;
	}

	system("pause");
	return 0;
}
