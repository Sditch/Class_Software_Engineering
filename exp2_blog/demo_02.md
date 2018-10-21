实验二总结：

​	由于本次实验是在第一次实验基础下进行的，所以本次实验过程中细节性的问题出现较少。

任务1：把图像指定区域的红色通道置为255

所有代码：

	// demo_02.cpp : 定义控制台应用程序的入口点。
	//
	
	#include "stdafx.h"
	#include <iostream>
	using namespace std;
	#include "./gdal/gdal_priv.h"
	#pragma comment(lib, "gdal_i.lib")
	
	int main()
	{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	//输入图像路径
	char* srcPath = "wusi1.jpg";
	//输出图像路径
	char* dstPath = "wusi2.tif";
	//图像内存存储
	GByte* buffTmp;
	GByte* buffTmp2;
	//图像波段数
	int i, j, bandNum;
	//起始位置坐标
	int startX = 100;
	int startY = 100;
	//区域高度和宽度
	int tmpXlen = 200;
	int tmpYlen = 150;
	//注册驱动
	GDALAllRegister();
	
	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	
	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//输出获取的结果
	cout << "Image X Length: " << imgXlen << endl;
	cout << "Image Y Length: " << imgYlen << endl;
	cout << "Band number:    " << bandNum << endl;
	//根据图像的宽度和高度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//根据图像的高度和宽度分配内存
	buffTmp2 = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
	//一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf(" ... ... band %d processing ... ...\n", i);
	}
	
	//读取红色通道缓存在buffTmp中
	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,
		startX, startY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	//遍历区域，逐像素置为255
	for (j = 0; j < tmpYlen; j++)
	{
		for (i = 0; i < tmpXlen; i++) {
			buffTmp2[j*tmpXlen + i] = (GByte)255;
		}
	}
	//数据写入poDstDS
	poDstDS->GetRasterBand(1)->RasterIO(GF_Write,
		startX, startY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	
	// 清除内存
	CPLFree(buffTmp);
	CPLFree(buffTmp2);
	// 关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);
	
	system("PAUSE");
	return 0;
	}
输出结果：

![](http://ww1.sinaimg.cn/large/006AVk9bly1fwg2ro4um0j30ur0jr0ti.jpg)

![](http://ww1.sinaimg.cn/large/006AVk9bly1fwg2ygx6v2j30s80kddio.jpg)

![](http://ww1.sinaimg.cn/large/006AVk9bly1fwg30l91e3j316r0sikj1.jpg)

任务2：把图像指定区域置为白色和黑色

所有代码：

	// demo_02.cpp : 定义控制台应用程序的入口点。
	//
	
	#include "stdafx.h"
	#include <iostream>
	using namespace std;
	#include "./gdal/gdal_priv.h"
	#pragma comment(lib, "gdal_i.lib")
	
	int main()
	{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	//输入图像路径
	char* srcPath = "wusi1.jpg";
	//输出图像路径
	char* dstPath = "wusi3.tif";
	//图像内存存储
	GByte* buffTmp;
	GByte* buffTmp2;
	GByte* buffTmp3;
	//图像波段数
	int i, j, k, bandNum;
	//起始位置坐标
	int startX = 300;
	int startY = 300;
	int startX2 = 500;
	int startY2 = 500;
	//区域高度和宽度
	int tmpXlen = 100;
	int tmpYlen = 50;
	int tmpXlen2 = 50;
	int tmpYlen2 = 100;
	//注册驱动
	GDALAllRegister();
	
	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	
	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//输出获取的结果
	cout << "Image X Length: " << imgXlen << endl;
	cout << "Image Y Length: " << imgYlen << endl;
	cout << "Band number:    " << bandNum << endl;
	//根据图像的宽度和高度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	
	//一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf(" ... ... band %d processing ... ...\n", i);
	}
	
	//根据图像的高度和宽度分配内存
	buffTmp2 = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			startX, startY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		//遍历区域，逐像素置为255
		for (j = 0; j < tmpYlen; j++)
		{
			for (k = 0; k < tmpXlen; k++) {
				buffTmp2[j*tmpXlen + k] = (GByte)255;
			}
			poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
				startX, startY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		}
	}
	
	//根据图像的高度和宽度分配内存
	buffTmp3 = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			startX2, startY2, tmpXlen2, tmpYlen2, buffTmp3, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
		//遍历区域，逐像素置为0
		for (j = 0; j < tmpYlen2; j++)
		{
			for (k = 0; k < tmpXlen2; k++) {
				buffTmp3[j*tmpXlen2 + k] = (GByte)0;
			}
			poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
				startX2, startY2, tmpXlen2, tmpYlen2, buffTmp3, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
		}
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
问题：

在实验过程中由于部分参数使用混乱，导致输出图像仍为原图，修改后得到实验结果。

输出结果：

![](http://ww1.sinaimg.cn/large/006AVk9bly1fwg2ro4um0j30ur0jr0ti.jpg)

![](http://ww1.sinaimg.cn/large/006AVk9bly1fwg2ygx6v2j30s80kddio.jpg)

![](http://ww1.sinaimg.cn/large/006AVk9bly1fwg3o1a5vdj30zv0rqe6x.jpg)