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