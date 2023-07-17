#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CrossTrafficAlert.h"
//#include "object_tracker_types.h"
//2322
#pragma warning(disable : 4996)
static void PlotInterface(FILE* gnupipe, trackList_t* p_InputTrackList, CTA_ClassMembers_t* p_thisMember);

int main()
{
    vehicleData_t* p_vehData;
    CTA_ClassMembers_t* p_thisMember;
    trackList_t* p_InputTrackList;

    //memset(p_thisTrackList, 0, sizeof(BSD_TrackList_t));
    p_thisMember = (CTA_ClassMembers_t*)malloc(sizeof(CTA_ClassMembers_t));
    memset(p_thisMember, 0, sizeof(CTA_ClassMembers_t));
    p_vehData = (vehicleData_t*)malloc(sizeof(vehicleData_t));
    memset(p_vehData, 0, sizeof(vehicleData_t));
    p_InputTrackList = (trackList_t*)malloc(sizeof(trackList_t));
    memset(p_InputTrackList, 0, sizeof(trackList_t));
    /********************************Define_for_tracks*********************************/
    Init_CrossTrafficAlert(p_thisMember, p_vehData);

    FILE* fp = fopen("D:/data_files/Data/Rtable.csv", "r");
    if (fp == NULL) {
        fprintf(stderr, "fopen() failed.\n");
        exit(EXIT_FAILURE);
    }

    char row[100];
    char* token;
    double Data[200][9];
    double temp;
    char* ptr;
    unsigned int indexRow = 0;
    unsigned int loops = 0;
    int indexCol = 0;
    float step = 0.1;
    float timer = 0;
    bool isfirsttoken;
    int ActorNum = 0;
    int indexActor = 0;

    FILE* gnupipe = NULL;

    gnupipe = _popen("gnuplot -persist", "w");
    fprintf(gnupipe, "set style line 1 lc rgb 'grey' pt 3 ps 0.5 lt 1 lw 1 \n");
    fprintf(gnupipe, "set style line 2 lc rgb 'yellow' pt 3 ps 0.5 lt 1 lw 1 \n");
    fprintf(gnupipe, "set xrange [-10:40]\n");
    fprintf(gnupipe, "set yrange [-10:40]\n");
    fprintf(gnupipe, "set datafile separator \",\"\n");
    fprintf(gnupipe, "set key off \n");

    while (fgets(row, 100, fp) != NULL) {
        printf("Row: %s", row);
        token = strtok(row, ",");
        indexRow++;
        indexCol = 0;
        isfirsttoken = true;
        while (token != NULL) {

            //printf("Data[%d][%d] = %lf \n",indexRow,indexCol, Data[indexRow][indexCol] );
            printf("Token: %s\n", token);
            Data[indexRow][indexCol] = strtod(token, &ptr);
            indexCol++;
            token = strtok(NULL, ",");
        }
        ActorNum = indexCol / 4;
        p_InputTrackList->numTracks = 0;
        for (indexActor = 1; indexActor < ActorNum; indexActor++)
        {
            //p_thisMember->bsdTrackList.track[indexActor-1].pTrack = (track_t*)malloc(sizeof(track_t));
            //memset(p_thisMember->bsdTrackList.track[indexActor-1].pTrack, 0, sizeof(track_t));
            //p_thisMember->bsdTrackList.track[indexActor-1].pTrack->pos.ptClose.x = Data[indexRow][0] - Data[indexRow][4 * indexActor];
            //p_thisMember->bsdTrackList.track[indexActor-1].pTrack->pos.ptClose.y = Data[indexRow][4 * indexActor + 1] - Data[indexRow][1] - 1;
            //p_thisMember->bsdTrackList.track[indexActor-1].pTrack->relVel.x = Data[indexRow][4 * indexActor + 2] - Data[indexRow][2];
            //p_thisMember->bsdTrackList.track[indexActor-1].pTrack->relVel.y = Data[indexRow][4 * indexActor + 3];
            //p_thisMember->bsdTrackList.track[indexActor - 1].pTrack->info.trackId = indexActor;
            //p_thisMember->bsdTrackList.track[indexActor - 1].ObjectID = indexActor;
            //p_thisMember->bsdTrackList.track[indexActor - 1].b_isAssociated = TRUE;
            //p_thisMember->bsdTrackList.numTracks++;
            //这是画图用的；
            p_InputTrackList->track[indexActor - 1].boundingBox.center.x = Data[indexRow][0] - Data[indexRow][4 * indexActor];//与车辆的相对坐标
            p_InputTrackList->track[indexActor - 1].boundingBox.center.y = Data[indexRow][4 * indexActor + 1] - Data[indexRow][1] - 1;//y为什么考虑车宽而x不用考虑车长。
            p_InputTrackList->track[indexActor - 1].boundingBox.heading = 0;
            p_InputTrackList->track[indexActor - 1].boundingBox.length = 5;
            p_InputTrackList->track[indexActor - 1].boundingBox.width = 2;
            
            p_InputTrackList->track[indexActor-1].pos.ptClose.x = Data[indexRow][0] - Data[indexRow][4 * indexActor];//最近点的坐标
            p_InputTrackList->track[indexActor - 1].pos.ptClose.y = Data[indexRow][4 * indexActor + 1] - Data[indexRow][1] - 2;//自车宽度和目标车跨度,最终得到辆车的实际距离
            p_InputTrackList->track[indexActor - 1].pos.ptLeft.x = p_InputTrackList->track[indexActor - 1].pos.ptClose.x;
            p_InputTrackList->track[indexActor - 1].pos.ptLeft.y = p_InputTrackList->track[indexActor - 1].pos.ptClose.y - 2;
            p_InputTrackList->track[indexActor - 1].pos.ptRight.x = p_InputTrackList->track[indexActor - 1].pos.ptClose.x;
            p_InputTrackList->track[indexActor - 1].pos.ptRight.y = p_InputTrackList->track[indexActor - 1].pos.ptClose.y + 2;
            p_InputTrackList->track[indexActor - 1].vel.x = Data[indexRow][4 * indexActor + 2]; //Positive value when the vehicle driving forward
            p_InputTrackList->track[indexActor - 1].vel.y = Data[indexRow][4 * indexActor + 3];
            p_InputTrackList->track[indexActor - 1].relVel.x = Data[indexRow][4 * indexActor + 2] - Data[indexRow][2]; //Positive value when the vehicle driving forward
            p_InputTrackList->track[indexActor - 1].relVel.y = Data[indexRow][4 * indexActor + 3];
            p_InputTrackList->track[indexActor - 1].info.trackId = indexActor;
            p_InputTrackList->numTracks++;
        }
        p_vehData->dynamicData.hostVel_mps = Data[indexRow][2];
        p_vehData->dynamicData.curvature = 500;
        p_vehData->staticData.yOffset_m = 0.5;
        Main_CrossTrafficAlert(p_thisMember, (vehicleData_t*)p_vehData, p_InputTrackList);//(CTA标识，Host vehicle state，TrakList)
        PlotInterface(gnupipe, p_InputTrackList, p_thisMember);
    }

    /***********************Track_Mapping*******************************************/
    //p_thisMember->bsdTrackList.numTracks = 1;
    //loops = indexRow;
    /***********************Track_Mapping*******************************************/

    //for (indexRow = 0; indexRow < loops; indexRow++)
    //{
        //  for(indexCol=0;indexCol<6;indexCol++)
        //  {
        //      printf("%lf ",Data[indexRow][indexCol] );
        //  }
        //  printf("\n");
        //p_thisMember->bsdTrackList.track[0].pTrack->pos.ptClose.x = -Data[indexRow + 1][5];
        //p_thisMember->bsdTrackList.track[0].pTrack->pos.ptClose.y = Data[indexRow + 1][6];
        //p_thisMember->bsdTrackList.track[0].pTrack->relVel.x = Data[indexRow + 1][7];
        //p_thisMember->bsdTrackList.track[0].pTrack->relVel.y = Data[indexRow + 1][8];
       // p_thisMember->bsdTrackList. = (track_t*)malloc(sizeof(track_t));
        //memset(p_thisMember->bsdTrackList.pTrack->p_first, 0, sizeof(track_t));
        //p_thisMember->bsdTrackList.p_first->pos.ptClose.x = -Data[indexRow + 1][5];
       // p_thisMember->bsdTrackList.p_first->pos.ptClose.y = Data[indexRow + 1][6];
        //p_vehData->dynamicData.hostVel_mps = Data[indexRow + 1][3];
        //Main_BlindSpotDetection(p_thisMember, (vehicleData_t*)p_vehData);

        /*BSD Function Start*/
    //}

    free(p_thisMember);
    free(p_vehData);
    fclose(fp);
    return 0;
}

static void PlotInterface(FILE* gnupipe, trackList_t* p_InputTrackList, CTA_ClassMembers_t* p_thisMember)
{
    int index = 0;
    int index_RT = 0;
    float PosX = 0;
    float PosY = 0;
    float Velx = 0;
    float Vely = 0;
    float LabelY = 0;
    fVec2_t arrawEnd[30];
    int ID = 0;
    track_t* pCurtrk = NULL;
    bool_t isWarningTrack = FALSE;
    for (index = 1; index < (p_InputTrackList->numTracks + 1); index++)
    {
        isWarningTrack = FALSE;
        pCurtrk = &p_InputTrackList->track[index - 1];
        PosX = pCurtrk->pos.ptClose.x;
        PosY = pCurtrk->pos.ptClose.y;
        Velx = pCurtrk->relVel.x;
        Vely = pCurtrk->relVel.y;
        ID = pCurtrk->info.trackId;
        LabelY = 39 - index * 1.5;
        fprintf(gnupipe, "unset label %d\n", index);
        fprintf(gnupipe, "unset arrow\n");
        for (index_RT = 0; index_RT < p_thisMember->CTATrackList.numTracks; index_RT++)
        {
            if (pCurtrk->info.trackId == p_thisMember->CTATrackList.track[index_RT].ObjectID)
            {
                if ((p_thisMember->CTATrackList.track[index_RT].b_isWarningConditionMeet == TRUE) && (p_thisMember->CTATrackList.track[index_RT].b_isMovingInCTAZone == TRUE))
                {
                    isWarningTrack = TRUE;
                }
            }
        }
        if (isWarningTrack == TRUE)
        {
            fprintf(gnupipe, "set object %d circle at %f,%f size 0.3 arc [0:360] fc rgb \"red\" fs solid\n", index, PosX, PosY);
        }
        else
        {
            fprintf(gnupipe, "set object %d circle at %f,%f size 0.3 arc [0:360] fc rgb \"orange\" fs solid\n", index, PosX, PosY);
        }
        fprintf(gnupipe, "set arrow from %f,%f to %f,%f size 0.1 filled\n", PosX, PosY, PosX - Velx, Vely + PosY);
        fprintf(gnupipe, "set label %d \"%d\" at %f,%f center\n", index, ID, PosX, PosY + 1);
        fprintf(gnupipe, "set label %d \"ObjectID     x          y          Vx          Vy\" at 25,39\n", p_InputTrackList->numTracks + index);
        fprintf(gnupipe, "set label %d \"%d      %.3f    %.3f    %.3f    %.3f\" at 27,%f\n", p_InputTrackList->numTracks + 2 * index, ID, PosX, PosY, Velx, Vely, LabelY);

    }


    fprintf(gnupipe, "plot 'D:/Code/RadarFeature/temp/CTARectangle.tmp' using 1:2 with lines lw 2 lc rgb \"black \"\n");




    fprintf(gnupipe, "pause 0.2\n");
    fflush(gnupipe);
    fprintf(gnupipe, "unset multiplot\n");
    //_sleep(0.5);
}