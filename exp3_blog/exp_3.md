# <center>实验三：简单抠像</center>
### 一、实验要求
提供两张图：space.jpg和superman.jpg，两张图的大小均为640*480像素。需要把superman.jpg中的超人抠出，加到 space.jpg 的太空背景中。
### 二、实验步骤
1、在原来实验二的代码基础上，修改定义，输入头像，定义图像内存存储等：
	
	// 输入头像
	GDALDataset* poSrcDS_space;
	GDALDataset* poSrcDS_superman;
2、确认任务逻辑
		
	读取space.jpg-->输出space.jpg-->读取superman.jpg-->判断满足超人的像素点值-->将满足超人的像素点值输出到space.jpg中

3、代码实现

* 既然需要判断像素值大小，便将每个像素点的值根据其未知读取并判断（读取的代码)：

	<pre>
	for (i = 0; i < imgXlen; i++)
	{
		for (j = 0; j < imgYlen; ++j)
		{
			for (int k = 0; k < bandNum; ++k)
			{
				poSrcDS_superman->GetRasterBand(k + 1)->RasterIO(GF_Read,
					0, 0, imgXlen, imgYlen, buffTmp_superman, imgXlen, imgYlen, GDT_Byte, 0, 0);
				buff[k] = buffTmp_superman[i*imgYlen + j];
			}
	</pre>

	这样就导致代码运行超慢超慢。。。因为每次i，j位置处都要读取整个文件的像素，这样完全没必要，浪费很多时间，经过多次尝试，发现只需读取当前未知像素即可，改进代码：
	<pre>
	for (i = 0; i < imgXlen; i++)
	{
		for (j = 0; j < imgYlen; ++j)
		{
			for (int k = 0; k < bandNum; ++k)
			{
				poSrcDS_superman->GetRasterBand(k + 1)->RasterIO(GF_Read,
					i, j, 1, 1, buffTmp_superman, 1, 1, GDT_Byte, 0, 0);
			}
	</pre>
* 将当前像素值保存到定义数组buff[0/1/2]中，在读取像素值后面加上代码：

	<pre>
	buff[k] = buffTmp_superman[i*imgYlen + j];</pre>

	运行测试printf("%d,", buff[k]);输出都是0（因为buff初始化为0）。自认为没有问题，读取是第i行第j列位置像素点的值，和同学讨论发现：我读取的buffTmp_superman当前只有一个像素点的位置（只有一个元素），我这样写显然数组越界了。修改代码：

		buff[k] = buffTmp_superman[0];

* 参考实验二代码，将满足条件的第i行第j列位置的像素点输出到space.jpg图片中，代码示例： 

		poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write, i, j, 1, 1, buffTmp_space, 1, 1, GDT_Byte, 0, 0);

	可是忽略到此时buffTmp_space值不会随着波段数bandNum变化，写入的值应该是保存的buff[k]数组，可是buff值随着k值变化的，需要写入的操作for循环不能包含在读取里面，因为要将一个像素点的三个颜色读取出来一起进行比较后才能执行写入操作，读取和写入是并列的两个for循环，最终决定可以用buff+k表示（不能用buff[k]表示，此时参数为地址）
	
		poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write, i, j, 1, 1, buff + k, 1, 1, GDT_Byte, 0, 0);
* 以为这就可以正常输出了，发现输出图像这样：

	![](https://i.imgur.com/ja8mH0C.png)

	原来是输出条件弄反了，老师给的是绿色像素的图像，条件取反即可：

		if (!((buff[0]<(GByte)160 && buff[0]>(GByte)10)&&(buff[1]<(GByte)220 && buff[1]>(GByte)100)&&(buff[2]<(GByte)110 && buff[2]>(GByte)10)))

	输出图像：

	![](https://i.imgur.com/agvg1FC.png)


### 三、实验代码
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
<br>
<br>

