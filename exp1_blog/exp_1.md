## 实验一

按照实验教程下载VS2010和三种文件，然后添加头部引用的时候，直接在VS里的工程添加gdal文件，发现头文件的引用还是报错!

![QQ截图20181204090311](C:\Users\Administrator\Desktop\2018秋季学期\typora插入的图片\QQ截图20181204090311.png)

后来仔细看了教程的文字，是要将gdal文件复制在工程所在的目录路径下，编译还是出错，vc编程中出现fatal error C1010：在查找预编译头时遇到意外的文件结尾，是否忘记了向源中添加“#include"./gdal/gdal_priv.h"，于是找到出错的文件，去掉预编译头，该问题得以解决。然后编写代码，运行时有错误。

![jieguo](C:\Users\Administrator\Desktop\2018秋季学期\typora插入的图片\jieguo.png)

运行成功，没有出现error LINK2001,估计我下载的vs自带有lib文件。但是运行界面报错，仔细看了代码，波段输入时把poDstDS写成了poSrcDS，改正后，运行成功。

![结果](C:\Users\Administrator\Desktop\2018秋季学期\typora插入的图片\结果.png)

tree.tif写入成功。

![tif](C:\Users\Administrator\Desktop\2018秋季学期\typora插入的图片\tif.png)

main.cpp源代码如下：

```
#include <iostream>
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
	int imgXlen,imgYlen;
	//输入图像路径
	char *srcPath = "trees.jpg";
	//输出图像路径
	char *dstPath = "res.tif";
	//图像内存储存
	GByte* buffTmp;
	//图像波段数
	int i, bandNum;
	//注册驱动
```



	GDALAllRegister();
	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum =  poSrcDS->GetRasterCount();
	//输出获取的结果
	cout << "Image X Length: "<< imgXlen << endl;
	cout << "Image Y Length: "<< imgYlen << endl;
	cout << "Band Number: "<< bandNum << endl;
	//根据图像的宽度和高度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen*sizeof(GByte));
	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//一个个波段的输入，然后一个个波段的输出
	for (i = 0; i  < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read, 0 , 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i+1)->RasterIO(GF_Write, 0 , 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf(".....band  %d processing.......\n", i);
	}
	//清楚内存
	CPLFree(buffTmp);
	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);
	
	system("PAUSE");
	return 0;
}

​		