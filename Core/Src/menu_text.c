#include "menu_text.h"

// this file contain all text(string) variable using in project

// sequence of string must be corresponded to: SETTTEXTBLOCK

// length of each string should be less than MENU_TEXT_BLOCK_STR_LEN
// Reason: Merging few text blocks into one -> more easier to read it via Modbus as one block
const char *MenuTextBlock[] = {
    // --- Strings for SETREGINDX.START.
    // --- UNIT-0. Internal  parameters of settings.
    "Sys0",  //"SettDataCrc",
    "Sys1",  //"SettWeight",
    "SettNum",
    "UnitNum",
//    "Unit0",
//    "Unit1",
//    "Unit2",
//    "Unit3",
//    "Unit4",
//    "Unit5",
//    "Unit6",
//    "Unit7",
    //"Unit8",

    "SysTemp",
    // config words -> they CAN be changed via special Display menu
//    "Cnfg0",
//    "Cnfg1",
//    "Cnfg2",
//    "Cnfg3",
//    "SettGroup",

    // --- UNIT-1. Gate settings
    // common gate settings
	"S1 WEIGHT kg",
	"S1 MAX kg",
	"S2 WEIGHT kg",
	"S2 MAX kg",
	"S1 RAW",
	"OFFSET",
	"S2 RAW",
	"OFFSET",
	"SYNCHRO MODE",
	"TRANSFER MODE",
	"THRESHOLD kg",
	"AVERAGING NUM",
	"BUZZER TIME",
	"DATA NORMALIZE TIME",
	"RX ch1 pkt cnt",
	"ERR ch1 pkt cnt",
	"TX ch1 pkt cnt",
	"RX ch2 pkt cnt",
	"ERR ch2 pkt cnt",
	"TX ch2 pkt cnt",

	"OFF",
	"ON",

	"special protocol",
	"UART protocol"
};

// --- BLDCCTRL names
// - for log
const char *BldcCtrlName[] = {
    "Speed",
    "Curr",
};

// --- BLDCSTAT names
// - for log
const char *BldcStatName[] = {
    "UNDEF",
    "OFF",
    "ON"};

// --- BLDCCALSTAT names
// - for log
const char *BldcCalStatName[] = {
    "Off",
    "RunFree",
    "RollBack",
    "RollForward",
    "RunCcw",
    "RunCw",
};

// --- BSERRMAP names
// - for displays
const char *BsErrMapName[] = {
    "No Connection",
    "Packet Err",
    "Magnet Sensor Conn",
    "Magnet Field Lost",
    "Reset Mode Err",
    "Step Signal Err",
    "Dir Signal Err",
    "Zero Signal Err",
    "Zero Set Err"};

// --- GSERRMAP names:
// - for displays
const char *GsErrMapName[] = {
    "No Connection",
    "Not Ready",
    "Packet Err",
    "Magnet Sensor Conn",
    "Magnet Field Lost",
    "Data Err",
    "Hold Zero Err"};

// --- BLDCHALLERR names:
// - for displays
const char *BldcHallErrName[] = {
    "Hall Signal Err",
    "Hall Sync Wrn"};

// --- GATECOMM names
// - for log
// - for PC application
const char *GateCommName[] = {
    "CLOSE",
    "OPEN.A",
    "OPEN.B",
    "OPEN.C",
    "PANIC",
    "NO",
};

// --- GATEMODE names
// - for log
// - for PC application
const char *GateModeName[] = {
    "INIT",
    "OFF",
    "CAL",
    "GS_CAL",
    "DIAG.",
    "SRCH",
    "CTRL.DISP",
    "CTRL.INP",
    "CTRL.APPL",
    "TEST"};

// --- GATESTAT names:
// - for log
// - for PC application
const char *GateStatName[] = {
    "OFF",
    "ERR",
    "CAL",
    "GS_CAL",
    "DIAG",
    "SRCH",
    "RUN",
    "HOLD",
    "BRR",
    "BRR-S",
};

// --- GATEDIAG names:
// - for display
const char *GateDiagName[] = {
    "Undef",
    "BLDC-sensor",
    "Gear Backlash",
    "Sector Backlash",
#ifdef __STEP_MOTOR__H__
    "StepMotor",
#endif
    //"Mosfet unit",
};

// --- GATEDIAGSTEP names:
// - for display
const char *GateDiagStepName[] = {
    "Lock",
    "RunCcw",
    "RunCw",
    "UnLock",
    "CCW-dir preparation",
    "CW-dir preparation",
    "BLDC- and Hall-sensors check",
#ifdef __STEP_MOTOR__H__
    "Push-out",
    "Pull-in",
#endif
};

// --- GATEDIAGBLDCRES names:
// - for display
const char *GateDiagBldcResName[] = {
    "STEP: ",
    "DIR: ",
    "ZERO: ",
    "HALL: "};

// --- GATESRCHSTEP names:
// - for display
const char *GateSrchStepName[] = {
    "Search MIN",
    "Search MAX",
    "Search MIDDLE",
    "Gate Sensor Reset",
    "Test After Search",
};

// --- GATETESTRES names:
// - for display
const char *GateTestResName[] = {
    "Undefined",
    "Successful",
    "Wrn:Errors",
    "Wrn:Barriers",
    "Wrn:Err+Brr",
    "Fail:Errors",
    "Fail:Barriers"};

// --- GATETESTMODE names:
// - for display
const char *GateTestModeName[] = {
    "Undefined",
    "WorkAreaTest",
#ifdef GATE_DEBUG
    "Continuous"
#else
    "1000-cycles"
#endif
};

// --- DRVSTAT names:
// - for display
const char *DrvStatName[] = {
    "OFF",
    "ON",
    "ERR"};

// --- DRVERRMAP names:
const char *DrvErrMapName[] = {
    "System Err",
    "SpiErr Err",
    "OverCurr Err",
    "ExtPwr Err",
    "IntPwr Err",
    "OverTemp Err",
    "Offset Err",
    "Gain Err"};

// --- NTCSTAT names:
// - for display
const char *NtcStatName[] = {
    "NRDY",
    "RDY",
    "SHRT",
    "OPEN",
    "NEG",
    "WRN",
    "SHTD",
};

// --- GATELOCKERR names:
// - for display
const char *GateLockErrName[] = {
    "ToothLock Err",
    "PinLock Err",
    "ToothLock Wrn",
    "PinLock Wrn",
};

// --- GATELOCKSTAT names:
// - for display
const char *GateLockStatName[] = {
    "OFF",
    "ON",
    "HOLD",
    "RELEASE"};

// --- CanStatTypeDef, struct{}flag;
// - for display
const char *CanStatFlagName[] = {
    "SyncEn",
    "Conn."};

#ifdef APPL_USE  // for PC application. Copy it into TurnstileService project
// --- MDBSUNIT names:
// - for PC application
const char *MdbsUnitName[] = {
    "SettingData",
    "BasicData",
    "SettingCtrl",
    "GateControl",
    "BldcMotor",
    "MosfetDriver",
    "Sensors",
    "LockMech.",
    "DI-DO",
    "Interfaces",
    "Calibration",
    "GS_Calibr.",
    "Search",
    "Test",
    "Errors",
    "Logs",
    "Plot",
    "SwVersion",
};
// --- DRVGAIN names:
// - for PC application
const char *DrvGainName[] = {
    "Gain10",
    "Gain20",
    "Gain40",
    "Gain80"};
// --- GATEBLOCK names:
// - for PC application
static char *GateBlockName[] = {
    "NO",
    "DIR.A",
    "DIR.B"};
#endif

// ld2410_trgt_state_t
// for display
const char *ld2410_trgt_state_text[] = {
    "Empty",
    "Moving",
    "Static",
    "MovStat",
};

// ld2410_init_status_t
// for display
const char *ld2410_init_status_text[] = {
    "Undef",
    "Process",
    "Ok",
    "Error",
};
