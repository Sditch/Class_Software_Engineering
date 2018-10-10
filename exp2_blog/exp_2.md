#<center>实验二：图像像素值的修改</center>
##任务1：把图像指定区域的红色通道置为255
###1、建立项目
&emsp;&emsp;原本直接想在实验一gdalDemo文件的基础上修改，于是用vs直接打开此文件夹，点击运行发现dll文件找不到，于是新建项目gdal_2，准备图片wusi.jpg，采用实验一的方法将项目进行相关配置<br>
###2、代码添加并修改运行
(1)将实验一的代码复制，然后添加实验二老师提供的代码，将分配内存的buffTmp修改为老师的代码：

	buffTmp = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
同时，自以为老师的代码已经给出波段的输入输出，便将之前的波段输入输出for循环注释，运行得到newwusi.tif图片如下：

![](https://i.imgur.com/IY9SlVV.png)

明显不对，自己上传的wusi.jpg未得到任何显示，原因分析：<br>

&emsp;&emsp;创建的输出图像与原图一样大，但只是读取了缓存通道buffTmp，并将其全部置为红色像素，所以输出只有buffTmp区域且全为红色，而背景显示为黑色，与原图没有任何联系<br>

(2)将注释掉的和三种颜色有关的for循环代码恢复，但此时报错：

![](https://i.imgur.com/c4lQuB1.png)

原因分析：<br>
&emsp;&emsp;栈溢出，buffTmp分配空间有限，但像素值改变的区域只有那一部分，于是重新定义图像内存存储buffTmp2并分配：<br>
	buffTmp2 = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));

运行得到：

![](https://i.imgur.com/s9LXBEX.jpg)

(3)此时没有红色像素区域的输出，原因可能是整个图像的波段输出的时候覆盖了红色区域，所以将此段代码提前至红色区域输出之前，运行输出得到newwusi.tif：

![](https://i.imgur.com/ob3zjim.jpg)

(4)主要代码：</br>

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
		// 图像波段数
		int i, j, bandNum;
		// 起始位置坐标
		int StartX = 100;
		int StartY = 100;
		// 区域宽度和高度
		int tmpXlen = 200;
		int tmpYlen = 150;
	
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
		// 分配内存
		buffTmp = (GByte*)CPLMalloc(tmpXlen*tmpYlen * sizeof(GByte));
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
			printf(" ... ... band %d processing ... ...\n", i);
		}
		
		// 读取红色通道缓存在buffTmp中
		poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		// 遍历区域，逐像素置为255
		for (j = 0; j < tmpYlen; j++)
		{
			for (i = 0; i < tmpXlen; i++)
			{
				buffTmp[j*tmpXlen + i] = (GByte)255;
			}
		}
		// 数据写入poDstDS
		poDstDS->GetRasterBand(1)->RasterIO(GF_Write,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		// 清除内存
		CPLFree(buffTmp);
		CPLFree(buffTmp2);
		// 关闭dataset
		GDALClose(poDstDS);
		GDALClose(poSrcDS);
	
		system("PAUSE");
		return 0;
	}

##任务2：把图像指定区域置为白色和黑色
###1、观察并学习实验一的代码，修改相应的起始点坐标，将所需的白色区域代码三种色素置为255，即每种色素读入后将其像素置为255，再将像素写入图片；黑色区域同理（当然还需注意内存的清除）
###2、主要代码：
<pre>
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
		// 遍历区域，逐像素置为255
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
</pre>
###3、结果展示
![](https://i.imgur.com/dt5sP53.jpg)
<br><br><br>