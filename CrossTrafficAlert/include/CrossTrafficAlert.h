#ifdef __cplusplus
extern "C" {
#endif

#ifndef CROSSTRAFFICALERT_H
#define CROSSTRAFFICALERT_H

#include "common_types.h"
#include "common_math.h"
#include "common_tracker.h"
#include "CTA_cfg.h"
#include "CTA_track.h"
#include "FeatureTrackerAdapter.h"

	typedef enum
	{
		CTA_Off,
		CTA_Init,
		CTA_Standby,
		CTA_Passive,
		CTA_WarningLv1,
		CTA_WarningLv2,
		CTA_Fault,
		CTA_Status_Num
	}CTAFunctionStatus;


	typedef struct
	{
		u32_t stLen;
		u16_t stType;
		u16_t stVer;
	}CTAStructInfo_t;

	typedef struct
	{
		u16_t ZoneHysteresisCounter;
		u16_t reserved;
	}CTACounters_t;

	typedef struct
	{
		u8_t State;
		u8_t ObjID;
		u8_t warnRequest;
		u8_t reserved;
	}CTA_Out_t;

	typedef struct
	{
		CTAStructInfo_t strInfo; /*8 bytes*/
		CTA_ParamConfig_t ctaparamconfig; /*60 bytes*/
		CTA_TrackList_t CTATrackList;  /*608 bytes*/
		CTA_Out_t cta_out; /*4 bytes*/
		u16_t CTA_FunctionState;
		u16_t CTA_FunctionStatePre;
		CTACounters_t CTAcounters; /*4 bytes*/
		bool_t isWarningZoneHysteresis;
		u8_t reserved;
		u16_t reserved2;
	}CTA_ClassMembers_t; /*692 bytes*/

	extern void Main_CrossTrafficAlert(vehicleData_t* vehicleData, FeatureTrackList_t* p_InputTrackList);
	extern void Init_CrossTrafficAlert(void);
	extern void OutputData_CrossTrafficAlert(CTA_ClassMembers_t* Output_DebugData_CTA);
#endif //CROSSTRAFFICALERT_H

#ifdef __cplusplus
}
#endif