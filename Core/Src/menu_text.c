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
    "Unit3",
    "Unit4",
    "Unit5",
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


    // --- UNIT-1. run time
	"INTERVAL RISE",//"S1 WEIGHT kg",
	"INTERVAL LOW",//"S1 MAX kg",
	"second",
	"minaute",
	"hour",
	"day",
	"month",
	"year",
	"SET TIME",
	"TEST MODE"
	"SET START TEST",
	"SET STOP TEST",
	"INTERFACE INFO",

	// --- UNIT-2. SET TIME
	"SET minutes",
	"SET hours",
	"SET day",
	"SET month",
	"SET year",
	// --- UNIT-3. SET_START_TEST
	"SET minutes",
	"SET hours",
	// --- UNIT-4. SET_STOP_TEST
	"SET minutes",
	"SET hours",

	// --- UNIT-5. Interface info
	"Ch1 RX pkt cnt",
	"Ch1 TX pkt cnt",
	"Ch1 missed pkt cnt",
	"Ch1 ERR pkt cnt",
	"Ch2 RX pkt cnt",
	"Ch2 TX pkt cnt",
	"Ch2 missed pkt cnt",
	"Ch2 ERR pkt cnt",

	"OFF",
	"ON"
};

