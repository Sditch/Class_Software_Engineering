#<center>线性滤波</center>
##一、分析题意、确定思路：

* 将图像分割成一个个孤立的点，按位置二重循环读取其像素值
* 以当前点为中心，按卷积核的大小取相邻点的像素值（3x3 或 5x5）存入数组
* 将存入的数组和卷积核数组按位置依次对应乘积并求和
* 求和的像素值便为当前点的像素值，写入并生成图像


##二、主要代码：

经过分析发现，代码需要五重循环：

* 一二重循环确定当前图像像素点位置
* 第三重循环为波段bandNum三种
* 四五重循环确定当前位置的像素点对应的数组

以卷积核一为例：

注意：

###1、数组求和定义类型为float类型，因为存在小数运算<br>
最开始按思路写出输出图像全黑，后来才发现乘积小数小于1时，类型转换为0了
###2、float类型是重点
###3、注意读取当前像素点[i, j]的相邻区域时，相邻区域点的坐标为[i+a-1, j+b-1]

算法：

	// 数组求和
	float sum[1] = { 0 };
	for (int i = 1; i < imgYlen-1; ++i)
	{
		for (int j = 1; j < imgXlen-1; ++j)
		{
			for (int k = 0; k < bandNum; ++k)
			{
				sum[0] = 0.0;
					// for循环求和---每个像素点的像素值
				for (int a = 0; a < array; ++a)
				{
					for (int b = 0; b < array; ++b)
					{
						//读取数组的像素点
						poSrcDS->GetRasterBand(k + 1)->RasterIO(GF_Read,
							i + a - array / 2, j + b - array / 2, 1, 1, buffTmp, 1, 1, GDT_Float32, 0, 0);
						buff[a][b] = buffTmp[0];
						sum[0] += buff[a][b] * buffArray[a][b];
						//cout << buff[a][b] << "   " << sum[0] << endl;
					}
				}
				poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
					i, j, 1, 1, sum, 1, 1, GDT_Float32, 0, 0);
			}

		}
	}

##三、细节问题
###1、所求卷积核类型分为两类（3x3和5x5），为了简化代码，实现代码复用，高内聚低耦合，将算法重新写入一个子函数，方便六种卷积核分别调用
###2、为了统一调用，我将所有数组定义为5x5大小，可是3x3类型的卷积核出现问题，原因在于：数组计算是从左到右依次进行的，每行应该补上空缺的2个数0，否则使用的第二行第一个数可能是实际的第二行第3个数
定义示例：

	float buffArray_1[5][5] = {
		0, 1 * 0.2, 0, 0, 0,
		1 * 0.2, 1 * 0.2, 1 * 0.2, 0, 0,
		0, 1 * 0.2, 0, 0, 0
	};
###3、忘记卷积核六求和后的除以25，导致实验结果的不一致
###4、重点问题：后来运行老师代码发现实验结果与我的结果不一致，看老师代码才发现是float类型的原因，折腾了好久。。。这意味着代码、截图、博客全部需要改正
##四、完整代码

	#include <iostream>
	using namespace std;
	#include "./gdal/gdal_priv.h"
	#pragma comment(lib, "gdal_i.lib")
	
	void inputIMG(int imgXlen, int imgYlen, int bandNum, int array, float buffArray[][5], GDALDataset* poSrcDS, GDALDataset* poDstDS, float* buffTmp, float buff[][5])
	{
		// 数组求和float类型
		float sum[1] = { 0.0 };
		// 多个for循环嵌套
		for (int i = array / 2; i < imgYlen - array / 2; ++i)
		{
			for (int j = array / 2; j < imgXlen - array / 2; ++j)
			{
				for (int k = 0; k < bandNum; ++k)
				{
					sum[0] = 0.0;
					// for循环求和---每个像素点的像素值
					for (int a = 0; a < array; ++a)
					{
						for (int b = 0; b < array; ++b)
						{
							//读取数组的像素点
							poSrcDS->GetRasterBand(k + 1)->RasterIO(GF_Read,
								i + a - array / 2, j + b - array / 2, 1, 1, buffTmp, 1, 1, GDT_Float32, 0, 0);
							buff[a][b] = buffTmp[0];
							sum[0] += buff[a][b] * buffArray[a][b];
							//cout << buff[a][b] << "   " << sum[0] << endl;
						}
					}
	
					poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
						i, j, 1, 1, sum, 1, 1, GDT_Float32, 0, 0);
				}
			}
		}
	}
	
	
	int main()
	{
		// 输入头像
		GDALDataset* poSrcDS;
		// Êä³öÍ·Ïñ
		GDALDataset* poDstDS_1;
		GDALDataset* poDstDS_2;
		GDALDataset* poDstDS_3;
		GDALDataset* poDstDS_4;
		GDALDataset* poDstDS_5;
		GDALDataset* poDstDS_6;
		// 图像的宽度和高度
		int imgXlen, imgYlen;
		// 输入图像路径
		char* srcPath = "lena.jpg";
		// 输出图像路径----卷积核一
		char* dstPath_1 = "newlena1_1.tif";
		char* dstPath_2 = "newlena2_1.tif";
		char* dstPath_3 = "newlena3_1.tif";
		char* dstPath_4 = "newlena4_1.tif";
		char* dstPath_5 = "newlena5_1.tif";
		char* dstPath_6 = "newlena6_1.tif";
		// 图像内存存储
		float* buffTmp;
		// 图像波段数
		int bandNum;
	
		// 注册驱动
		GDALAllRegister();
	
		// 打开图像
		poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	
		// 获取图像宽度，高度和波段数量
		imgXlen = poSrcDS->GetRasterXSize();
		imgYlen = poSrcDS->GetRasterYSize();
		bandNum = poSrcDS->GetRasterCount();
		// 输出获取的结果
		cout << " Image X Length: " << imgXlen << endl;
		cout << " Image Y Length: " << imgYlen << endl;
		cout << " Band number :   " << bandNum << endl;
	
		// 创建输出图像
		poDstDS_1 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
			dstPath_1, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		poDstDS_2 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
			dstPath_2, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		poDstDS_3 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
			dstPath_3, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		poDstDS_4 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
			dstPath_4, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		poDstDS_5 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
			dstPath_5, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		poDstDS_6 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
			dstPath_6, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	
		// 根据图像的宽度和高度分配内存
		buffTmp = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
		// 卷积核数组，记录数组维度
		float buffArray_1[5][5] = {
			0, 1 * 0.2, 0, 0, 0,
			1 * 0.2, 1 * 0.2, 1 * 0.2, 0, 0,
			0, 1 * 0.2, 0, 0, 0
		};
		int array_1 = 3;
		float buffArray_2[5][5] = {
			1 * 0.2, 0, 0, 0, 0,
			0, 1 * 0.2, 0, 0 ,0,
			0, 0, 1 * 0.2, 0, 0,
			0, 0, 0, 1 * 0.2, 0,
			0, 0, 0, 0, 1 * 0.2
		};
		int array_2 = 5;
		float buffArray_3[5][5] = {
			-1, -1, -1, 0, 0,
			-1, 8, -1, 0, 0,
			-1, -1, -1, 0, 0
		};
		int array_3 = 3;
		float buffArray_4[5][5] = {
			-1, -1, -1, 0, 0,
			-1, 9, -1, 0, 0,
			-1, -1, -1, 0, 0
		};
		int array_4 = 3;
		float buffArray_5[5][5] = {
			-1, -1, 0, 0, 0,
			-1, 0, 1, 0, 0,
			0, 1, 1, 0, 0
		};
		int array_5 = 3;
		float buffArray_6[5][5] = {
			0.0120 / 25, 0.1253 / 25, 0.2736 / 25, 0.1253 / 25, 0.0120 / 25,
			0.1253 / 25, 1.3054 / 25, 2.8514 / 25, 1.3054 / 25, 0.1253 / 25,
			0.2736 / 25, 2.8514 / 25, 6.2279 / 25, 2.8514 / 25, 0.2736 / 25,
			0.1253 / 25, 1.3054 / 25, 2.8514 / 25, 1.3054 / 25, 0.1253 / 25,
			0.0120 / 25, 0.1253 / 25, 0.2736 / 25, 0.1253 / 25, 0.0120 / 25
		};
		int array_6 = 5;
		// 开创数组用于存储像素点，方便后面数组乘机
		float buff[5][5];
	
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < 5; ++j)
			{
				buff[i][j] = 0.0;
			}
		}
	
		// 函数调用
		inputIMG(imgXlen, imgYlen, bandNum, array_1, buffArray_1, poSrcDS, poDstDS_1, buffTmp, buff);
		cout << "卷积核__1......" << endl;
		inputIMG(imgXlen, imgYlen, bandNum, array_2, buffArray_2, poSrcDS, poDstDS_2, buffTmp, buff);
		cout << "卷积核__2......" << endl;
		inputIMG(imgXlen, imgYlen, bandNum, array_3, buffArray_3, poSrcDS, poDstDS_3, buffTmp, buff);
		cout << "卷积核__3......" << endl;
		inputIMG(imgXlen, imgYlen, bandNum, array_4, buffArray_4, poSrcDS, poDstDS_4, buffTmp, buff);
		cout << "卷积核__4......" << endl;
		inputIMG(imgXlen, imgYlen, bandNum, array_5, buffArray_5, poSrcDS, poDstDS_5, buffTmp, buff);
		cout << "卷积核__5......" << endl;
		inputIMG(imgXlen, imgYlen, bandNum, array_6, buffArray_6, poSrcDS, poDstDS_6, buffTmp, buff);
		cout << "卷积核__6......" << endl;
		// 清除内存
		CPLFree(buffTmp);
		// 关闭dataset
		GDALClose(poDstDS_1);
		GDALClose(poDstDS_2);
		GDALClose(poDstDS_3);
		GDALClose(poDstDS_4);
		GDALClose(poDstDS_5);
		GDALClose(poDstDS_6);
		GDALClose(poSrcDS);
	
		system("PAUSE");
		return 0;
	}

##五、实验结果

* newlena1.tif------均值模糊:

![](https://i.imgur.com/Fr1IoBj.png)


* newlena2.tif------运动模糊:

![](https://i.imgur.com/Jt5ACn9.png)

* newlena3.tif------边缘检测:

![](https://i.imgur.com/HE3WM8s.png)


* newlena4.tif------图像锐化滤波器:

![](https://i.imgur.com/WQPIzZG.png)

* newlena5.tif------浮雕:

![](https://i.imgur.com/2Uhtu3D.png)

* newlena6.tif------高斯模糊:

![](https://i.imgur.com/VB6xmPk.png)
