#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main()
{
	// 输入头像
	GDALDataset* poSrcDS;
	// 输出头像
	GDALDataset* poDstDS;
	// 图像的宽度和高度
	int imgXlen, imgYlen;
	// 输入图像路径
	char* srcPath = "wusi.jpg";
	// 输出图像路径
	char* dstPath = "newwusi.tif";
	// 图像内存存储
	GByte* buffTmp;
	GByte* buffTmp2;
	GByte* buffTmp3;
	// 图像波段数
	int i, j, bandNum;
	// 起始位置坐标
	int StartX = 300;
	int StartY = 300;
	int StartX2 = 500;
	int StartY2 = 500;
	// 区域宽度和高度
	int tmpXlen = 100;
	int tmpYlen = 50;
	int tmpXlen2 = 50;
	int tmpYlen2 = 100;

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
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	// 根据图像的宽度和高度分配内存
	buffTmp2 = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	// 一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);
		//printf(" ... ... band %d processing ... ...\n", i);
	}
	
	
	// 分配内存
	buffTmp = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
	// 一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		// 遍历区域，逐像素置为255
		for (j = 0; j < tmpYlen; j++)
		{
			for (int k = 0; k < tmpXlen; k++)
			{
				buffTmp[j*tmpXlen + k] = (GByte)255;
			}
		}
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		//printf(" ... ... band %d processing ... ...\n", i);
	}
	
	// 分配内存
	buffTmp3 = (GByte*)CPLMalloc(tmpXlen2*tmpYlen2 * sizeof(GByte));
	// 一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			StartX2, StartY2, tmpXlen2, tmpYlen2, buffTmp3, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
		// 遍历区域，逐像素置为0
		for (j = 0; j < tmpYlen2; j++)
		{
			for (int k = 0; k < tmpXlen2; k++)
			{
				buffTmp3[j*tmpXlen2 + k] = (GByte)0;
			}
		}
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			StartX2, StartY2, tmpXlen2, tmpYlen2, buffTmp3, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
		//printf(" ... ... band %d processing ... ...\n", i);
	}

	// 清除内存
	CPLFree(buffTmp);
	CPLFree(buffTmp2);
	CPLFree(buffTmp3);
	// 关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
	return 0;
}