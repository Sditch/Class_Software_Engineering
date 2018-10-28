# 软件工程实验二

上次折腾Mac下Clion环境本以为可以一劳永逸，但是上周随着我升级了MacOS Mojave之后，各种问题又开始出现，首先发现是clang error。于是乎又开始排查错误，最终发现是我的cmake文件中配置出错。以下是修改后的cmake文件。

```cmake
cmake_minimum_required(VERSION 3.12)
project(mac_exp2)

set(CMAKE_CXX_STANDARD 14)


add_executable(mac_exp2 main.cpp)

include_directories(/usr/local/Cellar/gdal/2.3.1_2/include)
link_directories(/usr/local/Cellar/gdal/2.3.1_2/include)


target_link_libraries(mac_exp2 /usr/local/Cellar/gdal/2.3.1_2/lib/libgdal.20.dylib)


```

###任务一：

敲完代码后报了一个错：

![](https://ws1.sinaimg.cn/large/006aaBRuly1fw86tulb49j30vg046mxf.jpg)

debug后发现是打开文件的问题

![](https://ws1.sinaimg.cn/large/006aaBRuly1fw86zd1xl9j318i0icgow.jpg)

开始怀疑是文件格式问题，但是查了一波gdal文档后发现，官方文档中说要先调用`GDALAllRegister()`函数，再打开文件…在前面加上`GDALAllRegister()`后成功运行。

![](https://ws1.sinaimg.cn/large/006aaBRuly1fw874kjfuhj327o0acdjs.jpg)

输出文件是一个红方块。

![](https://ws1.sinaimg.cn/large/006aaBRuly1fw8788mziwj30p80hk3yn.jpg)

这个是和我的输出有关，简单修改

由于我最后只进行了`GetRasterBand(1)`的写操作，所以只有一个通道的颜色。

```c
poDstDs->GetRasterBand(1)->RasterIO(GF_Write,
            StartX,StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
```

如果在后面添加原来的三通道会发现，新的三通道直接覆盖原来的修改，得到的还是原图。所以先将原图输出，在修改进行叠加。

由于两次的图像大小不同，所以malloc的空间也不同，注意使用两个不同的地址空间。

`buffTmp2 = (GByte *) CPLMalloc(imgXlen*imgYlen* sizeof(GByte));`

得到效果图。

![](https://ws1.sinaimg.cn/large/006aaBRuly1fw88fusds5j31jk172kjo.jpg)

代码如下：

```c
#include <iostream>
#include <gdal_priv.h>
using namespace std;

int main() {
    GDALDataset *poSrcDS;
    GDALDataset *poDstDs;

    char *srcPath = "1.jpeg";
    char *dstPath = "t.tif";

    int StartX;
    int StartY;

    int tmpXlen;
    int tmpYlen;
    int imgXlen;
    int imgYlen;
    int bandNum;

    int i,j;

    GDALAllRegister();
    poSrcDS = (GDALDataset *)GDALOpenShared(srcPath, GA_ReadOnly);

    StartX = 100;
    StartY = 100;
    tmpXlen = 200;
    tmpYlen = 150;
    imgXlen = poSrcDS->GetRasterXSize();
    imgYlen = poSrcDS->GetRasterYSize();
    bandNum = poSrcDS->GetRasterCount();

    GByte *buffTmp;
    GByte *buffTmp2;


    buffTmp = (GByte *) CPLMalloc(imgXlen*imgYlen* sizeof(GByte));
    poDstDs = GetGDALDriverManager()->GetDriverByName("GTIff")->Create(dstPath,
            imgXlen, imgYlen,  bandNum, GDT_Byte, NULL);


    //添加剩下的两个通道
    buffTmp2 = (GByte *) CPLMalloc(imgXlen*imgYlen* sizeof(GByte));
    for (i = 0; i < bandNum; i++) {
        poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read,
                0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);

        poDstDs->GetRasterBand(i+1)->RasterIO(GF_Write,
                0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);
    }

    //对原图进行修改
    poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,
            StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);

    for (j = 0; j < tmpYlen; j++) {
        for (i = 0; i < tmpXlen; i++) {
            buffTmp[j*tmpXlen+i] = (GByte) 255;
        }
    }
    poDstDs->GetRasterBand(1)->RasterIO(GF_Write,
            StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);




    CPLFree(buffTmp);
    CPLFree(buffTmp2);
    GDALClose(poSrcDS);
    GDALClose(poDstDs);
    return 0;
}
```

### 任务二：

修改了好久发现输出图片还是原来的，估计是在进行通道操作的时候相互覆盖了。

对源码进行一定修改后发现是数组下标的问题，修改后即成功实现。

```c
#include <iostream>
#include <gdal_priv.h>
using namespace std;

int main() {
    GDALDataset *poSrcDS;
    GDALDataset *poDstDs;

    char *srcPath = "1.jpeg";
    char *dstPath = "task2.tif";

    int StartX;
    int StartY;

    int tmpXlen;
    int tmpYlen;
    int imgXlen;
    int imgYlen;
    int bandNum;

    int i,j,k;

    GDALAllRegister();
    poSrcDS = (GDALDataset *)GDALOpenShared(srcPath, GA_ReadOnly);

    StartX = 100;
    StartY = 100;
    tmpXlen = 200;
    tmpYlen = 150;
    imgXlen = poSrcDS->GetRasterXSize();
    imgYlen = poSrcDS->GetRasterYSize();
    bandNum = poSrcDS->GetRasterCount();

    GByte *buffTmp;
    GByte *buffTmp2;
    GByte *buffTmp3;
    GByte *buffTmp4;


    buffTmp = (GByte *) CPLMalloc(imgXlen*imgYlen* sizeof(GByte));
    poDstDs = GetGDALDriverManager()->GetDriverByName("GTIff")->Create(dstPath,
                                                                       imgXlen, imgYlen,  bandNum, GDT_Byte, NULL);


    //添加剩下的两个通道
    buffTmp2 = (GByte *) CPLMalloc(imgXlen*imgYlen* sizeof(GByte));
    for (i = 0; i < bandNum; i++) {
        poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read,
                                              0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);

        poDstDs->GetRasterBand(i+1)->RasterIO(GF_Write,
                                              0, 0, imgXlen, imgYlen, buffTmp2, imgXlen, imgYlen, GDT_Byte, 0, 0);
    }

    //对原图进行修改
    buffTmp3 = (GByte *) CPLMalloc(100*50* sizeof(GByte));
    buffTmp4 = (GByte *) CPLMalloc(50*100* sizeof(GByte));

    for (int i = 0; i < bandNum; ++i) {
        poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read,
                300, 300, 100, 50, buffTmp3, 100, 50, GDT_Byte, 0, 0);


        poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read,
                500, 500, 50, 100, buffTmp4, 50, 100, GDT_Byte, 0, 0);


        for (j = 0; j < tmpYlen; j++) {
            for (k = 0; k < tmpXlen; k++) {
                buffTmp3[j*tmpXlen+k] = (GByte) 255;
            }

        }
        for (j = 0; j < tmpYlen; j++) {
            for (k = 0; k < tmpXlen; k++) {
                buffTmp4[j*tmpXlen+k] = (GByte) 0;
            }
        }

        poDstDs->GetRasterBand(i+1)->RasterIO(GF_Write,
                500, 500, 50, 100, buffTmp4, 50, 100, GDT_Byte, 0, 0);

        poDstDs->GetRasterBand(i+1)->RasterIO(GF_Write,
                300, 300, 100, 50, buffTmp3, 100, 50, GDT_Byte, 0, 0);
    }




    CPLFree(buffTmp);
    CPLFree(buffTmp2);
    CPLFree(buffTmp3);
    CPLFree(buffTmp4);
    GDALClose(poSrcDS);
    GDALClose(poDstDs);
    return 0;
}
```

![](https://ws1.sinaimg.cn/large/006aaBRuly1fw8p153y2aj31jq176kjo.jpg)

