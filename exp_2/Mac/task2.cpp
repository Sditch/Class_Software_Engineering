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