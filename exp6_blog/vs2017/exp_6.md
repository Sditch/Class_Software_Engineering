# <center>遥感图像的分块处理</center>

### 一、实验内容

1、根据上周学习的IHS方法，对宽幅遥感图像分别使用两种分块方式进行融合：

​      第一种方式：使用256 * 256 大小的块；

​      第二种方式：每块的宽度为图像宽度，高度为256像素。

2、比较两种分块方式的处理效率。

### 二、实验步骤

1、根据实验五的代码进行修改，完成图像的第一种方式和第二种方式（实际思路一样，代码其实可以定义函数简化），进行分块处理。细节性很重要。特别是分配空间初始化时容易产生很多细节性错误，导致代码不能正常运行。

2、完成代码，标记打印输出块，在使用第一种方式 256 * 256 输出图像代码块__处理状态__（如下图）时，观察控制台，发现在 __每一行的最后一个图像块__ 和__下一行的第一个图像块__输出之间有较大延迟。

分析觉得，__指针移动花费了很多时间__

![](http://ww1.sinaimg.cn/large/005JydxZly1fxmtraf8vpj30gr0tcdk1.jpg)

3、为了比较两种处理方式的差异，加入时间控制

```C++
startTime = clock();
​```
//创建内存，运行算法，释放内存
​```
endTime = clock();
cout << "用时：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << " s " << endl;
```

在同一运行状态下，时间比较如下图：

![](http://ww1.sinaimg.cn/large/005JydxZly1fxmu374xgmj30l10ea758.jpg)

即比较发现：

​	分成 256 x 256 的图像块进行图像处理时花费巨大时间。而将图像分成横条横条图像块时节约了大量时间。

### 三、实验代码

``` c++ #include <iostream>
#include <cmath>
#include <time.h>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")


int main()
{
	// 设立时间，比较两种运行
	clock_t startTime, endTime;
	// 输入头像
	GDALDataset* poSrcDS_MUL;
	GDALDataset* poSrcDS_PAN;
	// 输出图像
	GDALDataset* poDstDS_256;
	GDALDataset* poDstDS_256x256;
	// 图像的宽度和高度
	int imgXlen = 0, imgYlen = 0;
	// 输入图像路径
	char* srcPath_MUL = "Mul_large.tif";
	char* srcPath_PAN = "Pan_large.tif";
	// 输出图像路径
	char* dstPath_256 = "FUS_large_256.tif";
	char* dstPath_256x256 = "FUS_large_256x256.tif";
	// 图像波段数
	int bandNum;

	// 注册驱动
	GDALAllRegister();

	// 打开图像
	poSrcDS_MUL = (GDALDataset*)GDALOpenShared(srcPath_MUL, GA_ReadOnly);
	poSrcDS_PAN = (GDALDataset*)GDALOpenShared(srcPath_PAN, GA_ReadOnly);

	// 获取图像宽度，高度和波段数量
	imgXlen = poSrcDS_MUL->GetRasterXSize();
	imgYlen = poSrcDS_MUL->GetRasterYSize();
	bandNum = poSrcDS_MUL->GetRasterCount();
	// 输出获取的结果
	cout << " Image X Length: " << imgXlen << endl;
	cout << " Image Y Length: " << imgYlen << endl;
	cout << " Band number :   " << bandNum << endl;

	// 横行块数、纵向块数
	int m = imgXlen / 256;
	int n = imgYlen / 256;

	// 创建输出图像
	poDstDS_256 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath_256, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	poDstDS_256x256 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath_256x256, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	// RGB
	float *R;
	float *G;
	float *B;
	float **trans_in;
	float **trans_res;
	float **trans_res_end;

	float trans1[3][3] = {
		1 / 3, 1 / 3, 1 / 3,
		-1 * sqrtf(2) / 6, -1 * sqrtf(2) / 6, sqrtf(2) / 3,
		1 / sqrtf(2), -1 / sqrtf(2), 0 };

	float trans2[3][3] = {
		1, -1 / sqrtf(2), 1 / sqrtf(2),
		1, -1 / sqrtf(2), -1 / sqrtf(2),
		1, sqrtf(2), 0 };
	
	// 256 x 长度imgXlen	分块方式2
	startTime = clock();
	trans_in = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_in[i] = (float*)CPLMalloc(imgXlen * 256 * sizeof(float));
	}

	trans_res = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_res[i] = (float*)CPLMalloc(imgXlen * 256 * sizeof(float));
	}

	trans_res_end = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_res_end[i] = (float*)CPLMalloc(imgXlen * 256 * sizeof(float));
	}

	R = (float*)CPLMalloc(imgXlen*256 * sizeof(float));
	G = (float*)CPLMalloc(imgXlen*256 * sizeof(float));
	B = (float*)CPLMalloc(imgXlen*256 * sizeof(float));

	// 循环，分块处理
	cout << "分成 256x长度 大小:" << endl;
	for (int p = 0; p < n; p++)
	{
		//cout << "第" << p << "个图像块开始......" << endl;
		//读取数组的像素点
		poSrcDS_MUL->GetRasterBand(1)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			R, imgXlen, 256, GDT_Float32, 0, 0);

		poSrcDS_MUL->GetRasterBand(2)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			G, imgXlen, 256, GDT_Float32, 0, 0);

		poSrcDS_MUL->GetRasterBand(3)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			B, imgXlen, 256, GDT_Float32, 0, 0);
		// trans_in的标记
		for (int i = 0; i < imgXlen * 256; i++)
		{
			trans_in[0][i] = R[i];
			trans_in[1][i] = G[i];
			trans_in[2][i] = B[i];
		}

		// 进行 RGB ==> IHS 正变换
		for (int i = 0; i < imgXlen * 256; i++)
		{
			for (int j = 0; j < bandNum; j++)
			{
				float sum = 0;
				for (int k = 0; k < bandNum; k++)
				{
					sum += trans_in[k][i] * trans1[j][k];
				}
				trans_res[j][i] = sum;
			}
		}

		// 利用 PAN 替换 I 分量
		poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			trans_res[0], imgXlen, 256, GDT_Float32, 0, 0);

		// 进行 IHS == > RGB 反变换
		for (int i = 0; i < imgXlen * 256; i++)
		{
			for (int j = 0; j < bandNum; j++)
			{
				float sum = 0;
				for (int k = 0; k < bandNum; k++)
				{
					sum += trans_res[k][i] * trans2[j][k];
				}
				trans_res_end[j][i] = sum;
			}
		}

		// 把变换结果输出
		for (int i = 0; i < bandNum; i++)
		{
			poDstDS_256->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 256 * p, imgXlen, 256,
				trans_res_end[i], imgXlen, 256, GDT_Float32, 0, 0);
		}
	}
	CPLFree(R);
	CPLFree(G);
	CPLFree(B);
	for (int i = 0; i < bandNum; i++)
	{
		CPLFree(trans_in[i]);
		CPLFree(trans_res[i]);
		CPLFree(trans_res_end[i]);
	}
	CPLFree(trans_in);
	CPLFree(trans_res);
	CPLFree(trans_res_end);
	endTime = clock();
	cout << "用时：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << " s " << endl;

	// 256 x 256	分块方式1
	startTime = clock();
	trans_in = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_in[i] = (float*)CPLMalloc(256 * 256 * sizeof(float));
	}

	trans_res = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_res[i] = (float*)CPLMalloc(256 * 256 * sizeof(float));
	}

	trans_res_end = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_res_end[i] = (float*)CPLMalloc(256 * 256 * sizeof(float));
	}

	R = (float*)CPLMalloc(256 * 256 * sizeof(float));
	G = (float*)CPLMalloc(256 * 256 * sizeof(float));
	B = (float*)CPLMalloc(256 * 256 * sizeof(float));


	// 循环，分块处理
	cout << "分成 256x256 大小:" << endl;
	for (int p = 0; p < n; p++)
	{
		for (int q = 0; q < m; q++)
		{
			//cout << "第" << p << "行、第" << q << "个图像块开始......" << endl;
			//读取数组的像素点
			poSrcDS_MUL->GetRasterBand(1)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				R, 256, 256, GDT_Float32, 0, 0);

			poSrcDS_MUL->GetRasterBand(2)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				G, 256, 256, GDT_Float32, 0, 0);

			poSrcDS_MUL->GetRasterBand(3)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				B, 256, 256, GDT_Float32, 0, 0);
			// trans_in的标记
			for (int i = 0; i < 256 * 256; i++)
			{
				trans_in[0][i] = R[i];
				trans_in[1][i] = G[i];
				trans_in[2][i] = B[i];
			}

			// 进行 RGB ==> IHS 正变换
			for (int i = 0; i < 256 * 256; i++)
			{
				for (int j = 0; j < bandNum; j++)
				{
					float sum = 0;
					for (int k = 0; k < bandNum; k++)
					{
						sum += trans_in[k][i] * trans1[j][k];
					}
					trans_res[j][i] = sum;
				}
			}

			// 利用 PAN 替换 I 分量
			poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				trans_res[0], 256, 256, GDT_Float32, 0, 0);

			// 进行 IHS == > RGB 反变换
			for (int i = 0; i < 256 * 256; i++)
			{
				for (int j = 0; j < bandNum; j++)
				{
					float sum = 0;
					for (int k = 0; k < bandNum; k++)
					{
						sum += trans_res[k][i] * trans2[j][k];
					}
					trans_res_end[j][i] = sum;
				}
			}

			// 把变换结果输出
			for (int i = 0; i < bandNum; i++)
			{
				poDstDS_256x256->GetRasterBand(i + 1)->RasterIO(GF_Write, 256 * q, 256 * p, 256, 256,
					trans_res_end[i], 256, 256, GDT_Float32, 0, 0);
			}
		}
	}
	CPLFree(R);
	CPLFree(G);
	CPLFree(B);
	for (int i = 0; i < bandNum; i++)
	{
		CPLFree(trans_in[i]);
		CPLFree(trans_res[i]);
		CPLFree(trans_res_end[i]);
	}
	CPLFree(trans_in);
	CPLFree(trans_res);
	CPLFree(trans_res_end);
	endTime = clock();
	cout << "用时：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << " s " << endl;

	
	// 关闭dataset
	GDALClose(poSrcDS_MUL);
	GDALClose(poSrcDS_PAN);
	GDALClose(poDstDS_256);
	GDALClose(poDstDS_256x256);

	system("PAUSE");
	return 0;
}	
```

### 四、实验结果

第一种方式 256 x 256 图像块：

![](http://ww1.sinaimg.cn/large/005JydxZly1fxmums3c0oj31zw1efwqi.jpg)

第二种方式 256 x imgXlen 图像块：

![](http://ww1.sinaimg.cn/large/005JydxZly1fxmup2jvklj31gv12l4a8.jpg)

