## 实验二

实验二是建立在实验一的基础上，做起来相对容易一些。这次的实验是对图像的像素值进行更改，任务有两个，任务一是“把图像指定区域的红色通道置为255”，任务二是“把图像指定区域置为白色和黑色”。

任务一：

做的时候还是不够细心，代码中的read和write搞不清楚，还有就是临时变量tmpXlen和imgXlen搞混了，导致程序出错。

![](http://ww1.sinaimg.cn/large/006Nbb2oly1fys1fngkzyj30rl0efglr.jpg)
![](http://ww1.sinaimg.cn/large/006Nbb2oly1fys1fnfec9j30ey09ydfw.jpg)

![](http://ww1.sinaimg.cn/large/006Nbb2oly1fys1fn33mtj30u8039wen.jpg)

修改过后结果输出正确：

![](http://ww1.sinaimg.cn/large/006Nbb2oly1fys1ji14xuj30rl0ef0sr.jpg)

更改过后的图像显示正常：

![](http://ww1.sinaimg.cn/large/006Nbb2oly1fys1jkn615j30nz0gr1ck.jpg)

程序源代码：

```
// EXP2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
using namespace std;
#include"./gdal/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

int main()
{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	//输入图像路径
	char *srcPath = "wusi.jpg";
	//输出图像路径
	char *dstPath = "Nwusi.tif";
	//图像内存存储
	GByte* buffTmp;
	GByte* buffTmp2;
	//图像波段数
	int i, j, bandNum;
	//起始位置坐标
	int starX = 100;
	int starY = 100;
	//区域宽度和高度
	int tmpXlen = 200;
	int tmpYlen = 150;
	
	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	//获取图像宽度、高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//输出获取的结果
	cout << " Image X Length :" << imgXlen << endl;
	cout << " Image Y Length :" << imgYlen << endl;
	cout << " Band Number :" << bandNum << endl;
	//分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//根据图像的宽度和高度分配内存
	buffTmp2 = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
	//一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf(".......band %d processing ......\n", i);
	}
	//读取红色通道缓存在buffTmp中
	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read, starX, starY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	//遍历区域，逐像素值置为255
	for (j = 0; j < tmpYlen; j++)
	{
		for (i = 0; i < tmpXlen; i++)
		{
			buffTmp2[j*tmpXlen + i] = (GByte)255;
		}
	}
	//数据写入poDstDS
	poDstDS->GetRasterBand(1)->RasterIO(GF_Write, starX, starY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	//清除内存
	CPLFree(buffTmp);
	CPLFree(buffTmp2);
	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
	return 0;


}

```

任务二：

实验源代码如下：

```
// EXP2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
using namespace std;
#include"./gdal/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

int main()
{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	//输入图像路径
	char *srcPath = "wusi.jpg";
	//输出图像路径
	char *dstPath = "Nwusi1.tif";
	//图像内存存储
	GByte* buffTmp;
	GByte* buffTmp2;
	GByte* buffTmp3;

	//图像波段数
	int i, j,k, bandNum;
	//起始位置坐标
	int starX = 300;
	int starY = 300;
	int starX2 = 500;
	int starY2 = 500;
	//区域宽度和高度
	int tmpXlen = 100;
	int tmpYlen = 150;
	int tmpXlen2 = 150;
	int tmpYlen2= 100;

	
	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	//获取图像宽度、高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//输出获取的结果
	cout << " Image X Length :" << imgXlen << endl;
	cout << " Image Y Length :" << imgYlen << endl;
	cout << " Band Number :" << bandNum << endl;
	//分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	
	//一个个波段的输入，然后一个个波段的输出
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf(".......band %d processing ......\n", i);
	}

	//根据图像的宽度和高度分配内存
	 buffTmp2 = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
	 for (i = 0; i < bandNum; i++)
	 {
		 poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read, starX, starY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		 //遍历像素，逐像素置为255
		 for (j = 0; j < tmpYlen2; j++)
		 {
			 for (k = 0; k < tmpXlen2; k++) {
				 buffTmp2[j*tmpXlen2 + k] = (GByte)255;
			 }
			 poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write, starX, starY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
			 //遍历像素，逐像素置为0
		 }
	 }



	//根据图像的高度和宽度分配内存
	buffTmp3 = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read, starX2, starY2, tmpXlen2, tmpYlen2, buffTmp3, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
		//遍历像素，逐像素置为0
		for (j = 0; j < tmpYlen2; j++)
		{
			for (k = 0; k < tmpXlen2; k++) {
				buffTmp3[j*tmpXlen2 + k] = (GByte)0;
			}
			poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,starX2, starY2, tmpXlen2, tmpYlen2, buffTmp3, tmpXlen2, tmpYlen2, GDT_Byte, 0, 0);
			//遍历像素，逐像素置为0
		}
	}


	/*//读取红色通道缓存在buffTmp中
	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read, starX, starY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	//遍历区域，逐像素值置为255
	for (j = 0; j < tmpYlen; j++)
	{
		for (i = 0; i < tmpXlen; i++)
		{
			buffTmp2[j*tmpXlen + i] = (GByte)255;
		}
	}*/

	//数据写入poDstDS
	//poDstDS->GetRasterBand(1)->RasterIO(GF_Write, starX, starY, tmpXlen, tmpYlen, buffTmp2, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	//清除内存
	CPLFree(buffTmp);
	CPLFree(buffTmp2);
	CPLFree(buffTmp3);
	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
	return 0;


}



```

结果：![](http://ww1.sinaimg.cn/large/006Nbb2oly1fys285x77ej30kr0efgls.jpg)

图片显示：

![](http://ww1.sinaimg.cn/large/006Nbb2oly1fys28cp7qtj30m40f4at0.jpg)

