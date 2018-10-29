#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main()
{
	// 输入头像
	GDALDataset* poSrcDS_space;
	GDALDataset* poSrcDS_superman;
	// 输出头像
	GDALDataset* poDstDS;
	// 图像的宽度和高度
	int imgXlen, imgYlen;
	// 输入图像路径
	char* srcPath_space = "space.jpg";
	char* srcPath_superman = "superman.jpg"; 
	// 输出图像路径
	char* dstPath = "newspace.tif";
	// 图像内存存储
	GByte* buffTmp_space;
	GByte* buffTmp_superman;
	// 图像波段数
	int i, j, bandNum;

	// 注册驱动
	GDALAllRegister();

	// 打开图像
	poSrcDS_space = (GDALDataset*)GDALOpenShared(srcPath_space, GA_ReadOnly);
	poSrcDS_superman = (GDALDataset*)GDALOpenShared(srcPath_superman, GA_ReadOnly);

	// 获取图像宽度，高度和波段数量
	imgXlen = poSrcDS_space->GetRasterXSize();
	imgYlen = poSrcDS_space->GetRasterYSize();
	bandNum = poSrcDS_space->GetRasterCount();
	// 输出获取的结果
	cout << " Image X Length: " << imgXlen << endl;
	cout << " Image Y Length: " << imgYlen << endl;
	cout << " Band number :   " << bandNum << endl;

	// 创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	// 根据图像的宽度和高度分配内存
	buffTmp_space = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	// 一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS_space->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp_space, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp_space, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf(" ... ... band %d processing ... ...\n", i);
	}


	// 分配内存
	buffTmp_superman = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	GByte buff[3];
	for (i = 0; i < 3; i++)
	{
		buff[i] = (GByte)0;
	}
	// 一个个波段的输入，然后一个个波段的输出
	printf("******\n");
	for (i = 0; i < imgXlen; i++)
	{
		//printf("%d, ", i);
		for (j = 0; j < imgYlen; ++j)
		{
			for (int k = 0; k < bandNum; ++k)
			{
				poSrcDS_superman->GetRasterBand(k + 1)->RasterIO(GF_Read,
					i, j, 1, 1, buffTmp_superman, 1, 1, GDT_Byte, 0, 0);
				buff[k] = buffTmp_superman[0];
				//printf("%d,", buff[k]);
			}
			//printf("\n");
			if (!((buff[0]<(GByte)160 && buff[0]>(GByte)10)&&(buff[1]<(GByte)220 && buff[1]>(GByte)100)&&(buff[2]<(GByte)110 && buff[2]>(GByte)10)))
			{
				for (int k = 0; k < bandNum; ++k)
				{
					poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
						i, j, 1, 1, buff + k, 1, 1, GDT_Byte, 0, 0);
					//printf("%d,", i, j, k);
				}
			}
			
		}
		//printf("\n");
	}


	// 清除内存
	CPLFree(buffTmp_space);
	CPLFree(buffTmp_superman);
	// 关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS_space);
	GDALClose(poSrcDS_superman);

	system("PAUSE");
	return 0;
}