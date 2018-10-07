#<center>实验一：Visual Studio 2017 + GDAL 图像处理</center>
###前期准备
&emsp;&emsp;实验开始前，在电脑上安装Visual Studio 2017版本，并下载gdal库需要的头文件、lib文件、dll文件以及需要的图片trees.jpg
##实验步骤
####1、建立项目
&emsp;&emsp;建立工程gdalDemo，选择win32控制台应用程序确定

![](https://i.imgur.com/reB0aKo.png)

&emsp;&emsp;接着出现Win32应用程序向导，不要着急点击完成，选择下一步附加选项选择“空项目”，否则新建的项目冗余文件太多，编译会遇到很多问题，很麻烦

![](https://i.imgur.com/tWCcnAx.png)

####2、右击源文件添加代码main.cpp<br>

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
		char* srcPath = "trees.jpg";
		// 输出图像路径
		char* dstPath = "res.tif";
		// 图像内存存储
		GByte* buffTmp;
		// 图像波段数
		int i, bandNum;
	
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
		// 根据图像的宽度和高度分配内存
		buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
		// 创建输出图像
		poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
			dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		// 一个个波段的输入，然后一个个波段的输出
		for (i = 0; i < bandNum; i++)
		{
			poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
				0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
			poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
				0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
			printf(" ... ... band %d processing ... ...\n", i);
		}
	
		// 清除内存
		CPLFree(buffTmp);
		// 关闭dataset
		GDALClose(poDstDS);
		GDALClose(poSrcDS);
	
		system("PAUSE");
		return 0;
	}

####3、问题解决<br>
（1）提示“无法打开源文件"./gdal/gdal_priv.h"”，根据老师提示“当前目录没有gdal头文件”：<br>

- 尝试将gdal文件直接复制到gdalDemo文件夹，结果gdal文件夹内的文件自动散乱在项目头文件中，且问题未解决

![](https://i.imgur.com/pMbvWzX.jpg)


- 在头文件目录下建立文件夹gdal，将散乱文件归并到gdal文件中，以为满足代码头文件格式，结果仍未解决<br>
- 尝试将gdal文件移至源文件文件夹，仍未解决<br>
- 将gdal文件夹移回头文件，且将文件夹放到gdalDemo项目文件路径中，问题得到解决<br>

（2）运行程序，报错LNK错误，因为没有项目没有lib文件，将其直接复制到gdalDemo项目，问题解决<br>

（3）再次运行程序，报错，“找不到dll文件”<br>

![](https://i.imgur.com/9f96u09.png)


- 将gdal18.dll文件直接复制到gdalDemo项目，问题未解决
- 将其复制到项目文件路径中，问题解决


![](https://i.imgur.com/d9KFPYn.png)
<br><br>
###实验结果
&emsp;&emsp;代码正常运行，且 res.tif 正常输出<br>
![](https://i.imgur.com/a8nzDI8.png)
<br><br><br><br>
	