#include "CTA_track.h"



/*******************************Function for warning condition check***********************************************/
typedef bool_t(*CTA_Track_checkTrackWarningCondition_Func_t) (const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg);

static bool_t CTA_Track_isTrackVelocityMeet(const CTATrack_t* CTATrack, CTA_ParamConfig_t* paraCfg);
static bool_t CTA_Track_isTrackDTEMeet(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg);
static bool_t CTA_Track_isTrackTTEMeet(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg);
static bool_t CTA_Track_isTrackHeadingMeet(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg);

static const CTA_Track_checkTrackWarningCondition_Func_t CTA_Track_WarningConditionCheckFuncArray[] = {
    CTA_Track_isTrackVelocityMeet,
    CTA_Track_isTrackDTEMeet,
    CTA_Track_isTrackTTEMeet,
    CTA_Track_isTrackHeadingMeet
};
static inline bool_t CTA_Track_checkTrackWarningConditionFunc(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg, CTA_TRACK_trackWarningCondition_t warningCondition);
static bool_t CTA_Track_CheckWarningCondition(CTA_TrackList_t* CTAtracklist, CTA_ParamConfig_t* paraCfg);
/*******************************Function for warning condition check***********************************************/


/*******************************Function for Criteria to remove track Special for feature**************************/
typedef bool_t(*CTA_Track_checkTrackRemovalCriteria_Func_t) (const track_t* track);

static bool_t CTA_Track_checkLowQualityTrack(const track_t* track);
static const CTA_Track_checkTrackRemovalCriteria_Func_t CTA_Track_matchTrackRemovalCriteriaFuncArray[] = {
   CTA_Track_checkLowQualityTrack 
};
static inline bool_t CTA_Track_matchTrackRemovalCriteriaFunc(const track_t* track, CTA_TRACK_trackRemovalCriteria_t removalCriteria);
static bool_t CTA_Track_removeTracksByCriteria(CTA_TrackList_t* CTAtracklist, const FeatureTrackList_t* const input_objlist, CTA_TRACK_trackRemovalCriteria_t removalCriteria);
/*******************************Function for Criteria to remove track Special for feature**************************/


static void CTA_track_init_track(track_t* track);

/******************************* Calculate for warning check ***************************************/
static bool_t CTA_track_isMovinginRelevantZone(CTA_ParamConfig_t* paraCfg, CTA_TrackList_t* CTATrackList);
static bool_t CTA_calculate_TTE(CTA_TrackList_t* CTATrackList);
static bool_t CTA_calculate_EntryPoint(CTA_TrackList_t* CTATrackList, dynamicData_t* VehDyn);
static bool_t CTA_calculate_ExitPoint(CTA_TrackList_t* CTATrackList, dynamicData_t* VehDyn, f32_t VehicleWidth);
static void CTA_track_CheckReleventObject(CTA_TrackList_t* CTATrackList);
/******************************* Calculate for warning check ***************************************/

#define CURVECASE (1.0f) //rad/s

/********************Criteria to remove track Special for feature start*****************************/
static bool_t CTA_Track_checkLowQualityTrack(const track_t* track)
{
    bool_t ret = FALSE;

    if (track != NULL)
    {
    }

    return ret;
}

static bool_t CTA_Track_removeTracksByCriteria(CTA_TrackList_t* CTAtracklist, const FeatureTrackList_t* const input_objlist, CTA_TRACK_trackRemovalCriteria_t removalCriteria)
{
    bool_t ret;
    u16_t index;
    bool_t removeTrack;

    if ((input_objlist == NULL)
        || (input_objlist->numTracks > MAX_NUM_TRACKS)
        || ((u16_t)removalCriteria >= CTA_TRACK_MATCH_REMOVECRITERIA_MAX))
    {
        ret = FALSE;
    }
    else
    {
        CTAtracklist->numTracks = 0;
        for (index = 0; index < input_objlist->numTracks; index++)
        {
            removeTrack = CTA_Track_matchTrackRemovalCriteriaFunc(&(input_objlist->track[index]), removalCriteria);

            //if (removeTrack == TRUE)
            //{
            //    CTA_track_init_track(&(CTAtracklist->track[index]));
            //}
            if (removeTrack == FALSE)
            {
                CTAtracklist->track[CTAtracklist->numTracks].pTrack = &input_objlist->track[index].pTrack;
                CTAtracklist->track[CTAtracklist->numTracks].ObjectID = input_objlist->track[index].ObjectID;
                CTAtracklist->numTracks++;
            }
        }

        ret = TRUE;
    }

    return ret;
}


static inline bool_t CTA_Track_matchTrackRemovalCriteriaFunc(const track_t* track, CTA_TRACK_trackRemovalCriteria_t removalCriteria)
{
    bool_t ret = FALSE;
    CTA_Track_checkTrackRemovalCriteria_Func_t checkTrackRemovalCriteriaFuncPtr;

    if (track != NULL)
    {
        checkTrackRemovalCriteriaFuncPtr = CTA_Track_matchTrackRemovalCriteriaFuncArray[removalCriteria];
        ret = (*checkTrackRemovalCriteriaFuncPtr)(track);
    }

    return ret;
}
/********************Criteria to remove track Special for feature end******************************/



/*********************Warning Condition of track check for CTA start********************************/
static bool_t CTA_Track_isTrackVelocityMeet(const CTATrack_t* CTATrack, const  CTA_ParamConfig_t* paraCfg)
{
    bool_t ret = FALSE;
    float RelVel = 0;

    if (CTATrack != NULL)
    {
        RelVel = FCN_IQMag(CTATrack->pTrack->relVel.y, CTATrack->pTrack->relVel.x);
        if ((RelVel > paraCfg->relVelMin_mps) && (RelVel < paraCfg->relVelMax_mps))
        {
            ret = TRUE;
        }
    }

    return ret;
}

static bool_t CTA_Track_isTrackDTEMeet(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg)
{
    bool_t ret = FALSE;

    if (CTATrack != NULL)
    {
        if (((CTATrack->DTE_Entry < paraCfg->OrthoDistMax_m) && (CTATrack->DTE_Entry > paraCfg->OrthoDistMin_m))||
            (CTATrack->DTE_Exit < paraCfg->OrthoDistMax_m))
        {
            ret = TRUE;
        }
    }

    return ret;
}

static bool_t CTA_Track_isTrackTTEMeet(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg)
{
    bool_t ret = FALSE;

    if (CTATrack != NULL)
    {
        if (CTATrack->TTE < paraCfg->ttc_limit_ms)
        {
            ret = TRUE;
        }
    }

    return ret;
}

static bool_t CTA_Track_isTrackHeadingMeet(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg)
{
    bool_t ret = FALSE;

    if (CTATrack != NULL)
    {
        if (CTATrack->pTrack->boundingBox.heading > paraCfg->ttc_limit_ms) //TBC: which heading is needed? from BOX or from close point
        {
            ret = TRUE;
        }
    }

    return ret;
}

static bool_t CTA_Track_CheckWarningCondition(CTA_TrackList_t* CTAtracklist, CTA_ParamConfig_t* paraCfg)
{
    bool_t ret;
    u16_t index;
    u16_t warnConditionNum;
    bool_t ConditionMeet = TRUE;

    if ((CTAtracklist == NULL)
        || (CTAtracklist->numTracks > MAX_NUM_TRACKS))
    {
        ret = FALSE;
    }
    else
    {

        for (index = 0; index < CTAtracklist->numTracks; index++)
        {
            CTAtracklist->track[index].b_isWarningConditionMeet = TRUE;
            for (warnConditionNum = 0; warnConditionNum < CTA_TRACK_MATCH_WARNINGCONDITION_MAX; warnConditionNum++)
            {
                ConditionMeet = CTA_Track_checkTrackWarningConditionFunc(&(CTAtracklist->track[index]), paraCfg, warnConditionNum);
                if (ConditionMeet == FALSE)
                {
                    CTAtracklist->track[index].b_isWarningConditionMeet = FALSE;
                }
                else
                {

                }
            }

        }

        ret = TRUE;
    }

    return ret;
}

static inline bool_t CTA_Track_checkTrackWarningConditionFunc(const CTATrack_t* CTATrack, const CTA_ParamConfig_t* paraCfg, CTA_TRACK_trackWarningCondition_t warningCondition)
{
    bool_t ret = FALSE;
    CTA_Track_checkTrackWarningCondition_Func_t checkTrackWarningConditionFuncPtr;

    if (CTATrack != NULL)
    {
        checkTrackWarningConditionFuncPtr = CTA_Track_WarningConditionCheckFuncArray[warningCondition];
        ret = (*checkTrackWarningConditionFuncPtr)(CTATrack, paraCfg);
    }

    return ret;
}

/*********************Warning Condition of track check for CTA end**********************************/




void CTA_trackList_Init(CTA_TrackList_t* self)
{
    u32_t idx;
    track_t* p_curTrk;

    /* Initialize the trackList header */
    FCN_memset(self, 0, sizeof(CTA_TrackList_t));

    /* Initialize individual track slots */
    for (idx = 0u; idx < NUM_TRACKS_OUT; idx++) {
        self->track[idx].pTrack = NULL;
    }

}

//（加载目标的具体状态）（自车状态）（CTA的系统属性）
void run_CTA_Track(CTA_TrackList_t* CTATrackList, const FeatureTrackList_t* const input_objlist, dynamicData_t* DynData, CTA_ParamConfig_t* paraCfg)
{
    u16_t Sensor_pos = Rear_Left; //Todo: need toget the sensor position from APP
    f32_t VehicleWidth = 2.0f;  //Todo: need toget the sensor position from APP
    CTA_Track_removeTracksByCriteria(CTATrackList, input_objlist, CTA_TRACK_REMOVE_LOW_QUALITY_TRACK);//过滤掉不好的tracker。
    CTA_track_isMovinginRelevantZone(paraCfg, CTATrackList);
    CTA_calculate_TTE(CTATrackList);
    CTA_calculate_EntryPoint(CTATrackList, DynData);
    CTA_calculate_ExitPoint(CTATrackList, DynData, VehicleWidth);
    CTA_Track_CheckWarningCondition(CTATrackList, paraCfg);  // check of the track meet warning condition
    CTA_track_CheckReleventObject(CTATrackList); //find out the track moving in warning Zone and meet warning condition

    

    //Debug_WriteLCWTrackList(CTATrackList);
}


static void CTA_track_init_track(track_t* track)
{
    FCN_memset(track, 0, sizeof(track_t));
    track->p_next = NULL;
}

/*
*Check if track is in CTA warning Zone.
* Use the ClosePoint
* Called by run_CTA_Track in CTA_track.c
*/
static bool_t CTA_track_isMovinginRelevantZone(CTA_ParamConfig_t* paraCfg, CTA_TrackList_t* CTATrackList)
{
    u16_t indexTrack = 0;
    u16_t indexVert = 0;
    CTATrack_t* pcurTrk = NULL;
    fVec2_t polygon[4];
    polygon[0].x = paraCfg->relevantZoneBottomLeft_PosX_m;
    polygon[0].y = paraCfg->relevantZoneBottomLeft_PosY_m;
    polygon[1].x = paraCfg->relevantZoneBottomRight_PosX_m;
    polygon[1].y = paraCfg->relevantZoneBottomRight_PosY_m;
    polygon[2].x = paraCfg->relevantZoneTopRight_PosX_m;
    polygon[2].y = paraCfg->relevantZoneTopRight_PosY_m;
    polygon[3].x = paraCfg->relevantZoneTopLeft_PosX_m;
    polygon[3].y = paraCfg->relevantZoneTopLeft_PosY_m;
    u16_t num_vertices = 4;
    u16_t intersections = 0;
    bool_t isRightofLineSegment = FALSE;
    fVec2_t pointA;
    fVec2_t pointB;
    fVec2_t pointP;
    bool_t ret = FALSE;
    float temp = 0;

    if ((CTATrackList == NULL)
        || (CTATrackList->numTracks > MAX_NUM_TRACKS))
    {
        ret = FALSE;
    }
    else
    {
        for (indexTrack = 0; indexTrack < CTATrackList->numTracks; indexTrack++)
        {
            /*point in plygon test*/
            intersections = 0;
            pcurTrk = &CTATrackList->track[indexTrack];
            pcurTrk->b_isMovingInCTAZone = FALSE; //Reset for Zone check
            pointP.x = pcurTrk->pTrack->pos.ptClose.x;
            pointP.y = pcurTrk->pTrack->pos.ptClose.y;
            for (indexVert = 0; indexVert < num_vertices; indexVert++)
            {
                pointA = polygon[indexVert];
                pointB = polygon[(indexVert + 1) % num_vertices];
                /*Ray Casting Algorithm*/
                //isRightofLineSegment = (((pointB.x - pointA.x) * (pointP.y - pointA.y) - (pointB.y - pointA.y) * (pointP.x - pointA.x)) < 0) ? TRUE : FALSE;
                isRightofLineSegment = FALSE;
                if ((pointA.x > pointP.x) != (pointB.x > pointP.x))
                {
                    temp = (pointB.y - pointA.y) * (pointP.x - pointA.x) / (pointB.x - pointA.x) + pointA.y;
                    if(pointP.y < temp)
                    {
                        isRightofLineSegment = TRUE;
                    }

                }

                if (isRightofLineSegment == TRUE)
                {
                    intersections++;
                }
            }
            if ((intersections % 2) != 0)
            {
                pcurTrk->b_isMovingInCTAZone = TRUE;
                ret = TRUE;
            }
        }

    }
    WriteDOWZoneLine(paraCfg);
/*Todo : should be a check of if target is moving on the boundary, specail process should be used for the target*/
}

static bool_t CTA_calculate_TTE(CTA_TrackList_t* CTATrackList)
{
    u16_t indexTrack = 0;
    bool_t ret = FALSE;
    CTATrack_t* pcurTrk = NULL;

    if ((CTATrackList == NULL)
        || (CTATrackList->numTracks > MAX_NUM_TRACKS))
    {
        ret = FALSE;
    }
    else
    {
        for (indexTrack = 0; indexTrack < CTATrackList->numTracks; indexTrack++)
        {
            pcurTrk = &CTATrackList->track[indexTrack];
            if (pcurTrk->pTrack->relVel.y < -VERY_SMALL_VALUE)
            {
                pcurTrk->TTE = pcurTrk->pTrack->pos.ptClose.y / FCN_fabs(pcurTrk->pTrack->relVel.y);
            }
            else
            {
                pcurTrk->TTE = F32_MIN;
            }
        }

    }
}


static bool_t CTA_calculate_EntryPoint(CTA_TrackList_t* CTATrackList, dynamicData_t* VehDyn)
{
    u16_t indexTrack = 0;
    bool_t ret = FALSE;
    CTATrack_t* pcurTrk = NULL;
    fVec2_t EgoPos;
    if ((CTATrackList == NULL)
        || (CTATrackList->numTracks > MAX_NUM_TRACKS))
    {
        ret = FALSE;
    }
    else
    {
        for (indexTrack = 0; indexTrack < CTATrackList->numTracks; indexTrack++)
        {
            pcurTrk = &CTATrackList->track[indexTrack];
            if ((pcurTrk->b_isMovingInCTAZone == TRUE) && (pcurTrk->TTE > VERY_SMALL_VALUE) && (VehDyn->hostVel_mps > -VERY_SMALL_VALUE))
            {
                if (VehDyn->yawRate_radps < CURVECASE)
                {
                    EgoPos.x = VehDyn->hostVel_mps * (pcurTrk->TTE);
                }
                else
                {
                    /*Todo: if special process is needed in curve case?*/
                }
                pcurTrk->DTE_Entry = pcurTrk->pTrack->pos.ptClose.x + pcurTrk->pTrack->relVel.x * pcurTrk->TTE - EgoPos.x;

            }
            else
            {
                pcurTrk->DTE_Entry = F32_MAX;
            }
        }
    }

}

static bool_t CTA_calculate_ExitPoint(CTA_TrackList_t* CTATrackList, dynamicData_t* VehDyn, f32_t VehicleWidth)
{
    u16_t indexTrack = 0;
    bool_t ret = FALSE;
    CTATrack_t* pcurTrk = NULL;
    fVec2_t EgoPos;
    f32_t ExitTime;
    if ((CTATrackList == NULL)
        || (CTATrackList->numTracks > MAX_NUM_TRACKS))
    {
        ret = FALSE;
    }
    else
    {
        for (indexTrack = 0; indexTrack < CTATrackList->numTracks; indexTrack++)
        {
            pcurTrk = &CTATrackList->track[indexTrack];
            if ((pcurTrk->b_isMovingInCTAZone == TRUE) && (pcurTrk->TTE > VERY_SMALL_VALUE) && (VehDyn->hostVel_mps > -VERY_SMALL_VALUE))
            {
                ExitTime = (pcurTrk->pTrack->pos.ptClose.y + VehicleWidth + pcurTrk->pTrack->boundingBox.length * FCN_sin(pcurTrk->pTrack->boundingBox.heading));
                if (VehDyn->yawRate_radps < CURVECASE)
                {
                    EgoPos.x = VehDyn->hostVel_mps * ExitTime;
                }
                else
                {
                    /*Todo: if special process is needed in curve case?*/
                }
                pcurTrk->DTE_Entry = pcurTrk->pTrack->pos.ptClose.x + pcurTrk->pTrack->relVel.x * ExitTime - EgoPos.x;

            }
            else
            {
                pcurTrk->DTE_Entry = F32_MAX;
            }
        }
    }
}

static void CTA_track_CheckReleventObject(CTA_TrackList_t* CTATrackList)
{
    bool_t ret = FALSE;
    u16_t index = 0;
    CTATrack_t* ReleventObject = NULL;
    if (CTATrackList == NULL)
    {
        ret = FALSE;
    }
    else
    {
        CTATrackList->Flags.wasReleventObjectExist = FALSE;
        if (CTATrackList->Flags.isReleventObjectExist == TRUE)
        {
            CTATrackList->Flags.wasReleventObjectExist = TRUE;
        }
        CTATrackList->Flags.isReleventObjectExist = FALSE;
        for (index = 0; index < CTATrackList->numTracks; index++)
        {
            if ((CTATrackList->track[index].b_isMovingInCTAZone == TRUE) &&
                (CTATrackList->track[index].b_isWarningConditionMeet == TRUE))
            {
                ReleventObject = &CTATrackList->track[index];
                CTATrackList->Flags.isReleventObjectExist = TRUE;
                /*ToDo: if more than one track relevent?*/
            }
        }
    }
}
