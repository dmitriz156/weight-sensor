

#define DISP_START_TMR			100	// ms

typedef enum
{
	MENU_PAGE_HELLO=0,		// greating page
	MENU_PAGE_DUMMY,			// not used
	MENU_PAGE_MEASURE,			// weight sensors measurements list
	MENU_PAGE_PROC,				// processing page
	MENU_PAGE_EMPTY,			// empty page. For system message
	MENU_PAGE_FILE,				// file list page
	MENU_PAGE_NUM
} MENUPAGE;

typedef enum
{
	MENU_SM_NO=0,
	MENU_SM_BOOT,					// start bootloader? 								Buttons: NO-YES
	MENU_SM_NO_USB,				// no USB-flash drive. 							Buttons: OK
	MENU_SM_SELECT_USB_MODE,
	MENU_SM_HOLD_FILE,
	MENU_SM_UPDATE,				// file is found. Update firmware?	Buttons: NO-YES
	MENU_SM_NO_FILE,			// correct file is not found. 			Buttons: OK
	MENU_SM_UPDATE_OK,		// Firmware is updated successfully	Buttons: OK
	MENU_SM_UPDATE_FAIL,	// Firmware updating failed.Repeat?	Buttons: NO-YES
	MENU_SM_TRN_FAIL,			// Turnstile is not available				No buttons
	MENU_SM_INTERR,				// internal error of programm
	MENU_SM_PCBERR,				// PCB code is failed
	MENU_SM_MSG_NUM,
	MENU_SM_TIMOUT_ERROR	
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
		MENU_PROC_ER=0,
		MENU_PROC_LOAD,
		MENU_PROC_VERIF,
		MENU_PROC_NUM,
	} MENUSEL;
	
	static char* MenuProcName[]=
	{
		"Erasing",
		"Loading",
		"Verification"
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
		MEASURE_ITEM_NUM
	} MESUREITEM;


	static char* measure_name [] =
	{
		"KG S1:",
		"MAX KG S1:",
		"KG S2:",
		"MAX KG S2:",
		"RAW S1:",
		"OFFSET:",
		"RAW S2:",
		"OFFSET:"
	};
	
