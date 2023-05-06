#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CTA_CFG_H
#define CTA_CFG_H

#include "common_types.h"

	/** CTA Default Config values*/
	/** Default top right tetragonal position [CRCS Coordinates] */
#define CTA_DEF_CFG_RELEVANT_POS_TOPRIGHT_X_M      (0.0f)
#define CTA_DEF_CFG_RELEVANT_POS_TOPRIGHT_Y_M      (0.0f)
/** Default top left tetragonal position [CRCS Coordinates] */
/** Calculated by TOP_LEFT_X = tan 40 deg * TOP_LEFT_Y */
#define CTA_DEF_CFG_RELEVANT_POS_TOPLEFT_X_M       (-23.1f)
#define CTA_DEF_CFG_RELEVANT_POS_TOPLEFT_Y_M       (40.0f)
/** Default bottom right tetragonal position [CRCS Coordinates] */
#define CTA_DEF_CFG_RELEVANT_POS_BOTTOMRIGHT_X_M   (18.0f)
#define CTA_DEF_CFG_RELEVANT_POS_BOTTOMRIGHT_Y_M   (0.0f)
/** Default top left tetragonal position [CRCS Coordinates] */
/** Calculated by BOTTOM_LEFT_X = tan (-40 deg) * BOTTOM_LEFT_Y + BOTTOM_RIGHT_X */
#define CTA_DEF_CFG_RELEVANT_POS_BOTTOMLEFT_X_M    (41.1f)
#define CTA_DEF_CFG_RELEVANT_POS_BOTTOMLEFT_Y_M    (40.0f)

/** Default minimum speed at which CTA is activated*/
#define CTA_DEF_CFG_MIN_SPEED_ACTIVATION_MPS       (0.0f)
/** Default minimum speed at which CTA is deactivated*/
#define CTA_DEF_CFG_MIN_SPEED_DEACTIVATION_MPS     (2.7f)
/** Default upper TTC threshold to trigger a warning [ms] */
#define CTA_DEF_CFG_TTC_UPPER_THRESHHOLD_MS        (3500u)
/** Default lower TTC threshold to trigger a warning [ms] */
#define CTA_DEF_CFG_TTC_LOWER_THRESHHOLD_MS        (2100u)
/** Default speed for lower TTC threshold to trigger a warning [ms] */
#define CTA_DEF_CFG_TTC_UPPER_THRESHHOLD_SPEED_MPS (2.77f)
/** Default speed for lowerTTC threshold to trigger a warning [ms] */
#define CTA_DEF_CFG_TTC_LOWER_THRESHHOLD_SPEED_MPS (8.33f)
/** Minimal orthogonal distance to trigger a warning */
#define CTA_DEF_CFG_ORTHO_DIST_MIN_M               (-2.0f)
/** Maximal orthogonal distance to trigger a warning; todo: determine PF default; 5m for HKMC [TC R-2-] */
#define CTA_DEF_CFG_ORTHO_DIST_MAX_M               (5.0f)
/** Default max ratio of target v_x / v_y to become relevant */
#define CTA_DEF_CFG_HEADING_ANGLE_VEL_X_Y_RATIO    (0.5f)
/** Default minimum lifetime of the track to be warn capable*/
#define CTA_CFG_MIN_LIFETIME_CYCLES                (10u)
/** Default value for warning hysteresis, cycles */
#define CTA_DEF_CFG_WARN_HYSTERESIS_CYCLES         (15u)
/** Default value relevant motion class, typ */
#define CTA_DEF_CFG_RELEVANT_MOTIONCLASS           (RTE_OBJ_MOTION_MOVING)
/** Default min relative velocity of track to trigger warning, [m/s] */
#define CTA_DEF_REL_VELOCITY_MIN_MPS               (1.4f)
/** Default max relative velocity of track to trigger warning, [m/s] */
#define CTA_DEF_REL_VELOCITY_MAX_MPS               (18.1f)
/** Default squared value of CTA_REL_VELOCITY_MIN_MPS for calculation optimizations, [mps*mps] */
#define CTA_DEF_REL_VELOCITY_MIN_MPS_SQ            (CTA_DEF_REL_VELOCITY_MIN_MPS * CTA_DEF_REL_VELOCITY_MIN_MPS)
/** Default squared value of CTA_REL_VELOCITY_MAX_MPS for calculation optimizations, [mps*mps]*/
#define CTA_DEF_REL_VELOCITY_MAX_MPS_SQ            (CTA_DEF_REL_VELOCITY_MAX_MPS * CTA_DEF_REL_VELOCITY_MAX_MPS)
#define CTA_DEF_TTE_GATE                           (3.5f)

#define Rear_Right                   0x01  //Todo: Define the sensor ID of Radar in different position
#define Rear_Left                   0x02
/** Type definitions
 * -------------------------------------------------------------------------
 */
 /**/

 /** CTA configurable Parameters */
	typedef struct CTA_config_t_TAG
	{

		/** warning zone top right position from sensor */
		f32_t relevantZoneTopRight_PosX_m;
		f32_t relevantZoneTopRight_PosY_m;
		/** warning zone top left position from sensor*/
		f32_t relevantZoneTopLeft_PosX_m;
		f32_t relevantZoneTopLeft_PosY_m;
		/** warning zone bottom right position from sensor*/
		f32_t relevantZoneBottomRight_PosX_m;
		f32_t relevantZoneBottomRight_PosY_m;
		/** warning zone bottom left position from sensor*/
		f32_t relevantZoneBottomLeft_PosX_m;
		f32_t relevantZoneBottomLeft_PosY_m;
		/**  host speed [m/s] activation threshold */
		f32_t ActiveHostVelocity_mps;
		/**  host speed [m/s] deactivation threshold */
		f32_t DeactiveHostVelocity_mps;
		/** minimum orthogonal distance, m*/
		f32_t OrthoDistMin_m;
		/** maximum orthogonal distance, m*/
		f32_t OrthoDistMax_m;
		/**  Min relative track velocity in mps */
		f32_t relVelMin_mps;
		/**  Max relative track velocity in mps */
		f32_t relVelMax_mps;
		/**  time to crossing limit in msec */
		u16_t ttc_limit_ms;
		/** warning hysteresis, cycles*/
		u16_t warnHysteresis_cycles;
	} CTA_ParamConfig_t; /*60 bytes*/

#endif //CTA_CFG_H

#ifdef __cplusplus
 }
#endif