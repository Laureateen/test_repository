#ifdef   __cplusplus
extern "C" {
#endif

#ifndef CTA_TRACK_H
#define CTA_TRACK_H


#include "object_tracker_types.h"
#include "vehicle_data_types.h"
#include "common_vector.h"
#include "object_tracker_types.h"
#include "common_math.h"
#include "CTA_cfg.h"
#include "FeatureTrackerAdapter.h"
	//#include "BlindSpotDetection.h"

#define NUM_TRACKS_OUT 30 /*Max num of tracks sent out*/
#define REFERANCE_X 0 /*referance line to calculate the cross point, line x=REFERANCE_X*/


	typedef struct
	{
		bool_t isReleventObjectExist; /*Target Moving in Zone in current cycle*/
		bool_t wasReleventObjectExist;
		u16_t reserved;
		//char isBSDWarning; /*Only for warning in feature, Not the warning state output*/
	}CTA_Track_Flags_t;

	typedef struct {
		bool_t b_isAssociated;
		bool_t b_isMovingInCTAZone;
		bool_t b_isWarningConditionMeet;
		u8_t ObjectID;
		f32_t TTE;
		f32_t DTE_Entry;
		f32_t DTE_Exit;
		p_track_t pTrack;
		u8_t reserved;
		u16_t reserved2;
	} CTATrack_t; /*20 bytes*/

	typedef struct {
		CTATrack_t track[NUM_TRACKS_OUT]; /*600 bytes*/
		u16_t numTracks; 
		CTA_Track_Flags_t Flags;
		u16_t reserved;
	} CTA_TrackList_t; /*608 bytes*/

	typedef enum
	{
		CTA_TRACK_REMOVE_LOW_QUALITY_TRACK, /* Remove low quality tracks */
		CTA_TRACK_MATCH_REMOVECRITERIA_MAX
	} CTA_TRACK_trackRemovalCriteria_t;

	typedef enum
	{
		CTA_TRACK_ISTRACKVELOCITYMEET,
		CTA_TRACK_ISTRACKDTEMEET,
		CTA_TRACK_ISTRACKTTEMEET,
		CTA_TRACK_MATCH_WARNINGCONDITION_MAX
	} CTA_TRACK_trackWarningCondition_t;


	void run_CTA_Track(CTA_TrackList_t* CTATrackList, const FeatureTrackList_t* const input_objlist, dynamicData_t* DynData, CTA_ParamConfig_t* paraCfg);
	void CTA_trackList_Init(CTA_TrackList_t* self);



#endif // CTA_TRACK_H

#ifdef   __cplusplus
}
#endif