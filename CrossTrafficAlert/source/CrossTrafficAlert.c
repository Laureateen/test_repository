#include "CTA_cfg.h"
#include "CrossTrafficAlert.h"
#include "CTA_track.h"


static void Run_CrossTrafficAlert(vehicleData_t* vehicleData);
static bool_t Do_CTA_init(CTA_ParamConfig_t bsmparamconfig);
static bool_t Do_CTA_Standby(f32_t Host_Velocity);
static bool_t Do_CTA_Passive(CTA_Track_Flags_t* track_flags);
static bool_t Do_CTA_WarningLv1(void);
static bool_t Do_CTA_WarningLv2();
static bool_t Do_CTA_Fault();
static bool_t CheckFaultExistance();


CTA_ParamConfig_t CTA_cfg; 
CTA_ClassMembers_t CTA_ClassMenbers;

extern void Init_CrossTrafficAlert() //FakeApp need the para to run, for the function later, get from APP is needed
{
	//memset(self, 0, sizeof(CTA_ClassMembers_t));  // Todo: the config parameter is now local global para, the input parameter is not work, need fix later
	CTA_ClassMenbers.ctaparamconfig.relevantZoneBottomLeft_PosX_m = CTA_DEF_CFG_RELEVANT_POS_BOTTOMLEFT_X_M;
	CTA_ClassMenbers.ctaparamconfig.relevantZoneBottomLeft_PosY_m = CTA_DEF_CFG_RELEVANT_POS_BOTTOMLEFT_Y_M;
	CTA_ClassMenbers.ctaparamconfig.relevantZoneBottomRight_PosX_m = CTA_DEF_CFG_RELEVANT_POS_BOTTOMRIGHT_X_M;
	CTA_ClassMenbers.ctaparamconfig.relevantZoneBottomRight_PosY_m = CTA_DEF_CFG_RELEVANT_POS_BOTTOMRIGHT_Y_M;
	CTA_ClassMenbers.ctaparamconfig.relevantZoneTopLeft_PosX_m = CTA_DEF_CFG_RELEVANT_POS_TOPLEFT_X_M;
	CTA_ClassMenbers.ctaparamconfig.relevantZoneTopLeft_PosY_m = CTA_DEF_CFG_RELEVANT_POS_TOPLEFT_Y_M;
	CTA_ClassMenbers.ctaparamconfig.relevantZoneTopRight_PosX_m = CTA_DEF_CFG_RELEVANT_POS_TOPRIGHT_X_M;
	CTA_ClassMenbers.ctaparamconfig.relevantZoneTopRight_PosY_m = CTA_DEF_CFG_RELEVANT_POS_TOPRIGHT_Y_M;
	CTA_ClassMenbers.ctaparamconfig.relVelMax_mps = CTA_DEF_REL_VELOCITY_MAX_MPS;
	CTA_ClassMenbers.ctaparamconfig.relVelMin_mps = CTA_DEF_REL_VELOCITY_MIN_MPS;
	CTA_ClassMenbers.ctaparamconfig.OrthoDistMax_m = CTA_DEF_CFG_ORTHO_DIST_MAX_M;
	CTA_ClassMenbers.ctaparamconfig.OrthoDistMin_m = CTA_DEF_CFG_ORTHO_DIST_MIN_M;
	CTA_ClassMenbers.ctaparamconfig.ttc_limit_ms = CTA_DEF_TTE_GATE;

	//CTA_ClassMenbers.CTA_FunctionState = 0;
}

extern void Main_CrossTrafficAlert(vehicleData_t* vehicleData, FeatureTrackList_t* p_InputTrackList)
{
	f32_t VehicleWidth = 1;// need get it from vehicle data, but current version doesn't involve
	if (CTA_ClassMenbers.CTA_FunctionState == CTA_Off)
	{
		if (1) //if(CTA user switch is on)
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Init;
		}
	}
	else if (CTA_ClassMenbers.CTA_FunctionState < CTA_Status_Num)//这个判断的依据
	{
		run_CTA_Track(&CTA_ClassMenbers.CTATrackList, p_InputTrackList, &vehicleData->dynamicData, &CTA_ClassMenbers.ctaparamconfig);//（加载目标的具体状态）（目标车状态）（CTA的系统属性）
		Run_CrossTrafficAlert(&CTA_ClassMenbers, vehicleData);

	}
	else
	{

	}
}


static void Run_CrossTrafficAlert(vehicleData_t* vehicleData)
{
	bool_t status = FALSE;
	bool_t Err = FALSE;

	switch (CTA_ClassMenbers.CTA_FunctionState)
	{
	case CTA_Init:
	{
		CTA_ClassMenbers.CTA_FunctionStatePre = CTA_ClassMenbers.CTA_FunctionState;
		status = Do_CTA_init(CTA_ClassMenbers.ctaparamconfig);
		if (status == TRUE)
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Standby;
		}
		else
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Fault; //Todo: Can add a counter if needed
		}
		break;
	}
	case CTA_Standby:
	{
		CTA_ClassMenbers.CTA_FunctionStatePre = CTA_ClassMenbers.CTA_FunctionState;
		status = Do_CTA_Standby(vehicleData->dynamicData.hostVel_mps);
		if (status == TRUE)
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Passive;
		}

		Err = CheckFaultExistance();
		if (Err == TRUE)
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Fault;
		}
		break;
	}
	case CTA_Passive:
	{
		CTA_ClassMenbers.CTA_FunctionStatePre = CTA_ClassMenbers.CTA_FunctionState;
		if (vehicleData->dynamicData.hostVel_mps > CTA_ClassMenbers.ctaparamconfig.DeactiveHostVelocity_mps)
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Standby;
		}
		else
		{
			status = Do_CTA_Passive(&CTA_ClassMenbers.CTATrackList.Flags);
			if (status == TRUE)
			{
				CTA_ClassMenbers.CTA_FunctionState = CTA_WarningLv1;
			}
		}
		Err = CheckFaultExistance();
		if (Err == TRUE)
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Fault;
		}
		break;
	}
	case CTA_WarningLv1:
	{
		CTA_ClassMenbers.CTA_FunctionStatePre = CTA_ClassMenbers.CTA_FunctionState;
		if (CTA_ClassMenbers.CTATrackList.Flags.isReleventObjectExist == TRUE)
		{
			status = Do_CTA_WarningLv1();
			if (status == TRUE)
			{
				CTA_ClassMenbers.CTA_FunctionState = CTA_WarningLv2;
			}
			CTA_ClassMenbers.CTAcounters.ZoneHysteresisCounter = 0;
			CTA_ClassMenbers.isWarningZoneHysteresis = FALSE;
		}
		else if (CTA_ClassMenbers.CTATrackList.Flags.wasReleventObjectExist == TRUE)
		{
			CTA_ClassMenbers.CTAcounters.ZoneHysteresisCounter++;
			CTA_ClassMenbers.isWarningZoneHysteresis = TRUE;
		}

		if ((CTA_ClassMenbers.isWarningZoneHysteresis == TRUE) && (CTA_ClassMenbers.CTAcounters.ZoneHysteresisCounter < (CTA_ClassMenbers.ctaparamconfig.warnHysteresis_cycles + 1)))
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_WarningLv1;
			CTA_ClassMenbers.CTAcounters.ZoneHysteresisCounter++;  //Todo: counter will add 2 the first cycle, need to fix it later
		}
		else if ((CTA_ClassMenbers.isWarningZoneHysteresis == TRUE) && (CTA_ClassMenbers.CTAcounters.ZoneHysteresisCounter > CTA_ClassMenbers.ctaparamconfig.warnHysteresis_cycles))
		{
			CTA_ClassMenbers.isWarningZoneHysteresis = FALSE;
			CTA_ClassMenbers.CTA_FunctionState = CTA_Passive;
		}

		Err = CheckFaultExistance();
		if (Err == TRUE)
		{
			CTA_ClassMenbers.CTA_FunctionState = CTA_Fault;
		}
		break;
	}
	case CTA_WarningLv2:
	{
		CTA_ClassMenbers.CTA_FunctionStatePre = CTA_ClassMenbers.CTA_FunctionState;
	}
	case CTA_Fault:
	{
		CTA_ClassMenbers.CTA_FunctionStatePre = CTA_ClassMenbers.CTA_FunctionState;
	}
	}
}

static bool_t Do_CTA_init(CTA_ParamConfig_t bsmparamconfig)
{
	/*Init here should be reload the config parameter from APP to local parameter*/
	return TRUE;
}

static bool_t Do_CTA_Standby(f32_t Host_Velocity)
{
	bool_t status = FALSE;

	if (Host_Velocity < CTA_ClassMenbers.ctaparamconfig.ActiveHostVelocity_mps) //Todo: if other infomation should be take in concern
	{
		status = TRUE;
	}
}

static bool_t Do_CTA_Passive(CTA_Track_Flags_t* track_flags)
{
	bool_t status = FALSE;
	if (track_flags->isReleventObjectExist == TRUE)
	{
		status = TRUE;
	}
}

bool_t Do_CTA_WarningLv1(void)
{
	//import the turning signal to trigger Lv2
	return FALSE; //for debug without Warning Lv2
}

bool_t Do_CTA_WarningLv2()
{

}

bool_t Do_CTA_Fault()
{

}

void OutputData_CrossTrafficAlert(CTA_ClassMembers_t* Output_DebugData_CTA)
{
	memcpy(Output_DebugData_CTA, &CTA_ClassMenbers, sizeof(CTA_ClassMembers_t));
}

static bool_t CheckFaultExistance()
{
	return FALSE;
}