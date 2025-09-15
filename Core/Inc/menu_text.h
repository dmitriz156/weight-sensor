#ifndef __MENU_TEXT__H__
#define __MENU_TEXT__H__

// MenuTextBlock has few blocks inside.
// #defines below for easier access to MenuTextBlock
// Some kind of block separation
#define SettGetParamName(a)     MenuTextBlock[a]                                  // read from 1st part (SETREGINDX)
#define SettGetTextVal(a)       MenuTextBlock[a]                                  // read from 2nd part (SettGetTextVal)
#define SettGetGateTypeName(a)  MenuTextBlock[a + SETT_TEXT_RB_TYPE]            // read from 3rd part (GATETYPE)
#define SettGetGateModelName(a) MenuTextBlock[a + SettParam[SETT_RB_MODEL].pText]  // read from 4th part (GATEMODEL)
#define SettGetCnfg0Name(a)     MenuTextBlock[a + SETT_TEXT_CNFG0]
#define SettGetCnfg1Name(a)     MenuTextBlock[a + SETT_TEXT_CNFG1]
#define SettGetCnfg2Name(a)     MenuTextBlock[a + SETT_TEXT_CNFG2]
#define SettGetCnfg3Name(a)     MenuTextBlock[a + SETT_TEXT_CNFG3]
#define SettGetErrName(a, b)    MenuTextBlock[a * 16 + SETT_TEXT_ERR_A + b]

// length of each string set in MENU_TEXT_BLOCK_STR_LEN
extern const char *MenuTextBlock[];
extern const char *BldcCtrlName[];
extern const char *BldcStatName[];
extern const char *BldcCalStatName[];
extern const char *BsErrMapName[];
extern const char *GateCommName[];
extern const char *GateModeName[];
extern const char *GateStatName[];
extern const char *GateDiagName[];
extern const char *GateDiagStepName[];
extern const char *GateDiagBldcResName[];
extern const char *GateSrchStepName[];
extern const char *GateTestResName[];
extern const char *GateTestModeName[];
extern const char *GsErrMapName[];
extern const char *BldcHallErrName[];
extern const char *DrvStatName[];
extern const char *DrvErrMapName[];
extern const char *NtcStatName[];
extern const char *GateLockErrName[];
extern const char *GateLockStatName[];
extern const char *CanStatFlagName[];
extern const char *ld2410_trgt_state_text[];
extern const char *ld2410_init_status_text[];

#endif  // __MENU_TEXT__H__
