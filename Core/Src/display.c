
#include "display.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <math.h>


extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart3;
//extern weight_t weight [NUM_OF_WEIGHT_SENSOR];

extern SettParamDef SettParam[];
extern uint16_t *pSettReg[];  // pointer to settings value

//MenuTypeDef Menu;

char *DispIntToStr(s32 data, u8 dot, const char *suffix);


uint16_t DispTout = 2;			// display timeout connection, sec

char str_item_value[32];


#define DISP_PAUSE_TMR		25	// ms
#define DISP_SHIFT_TMR		80	

#define BtnAny()               ((btn.UP_state == BTN_PRESS) || (btn.DOWN_state == BTN_PRESS) || (btn.RIGHT_state == BTN_PRESS) || (btn.LEFT_state == BTN_PRESS))//(((btn == DISP_BTN_L) || (btn == DISP_BTN_R) || (btn == DISP_BTN_UP) || (btn == DISP_BTN_DN)) && (mode == DISP_BTN_SHRT))
#define BtnBack()              ((btn == DISP_BTN_L) && (mode == DISP_BTN_SHRT))
#define BtnNext()              ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnSelect()            ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnLogInfo()           ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG))
#define BtnClearErr()          ((btn == DISP_BTN_R) && (mode == DISP_BTN_LONG))
#define BtnOk()                ((btn == DISP_BTN_R) && (mode == DISP_BTN_SHRT))
#define BtnUp()                (btn.UP_state == BTN_PRESS)//((btn == DISP_BTN_UP) && (mode == DISP_BTN_SHRT))
#define BtnDown()              (btn.DOWN_state == BTN_PRESS)//((btn == DISP_BTN_DN) && (mode == DISP_BTN_SHRT))
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

char dStr0[DISP_LISTPARAM_LEN];
char dStr1[DISP_LISTPARAM_LEN];
char dStr2[DISP_LISTPARAM_LEN];
char dStr3[DISP_LISTPARAM_LEN];
char dStr4[DISP_LISTPARAM_LEN];

const uint8_t measure_item [] = {
		MEASURE_KG_S1,
		MEASURE_KG_MAX_S1,
		MEASURE_KG_S2,
		MEASURE_KG_MAX_S2,
		MEASURE_RAW_S1,
		MEASURE_OFFSETT_S1,
		MEASURE_RAW_S2,
		MEASURE_OFFSETT_S2,
		SETT_SYNCHRO_MODE,
		SETT_THRESHOLD_WEIGHT,
		MEASURE_ITEM_NUM
};

struct
{
	char str[DISP_SYS_MSG_STR_LEN * 5];  // 5 - number of strings
	char *pnt[5];
	u8 type;                             // type of message
	u8 tmr;
} MenuFreeSysMsg;


//char *DispSettParamToStr(u8 type, u8 indx)
//{
//#ifndef GATE_SW_SLIDER
//  u8 div = 2;
//#else
//  u8 div = 4;
//#endif
//  u16 data;
//  static char strSett[32];
//
//  if (indx >= MEASURE_ITEM_NUM) {
//    strcpy(strSett, "?");
//  } else {
//    switch (type) {
//      case DISP_SETT_DEF:
//        data = SettParam[indx].def;
//        break;
//      case DISP_SETT_MIN:
//        data = SettParam[indx].min;
//        break;
//      case DISP_SETT_MAX:
//        data = SettParam[indx].max;
//        break;
//      case DISP_SETT_VAL:
//      default:
//        data = measure_name[indx];
//        break;
//    }
//
//    if (SettParam[indx].pText == SETT_TEXT_NO) {
//      if (SettParam[indx].conv == SETT_CONV_COEFFICIENT) {
//        strcpy(strSett, DispFloatToStr(CONVERT_VALUE_TO_COEFFICIENT(data), 1));
//      } else if (SettParam[indx].conv == SETT_CONV_POS) {
//        strcpy(strSett, DispIntToStr(data * 10 / div, 1, 0));
//      } else if (SettParam[indx].conv == SETT_CONV_POS_MIRR) {
//        strcpy(strSett, DispIntToStr(data * 10 / div, 1, 0));
//      } else {
//        strcpy(strSett, DispIntToStr(data, SettParam[indx].conv, 0));
//      }
//
//    } else {
//      strcpy(strSett, SettGetTextVal(SettParam[indx].pText + data));
//    }
//  }
//
//  return &strSett[0];
//}



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
char *DispIntToStr(s32 data, u8 dot, const char *suffix)
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

	//if (add) dig[indx++] = add;
	if (suffix != NULL) {
		while (*suffix && indx < (DIG_BUFF - 1)) {
			dig[indx++] = *suffix++;
		}
	}

	dig[indx] = '\0'; // обов'язковий термінатор

	return &dig[0];
}

//char *DispFloatToStr(float data, u8 dot)
//{
//  static char str[13];
//  snprintf(str, 13, "%.*f", dot, data);
//  return str;
//}

int count_digits(int32_t num, u8 decimals) {
    if (num == 0) return 1;   // нуль має 1 цифру
    if (num < 0) num = -num;  // відкидаємо мінус

    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }
    if(count < decimals) count += (decimals-1);
    return count;
}

char *FloatToString(float value, u8 decimals, const char *suffix)
{
	int32_t int_part = (int32_t)value;
	int32_t float_to_disp = (int32_t)(value * pow(10, decimals));
    uint8_t dot_pos = count_digits(int_part, decimals);
    return DispIntToStr(float_to_disp, dot_pos, suffix);
}

void MenuChangeLine(void)
{
	if (btn.UP_state == BTN_PRESS) {
		if (Menu.lineSel) {
			Menu.lineSel--;
		} else {
			if (Menu.linePos) {
				Menu.linePos--;
			}
		}
	}
	if (btn.DOWN_state == BTN_PRESS) {
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
// for internal access
void MenuMakeSysMsg(MENUSM type, u16 tmr)
{
	Menu.sysMsg    = type;
	Menu.sysMsgTmr = tmr;
}

void MenuMakeFreeSysMsg(u8 type, char *str0, char *str1, char *str2, char *str3, char *str4, u16 tmr)
{
	Menu.sysMsg         = MENU_SM_FREE;
	Menu.sysMsgTmr      = tmr;
	MenuFreeSysMsg.type = type;
	memset((void *)&MenuFreeSysMsg.str, 0x00, sizeof(MenuFreeSysMsg.str));
	if (str0) strncpy(MenuFreeSysMsg.pnt[0], str0, DISP_SYS_MSG_STR_LEN);
	if (str1) strncpy(MenuFreeSysMsg.pnt[1], str1, DISP_SYS_MSG_STR_LEN);
	if (str2) strncpy(MenuFreeSysMsg.pnt[2], str2, DISP_SYS_MSG_STR_LEN);
	if (str3) strncpy(MenuFreeSysMsg.pnt[3], str3, DISP_SYS_MSG_STR_LEN);
	if (str4) strncpy(MenuFreeSysMsg.pnt[4], str4, DISP_SYS_MSG_STR_LEN);
}

void MenuDelSysMsg(void)
{
	Menu.sysMsg    = MENU_SM_NO;
	Menu.sysMsgTmr = 0;
	if (Menu.flag.bExtEvent) {
		Menu.flag.bExtEvent                   = 0;
		Menu.extEvent[Menu.extEventPnt].event = MENU_EVENT_NO;
	}
}


void DispInit(void)
{
	memset((void*)&DispUart,0x00,sizeof(DispUart));

	// prepare number of packet for each type of display pages
	DispUart.txPackNum[DISP_PAGE_HELLO]	= DISP_PAGE_HELLO_PACK_NUM;
	DispUart.txPackNum[DISP_PAGE_MAIN]	= DISP_PAGE_MAIN_PACK_NUM;
	DispUart.txPackNum[DISP_PAGE_LIST]	= DISP_PAGE_LIST_PACK_NUM;
	DispUart.txPackNum[DISP_PAGE_EMPTY]	= DISP_PAGE_EMPTY_PACK_NUM;
	DispUart.txPackNum[DISP_PAGE_UNDEF]	= DISP_PAGE_UNDEF_PACK_NUM;

	DispUart.pauseTmr = 1;		// start of pause

	memset((void*)&Menu,0x00,sizeof(Menu));

	Menu.pageIndx = MENU_PAGE_HELLO;
	Menu.startTmr = DISP_START_TMR;

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

// void SettInit(void){
// 	SettSetParam(SETT_SYNCHRO_MODE, &settings.mod_config, ALARM_ST_ALONE, ALARM_SYNCHRO, 1, measure_name[SETT_SYNCHRO_MODE], SETT_CONV_NO);
// 	SettSetParam(SETT_THRESHOLD_WEIGHT, &settings.alarm_treshold_kg, 1, 30, 1, measure_name[SETT_THRESHOLD_WEIGHT], SETT_CONV_NO);
// }


void DispPushBtn(void)
{
	MenuChangeLine();

	if (Menu.sysMsg)  // there is active message
	{
		switch(Menu.sysMsg) {
		case MENU_SM_SETT_WIND:
			if (BtnDown()) {
				SettRegChange(SETT_M_SYNCHRO_MODE, SETT_REG_UP);
			} else if (BtnUp()) {
				SettRegChange(SETT_M_SYNCHRO_MODE, SETT_REG_DN);
			}

			if(btn.LEFT_state == BTN_PRESS) {
				Menu.sysMsg	   = MENU_SM_NO;
				Menu.valueEdit = 0;
			}
			break;
		// other messages
		default:                          // not determined message
			if (BtnAny()) MenuDelSysMsg();  // just in case
			break;
		}
	}
	else // there is no active message
	{
		switch(Menu.pageIndx){
		case MENU_PAGE_HELLO:
			if(Menu.startTmr) {
				Menu.pageIndx   = MENU_PAGE_HELLO;
				Menu.sysMsg		= MENU_SM_NO;
			} else {
				Menu.pageIndx 	= MENU_PAGE_MEASURE;
				Menu.sysMsg 	= MENU_SM_NO;
				Menu.lineNum 	= MEASURE_ITEM_NUM;
				Menu.linePos	= 0;
				Menu.lineSel    = 0;
			}
			break;
		case MENU_PAGE_MEASURE:
			Menu.lineNum 	= MEASURE_ITEM_NUM;
			if(btn.LEFT_state == BTN_LONG_PRESS) {
				Menu.pageIndx   = MENU_PAGE_MODE;
				Menu.lineNum    = MENU_MOD_NUM;
				Menu.linePos    = 0;
				Menu.lineSel    = settings.mod_config;
			}
			if(btn.RIGHT_state == BTN_PRESS ) {
				Menu.valueEdit = 1;
				MenuMakeSysMsg(MENU_SM_SETT_WIND, 0);
			}

//			if(Menu.valueEdit == 1){
//				if(btn.UP_state == BTN_PRESS) settings.mod_config ++;
//				if(btn.DOWN_state == BTN_PRESS) settings.mod_config --;
//				Menu.valueEdit = 0;
//			}
			break;
		case MENU_PAGE_MODE:
			if(btn.LEFT_state == BTN_LONG_PRESS) {
				if(settings.mod_config_prev != settings.mod_config){
					Menu.pageIndx   = MENU_PAGE_EMPTY;
					Menu.sysMsg 	= MENU_SM_SAVE_MODE;
				} else {
					Menu.pageIndx   = MENU_PAGE_MEASURE;
					Menu.sysMsg 	= MENU_SM_NO;
					Menu.lineNum 	= MEASURE_ITEM_NUM;
					Menu.linePos	= 0;
					Menu.lineSel    = 0;
				}
			}
			break;
		case MENU_PAGE_EMPTY:
			if(btn.DOWN_state == BTN_PRESS) {
				settings.flash_write_flag = 1;
				Menu.pageIndx   = MENU_PAGE_MEASURE;
				Menu.sysMsg 	= MENU_SM_NO;
				Menu.lineNum 	= MEASURE_ITEM_NUM;
			}
			if(btn.UP_state == BTN_PRESS) {
				settings.flash_write_flag = 0;
				Menu.pageIndx   = MENU_PAGE_MEASURE;
				Menu.sysMsg 	= MENU_SM_NO;
				Menu.lineNum 	= MEASURE_ITEM_NUM;
			}
			break;
		default:
			Menu.pageIndx = MENU_PAGE_EMPTY;
			Menu.sysMsg   = MENU_SM_MSG_NEW_ERR;
			break;
		}
	}

	ButtonsReset();
	//button long press reset here
	ButtonsResetLong();

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

// --- next packet		
		DispUart.txPackPnt++;
		if(DispUart.txPackPnt>=DispUart.txPackNum[DispUart.pageIndex])
			DispUart.txPackPnt=0;
		else;
		
		if(Menu.pageIndx <= MENU_PAGE_EMPTY)
			DispUart.pageIndex = Menu.pageIndx;
		else //MENU_PAGE_FILE
			DispUart.pageIndex = DISP_PAGE_LIST;
		
		// prepare display Tx buffer
		memset((void*)&DispUart.txBuff[0],0x00,DISP_TX_BUFF);
		
		// common register for all packages
		DispUart.txBuff[DISP_REG_PACKTYPE] = DispUart.txPackPnt;			// number of packet
		DispUart.txBuff[DISP_REG_PAGE] = DispUart.pageIndex;					// current Display page index

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
			switch(Menu.sysMsg)
			{
			// !!! WARNING !!!
			// DO NOT EXCEED 12 SYMBOLS FOR EACH STRING
			case MENU_SM_SAVE_MODE:
				MenuSysMsgFill(DISP_SYS_MSG_QUE, "CHANGE ALARM","MODE?","",0,0);//btn: NO-YES
				break;
			case MENU_SM_SETT_WIND:
				// middle line
				strcpy(dStr1, MenuModName[settings.mod_config]);

				// first line
				if (MenuModName[settings.mod_config] == MENU_ALARM_ST_ALONE) {  // first value in
					strcpy(dStr0, "-");
				} else {
					strcpy(dStr0, MenuModName[settings.mod_config - 1]);
				}
				// last line
				if (MenuModName[settings.mod_config] == MENU_ALARM_SYNCHRO) {
					strcpy(dStr2, "-");
				} else {
					strcpy(dStr0, MenuModName[settings.mod_config + 1]);
				}

				MenuSysMsgFill(DISP_SYS_MSG_SETT, "]", dStr0, dStr1, dStr2, "[");
				break;
			case MENU_SM_NO:		// no message -> do nothing
				break;
			default:				// undefined message
				MenuSysMsgFill(DISP_SYS_MSG_WRN,"SYSTEM","MESSAGE","IS NOT DEF.!","CHECK","CODE!");
				break;
			}
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
				if(GetListPos(DISP_PACK_STR_1) >= SETT_SYNCHRO_MODE) {
					Menu.valueEdit = 1;
				} else {
					Menu.valueEdit = 0;
				}
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
									SetListValue(FloatToString(weight[0].kg, 2, "kg"));
									break;
								case MEASURE_KG_MAX_S1:
									SetListValue(FloatToString(weight[0].max_kg, 2, "kg"));
									break;
								case MEASURE_KG_S2:
									SetListValue(FloatToString(weight[1].kg, 2, "kg"));
									break;
								case MEASURE_KG_MAX_S2:
									SetListValue(FloatToString(weight[1].max_kg, 2, "kg"));
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
								case SETT_SYNCHRO_MODE:
									//SetListValue(DispSettParamToStr(DISP_SETT_VAL, 1));
									SetListValue(MenuModName[settings.mod_config]);
									break;
								case SETT_THRESHOLD_WEIGHT:
									SetListValue(FloatToString((float)settings.alarm_treshold_kg, 2, "kg"));
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
			
			case MENU_PAGE_MODE:
				switch(DispUart.txPackPnt)
				{
				// DISP_PACKTYPE_DATA_0 - common data
				case DISP_PACK_DATA_0:
					SetListSelLine(Menu.lineSel);
					SetListValueEdit(Menu.valueEdit);
					SetListValueExist(DISP_LIST_VALUE_NO);
					SetListSymbMode(DISP_LIST_SYMB_L);
					SetListLineShow();
					break;

				case DISP_PACK_STR_0:
					// status of loading
					SetListName("SELECT MODE");
					//					DispUart.txBuff[DISP_PHD0_TMR]=(Menu.startTmr);
					//					DispUart.txBuff[DISP_PHD0_TMRMAX]=(DISP_START_TMR);
					break;
				case DISP_PACK_STR_1:
				case DISP_PACK_STR_2:
				case DISP_PACK_STR_3:
				case DISP_PACK_STR_4:
				case DISP_PACK_STR_5:
					if(GetListPos(DISP_PACK_STR_1) < Menu.lineNum)
					{
						SetListParam(MenuModName[GetListPos(DISP_PACK_STR_1)]);
						SetListSymbL(DISP_LISTMSG_SYMB_ARROW);
					}
					if (settings.flash_read_flag == 1) {
						switch(Menu.lineSel) {
						case MENU_ALARM_ST_ALONE:
							settings.mod_config = ALARM_ST_ALONE;
							break;
						case MENU_ALARM_SYNCHRO:
							settings.mod_config = ALARM_SYNCHRO;
							break;
						}
					}
					break;
				default:
					break;
				}
				break;
				break;


				default:
					break;
		}
		
		if(DispUart.txPackPnt==DISP_PACK_DATA_0)
			DispUart.txBuff[DISP_PMD0_UARTTOUT]=DispTout;		// seconds	
		else;

		
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

