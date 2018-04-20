#include "strutils.h"

#include <QDebug>

using namespace STU;

StrUtils::StrUtils()
{

}

int StrUtils::SubIndex(const char *chStr, int nSize, char ch)
{
    char* chTmp = (char*)malloc(nSize);
    strcpy(chTmp, chStr);
    int j = 0;
    while(chTmp[j] != ch)
    {
        j++;
    }
    free(chTmp);
    return j;
}

QPoint STU::StrUtils::SplitStrToPt(QString strXY, char chIdle)
{
    char* chX = (char*)malloc(strXY.toStdString().size());
    strcpy(chX, strXY.toStdString().data());
    int j = 0;
    while(chX[j] != chIdle)
    {
        j++;
    }
    qDebug() << "2" << j;
    char* chX1 = (char*)malloc(j);
    j = 0;
    while(chX[j] != chIdle)
    {
        chX1[j] = chX[j];
        j++;
    }
    qDebug() << "3";
    char* chX2 = (char*)malloc(j);
    const char* chBufx = strchr(chX1, '=');
    strcpy(chX, chBufx);
    qDebug() << chX;
    j = 1;
    while(chX[j] != '\0')
    {
        chX2[j - 1] = chX[j];
        j++;
    }

    const char* chBuf = strchr(strXY.toStdString().data(), chIdle);
    qDebug() << chBuf;
    const char* chY = strchr(chBuf, '=');
    char* chY1 = (char*)malloc(sizeof(chY));
    char* chY2 = (char*)malloc(sizeof(chY));
    strcpy(chY1, chY);
    int i = 1;
    while(chY1[i] != '\0')
    {
        chY2[i-1] = chY1[i];
        i++;
    }

    QPoint pt = QPoint(atoi(chX2), atoi(chY2));

    free(chX);
    free(chX1);
    free(chX2);
    free(chY1);
    free(chY2);
    return pt;
}
