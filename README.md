NN 为神经网络的对象

SGD 为训练

LoadNetworkStructure() 为读取训练模型

feedforward 为识别

# 编译运行
1. 新建 build 文件夹，并进入
2. cmake ..
3. make
4. 拷贝 cx.cxnn t10k-images.idx3-ubyte t10k-labels.idx1-ubyte train-images.idx3-ubyte train-labels.idx1-ubyte 到 build 下
5. 运行

# 显示图片
按照 MNIST 数据集将图片数据读取内存，MNIST 是灰度图片，如果要显示，可以使用 opencv 将数据直接放入 mat 对象中显示，这里不再进行展示
