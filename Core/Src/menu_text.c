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
    "Unit0",
    "Unit1",
    "Unit2",
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

    // --- UNIT-1. Weight measurements
	"S1 WEIGHT kg",
	"S1 MAX kg",
	"S2 WEIGHT kg",
	"S2 MAX kg",
	"S1 RAW",
	"OFFSET",
	"S2 RAW",
	"OFFSET",

	"CONFIG PARAMETERS",
	"INTERFACE INFO",

	// --- UNIT-2. Config parameters
	"SYNCHRO MODE",
	"TRANSFER MODE",
	"THRESHOLD kg",
	"AVERAGING NUM",
	"BUZZER TIME",
	"DATA NORMALIZE TIME",
	// --- UNIT-3. Interface info
	"Ch1 RX pkt cnt",
	"Ch1 TX pkt cnt",
	"Ch1 read cnt",
	"Ch1 ERR pkt cnt",
	"Ch2 RX pkt cnt",
	"Ch2 TX pkt cnt",
	"Ch2 read cnt",
	"Ch2 ERR pkt cnt",

	"OFF",
	"ON",

	"special protocol",
	"UART protocol"
};

