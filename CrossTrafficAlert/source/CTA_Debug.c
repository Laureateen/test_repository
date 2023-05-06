#define _CRT_SECURE_NO_WARNINGS
#include "CTA_Debug.h"

static void WriteLine(FILE* fp, char* filename, float* PosX, float* PosY, int length);
static void WritePolygon(FILE* fp, char* filename, fVec2_t* points, int pointsNum);


static void WriteLine(FILE* fp, char* filename, float* PosX, float* PosY, int length)
{
    int index = 0;

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);

    }
    setbuf(fp, NULL);

    for (index = 0; index < length; index++)
    {
        fprintf(fp, "%f, %f \n", PosX[index], PosY[index]);
    }
    //fseek(fp, 0, SEEK_SET);
    fflush(fp);
    fclose(fp);
    //_commit(_fileno(fp));
}

static void WritePolygon(FILE* fp, char* filename, fVec2_t* points, int pointsNum)
{
    int index = 0;

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);

    }
    setbuf(fp, NULL);

    for (index = 0; index < pointsNum; index++)
    {
        fprintf(fp, "%f, %f \n", points[index].x, points[index].y);
    }
    //fseek(fp, 0, SEEK_SET);
    fflush(fp);
    fclose(fp);
    //_commit(_fileno(fp));
}


void WriteDOWZoneLine(CTA_ParamConfig_t* ctapara)
{
    char* WarningZone = {"D:/Code/RadarFeature/temp/CTARectangle.tmp"};
    FILE* BSDLine = NULL;
    int pointsNum = 5;
    fVec2_t points[5];

    points[0].x = ctapara->relevantZoneTopLeft_PosX_m;
    points[0].y = ctapara->relevantZoneTopLeft_PosY_m;
    points[1].x = ctapara->relevantZoneTopRight_PosX_m;
    points[1].y = ctapara->relevantZoneTopRight_PosY_m;
    points[2].x = ctapara->relevantZoneBottomRight_PosX_m;
    points[2].y = ctapara->relevantZoneBottomRight_PosY_m;
    points[3].x = ctapara->relevantZoneBottomLeft_PosX_m;
    points[3].y = ctapara->relevantZoneBottomLeft_PosY_m;
    points[4].x = ctapara->relevantZoneTopLeft_PosX_m;
    points[4].y = ctapara->relevantZoneTopLeft_PosY_m;
    WritePolygon(BSDLine, WarningZone, points, pointsNum);


}
