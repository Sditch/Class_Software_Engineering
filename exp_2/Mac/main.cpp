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