

#define DISP_START_TMR			150	// ms

typedef enum
{
	MENU_PAGE_HELLO = 0,			// greating page
	MENU_PAGE_MEASURE = 2,			// weight sensors measurements list //MENU_PAGE_SETT_PARAM earlier
	MENU_PAGE_EMPTY,				// empty page. For system message
	MENU_PAGE_MODE,         		// page of selecting gate mode
	MENU_PAGE_PROC,					// processing page
	MENU_PAGE_NUM,
	MENU_PAGE_FREE = 0xFF,
} MENUPAGE;

typedef enum
{
	MENU_SM_NO=0,
	MENU_SM_SAVE_MODE,		//MENU_SM_FREE,  // free type of message -> create immediately in ButtonPress event ->
	// It is convenient for unique messages normally: ONE unique ButtonEvent = ONE unique message
	// Messages that are shown bellow are preCreated in ButtonPress event, but thea are filled in DispTask.
	// Type of messages for this using:
	// - few different event in menu refers to ONE typical message
	// - message with question
	// - info message
	// for MENU_PAGE_MAIN
	MENU_SM_MSG_SETT_DEF,      // settings are defaulted
	MENU_SM_MSG_NEW_ERR,       // new error appearance
	// for MENU_PAGE_MODE
	MENU_SM_MSG_GSNOCALIBR,    // " There is no calibration"
	MENU_SM_MSG_NOCALIBR,      // " There is no calibration"
	MENU_SM_MSG_NOAREASRCH,    // " Work area is not defined"
	// for MENU_PAGE_SETT_GR
	MENU_SM_QUE_SETT_EXIT,     // exit from settings?
	MENU_SM_QUE_SETT_RST,      // reset all settings?
	MENU_SM_QUE_SETT_RESTART,  // restart controller?
	// for MENU_PAGE_SETT_PARAM and MENU_PAGE_FAST_SETT
	MENU_SM_SETT_BITMAP,       // bitmap data
	MENU_SM_SETT_WIND,         // additional settings window
	MENU_SM_SETT_DEF,          // default value of parameter
	MENU_SM_QUE_NEWTYPE,       // new type of turnstile is selected....
	// for MENU_PAGE_LIMITSRCH:
	// for MENU_PAGE_ZEROSRCH
	MENU_SM_QUE_AREA_EXIT,     // exit from working area search
	MENU_SM_MSG_AREA_OK,       // working area search - Success
	MENU_SM_MSG_AREA_FAIL,     // working area search - FAIL

	MENU_SM_MSG_GSCAL_OK,
	MENU_SM_MSG_GSCAL_FAIL,

	// for MENU_PAGE_CAL_PROC
	MENU_SM_QUE_CAL_EXIT,
	MENU_SM_MSG_CAL_FAIL,          // fail of calibration
	MENU_SM_MSG_CAL_FAILCNT,       // increase fail count and repeat calibration
	// for MENU_SYS_LOG
	MENU_SM_MSG_SYS_LOG_INFO,      //
	MENU_SM_MSG_SYS_LOG_REC_INFO,  //
	// for MENU_INFO
	MENU_SM_MSG_INFO,
	// for MENU_SYS
	MENU_SM_MSG_SETT_SAVE_OK,    // settings is saved successfully
	MENU_SM_MSG_SETT_SAVE_FAIL,  // failure saved settings
	// for MENU_PAGE_DIAG_PROC
	MENU_SM_QUE_DIAG_START,      //
	MENU_SM_QUE_DIAG_EXIT,       // exit from diagnostic
	MENU_SM_QUE_DIAG_FIN_NORM,   // diagnostic is finished normally
	MENU_SM_QUE_DIAG_FIN_ERR,    // diagnostic is finished with external error
	MENU_SM_QUE_DIAG_FIN_EXT,    // diagnostic is finished by external stop
	// for MENU_PAGE_TEST
	MENU_SM_QUE_START_TEST,      // "Start Test"
	MENU_SM_QUE_STOP_TEST,       // "Stop Test"
	// for MENU_PAGE_CAL_SEL
	MENU_SM_QUE_START_CAL,       // "Start calibration...."
	MENU_SM_QUE_START_GSCAL,     // "Start GS calibration .."
	MENU_SM_QUE_START_ZEROSET,   // "Set gate in zero position and manually and..."
	// for all pages
	MENU_SM_QUE_START_SHARE,     // start to share screen of paired controller
	MENU_SM_WRN_SHARE_ERR,       // screen sharing is not possible - CAN connection error
	MENU_SM_WRN_HOLD_THIS_PAGE,  // paired controller hold THIS page
	MENU_SM_WRN_HOLD_MODE_PAGE,  // paired controller hold MODE page
	MENU_SM_WRN_TEMP_NOT_AV,     // function is not available temporary
	MENU_SM_WRN_CLEAR_ERR,       // clear all errors?
	MENU_SM_MSG_GSHOLDZERO,      // gate sensor. Zero button is pushed
	MENU_SM_MSG_TEST_FIN,        // finish of TEST1000
} MENUSM;

typedef struct
{
	uint16_t startTmr;
	MENUPAGE pageIndx;				// index of current page of menu
	MENUPAGE pageIndxNew;
	MENUSM sysMsg;					// system message
									// list parameters (current list parameters)
	uint8_t linePos;				// start position of visible part of list in common list
	uint8_t lineSel;				// selected line in visible part of list (0...DISP_LIST_LINE_MAX)
	uint8_t lineNum;				// number of available lines in list
	uint16_t lineData;
	uint16_t valueEdit;  			// value under editing
} MenuTypeDef;


// --- MENU_PAGE_PROC
	typedef enum
	{
		MENU_ALARM_ST_ALONE = 0,
		MENU_ALARM_SYNCHRO = 1,
		MENU_MOD_NUM
	} MENUSEL;
	
	static char* MenuModName[]=
	{
		"OFF",
		"ON"
	};


	typedef enum
	{
		MEASURE_KG_S1 = 0,
		MEASURE_KG_MAX_S1,
		MEASURE_KG_S2,
		MEASURE_KG_MAX_S2,
		MEASURE_RAW_S1,
		MEASURE_RAW_S2,
		MEASURE_OFFSETT_S1,
		MEASURE_OFFSETT_S2,
		SETT_SYNCHRO_MODE,
		SETT_THRESHOLD_WEIGHT,
		MEASURE_ITEM_NUM
	} MESUREITEM;


	static char* measure_name [] =
	{
		"S1 WEIGHT:",
		"S1 MAX:",
		"S2 WEIGHT:",
		"S2 MAX:",
		"S1 RAW:",
		"OFFSET:",
		"S2 RAW:",
		"OFFSET:",
		"SYNCHRO MODE:",
		"TRESHOLD:"
	};
	
