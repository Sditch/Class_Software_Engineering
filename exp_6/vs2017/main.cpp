#include <cmath>
#include <time.h>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")


int main()
{
	// ����ʱ�䣬�Ƚ���������
	clock_t startTime, endTime;
	// ����ͷ��
	GDALDataset* poSrcDS_MUL;
	GDALDataset* poSrcDS_PAN;
	// ���ͼ��
	GDALDataset* poDstDS_256;
	GDALDataset* poDstDS_256x256;
	// ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen = 0, imgYlen = 0;
	// ����ͼ��·��
	char* srcPath_MUL = "Mul_large.tif";
	char* srcPath_PAN = "Pan_large.tif";
	// ���ͼ��·��
	char* dstPath_256 = "FUS_large_256.tif";
	char* dstPath_256x256 = "FUS_large_256x256.tif";
	// ͼ�񲨶���
	int bandNum;

	// ע������
	GDALAllRegister();

	// ��ͼ��
	poSrcDS_MUL = (GDALDataset*)GDALOpenShared(srcPath_MUL, GA_ReadOnly);
	poSrcDS_PAN = (GDALDataset*)GDALOpenShared(srcPath_PAN, GA_ReadOnly);

	// ��ȡͼ���ȣ��߶ȺͲ�������
	imgXlen = poSrcDS_MUL->GetRasterXSize();
	imgYlen = poSrcDS_MUL->GetRasterYSize();
	bandNum = poSrcDS_MUL->GetRasterCount();
	// �����ȡ�Ľ��
	cout << " Image X Length: " << imgXlen << endl;
	cout << " Image Y Length: " << imgYlen << endl;
	cout << " Band number :   " << bandNum << endl;

	// ���п������������
	int m = imgXlen / 256;
	int n = imgYlen / 256;

	// �������ͼ��
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
	
	// 256 x ����imgXlen	�ֿ鷽ʽ2
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

	// ѭ�����ֿ鴦��
	cout << "�ֳ� 256x���� ��С:" << endl;
	for (int p = 0; p < n; p++)
	{
		//cout << "��" << p << "��ͼ��鿪ʼ......" << endl;
		//��ȡ��������ص�
		poSrcDS_MUL->GetRasterBand(1)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			R, imgXlen, 256, GDT_Float32, 0, 0);

		poSrcDS_MUL->GetRasterBand(2)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			G, imgXlen, 256, GDT_Float32, 0, 0);

		poSrcDS_MUL->GetRasterBand(3)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			B, imgXlen, 256, GDT_Float32, 0, 0);
		// trans_in�ı��
		for (int i = 0; i < imgXlen * 256; i++)
		{
			trans_in[0][i] = R[i];
			trans_in[1][i] = G[i];
			trans_in[2][i] = B[i];
		}

		// ���� RGB ==> IHS ���任
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

		// ���� PAN �滻 I ����
		poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read, 0, 256 * p, imgXlen, 256,
			trans_res[0], imgXlen, 256, GDT_Float32, 0, 0);

		// ���� IHS == > RGB ���任
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

		// �ѱ任������
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
	cout << "��ʱ��" << (double)(endTime - startTime) / CLOCKS_PER_SEC << " s " << endl;

	// 256 x 256	�ֿ鷽ʽ1
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


	// ѭ�����ֿ鴦��
	cout << "�ֳ� 256x256 ��С:" << endl;
	for (int p = 0; p < n; p++)
	{
		for (int q = 0; q < m; q++)
		{
			//cout << "��" << p << "�С���" << q << "��ͼ��鿪ʼ......" << endl;
			//��ȡ��������ص�
			poSrcDS_MUL->GetRasterBand(1)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				R, 256, 256, GDT_Float32, 0, 0);

			poSrcDS_MUL->GetRasterBand(2)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				G, 256, 256, GDT_Float32, 0, 0);

			poSrcDS_MUL->GetRasterBand(3)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				B, 256, 256, GDT_Float32, 0, 0);
			// trans_in�ı��
			for (int i = 0; i < 256 * 256; i++)
			{
				trans_in[0][i] = R[i];
				trans_in[1][i] = G[i];
				trans_in[2][i] = B[i];
			}

			// ���� RGB ==> IHS ���任
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

			// ���� PAN �滻 I ����
			poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read, 256 * q, 256 * p, 256, 256,
				trans_res[0], 256, 256, GDT_Float32, 0, 0);

			// ���� IHS == > RGB ���任
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

			// �ѱ任������
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
	cout << "��ʱ��" << (double)(endTime - startTime) / CLOCKS_PER_SEC << " s " << endl;

	
	// �ر�dataset
	GDALClose(poSrcDS_MUL);
	GDALClose(poSrcDS_PAN);
	GDALClose(poDstDS_256);
	GDALClose(poDstDS_256x256);

	system("PAUSE");
	return 0;
}	