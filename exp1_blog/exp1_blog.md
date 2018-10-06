# 实验报告

由于是mac的电脑，不支持老师给的dll，所以自己折腾了mac环境下的gdal环境，详细过程已经发到我的blog上：

`https://hufangyu.github.io/2018/10/04/gdalmacanzhuangpeizhi/`

这里简单陈述过程。

用brew安装gdal库

![image-20181004095153511](https://ws1.sinaimg.cn/large/006aaBRuly1fvw4dda35jj317m03atc3.jpg)

由于是brew默认安装，gdal的路径为`/usr/local/Cellar/gdal/`

用CLion配置CMake文件

```cmake
cmake_minimum_required(VERSION 3.12)
project(clion_1)

set(CMAKE_CXX_STANDARD 11)

add_executable(clion_1 main.cpp)

include_directories(/usr/local/Cellar/gdal/2.3.1_2/include)
link_directories(/usr/local/Cellar/gdal/2.3.1_2/lib)
link_libraries(gdal)

target_link_libraries(clion_1 gdal)


```

添加源码：

```C
#include <iostream>
#include <gdal_priv.h>
using namespace std;

int main() {
    GDALDataset *poSrcDS;
    GDALDataset *poDstDS;

    int imgXlen, imgYlen;
    char *srcPath = "trees.jpg";
    char *dstPath = "res.tif";
    GByte *buffTmp;
    int i, bandNum;

    GDALAllRegister();

    poSrcDS = (GDALDataset *)GDALOpenShared(srcPath, GA_ReadOnly);

    imgXlen = poSrcDS->GetRasterXSize();
    imgYlen = poSrcDS->GetRasterYSize();
    bandNum = poSrcDS->GetRasterCount();

    cout << " Image X Length: " << imgXlen << endl;
    cout << " Image Y Length: " << imgYlen << endl;
    cout << " Band number : " << bandNum << endl;

    buffTmp = (GByte *) CPLMalloc(imgXlen * imgYlen * sizeof(GByte));
    poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
            dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

    for(i = 0; i < bandNum; i++){
        poSrcDS->GetRasterBand(i+1)->RasterIO(GF_Read,
                                              0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
        poDstDS->GetRasterBand(i+1)->RasterIO(GF_Write,
                                              0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);

        printf(" ... ... band %d processing ... ... \n", i);
    }

    CPLFree(buffTmp);
    GDALClose(poDstDS);
    GDALClose(poSrcDS);

    return 0;

}

```

运行程序，正常显示

![image-20181004113543534](https://ws1.sinaimg.cn/large/006aaBRuly1fvw4dd9r1hj318y0badiy.jpg)

输出tif文件正常

![](https://ws1.sinaimg.cn/large/006aaBRuly1fvyutymcj6j31640u61kx.jpg)

