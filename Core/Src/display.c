#include "main.h"

#include "display.h"
#include "menu.h"

#include <stdio.h>
#include <string.h>


extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart3;
extern weight_t weight [NUM_OF_WEIGHT_SENSOR];

//MenuTypeDef Menu;

char *DispIntToStr(s32 data, u8 dot, u8 add);


uint16_t DispTout = 2;			// display timeout connection, sec

char str_item_value[32];


#define DISP_PAUSE_TMR		25	// ms
#define DISP_SHIFT_TMR		80	

#define BtnAny()               (((btn == DISP_BTN_L) || (btn == DISP_BTN_R) || (btn == DISP_BTN_UP) || (btn == DISP_BTN_DN)) && (mode == DISP_BTN_SHRT))
#define BtnBack()              ((btn == DISP_BTN_L) && (mode == DISP_BTN_SHRT))
#define BtnNext()              ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnSelect()            ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnLogInfo()           ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG))
#define BtnClearErr()          ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG))
#define BtnOk()                ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnUp()                ((btn == DISP_BTN_UP) && (mode == DISP_BTN_SHRT))
#define BtnDown()              ((btn == DISP_BTN_DN) && (mode == DISP_BTN_SHRT))
#define BtnLeft()              ((btn == DISP_BTN_L) && (mode == DISP_BTN_SHRT))
#define BtnRight()             ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnQueNo()             ((btn == DISP_BTN_UP) && (mode == DISP_BTN_SHRT))
#define BtnQueYes()            ((btn == DISP_BTN_DN) && (mode == DISP_BTN_SHRT))
#define BtnShare()             ((btn == DISP_BTN_UP) && (mode == DISP_BTN_LONG))
#define BtnSysMenu()           ((btn == DISP_BTN_DN) && (mode == DISP_BTN_LONG))
#define BtnSysLogMenu()        ((btn == DISP_BTN_L) && (mode == DISP_BTN_LONG))
#define BtnErrMenu()           ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG))
#define BtnDef()               ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG))
#define BtnAp()                ((btn == DISP_BTN_UP) && (mode == DISP_BTN_SHRT))
#define BtnLongUen()           ((btn == DISP_BTN_UP) && (mode == DISP_BTN_LONG))
#define BtnLongDen()           ((btn == DISP_BTN_DN) && (mode == DISP_BTN_LONG))
#define BtnLongUdis()          ((btn == DISP_BTN_UP) && (mode == DISP_BTN_LONG_R))
#define BtnLongDdis()          ((btn == DISP_BTN_DN) && (mode == DISP_BTN_LONG_R))
#define BtnLongLen()           ((btn == DISP_BTN_L) && (mode == DISP_BTN_LONG))
#define BtnLongRen()           ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG))
#define BtnLongLdis()          ((btn == DISP_BTN_L) && (mode == DISP_BTN_LONG_R))
#define BtnLongRdis()          ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG_R))
#define BtnOpenA()             ((btn == DISP_BTN_L) && (mode == DISP_BTN_SHRT))
#define BtnOpenB()             ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnClose()             ((btn == DISP_BTN_DN) && (mode == DISP_BTN_SHRT))

#define SetListSelLine(a)      DispUart.txBuff[DISP_PLD0_SELLINE] = a
#define SetListValueEdit(a)    DispUart.txBuff[DISP_PLD0_VALUE_EDIT] = a
#define SetListValueExist(a)   DispUart.txBuff[DISP_PLD0_VALUE_EXIST] = a
#define SetListSymbMode(a)     DispUart.txBuff[DISP_PLD0_SYMB_MODE] = a
#define SetListSymbL(a)        DispUart.txBuff[DISP_LISTMSG_SYMB_L] = a
#define SetListSymbR(a)        DispUart.txBuff[DISP_LISTMSG_SYMB_R] = a
#define SetListLineHide()      DispUart.txBuff[DISP_PLD0_LINE_HIDE] = DISP_LIST_LINE_HIDE
#define SetListLineShow()      DispUart.txBuff[DISP_PLD0_LINE_HIDE] = DISP_LIST_LINE_SHOW
#define SetListGearHide()      DispUart.txBuff[DISP_PLD0_GEAR_SHOW] = DISP_LIST_GEAR_HIDE
#define SetListGearShow()      DispUart.txBuff[DISP_PLD0_GEAR_SHOW] = DISP_LIST_GEAR_SHOW
#define SetListName(name)      strncpy((void *)&DispUart.txBuff[DISP_LISTMSG_STR0], name, DISP_LISTPARAM_LEN)
#define SetListNameAdd(name)   strncat((void *)&DispUart.txBuff[DISP_LISTMSG_STR0], name, DISP_LISTPARAM_LEN)
#define SetListParam(param)    strncpy((void *)&DispUart.txBuff[DISP_LISTMSG_STR0], param, DISP_LISTPARAM_LEN)
#define SetListParamAdd(param) strncat((void *)&DispUart.txBuff[DISP_LISTMSG_STR0], param, DISP_LISTPARAM_LEN)
#define SetListValue(value)    strncpy((void *)&DispUart.txBuff[DISP_LISTMSG_STR1], value, DISP_LISTVALUE_LEN)
#define SetListValueAdd(value) strncat((void *)&DispUart.txBuff[DISP_LISTMSG_STR1], value, DISP_LISTVALUE_LEN)
#define GetListPos(a)          (Menu.linePos + (DispUart.txPackPnt - a))
// additional parameter
#define ShareMFrameOn()        DispUart.txBuff[DISP_REG_PAGE] |= DISP_PAGE_M_SHARE
#define ShareSFrameOn()        DispUart.txBuff[DISP_REG_PAGE] |= DISP_PAGE_S_SHARE
#define EnAp()                 DispUart.txBuff[DISP_PMD0_AP_EN] = 1
#define DisAp()                DispUart.txBuff[DISP_PMD0_AP_EN] = 0
#define SetApPos(a)            DispUart.txBuff[DISP_PMD0_AP_POS] = a
#define SetApMax(a)            DispUart.txBuff[DISP_PMD0_AP_MAX] = a
#define SetApOverLoad(a)       DispUart.txBuff[DISP_PMD0_AP_OL] = a
#define GetPrevListPos()       Menu.pageSeq[Menu.pageSeqPos - 1].listPos
#define GetWPrevListPos()      Menu.pageSeq[Menu.pageSeqPos - 2].listPos
#define GetPrevPageIndx()      Menu.pageSeq[Menu.pageSeqPos - 1].pageIndx
#define GetWPrevPageIndx()     Menu.pageSeq[Menu.pageSeqPos - 2].pageIndx
// #define MenuGetBackPageIndx()	Menu.pageLvl[Menu.pageIndx]!=0 ? Menu.layer[Menu.pageLvl[Menu.pageIndx]-1].pageIndx : MENU_PAGE_MAIN
#define MenuGoBack()           Menu.pageIndxNew = Menu.pageSeqPos != 0 ? Menu.pageSeq[Menu.pageSeqPos - 1].pageIndx : MENU_PAGE_MAIN
#define MenuGoToPage(a)        Menu.pageIndxNew = a

DispUartTypeDef DispUart;
MenuTypeDef Menu;

const uint8_t measure_item [] = {
		MEASURE_KG_S1,
		MEASURE_KG_MAX_S1,
		MEASURE_KG_S2,
		MEASURE_KG_MAX_S2,
		MEASURE_RAW_S1,
		MEASURE_OFFSETT_S1,
		MEASURE_RAW_S2,
		MEASURE_OFFSETT_S2,
		MEASURE_ITEM_NUM
	};


/**
 * @brief Convert integer value into string
 *        value range  -99999...+99999 (5digits)
 * @param data integer value
 * @param dot	dot position (0...4)
 *        0 - no dot
 *        1 - after 1st char (ex.12345 -> 1234.5)
 *        2 - after 2st char (ex.12345 -> 123.45) and so on
 *        0xFF - print 123456789 -> 123 456 789
 * @param add additional char in the end of string
 */
char *DispIntToStr(s32 data, u8 dot, u8 add)
{
#define DIG_NUM  	10             //
#define DIV      	1000000000     // DIG_NUM-1
#define DIG_BUFF 	(DIG_NUM + 3)  // + sign + dot + add
	static char dig[DIG_BUFF];
	u8 indx    = 0;
	u8 printEn = 0;
	u8 len     = DIG_NUM - 1;
	u32 div10  = DIV;

	memset((void *)&dig, 0x00, DIG_BUFF);

	// if data less than zero
	if (data < 0) {
		dig[indx++] = '-';
		data *= -1;
	}

	while (len) {
		dig[indx] = (data / div10) + '0';
		data %= div10;
		if ((dig[indx] != '0') || (dot == len) || printEn) {
			printEn = 1;
			indx++;
			if (dot == len) {
				dig[indx++] = '.';
			}

			if ((dot == 0xFF) && ((div10 == 1000000000) || (div10 == 1000000) || (div10 == 1000))) {
				dig[indx++] = ' ';
			}
		}
		len--;
		div10 /= 10;
	}
	dig[indx++] = data + '0';

	if (add) dig[indx++] = add;

	return &dig[0];
}

int count_digits(int32_t num) {
    if (num == 0) return 1;   // нуль має 1 цифру
    if (num < 0) num = -num;  // відкидаємо мінус

    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }
    return count;
}

char *FloatToString(float value) {
#define DIG_BUFF 	(DIG_NUM + 3)  // + sign + dot + add
	static char dig[DIG_BUFF];
	int32_t float_to_disp = (int32_t)(value * 100);
    int32_t int_part = (int32_t)value;
    uint8_t dot_pos = count_digits(int_part);
    return DispIntToStr(float_to_disp, dot_pos, 0);
}

void MenuChangeLine(void)
{
  if (buttons.UP_state == BTN_PRESS) {
	  buttons.UP_state = BTN_IDLE;
    if (Menu.lineSel) {
      Menu.lineSel--;
    } else {
      if (Menu.linePos) {
        Menu.linePos--;
      }
    }
  }
  if (buttons.DOWN_state == BTN_PRESS) {
	  buttons.DOWN_state = BTN_IDLE;
    if (Menu.lineSel < (DISP_LIST_LINE_MAX - 1)) {
      Menu.lineSel++;
    } else {
      Menu.linePos++;
    }

    if ((Menu.lineSel + Menu.linePos) >= Menu.lineNum && Menu.lineSel) {
      Menu.lineSel--;
    }
  }
}

void DispInit(void)
{
		char String_L32_For_Disp_Init[32];	
		uint8_t Counter_For_Disp_Init;

		memset((void*)&DispUart,0x00,sizeof(DispUart));
		
		// prepare number of packet for each type of display pages
		DispUart.txPackNum[DISP_PAGE_HELLO]	= DISP_PAGE_HELLO_PACK_NUM;
		DispUart.txPackNum[DISP_PAGE_MAIN]	= DISP_PAGE_MAIN_PACK_NUM;
		DispUart.txPackNum[DISP_PAGE_LIST]	= DISP_PAGE_LIST_PACK_NUM;
		DispUart.txPackNum[DISP_PAGE_EMPTY]	= DISP_PAGE_EMPTY_PACK_NUM;
		DispUart.txPackNum[DISP_PAGE_UNDEF]	= DISP_PAGE_UNDEF_PACK_NUM;
		
		DispUart.pauseTmr=1;		// start of pause
		
		memset((void*)&Menu,0x00,sizeof(Menu));
		Menu.startTmr=DISP_START_TMR;
							
		//strcpy(SwCurrName,	DispIntToStr( 123, 0, 0 )); //*(__IO uint16_t*) 0x0800C400, 0)

		for ( Counter_For_Disp_Init = 0; Counter_For_Disp_Init<32; Counter_For_Disp_Init++ )
		{
				String_L32_For_Disp_Init[ Counter_For_Disp_Init ] = 123;//*(__IO uint8_t*) (0x0800C402 + Counter_For_Disp_Init);//0x08008402
		}

		strcpy(SwCurrName,	String_L32_For_Disp_Init );
											
}


/**
	* @brief  Fill system message from DispTask
	* @param 	u8 type 	- type of message from DISPSYSMSG
						str0...4 	- stings of message
						Length of str0...4 Must not exceed DISP_SYS_MSG_STR_LEN.
	* @retval None
	*/	
void MenuSysMsgFill(uint8_t type, char* str0, char* str1, char* str2, char* str3, char* str4)
{
	DispUart.txBuff[DISP_SYS_MSG_TYPE]=type;
	if(str0)	strncpy((void*)&DispUart.txBuff[DISP_SYS_MSG_STR0],str0,DISP_SYS_MSG_STR_LEN);	
	if(str1)	strncpy((void*)&DispUart.txBuff[DISP_SYS_MSG_STR1],str1,DISP_SYS_MSG_STR_LEN);	
	if(str2)	strncpy((void*)&DispUart.txBuff[DISP_SYS_MSG_STR2],str2,DISP_SYS_MSG_STR_LEN);							
	if(str3)	strncpy((void*)&DispUart.txBuff[DISP_SYS_MSG_STR3],str3,DISP_SYS_MSG_STR_LEN);							
	if(str4)	strncpy((void*)&DispUart.txBuff[DISP_SYS_MSG_STR4],str4,DISP_SYS_MSG_STR_LEN);							
}

void DispTask(void)
{
	char str0[DISP_LISTPARAM_LEN];
	char str1[DISP_LISTPARAM_LEN];
	char str2[DISP_LISTPARAM_LEN];
	char str3[DISP_LISTPARAM_LEN];
	char str4[DISP_LISTPARAM_LEN];
	uint16_t len;
	
	if(DispUart.pauseTmr)
	{
		DispUart.pauseTmr=0;
		DispUart.shiftTmr++;
		if(DispUart.shiftTmr>=DISP_SHIFT_TMR)
		{
			DispUart.shiftTmr=0;
			DispUart.shiftChar0++;
			DispUart.shiftChar1++;
		}
		else;
		
		if(Menu.startTmr)
		{
			Menu.pageIndx  	= MENU_PAGE_HELLO;
			Menu.sysMsg		= MENU_SM_NO;
		}
		else
		{
			Menu.pageIndx 	= MENU_PAGE_MEASURE;
			Menu.sysMsg 	= MENU_SM_NO;
			Menu.lineNum 	= MEASURE_ITEM_NUM;
			//Menu.linePos 	= 0;

//			switch(USB_Status_For_Display)
//			{
//				case USB_STAT_BOOT:					// start bootloader? 								buttons: NO-YES
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_BOOT;
//					break;
//				case USB_STAT_NO_USB:				// no USB-flash drive. 							buttons: OK
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_NO_USB;
//					break;
//				case USB_STAT_SELECT_USB_MODE:					// select USB mod? 								buttons: HOST-FLASH
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_SELECT_USB_MODE;
//					break;
//				case USB_STAT_HOLD_FILE:					// select USB mod? 								buttons: HOST-FLASH
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_HOLD_FILE;
//					break;
//				case USB_STAT_NO_FILE:			// correct file is not found. 			buttons: OK
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_NO_FILE;
//					break;
//				case USB_STAT_UPDATE:					// file is found. Update firmware?	buttons: NO-YES
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_UPDATE;
//					break;
//				case USB_STAT_PROC_ERASE:		// processing. Sector erasing.			LIST PAGE
//					Menu.pageIndx=MENU_PAGE_PROC;
//					Menu.sysMsg=MENU_SM_NO;
//					Menu.lineNum=MENU_PROC_NUM;
//					Menu.linePos=0;
//					break;
//				case USB_STAT_PROC_LOAD:		// processing. Sector writing.			LIST PAGE
//					Menu.pageIndx=MENU_PAGE_PROC;
//					Menu.sysMsg=MENU_SM_NO;
//					Menu.lineNum=MENU_PROC_NUM;
//					Menu.linePos=0;
//					break;
//				case USB_STAT_PROC_VERIF:		// processing. Sector verification	LIST PAGE
//					Menu.pageIndx=MENU_PAGE_PROC;
//					Menu.sysMsg=MENU_SM_NO;
//					Menu.lineNum=MENU_PROC_NUM;
//					Menu.linePos=0;
//					Menu.lineSel=USB_Status_For_Display-USB_STAT_PROC_ERASE;
//					break;
//				case USB_STAT_FILESEL:
//					Menu.pageIndx=MENU_PAGE_FILE;
//					Menu.sysMsg=MENU_SM_NO;
//					Menu.lineNum=DispFileNum;
//					if((DispFilePos>=Menu.linePos)&&(DispFilePos<(Menu.linePos+DISP_LIST_LINE_MAX)))
//					{
//						Menu.lineSel=DispFilePos-Menu.linePos;
//					}
//					else if(DispFilePos<Menu.linePos)
//					{
//						Menu.linePos=DispFilePos;
//						Menu.lineSel=0;
//					}
//					else // Menu.lineSel>=(Menu.linePos+DISP_LIST_LINE_MAX)
//					{
//						Menu.linePos+=(DispFilePos-(Menu.linePos+DISP_LIST_LINE_MAX)+1);
//						Menu.lineSel=4;
//					}
//					break;
//				case USB_STAT_UPDATE_OK:		// Firmware is updated successfully	buttons: OK
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_UPDATE_OK;
//					break;
//				case USB_STAT_UPDATE_FAIL:	// Firmware updating failed.Repeat?	buttons: NO-YES
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_UPDATE_FAIL;
//					break;
//				case USB_STAT_TRN_FAIL:			// Turnstile is not available				No buttons
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_TRN_FAIL;
//					break;
//
//				case USB_STAT_PCBERR:
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_PCBERR;
//					break;
//
//				case USB_STAT_TIMEOUT:
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_TIMOUT_ERROR;
//					break;
//
//				default:
//					Menu.pageIndx=MENU_PAGE_EMPTY;
//					Menu.sysMsg=MENU_SM_INTERR;
//					break;
//			}
		}
		
// --- next packet		
		DispUart.txPackPnt++;
		if(DispUart.txPackPnt>=DispUart.txPackNum[DispUart.pageIndex])
			DispUart.txPackPnt=0;
		else;
		
		if(Menu.pageIndx<=MENU_PAGE_EMPTY)
			DispUart.pageIndex=Menu.pageIndx;
		else //MENU_PAGE_FILE
			DispUart.pageIndex=DISP_PAGE_LIST;
		
		// prepare display Tx buffer
		memset((void*)&DispUart.txBuff[0],0x00,DISP_TX_BUFF);
		
		// common register for all packages
		DispUart.txBuff[DISP_REG_PACKTYPE]=DispUart.txPackPnt;			// number of packet
		DispUart.txBuff[DISP_REG_PAGE]=DispUart.pageIndex;					// current Display page index

		// clear it before using in code below
		memset(str0,0x00,sizeof(str0));
		memset(str1,0x00,sizeof(str1));
		memset(str2,0x00,sizeof(str2));					
		memset(str3,0x00,sizeof(str3));
		memset(str4,0x00,sizeof(str4));		
		
		// current packet is system message
		if(DispUart.txPackPnt==DISP_PACK_SYS_MGS)	
		{
			// code(index) of system message
//			switch(Menu.sysMsg)
//			{
//				// !!! WARNING !!!
//				// DO NOT EXCEED 12 SYMBOLS FOR EACH STRING
//				case MENU_SM_BOOT:					// start bootloader? 								buttons: NO-YES
//					MenuSysMsgFill(DISP_SYS_MSG_QUE, "START","BOOTLOADER","RBv.1.2 ?",0,0);
//					break;
//				case MENU_SM_NO_USB:				// no USB-flash drive. 							buttons: OK
//					MenuSysMsgFill(DISP_SYS_MSG_WRN_OK,	"USB-FLASH","IS NOT","CONNECTED!",0,0);
//					break;
//				case MENU_SM_NO_FILE:				// correct file is not found. 			buttons: OK
//					MenuSysMsgFill(DISP_SYS_MSG_WRN_OK, "BIN-FILES","ARE NOT","FOUND!",0,0);
//					break;
//				case MENU_SM_UPDATE:				// Update firmware?									buttons: NO-YES
//					len=strlen(SwCurrName);
//					if(len<=DISP_SYS_MSG_STR_LEN)
//						DispUart.shiftChar0=0;
//					else if(DispUart.shiftChar0>(len-DISP_SYS_MSG_STR_LEN))
//						DispUart.shiftChar0=0;
//					else;
//					strncpy(str0,&SwCurrName[DispUart.shiftChar0],DISP_SYS_MSG_STR_LEN);
//
//					len=strlen(SwNewName);
//					if(len<=DISP_SYS_MSG_STR_LEN)
//						DispUart.shiftChar1=0;
//					else if(DispUart.shiftChar1>(len-DISP_SYS_MSG_STR_LEN))
//						DispUart.shiftChar1=0;
//					else;
//					strncpy(str1,&SwNewName[DispUart.shiftChar1],DISP_SYS_MSG_STR_LEN);
//
//					MenuSysMsgFill(DISP_SYS_MSG_QUE,"CURRENT FW:",str0,"NEW FW:",str1,"UPLOAD?");
//					break;
//				case MENU_SM_UPDATE_OK:			// Firmware is updated successfully	buttons: OK
//					MenuSysMsgFill(DISP_SYS_MSG_BIRD_OK,"FIRMWARE","UPLOAD","SUCCESSFULLY",0,0);
//					break;
//				case MENU_SM_UPDATE_FAIL:		// Firmware updating failed.Repeat?	buttons: NO-YES
//					MenuSysMsgFill(DISP_SYS_MSG_WRN_NY,"FIRMWARE","UPLOAD","FAILED!","REPEAT","UPLOADING?");
//					break;
//				case MENU_SM_TRN_FAIL:			// Turnstile is not available				No buttons
//					MenuSysMsgFill(DISP_SYS_MSG_WRN,"FIRMWARE","ERROR!","TURNSTILE","BLOCKED!",0);
//					break;
//				case MENU_SM_INTERR:				// internal error of programm
//					MenuSysMsgFill(DISP_SYS_MSG_WRN,"INTERNAL","ERROR!","RESET","CONTROLLER!",0);
//					break;
//
//				case MENU_SM_PCBERR:			// PCB code is failed
//					MenuSysMsgFill(DISP_SYS_MSG_WRN_OK,"NEW FIRMWARE","IS NOT","SUITABLE","FOR PCB429!",0);
//					break;
//				// case MENU_SM_SELECT_USB_MODE:
//				// 	MenuSysMsgFill(DISP_SYS_MSG_SETT, "ENABLE USB","FLASH DEVICE", "MODE?",0,0);
//				// 	break;
//				// case MENU_SM_HOLD_FILE:
//				// 	MenuSysMsgFill(DISP_SYS_MSG_INFO_BACK, "CONECT TO PC","AND UPLOAD", "FILE .bin",0,0);
//				// 	break;
//
//
//				case MENU_SM_TIMOUT_ERROR:			// Timeout Error
//					MenuSysMsgFill(DISP_SYS_MSG_WRN_OK,"An Error has","occurred.","Please try","Update Again",0);
//					break;
//
//
//
//				case MENU_SM_NO:		// no message -> do nothing
//					break;
//				default:						// undefined message
//					MenuSysMsgFill(DISP_SYS_MSG_WRN,"SYSTEM","MESSAGE","IS NOT DEF.!","CHECK","CODE!");
//					break;
//			}
		}
		else;
		
		// clear it before using in code below...again
		memset(str0,0x00,sizeof(str0));
		memset(str1,0x00,sizeof(str1));
		memset(str2,0x00,sizeof(str2));					
		memset(str3,0x00,sizeof(str3));
		memset(str4,0x00,sizeof(str4));		
		
		// current menu page
		switch(Menu.pageIndx)
		{
// --- HELLO PAGE
			case MENU_PAGE_HELLO:
				switch(DispUart.txPackPnt)
				{
					// DISP_PACKTYPE_DATA_0 - common data
					case DISP_PACK_DATA_0:		
						// status of loading
						DispUart.txBuff[DISP_PHD0_TMR]=(Menu.startTmr);	
						DispUart.txBuff[DISP_PHD0_TMRMAX]=(DISP_START_TMR);		
						break;
					case DISP_PACK_STR_0:
						// use this string if it need
						strncpy((void*)&DispUart.txBuff[DISP_REG_CMN_NUM], "WEIGHT SENS v1.3",	DISP_LISTPARAM_LEN);
						break;						
					default:
						break;						
				}				
				break;
		
// --- MENU_PAGE_EMPTY	- do not print anything			
			case MENU_PAGE_EMPTY:
				break;

// --- MENU_PAGE_MEASURE
			case MENU_PAGE_MEASURE:
				Menu.valueEdit = 0;
				switch(DispUart.txPackPnt)
				{
					// DISP_PACKTYPE_DATA_0 - common data
					case DISP_PACK_DATA_0:		
						SetListSelLine(Menu.lineSel);
						SetListValueEdit(Menu.valueEdit);
						SetListValueExist(DISP_LIST_VALUE_YES);
						SetListSymbMode(DISP_LIST_SYMB_R);
						SetListLineShow();
						break;

					// DISP_PACKTYPE_STR_0		- name of list (upper bar)
					case DISP_PACK_STR_0:
						SetListName("MEASUREMENTS");
						break;
					case DISP_PACK_STR_1:
					case DISP_PACK_STR_2:
					case DISP_PACK_STR_3:
					case DISP_PACK_STR_4:
					case DISP_PACK_STR_5:

						if(ready_to_read == 0) {
							SetListParam("ZERO SETTING");
						}
						else
						{
							if(GetListPos(DISP_PACK_STR_1) < Menu.lineNum)
							{
								SetListParam(measure_name[GetListPos(DISP_PACK_STR_1)]);
								SetListSymbL(DISP_LISTMSG_SYMB_NO);
								switch(measure_item[GetListPos(DISP_PACK_STR_1)])
								{
								case MEASURE_KG_S1:
									SetListValue(FloatToString(weight[0].kg));
									break;
								case MEASURE_KG_MAX_S1:
									SetListValue(FloatToString(weight[0].max_kg));
									break;
								case MEASURE_KG_S2:
									SetListValue(FloatToString(weight[1].kg));
									break;
								case MEASURE_KG_MAX_S2:
									SetListValue(FloatToString(weight[1].max_kg));
									break;
								case MEASURE_RAW_S1:
									SetListValue(DispIntToStr(weight[0].raw_data, 0, 0));
									break;
								case MEASURE_OFFSETT_S1:
									SetListValue(DispIntToStr(weight[0].raw_zero_offset, 0, 0));
									break;
								case MEASURE_RAW_S2:
									SetListValue(DispIntToStr(weight[1].raw_data, 0, 0));
									break;
								case MEASURE_OFFSETT_S2:
									SetListValue(DispIntToStr(weight[1].raw_zero_offset, 0, 0));
									break;
								}
								//SetListValue(str_item_value);
								//SetListValue(DispSettParamToStr(DISP_SETT_VAL, measure_item[GetListPos(DISP_PACK_STR_1)]));
							}
							else;
						}

						break;		
						
					default:
						break;						
				}					
				break;
			
			default:
				break;
		}
		
		if(DispUart.txPackPnt==DISP_PACK_DATA_0)
			DispUart.txBuff[DISP_PMD0_UARTTOUT]=DispTout;		// seconds	
		else;

		//HAL_UART_Transmit_DMA(&huart2, DispUart.txBuff, DISP_TX_BUFF);
		//DMA1_Stream6->CR |= ((uint32_t)0x00000001);	
		
		if(Menu.startTmr)
			Menu.startTmr--;
		else;		
	}
	else;
}

void DispTmr1ms(void)
{
	if(DispUart.pauseTmr)
		DispUart.pauseTmr++;
	else;
}

