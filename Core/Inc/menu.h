

#define DISP_START_TMR			150	// ms

#define MENU_EVENT_BUFF      8    // length of menu event buffer

typedef enum
{
	MENU_PAGE_HELLO = 0,			// greating page
	MENU_PAGE_MAIN,         		// main working page
	MENU_PAGE_MEASURE = 2,			// weight sensors measurements list //MENU_PAGE_SETT_PARAM earlier
	MENU_PAGE_EMPTY,				// empty page. For system message
	MENU_PAGE_MODE,         		// page of selecting gate mode
	MENU_PAGE_PROC,					// processing page
	MENU_PAGE_NUM,
	MENU_PAGE_FREE = 0xFF,
} MENUPAGE;


#define MENU_TOUT_MAIN_PAGE 120      // sec
#define MenuSetSysMsgTmr(a) (a + 1)  // timer in seconds
#define MENU_LONG_HOLD_TMR  100      // ms
#define MENU_BLINK_TMR_SLOW 500      // ms
#define MENU_BLINK_TMR_FAST 150      // ms
#define MENU_GATE_UNDEF     0xFF

typedef struct{
	u16 param_1;
	u16 param_2;
	u16 param_3;
	u16 param_4;
	u16 param_5;
	u16 param_6;
	u16 param_7;
	u16 param_8;
} dummy_t;

typedef enum
{
	MENU_SM_NO=0,
	MENU_SM_FREE,  // free type of message -> create immediately in ButtonPress event ->
	// It is convenient for unique messages normally: ONE unique ButtonEvent = ONE unique message
	// Messages that are shown bellow are preCreated in ButtonPress event, but thea are filled in DispTask.
	// Type of messages for this using:
	// - few different event in menu refers to ONE typical message
	// - message with question
	// - info message
	// for MENU_PAGE_MAIN
	MENU_SM_SAVE_MODE,		//MENU_SM_MSG_SETT_DEF,      // settings are defaulted
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

typedef enum {
	MENU_EVENT_NO = 0,
	MENU_EVENT_CAL_FAILCNT,     // increase fail count and repeat calibration
	MENU_EVENT_CAL_FIN_OK,      // calibration is finished successfully
	MENU_EVENT_CAL_FIN_FAIL,    // failed finish of calibration
	MENU_EVENT_GS_CAL_OK,       // GS calibration is finished successfully
	MENU_EVENT_GS_CAL_FAIL,     // GS calibration is finished successfully
	MENU_EVENT_AREA_SRCH_OK,    // working area searching is successful
	MENU_EVENT_AREA_SRCH_FAIL,  // working area searching is Failed
	MENU_EVENT_DIAG_FIN_NORM,   // normal finish of diagnostic
	MENU_EVENT_DIAG_FIN_ERR,    // finished by external error
	MENU_EVENT_DIAG_FIN_EXT,    // finished by external stop
	MENU_EVENT_SETT_SAVE_OK,    // settings is saved successfully
	MENU_EVENT_SETT_SAVE_FAIL,  // fail to save settings
	MENU_EVENT_CANSHARE_FAIL,   // error of display sharing
	MENU_EVENT_TEST_FIN,        // finishing of TEST-1000
} MENUEVENT;                  // event of external layer

void MenuMakeNewEvent(MENUEVENT ev, u16 arg);

typedef struct
{
	u16 startTmr;

	MENUPAGE pageIndx;  // index of current page of menu
	MENUPAGE pageIndxNew;
	u16 toutMainPage;   // timer of return to main page
	// list parameters (current list parameters)
	u8 linePos;    // start position of visible part of list in common list
	u8 lineSel;    // selected line in visible part of list (0...DISP_LIST_LINE_MAX)
	u8 lineNum;    // number of available lines in list
	u16 lineData;
	u8 valueEdit;  // value under editing

	// footprints of menu layers
	struct
	{
		MENUPAGE pageIndx;
		u8 listPos;
		u8 linePos;
		u8 lineSel;
	} pageSeq[MENU_PAGE_NUM];

	u8 pageSeqPos;
	// page list content
	u8 pageListNum[MENU_PAGE_NUM];

	struct
	{
		// safety button emulation
		u8 bSft: 1;
		u8 bLongU: 1;
		u8 bLongD: 1;
		u8 bLongL: 1;
		u8 bLongR: 1;
		u8 bBlinkSlow: 1;
		u8 bBlinkFast: 1;
		u8 bExtEvent: 1;
		u8 bSkipInitPage: 1;
	} flag;

	// long push holding timer
	u16 tmrLongHold;
	u16 toutSlowState;
	// prevoios value of parameter (for MENU_SM_SETT_WIND, for String parameters)
	//GATEMODE gateMode;  // ... of gate
	u16 paramDummy;
	u16 paramRealIndx;
	u16 paramPrev;
	// string shifting (for info message)
	u16 strPos;
	u16 strLen;
	// blink timer
	u16 blinkTmrSlow;
	u16 blinkTmrFast;
	// system message
	MENUSM sysMsg;
	u8 sysMsgTmr;
	// external event
	u8 extEventPnt;
	u8 extEventCnt;

	  struct
	  {
	    MENUEVENT event;
	    u16 arg;
	  } extEvent[MENU_EVENT_BUFF];

	//  // additional parameter
	//  MENUAP addParam;
	//  u16 addParamData;
	//  u8 addParamTout;
	//  u8 addParamPos;
	//  u8 addParamMax;
	//  u8 addParamOL;
	//  // error message display
	//  u16 errMap[BLDC_ERR_GR_NUM];
	//  // gate control command (via display buttons)
	//  GATECOMM gateCtrlComm;
	// contrast control
	u8 contrast;
	// pos for display indication
	u16 posDispMin;
	u16 posDispMax;
	u16 posDispMdl;
	u16 posDispOffset;
	u16 posDispNegPnt;

	u16 dispGoBackTmr;
	u16 dispBrightnesTmr;
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

	static char* MenuTransferModName[]=
	{
		"special",
		"UART"
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
		SETT_DATA_TRANSFER_MODE,
		SETT_THRESHOLD_WEIGHT,
		SETT_AVRG_NUMBER,
		SETT_BUZZER_TIME,
		SETT_DATA_NORMALIZE_TIME,
		MEASURE_ITEM_NUM
	} MESUREITEM;


	static char* measure_name [] =
	{
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

		"OFF",
		"ON"
	};
	
