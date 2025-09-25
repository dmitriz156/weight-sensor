#ifndef __SETTING__H__
#define __SETTING__H__

#include "main.h"


// total number of supported road blocker (sum all models together)
#define RB_MDL_TTL_NUM  SETT_BUFF_LEN

typedef enum {
  INTERNAL_HPU,
  EXTERNAL_HPU,
  MOBILE_RB,      // Road blocker with modular structure
  RB_TYPE_NUM     // length of list
} RB_TYPE;

// There are two type of settings:
// - protected settings (P)  -> can't be change manually
// - adjustable settings (A) -> CAN be changed manually
#if defined(GATE_SW_FREEWAY)

  #define GATE_GR_DEF GATE_GR_BS

  #include "sett_sw.h"   // sweepers
  #include "sett_sws.h"  // sweeper-slims
  #include "sett_sb.h"   // speedblades
  #include "sett_jp.h"   // jetpan
  #include "sett_hw.h"   // highWay
  // total number of supported turnstile (sum all models together)
  #define GATE_MDL_TTL_NUM (GATE_SB_NUM + GATE_SW_NUM + GATE_JP_NUM + GATE_SWS_NUM + GATE_HW_NUM)

// Types of gates. !!! Names of Types put in MenuTextBlock[] in menu_text.c
typedef enum {
  GATE_SW = 0,   // Sweeper
  GATE_SWS,      // SweeperSlim
  GATE_SB,       // SpeedBlade.
  GATE_JP,       // JetPan
  GATE_HW,       // HighWay
  GATE_TYPE_NUM  // length of list
} GATETYPE;

#elif defined(GATE_SW_GATE)

  #define GATE_GR_DEF GATE_GR_GT

  #include "sett_gt.h"   // gate
  #include "sett_gts.h"  // gate-slim

  // total number of supported turnstile (sum all models together)
  #define GATE_MDL_TTL_NUM (GATE_GT_NUM + GATE_GTS_NUM)

// Types of gates. !!! Names of Types put in MenuTextBlock[] in menu_text.c
typedef enum {
  GATE_GT = 0,   // Gate
  GATE_GTS,      // Gate-S

  GATE_TYPE_NUM  // length of list
} GATETYPE;

#elif defined(GATE_SW_ONYX_GATE)

  #define GATE_GR_DEF GATE_GR_ONXGT

  #include "sett_onx_gt.h"  // gate onyx

  // total number of supported turnstile (sum all models together)
  #define GATE_MDL_TTL_NUM (GATE_ONXGT_NUM)

// Types of gates. !!! Names of Types put in MenuTextBlock[] in menu_text.c
typedef enum {
  GATE_ONXGT = 0,  // Onyx_Gate
  GATE_TYPE_NUM    // length of list
} GATETYPE;

#elif defined(GATE_SW_ROTOR)

  #define GATE_GR_DEF GATE_GR_TR
  #include "sett_tr.h"  // tripod

  // total number of supported turnstile (sum all models together)
  #define GATE_MDL_TTL_NUM (GATE_TR_NUM)

// Types of gates. !!! Names of Types put in MenuTextBlock[] in menu_text.c
typedef enum {
  GATE_TR = 0,   // Tripod
  GATE_TYPE_NUM  // length of list
} GATETYPE;

#elif defined(GATE_SW_TEST)

  #define GATE_GR_DEF GATE_GR_TS

  #include "sett_tst.h"  // test-bench

  // total number of supported turnstile (sum all models together)
  #define GATE_MDL_TTL_NUM (GATE_TST_NUM)

// Types of gates. !!! Names of Types put in MenuTextBlock[] in menu_text.c
typedef enum {
  GATE_TST = 0,  // Tester. For R&D department only
  GATE_TYPE_NUM  // length of list
} GATETYPE;

#elif defined(GATE_SW_SLIDER)

  #define GATE_GR_DEF GATE_GR_SL

  #include "sett_sl.h"  // sliders

  // total number of supported turnstile (sum all models together)
  #define GATE_MDL_TTL_NUM (GATE_SL_NUM)

// Types of gates. !!! Names of Types put in MenuTextBlock[] in menu_text.c
typedef enum {
  GATE_SL = 0,   // Slider. For R&D department only
  GATE_TYPE_NUM  // length of list
} GATETYPE;

#endif

typedef enum {
  GATE_GR_BS = 0,  // "Basic" 	software for Gate Type: Sweepers, speedblade, jetpan.
  GATE_GR_GT,      // "Gate" 	software for Gate Type: Gate.
  GATE_GR_TR,      // "Tripod" software for Gate Type: Tripod.
  GATE_GR_TS,      // "Tester" software for Gate Type: Tester
  GATE_GR_SL,      // "Slider" software for Gate Type: Slider
  GATE_GR_ONXGT,   // "OnyxGate" software for Gate Type: OnyxGate
  GATE_GR_NUM      // length of list
} GATEGROUP;


#define SettIsProt(a)                   SettParam[a].flag.bProt
#define SettIsAvail(a)                  SettParam[a].flag.bAvail
#define SettSetProt(indx)               SettParam[indx].flag.bProt = 1
#define SettClearProt(indx)             SettParam[indx].flag.bProt = 0
#define SettSetAvail(indx)              SettParam[indx].flag.bAvail = 1
#define SettRstAvail(indx)              SettParam[indx].flag.bAvail = 0

#define CONVERT_VALUE_TO_COEFFICIENT(v) ((v) <= 100 ? -((float)(100 - (v)) / 100.0f) : ((float)((v) - 100) / 100.0f))
#define CONVERT_COEFFICIENT_TO_VALUE(v) ((v) < 0 ? (100 - (int)(-((v) * 100.0f))) : (100 + (int)((v) * 100.0f)))

// BLDC-controller supports a given number of Models
#define GATE_MDL_MAX                    16

#define GATE_TYPE_DEF                   0
#define GATE_MDL_DEF                    0  // first model of GATE_TYPE_DEF type

// --- CONFIGURATION WORDS DEFINITION
#define CONFIG_LEN                      16  // bits per each config words

typedef enum {
#ifdef IN_IMP_MODE_EN
  CNFG0_INP_PULSE,  // 1 - Gate control signals is in impulse mode (work with ....)
#endif
#ifdef SAFETY_EN
  CNFG0_SFT_TYPE_SLOW_BIT,  // 1 - SlowMode, 0 - StopMode
#endif
#ifdef CAN_EN
  CNFG0_CANSYNC_EN_BIT,  // 1 - CAN-sync Enabled, 0 - CAN-sync Disabled
#endif
#ifdef GATEWAY_EN
  CNFG0_GATEWAY_EN_BIT,  // 1 - Gateway mode Enabled, 0 - Gateway mode Disabled
#endif
#ifdef PULLHOLD_EN
  CNFG0_HOLDPULL_A_BIT,  // 1 - enable gate pulling in Hold A-position
  CNFG0_HOLDPULL_B_BIT,  // 1 - enable gate pulling in Hold B-position
  CNFG0_HOLDPULL_C_BIT,  // 1 - enable gate pulling in Hold C-position
#endif
#ifdef PANIC_EN
  CNFG0_PANIC_INP_NC_BIT,  // 1 - PANIC input Normal Open, 0 - NormalClose
  CNFG0_PANIC_OPENA_BIT,   // 1 - PANIC - OpenA direction, 0 - OpenB
#endif
#ifdef DOUBLE_RAMP_EN
  CNFG0_DOUBLE_RAMP_BIT,   // 1 - Double RampMode. Use OpenRamp and CloseRamp
#endif
  CNFG0_BRR_ROLLBACK_BIT,  // 1 - roll back if barrier is caught
#ifdef CAN_EN
  CNFG0_BRR_SYNC_BIT,      // 1 - sync barrier-status between paired turnstile
#endif
#ifdef OUT_NOTIFIC_EN
  CNFG0_OUTPLOGIC_TYPE_BIT,  // 0 - direct logic, 1 - inverted
#endif
  CNFG0_NUM_BIT              // used bits in the word
} CNFGWRD0;

typedef enum {
  CNFG1_NUM_BIT = 0  // used bits in the word
} CNFGWRD1;

typedef enum {
  CNFG2_NUM_BIT = 0  // used bits in the word
} CNFGWRD2;

typedef enum {
#ifdef GATE_DEBUG
  CNFG3_EXT_V_MEAS = 0,  // 1 - enable of external power voltage measurement (instead RTC)
  CNFG3_HALL_LOST_SYNC,
  CNFG3_DIS_BRK,
  CNFG3_NUM_BIT  // used bits in the word
#else            // no GATE_DEBUG
  CNFG3_NUM_BIT = 0
#endif           // no GATE_DEBUG
} CNFGWRD3;

#define SETT_MAPBIT_LEN 16
#define CNFG_NUM_BIT    (CNFG0_NUM_BIT + CNFG1_NUM_BIT)  // for Menu

// Names for CNFGWRDx placed in MenuTextBlock[]
// CNFGWRD0 definition
#ifdef IN_IMP_MODE_EN
  #define IsGateCtrlImp() (ReadU16Bit(&Gate.cnfg0, CNFG0_INP_PULSE))
#else
  #define IsGateCtrlImp() (0)
#endif
#ifdef SAFETY_EN
  #define IsSftTypeSlow() (ReadU16Bit(&Gate.cnfg0, CNFG0_SFT_TYPE_SLOW_BIT))
  #define IsSftTypeStop() (!ReadU16Bit(&Gate.cnfg0, CNFG0_SFT_TYPE_SLOW_BIT))
#else
  #define IsSftTypeSlow() (1)
  #define IsSftTypeStop() (0)
#endif
#ifdef CAN_EN
  #define IsCanSyncEn() (ReadU16Bit(&Gate.cnfg0, CNFG0_CANSYNC_EN_BIT))
  #define IsBrrSyncEn() (ReadU16Bit(&Gate.cnfg0, CNFG0_BRR_SYNC_BIT))
#else
  #define IsCanSyncEn() (0)
  #define IsBrrSyncEn() (0)
#endif
#ifdef GATEWAY_EN
  #define IsGatewayModeEn() (ReadU16Bit(&Gate.cnfg0, CNFG0_GATEWAY_EN_BIT))
#else
  #define IsGatewayModeEn() (0)
#endif
#ifdef PULLHOLD_EN
  #define IsHoldPullAEn() (ReadU16Bit(&Gate.cnfg0, CNFG0_HOLDPULL_A_BIT))
  #define IsHoldPullBEn() (ReadU16Bit(&Gate.cnfg0, CNFG0_HOLDPULL_B_BIT))
  #define IsHoldPullCEn() (ReadU16Bit(&Gate.cnfg0, CNFG0_HOLDPULL_C_BIT))
#else
  #define IsHoldPullAEn() (0)
  #define IsHoldPullBEn() (0)
  #define IsHoldPullCEn() (0)
#endif
#ifdef DOUBLE_RAMP_EN
  #define IsDoubleRamp() (ReadU16Bit(&Gate.cnfg0, CNFG0_DOUBLE_RAMP_BIT))
#else
  #define IsDoubleRamp() (0)
#endif
#ifdef PANIC_EN
  #define IsInpPanicNC() (ReadU16Bit(&Gate.cnfg0, CNFG0_PANIC_INP_NC_BIT))
  #define IsPanicOpenA() (ReadU16Bit(&Gate.cnfg0, CNFG0_PANIC_OPENA_BIT))
#endif
#define IsBrrRollBack() (ReadU16Bit(&Gate.cnfg0, CNFG0_BRR_ROLLBACK_BIT))
#ifdef OUT_NOTIFIC_EN
  #define IsLogicInvert() (ReadU16Bit(&Gate.cnfg0, CNFG0_OUTPLOGIC_TYPE_BIT))
#endif
// CNFGWRD3 definition
#ifdef GATE_DEBUG
  #define IsExtVmeasEn()   (ReadU16Bit(&Gate.cnfg3, CNFG3_EXT_V_MEAS))
  #define IsHallLostSync() (ReadU16Bit(&Gate.cnfg3, CNFG3_HALL_LOST_SYNC))
  #define IsBreakDis()     (ReadU16Bit(&Gate.cnfg3, CNFG3_DIS_BRK))
#endif  // GATE_DEBUG

/*
typedef enum
{
  CNFG1_NUM_BIT							// used bits in the word
}CNFGWRD1;
typedef enum
{
  CNFG2_NUM_BIT							// used bits in the word
}CNFGWRD2;
typedef enum
{
  CNFG3_NUM_BIT							// used bits in the word
}CNFGWRD3;
*/

// maximal limit for settings
#define SETT_LIM_MIN      0
#define SETT_LIM_MAX      65535
#define SETT_PROT         0  // setting is protected -> it is impossible to change it
#define SETT_NOT_USED     SETT_LIM_MIN

// --- COMMON parameters for all type of turnstile
// typical parameters for all motors, gate
#define BLDC_HALL_NUM     6  // number of HallSectors in 360 electrical degrees	(E360)
#define BLDC_HALL_MIN     1  // min possible value of Hall position
#define BLDC_HALL_MAX     6  // max possible value of Hall position
#define BLDC_HALL_DEF     1  // defined value for hall position (code)

// ramp control definition
#define GATE_RMPTYPE_ARC  0  // arc-shaped ramp
#define GATE_RMPTYPE_LINE 1  // line-shaped ramp
#define GATE_RAMP_NUM     4  // number of sector in speed ramp
// for parameters adjusting
#define GATE_RMPTYPE_MIN  GATE_RMPTYPE_ARC
#define GATE_RMPTYPE_MAX  GATE_RMPTYPE_LINE
#define GATE_RMPTYPE_STEP 1
// length of ramp sectors
#define GATE_RMPLEN_MIN   0
#define GATE_RMPLEN_MAX   GS_POS_TURN_G
#define GATE_RMPLEN_STEP  1
// speed of ramp sectors (in percent)
#define GATE_RMPSPD_MIN   0
#define GATE_RMPSPD_MAX   100
#define GATE_RMPSPD_STEP  1

#define GATE_PNT_NUM_MAX  4


// definition for speed adjusting
#define GATE_SPD_MIN                      0
#define GATE_SPD_MAX                      10000
#define GATE_SPD_STEP                     20

// definition for barrier sensetive adjusting
// sensetive of barrier detection, 10 levels
// 0 - OFF
// 1 	- lowest sensetive -> painful gate's kick
// 10 - higher sensetive -> weak gate's kick
#define GATE_BR_SENS_LVL_MIN              0
#define GATE_BR_SENS_LVL_MAX              10
#define GATE_BR_SENS_LVL_STEP             1
#define GATE_BR_SENS_OFF                  0

// Timeout before barrier detection, ms
#define GATE_BR_TOUT_MIN                  0
#define GATE_BR_TOUT_MAX                  2000
#define GATE_BR_TOUT_STEP                 50

// Timeout of waiting for finish position value stabilization
#define GATE_FIN_TOUT_MIN                 1
#define GATE_FIN_TOUT_MAX                 1000
#define GATE_FIN_TOUT_STEP                10

// timer to handle INPUT2-3 after their input signal receive
#define GATE_INP_ON_TOUT_MIN              0
#define GATE_INP_ON_TOUT_MAX              1000
#define GATE_INP_ON_TOUT_STEP             5

// timeout of alarm called when barrier was caught and Alarm action enabled on ONYX_GATE type
#define SETT_G_BR_ALARM_TOUT_MIN          0
#define SETT_G_BR_ALARM_TOUT_MAX          99
#define SETT_G_BR_ALARM_TOUT_STEP         1

// Timeout of waiting for finish position value stabilization
#define GATE_SAF_ON_TOUT_MIN              5
#define GATE_SAF_ON_TOUT_MAX              1000
#define GATE_SAF_ON_TOUT_STEP             5

#define PCB405_LED_MODE_MIN               0
#define PCB405_LED_MODE_MAX               3
#define PCB405_LED_MODE_STEP              1

#define PCB405_LED_COLOR_MODE_MIN         0
#define PCB405_LED_COLOR_MODE_MAX         5
#define PCB405_LED_COLOR_MODE_STEP        1

#define PCB405_INDICATION_BRIGHTNESS_MIN  0
#define PCB405_INDICATION_BRIGHTNESS_MAX  100
#define PCB405_INDICATION_BRIGHTNESS_STEP 5

#define MWS_SAFETY_HOLD_TOUT_MIN          0
#define MWS_SAFETY_HOLD_TOUT_MAX          60
#define MWS_SAFETY_HOLD_TOUT_STEP         1

#define MWS_EVENT_PARAM_MIN               0
#define MWS_EVENT_PARAM_MAX               1
#define MWS_EVENT_PARAM_STEP              1

#define MWS_WRONG_WAY_ALARM_MIN           0
#define MWS_WRONG_WAY_ALARM_MAX           2
#define MWS_WRONG_WAY_ALARM_STEP          1

#define MWS_WRONG_WAY_ALARM_TOUT_MIN      0
#define MWS_WRONG_WAY_ALARM_TOUT_MAX      999
#define MWS_WRONG_WAY_ALARM_TOUT_STEP     1

#define MWS_DETECTION_DISTANCE_MIN        0
#define MWS_DETECTION_DISTANCE_MAX        LD2410_LIVE_DETECTION_DISTANCE_POINTS_NUM - 1
#define MWS_DETECTION_DISTANCE_STEP       1

#define MWS_SENSITIVITY_COEFFICIENT_MIN   CONVERT_COEFFICIENT_TO_VALUE(-1.0f)
#define MWS_SENSITIVITY_COEFFICIENT_MAX   CONVERT_COEFFICIENT_TO_VALUE(1.0f)
#define MWS_SENSITIVITY_COEFFICIENT_STEP  10

#define MWS_SENS_RANGE_MIN                0
#define MWS_SENS_RANGE_MAX                100
#define MWS_SENS_RANGE_STEP               1

#define MWS_BLUETOOTH_MODE_MIN            0
#define MWS_BLUETOOTH_MODE_MAX            1
#define MWS_BLUETOOTH_MODE_STEP           1

#define MWS_ENGINEERING_MODE_MIN          0
#define MWS_ENGINEERING_MODE_MAX          1
#define MWS_ENGINEERING_MODE_STEP         1

#define MWS_TARGET_DETECTION_DELAY_MIN    0
#define MWS_TARGET_DETECTION_DELAY_MAX    3000
#define MWS_TARGET_DETECTION_DELAY_STEP   100

// gate position limit, 0.5 degrees
// define value of gate sector
#define GATE_POS_MIN_DEF                  GS_POS_MIN  // allowed range of position
#define GATE_POS_MAX_DEF                  GS_POS_MAX
#define GATE_POS_SHIFT_LIMIT              10          // + for posMin and posMax in relation to FoundLimit
#define GATE_POS_SHIFT_POS                1           // - for posOpenA and posOpenB in relation to FoundLimit
#define GATE_POS_SECT_ADAPT               30          // adaptation sector near OpenA(B) position (in case of increasing value of gate backlash)
// for parameters adjusting
#define GATE_POS_MIN                      0
#define GATE_POS_MAX                      GS_POS_TURN_G
#define GATE_POS_STEP                     1
// work area deviation
#define GATE_WRKAREA_DEV                  30  // %, �15 from WorkArea parameter

// position deviation settings
// roll back deviation over start point
// maximum allowed deviation in HOLD mode.
// deviation around ClosePosition in LockSoft(LockOff) mode
#define GATE_DEV_MIN                      0
#ifndef GATE_SW_SLIDER
  #define GATE_DEV_MAX 100
#else
  #define GATE_DEV_MAX 200
#endif
#define GATE_DEV_STEP 1

// path settings
// path of emergency stop from max speed to FinishSpeed
// path between normal ramp and safety ramp
#define GATE_PATH_MIN 10
#ifndef GATE_SW_SLIDER
  #define GATE_PATH_MAX 100
#else
  #define GATE_PATH_MAX 400
#endif
#define GATE_PATH_STEP          1

// repeated moves settings
// number of attempts to make movement.
#define GATE_MOVE_TRY_NUM_MAX   10
#define GATE_MOVE_TRY_NUM_MIN   5
#define GATE_MOVE_TRY_NUM_STEP  1
// Timeout between attempts to make movement, ms
#define GATE_MOVE_TRY_TOUT_MIN  100
#define GATE_MOVE_TRY_TOUT_MAX  2000
#define GATE_MOVE_TRY_TOUT_STEP 100
// s, pause after number of attempts to make movement is overloaded
#define GATE_MOVE_TOUT_MAX      60
#define GATE_MOVE_TOUT_MIN      1
#define GATE_MOVE_TOUT_STEP     1

// lock mechanism settings
// lock mode
#define LOCK_MODE_OFF           0  // lock mechanism is not active
#define LOCK_MODE_HARD          1  // HiSecurity - lock is always clamp on in close position
#define LOCK_MODE_LOWNS         2  // LowNoise - lock mechanism is not active in usually but it is clamp on when gate move from close sector manually
// for parameters adjusting
#define LOCK_MODE_MIN           LOCK_MODE_OFF
#define LOCK_MODE_MAX           LOCK_MODE_LOWNS
#define LOCK_MODE_STEP          1

// LED indication type
#define INDIC_OFF             0
#define INDIC_R               1
#define INDIC_RGY             2
#define INDIC_RYG             3
#define INDIC_GRY             4
#define INDIC_GYR             5
#define INDIC_YRG             6
#define INDIC_YGR             7

// Position output type
#define POS_OUT_LOGIC         0
#define POS_OUT_TRAF_LITE     1
#define POS_OUT_DIRECT        2

#define INDIC_MIN               INDIC_NO
#define INDIC_MAX               INDIC_RGB
#define INDIC_STEP              1

#ifdef BRR_ACTION

typedef enum {
  BRR_ACTION_MODE_NO,
  BRR_ACTION_MODE_ALARM,
  BRR_ACTION_MODE_ROLLBACK
} brr_action_mode_t;

  #define BRR_ACTION_MIN  BRR_ACTION_MODE_NO
  #define BRR_ACTION_MAX  BRR_ACTION_MODE_ROLLBACK
  #define BRR_ACTION_STEP 1
#endif

#ifdef GATEWAY_EN
  #define GATEWAY_TOUT_MIN  0
  #define GATEWAY_TOUT_MAX  10000
  #define GATEWAY_TOUT_STEP 50
#endif

// for output1-7 notification algorithm.
#define DIRECT             0
#define INVERTED           1

#define OUTP_LOGIC_MAX     INVERTED
#define OUTP_LOGIC_MIN     DIRECT
#define OUTP_LOGIC_STEP    1
#define NOTIFIC_POINT_MIN  3
#define NOTIFIC_POINT_MAX  110
#define NOTIFIC_POINT_STEP 1

typedef enum {
  LOCK_TYPE_NO = 0,     // turnstile is not equipped by lock mechanism
  LOCK_TYPE_PIN_SWR,    // PinLock with el.switch in released position
  LOCK_TYPE_TOOTH_FB,   // ToothLock with DC-DC with feedback of coil current
#ifdef GATE_DEBUG
  LOCK_TYPE_PIN_NFB,    // PinLock without control					- for test
  LOCK_TYPE_TOOTH_NFB,  // ToothLock, DC-DC,  no feedback 	- for test
  LOCK_TYPE_MGN_NFB,    // MagnetLock, no feedback 					- for Slider test
  LOCK_TYPE_NUM
#else                   // GATE_DEBUG
  LOCK_TYPE_NUM,
  // they stay after LOCK_TYPE_NUM and they are not used in code directly
  // it just for avoiding if GATE_DEBUG is not defined
  LOCK_TYPE_PIN_NFB,
  LOCK_TYPE_TOOTH_NFB,
  LOCK_TYPE_MGN_NFB
#endif
} LOCKTYPE;
// for parameter control - there is no adjusting
#define LOCK_TYPE_MIN             LOCK_TYPE_NO
#define LOCK_TYPE_MAX             (LOCK_TYPE_NUM - 1)
// power of magnet adjusting limit (ON, HOLD), percent
#define LOCK_PWR_MIN              20
#define LOCK_PWR_MAX              100
#define LOCK_PWR_STEP             5
// timers of magnet limits (adjusting), ms
#define LOCK_TMR_MIN              50
#define LOCK_TMR_MAX              5000
#define LOCK_TMR_STEP             50

// current consumption limits (adjusting), mA
#define CURR_CONS_MIN             0
#define CURR_CONS_MAX             3000
#define CURR_CONS_STEP            10

// timeouts of single A and B command
#define GATE_TOUT_SINGLE_MIN      0   // s
#define GATE_TOUT_SINGLE_MAX      99  // s
#define GATE_TOUT_SINGLE_STEP     1   // s

#define GATE_GEN_TOUT_FACTOR_MIN  1
#define GATE_GEN_TOUT_FACTOR_MAX  99
#define GATE_GEN_TOUT_FACTOR_STEP 1

// controller RS-485 address
#define RS485_ADDR_DEF            1
#define RS485_ADDR_MIN            1
#define RS485_ADDR_MAX            255
#define RS485_ADDR_STEP           1

// controller Bluetooth address
#define BT_ADDR_MIN               1
#define BT_ADDR_MAX               9
#define BT_ADDR_STEP              1

// use RSMODE (uart.h) as reference
#define RS_SETT_MODE_MIN          RS_MODE_OFF
#ifndef GATE_DEBUG
  #define RS_SETT_MODE_MAX RS_MODE_MDBS
#else
  #define RS_SETT_MODE_MAX RS_MODE_PC
#endif
#define RS_SETT_MODE_STEP 1

// --- BLDC motor preset
typedef enum {
  SETT_BLDC_SPD_MIN,  // P - minimum allowed speed of BLDC
  SETT_BLDC_SPD_MAX,  // P - maximum allowed speed of BLDC
  SETT_BLDC_GEAR_R,   // P - gear ratio of reductor
  SETT_BLDC_HALL,     // P - length of hall sector in Bldc steps
  SETT_BLDC_EM360,    // P - how many E360 fit in M360
  SETT_BLDC_E360,     // P - steps of BLDC motor in E360
  SETT_BLDC_M360,     // P - steps of BLDC motor in M360
  SETT_BLDC_E90,      // P - steps of BLDC motor in E90 (for leading vector)
  SETT_BLDC_E90DIFF,  // P - possible deviation of E90 (change leading vector for FOC)
  SETT_BLDC_NUM
} SETTBLDCINDX;

// List of settings parameters
typedef enum {
  // --- UNIT-0. Hidden. Internal  parameters of settings.
  SETT_CRC_INDX = 0,  // P - CRC of buffer after SETT_CRC_INDX
  SETT_WEIGHT_INDX,   // P - weight of settings block
  SETT_SETT_NUM_INDX,
  SETT_UNIT_NUM_INDX,
//  SETT_UNIT0_INDX,  // Internal parameters
//  SETT_UNIT1_INDX,  // Gate parameters
//  SETT_UNIT2_INDX,  // Lock parameters
//  SETT_UNIT3_INDX,  // Current consumption
//  SETT_UNIT4_INDX,  // Motor settings
//  SETT_UNIT5_INDX,  // Interfaces
//  SETT_UNIT6_INDX,
//  SETT_UNIT7_INDX,

  SETT_DUMMY,       // P - dummy parameters. Contains temporary value for Menu parameters changing
                    // config words -> they CAN be changed via special Display menu
                    // --- UNIT-1. SETTINGS and MEASUREMENTS ITEMS
  SETT_M_KG_S1,
  SETT_M_KG_MAX_S1,
  SETT_M_KG_S2,
  SETT_M_KG_MAX_S2,
  SETT_M_RAW_S1,
  SETT_M_OFFSETT_S1,
  SETT_M_RAW_S2,
  SETT_M_OFFSETT_S2,
  SETT_M_SYNCHRO_MODE,
  SETT_M_THRESHOLD_WEIGHT,
  //MEASURE_ITEM_NUM,

  // last element of enumeration. Used as total number of enumeration.
  // !!! Don't use it directly in code
  SETT_BUFF_LEN  // !!! must be <= SETT_BUFF_LEN_MAX
} SETREGINDX;

#define SETT_UNIT_MAX 9

// real start index of unit set in SettInit()->Preset[cnt].settDef[...]
typedef enum {
  SETT_UNIT_INT = 0,
  SETT_UNIT_GENERAL,
  SETT_UNIT_ANGLE_SENS,
  SETT_UNIT_HPU,
  SETT_UNIT_INDICATION,
  SETT_UNIT_CONN,
  SETT_UNIT_CAL,
  SETT_UNIT_AUTOMATIC_OPTION,
  SETT_UNIT_NUM,
} SETTUNIT;
#if SETT_UNIT_NUM > SETT_UNIT_MAX
  #error Number of units must be less than SETT_UNIT_MAX!
#endif

#if SETT_BUFF_LEN > 256
  #error Number of settings must be less than 238!
#else
  #define SETT_BUFF_LEN_MAX (FLACH_SETT_BLOCK_LEN / 2)
#endif

#define SETT_TEXT_NO    0xFFFF

// List of texted value
// Strings are placed in MenuTextBlock[]
typedef enum {

  SETT_TEXT_KG_S1 = SETT_DUMMY + 1,
  SETT_TEXT_KG_MAX_S1,
  SETT_TEXT_KG_S2,
  SETT_TEXT_KG_MAX_S2,
  SETT_TEXT_RAW_S1,
  SETT_TEXT_OFFSETT_S1,
  SETT_TEXT_RAW_S2,
  SETT_TEXT_OFFSETT_S2,
  SETT_TEXT_SYNCHRO_MODE,
  SETT_TEXT_THRESHOLD_WEIGHT,

  SETT_TEXT_OFF,
  SETT_TEXT_ON,

  SETT_TEXT_NUM
} SETTTEXTVAL;
#define SETT_WEIGHT_ERR  0x0000
#define SETT_WEIGHT_DEF  0x0001
#define SETT_WEIGHT_FREE 0xFFFF
#define SETT_WEIGHT_MAX  512  // 1024 - for DEBUG
#define SETT_CRC_DEF     0x0000

// bitmap of settings Flash errors / warnings
typedef enum {
  SETT_ERR_CRC = 0,  // CRC error
  SETT_ERR_WR,       // problem of Flash writing
  SETT_ERR_RANGE,    // problem of Settings parameters range(limit)
  SETT_ERR_CAL,      // calibration data are failed
  SETT_ERR_GR,       // wrong settings group
  SETT_ERR_BLDC,     // Bldc-motor preset error
  SETT_ERR_EMPTY,    // empty flash
} SETTERR;

// additional conversion for menu displaying
typedef enum {
  SETT_CONV_NO = 0,      // no conversion for parameter
  SETT_CONV_DIV_10,      // divide by 10
  SETT_CONV_DIV_100,     // divide by 100
  SETT_CONV_DIV_1000,    // divide by 1000
  SETT_CONV_DIV_10000,   // divide by 10000
  SETT_CONV_DIV_100000,  // divide by 100000
  SETT_CONV_POS_MIRR,    // convert like position parameters (*10/2) with negative mirror over Zero
  SETT_CONV_POS,         // convert like position parameters (*10/2)
  SETT_CONV_BITMAP,      // convert like bitmap (according to textValue)
  SETT_CONV_COEFFICIENT  // convert value range from [0; 100] => [-1; 0] U [100; 200] => [0; 1]
} SETTCONV;

#define SETT_PARSTR_LEN 16

typedef struct
{
  u16 def;    // define value -> copy from Preset[]
  u16 min;    // minimum value of setting parameter
  u16 max;    // maximum value of setting parameter
  u16 step;   // step of changing value of setting parameter
  u16 pText;  // index to string value of parameters

  struct
  {
    u8 bErrMin: 1;  // parameter falls out over Min limit
    u8 bErrMax: 1;  // parameter falls out over Max limit
    u8 bProt: 1;    // current parameter is protected
    u8 bChng: 1;    // current parameter was changed
    u8 bAvail: 1;   // settings is available for current type(model) of turnstile
  } flag;

  // for menu
  u8 conv;  // additional conversion for settings parameter (using for menu displaying)
} SettParamDef;

typedef struct
{
  u16 settDef[SETT_BUFF_LEN];  // defined value of setting parameter
  u16 bldcDef[SETT_BLDC_NUM];
} PresetDef;

typedef struct
{
  u16 errMap;   // map of errors
  u16 wrnMap;   // map of warnings
  u16 weight;   // weight of setting block
  u16 block;    // index of last used block
  u16 crcRd;    // CRC of settings buffer. Read/Write to flash
  u16 crcCalc;  // CRC of settings buffer. Calculated

  struct
  {
    u8 bWasChng: 1;    // 1 - any setting was changed, 0 - not.
    u8 bNeedSave: 1;   // need to save settings
    u8 bSectErase: 1;  // all settings are lost. Both sectors will be erased in the next trying of save flash
    u8 bTtlProtEn: 1;  // 1 - enable protection for protected settings
  } flag;

  // --- for external control (via Modbus, etc)
  u8 mdlNum[MEASURE_ITEM_NUM];  // number of models for each types
  u16 tbLen;                // total len of MenuTextBlock[]
} SettCtrlDef;

typedef struct
{
  u16 unitIndx[SETT_UNIT_MAX];  // index of unit settings
  // settings registers control
  u16 settNum;  // total number of settings register
  u16 unitNum;  // units number
} SettUnitDef;

// external control flag of settings registers
#define SETT_REG_UP    0
#define SETT_REG_DN    1
#define SETT_RST_ALL   0  // all settings will be resetted
#define SETT_RST_MODEL 1  // only settings that related to model will be resetted

#ifndef GATE_SW_SLIDER
  // convert degrees into 0.5 degrees
  // ex: 20degr -> 40steps of gate sensor
  #define SettSetDegr(a) (a * 2)
#else
  // convert mm into 0.25 mm
  // ex: 20mm -> 80steps of gate sensor
  #define SettSetDegr(a) (a * 4)
  #define SettSetMm(a)   SettSetDegr(a)
#endif
// convert rps(rotate per second) into 0.01 rps
// ex: 1.5rps -> 1500 0.01rps
#define SettSetSpd(a) (a * 100)


extern SettCtrlDef SettCtrl;
extern u16 *pSettReg[];
extern PresetDef Preset[];
extern SettParamDef SettParam[];
extern SettUnitDef SettUnit;
void FlashConfigWrite(void);
void SettInit(void);
void SettClearErr(void);
void SettResetToDef(u8, u8, u8);
u8 SettSaveTask(void);
void SettRegChange(u16, u8);
void SettBitChange(u16, u8);
u16 SettGetData(u16);
void SettSetData(u16, u16);
void SettTask(void);
void SettNeedSave(char *str);
u8 SettIsNeedSave(void);
void SettLoad(void);
void SettRfrParam(void);
u16 SettGetBldcDef(u16 indx);
u16 SettGetModelPos(u8 type, u8 mdl);
void SettCopyToDummy(u16 indx);
void SettResetSingleToDef(u8 indx);
void SettProtEn(FunctionalState state);

#endif  //__SETTING__H__

