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

###1、数组求和定义类型为double类型，因为存在小数运算<br>
最开始按思路写出输出图像全黑，后来才发现乘积小数小于1时，类型转换为0了
###2、double类型运算后需要强制类型转换为GByte，否则类型不符容易报错
###3、注意读取当前像素点[i, j]的相邻区域时，相邻区域点的坐标为[i+a-1, j+b-1]

算法：

	// 数组求和
	double sum[1] = { 0 };
	for (int i = 1; i < imgYlen-1; ++i)
	{
		for (int j = 1; j < imgXlen-1; ++j)
		{
			for (int k = 0; k < bandNum; ++k)
			{
				sum[0] = 0;
				for (int a = 0; a < 3; ++a)
				{
					for (int b = 0; b < 3; ++b)
					{
						poSrcDS->GetRasterBand(k + 1)->RasterIO(GF_Read,
							i + a - 1, j + b - 1, 1, 1, buffTmp, 1, 1, GDT_Byte, 0, 0);
						buff[a][b] = buffTmp[0];
						sum[0] += (double)buff[a][b] * buffArray[a][b];
						//cout << buff[a][b] << "   " << sum[0] << endl;
					}
				}
				GByte pre[1];
				pre[0] = (GByte)sum[0];
				poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
					i, j, 1, 1, pre, 1, 1, GDT_Byte, 0, 0);
			}

		}
	}

##三、细节问题
###1、所求卷积核类型分为两类（3x3和5x5），为了简化代码，实现代码复用，高内聚低耦合，将算法重新写入一个子函数，方便六种卷积核分别调用
###2、为了统一调用，我将所有数组定义为5x5大小，可是3x3类型的卷积核出现问题，原因在于：数组计算是从左到右依次进行的，每行应该补上空缺的2个数0，否则使用的第二行第一个数可能是实际的第二行第3个数
定义示例：

	double buffArray_1[5][5] = {
		0, 1 * 0.2, 0, 0, 0,
		1 * 0.2, 1 * 0.2, 1 * 0.2, 0, 0,
		0, 1 * 0.2, 0, 0, 0
	};
###3、忘记卷积核六求和后的除以25，导致实验结果的不一致
##四、完整代码

	#include <iostream>
	using namespace std;
	#include "./gdal/gdal_priv.h"
	#pragma comment(lib, "gdal_i.lib")
	
	void inputIMG(int imgXlen, int imgYlen, int bandNum, int array, double buffArray[][5], GDALDataset* poSrcDS, GDALDataset* poDstDS, GByte* buffTmp, GByte buff[][5])
	{
		// 数组求和double类型
		double sum[1] = { 0.0 };
		// 数组求和GByte类型
		GByte pre[1] = { 0 };
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
								i + a - array / 2, j + b - array / 2, 1, 1, buffTmp, 1, 1, GDT_Byte, 0, 0);
							buff[a][b] = buffTmp[0];
							sum[0] += (double)buff[a][b] * buffArray[a][b];
							//cout << buff[a][b] << "   " << sum[0] << endl;
						}
					}
	
					pre[0] = (GByte)sum[0];
					poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
						i, j, 1, 1, pre, 1, 1, GDT_Byte, 0, 0);
				}
			}
		}
	}
	
	
	int main()
	{
		// 输入头像
		GDALDataset* poSrcDS;
		// 输出头像
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
		char* dstPath_1 = "newlena1.tif";
		char* dstPath_2 = "newlena2.tif";
		char* dstPath_3 = "newlena3.tif";
		char* dstPath_4 = "newlena4.tif";
		char* dstPath_5 = "newlena5.tif";
		char* dstPath_6 = "newlena6.tif";
		// 图像内存存储
		GByte* buffTmp;
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
		buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
		// 卷积核数组，记录数组维度
		double buffArray_1[5][5] = {
			0, 1 * 0.2, 0, 0, 0,
			1 * 0.2, 1 * 0.2, 1 * 0.2, 0, 0,
			0, 1 * 0.2, 0, 0, 0
		};
		int array_1 = 3;
		double buffArray_2[5][5] = {
			1 * 0.2, 0, 0, 0, 0,
			0, 1 * 0.2, 0, 0 ,0,
			0, 0, 1 * 0.2, 0, 0,
			0, 0, 0, 1 * 0.2, 0,
			0, 0, 0, 0, 1 * 0.2
		};
		int array_2 = 5;
		double buffArray_3[5][5] = {
			-1, -1, -1, 0, 0,
			-1, 8, -1, 0, 0,
			-1, -1, -1, 0, 0
		};
		int array_3 = 3;
		double buffArray_4[5][5] = {
			-1, -1, -1, 0, 0,
			-1, 9, -1, 0, 0,
			-1, -1, -1, 0, 0
		};
		int array_4 = 3;
		double buffArray_5[5][5] = {
			-1, -1, 0, 0, 0,
			-1, 0, -1, 0, 0,
			0, -1, -1, 0, 0
		};
		int array_5 = 3;
		double buffArray_6[5][5] = {
			0.0120 / 25, 0.1253 / 25, 0.2736 / 25, 0.1253 / 25, 0.0120 / 25,
			0.1253 / 25, 1.3054 / 25, 2.8514 / 25, 1.3054 / 25, 0.1253 / 25,
			0.2736 / 25, 2.8514 / 25, 6.2279 / 25, 2.8514 / 25, 0.2736 / 25,
			0.1253 / 25, 1.3054 / 25, 2.8514 / 25, 1.3054 / 25, 0.1253 / 25,
			0.0120 / 25, 0.1253 / 25, 0.2736 / 25, 0.1253 / 25, 0.0120 / 25
		};
		int array_6 = 5;
		// 开创数组用于存储像素点，方便后面数组乘机
		GByte buff[5][5];
	
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < 5; ++j)
			{
				buff[i][j] = 0;
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

![](https://i.imgur.com/8PF8fip.png)


* newlena2.tif------运动模糊:

![](https://i.imgur.com/8jCd81x.jpg)

* newlena3.tif------边缘检测:

![](https://i.imgur.com/B28YI2J.jpg)


* newlena4.tif------图像锐化滤波器:

![](https://i.imgur.com/U1Y3Erg.jpg)

* newlena5.tif------浮雕:

![](https://i.imgur.com/yKBaQO1.jpg)

* newlena6.tif------高斯模糊:

![](https://i.imgur.com/XN3j7xR.jpg)