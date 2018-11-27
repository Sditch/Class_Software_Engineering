#include <iostream>
#include <cmath>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")


int main()
{
	// 输入头像
	GDALDataset* poSrcDS_MUL;
	GDALDataset* poSrcDS_PAN;
	// 输出图像
	GDALDataset* poDstDS;
	// 图像的宽度和高度
	int imgXlen = 0, imgYlen = 0;
	// 输入图像路径
	char* srcPath_MUL = "American_MUL.bmp";
	char* srcPath_PAN = "American_PAN.bmp";
	// 输出图像路径
	char* dstPath = "American_FUS.tif";
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

	// 创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	// RGB
	float *R;
	R = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
	float *G;
	G = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
	float *B;
	B = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));

	//读取数组的像素点
	poSrcDS_MUL->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		R, imgXlen, imgYlen, GDT_Float32, 0, 0);

	poSrcDS_MUL->GetRasterBand(2)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		G, imgXlen, imgYlen, GDT_Float32, 0, 0);

	poSrcDS_MUL->GetRasterBand(3)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		B, imgXlen, imgYlen, GDT_Float32, 0, 0);
	

	float trans1[3][3] = {
		1 / 3, 1 / 3, 1 / 3,
		-1 * sqrtf(2) / 6, -1 * sqrtf(2) / 6, sqrtf(2) / 3,
		1 / sqrtf(2), -1 / sqrtf(2), 0 };


	float trans2[3][3] = {
		1, -1 / sqrtf(2), 1 / sqrtf(2),
		1, -1 / sqrtf(2), -1 / sqrtf(2),
		1, sqrtf(2), 0 };

	float **trans_in;
	trans_in = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_in[i] = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	}
	// trans_in的标记
	for (int i = 0; i < imgXlen * imgYlen; i++)
	{
		trans_in[0][i] = R[i];
		trans_in[1][i] = G[i];
		trans_in[2][i] = B[i];
	}

	// 进行 RGB ==> IHS 正变换
	float **trans_res;
	trans_res = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_res[i] = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	}
	for (int i = 0; i < imgXlen * imgYlen; i++)
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
	poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen,
		trans_res[0], imgXlen, imgYlen, GDT_Float32, 0, 0);
	
	// 进行 IHS == > RGB 反变换
	float **trans_res_end;
	trans_res_end = (float**)CPLMalloc(bandNum * sizeof(float*));
	for (int i = 0; i < bandNum; i++)
	{
		trans_res_end[i] = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	}
	for (int i = 0; i < imgXlen * imgYlen; i++)
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
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 0, imgXlen, imgYlen,
			trans_res_end[i], imgXlen, imgYlen, GDT_Float32, 0, 0);
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
	// 关闭dataset
	GDALClose(poSrcDS_MUL);
	GDALClose(poSrcDS_PAN);
	GDALClose(poDstDS);

	system("PAUSE");
	return 0;
}