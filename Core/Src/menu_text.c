#include "menu_text.h"

// this file contain all text(string) variable using in project

// sequence of string must be corresponded to: SETTTEXTBLOCK

// length of each string should be less than MENU_TEXT_BLOCK_STR_LEN
// Reason: Merging few text blocks into one -> more easier to read it via Modbus as one block
const char *MenuTextBlock[] = {
    // --- Strings for SETREGINDX.START.
    // --- UNIT-0. Internal  parameters of settings.
    "Sys0",
    "SettNum",
    "UnitNum",
    "Unit0",
    "Unit1",
    "Unit2",
    "Unit3",
    "SysTemp",

    // --- UNIT-1. run time
	"CMD INTERVAL",
	"second",
	"minute",
	"CURRENT TIME H:M",
	"day",
	"month",
	"year",
	"SET TIME",
	"TEST MODE",
	"SET START TEST",
	"SET STOP TEST",

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

	"OFF",
	"ON"
};

