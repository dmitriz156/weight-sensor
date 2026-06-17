#include "main.h"
//#include "menu.h"
#include <string.h>

#include "menu_text.h"

//#define LOG_SETT LOG_S_SETT

SettCtrlDef SettCtrl;          // control registers of settings
SettUnitDef SettUnit;          // unit parameters definition read from settings
u16 *pSettReg[SETT_BUFF_LEN];  // pointer to settings value
// u16 SettBuffTemp[SETT_BUFF_LEN_MAX];		// for temporary calculation
PresetDef Preset[RB_MDL_TTL_NUM];
SettParamDef SettParam[SETT_BUFF_LEN];  // min,max,def,step of parameters
u16 SettingsValue = 0; //settings parameter value
extern MenuTypeDef Menu;


/**
 * @brief  Set name of parameters. It is displayed on the screen and transmitted to BldcContol application
 *
 * @param indx     index in Settings buffer. Use SETREGINDX enum
 * @param pnt	     pointer to variable that need to be assign to settings buffer (if pnt==NULL it means that only min,max and step are changed)
 * @param min	     allowed range of variable (use for adjusting)
 * @param max      allowed range of variable (use for adjusting)
 * @param step     step of adjusting. If step=0 -> it is protected settings (P), otherwise -> adjustable settings (A)
 * @param indxText index of text description of value
 * @param conv     additional conversion for settings parameter (using for menu displaying) - use SETTCONV enumeration
 */
void SettSetParam(u16 indx, u16 *pnt, u16 min, u16 max, u16 step, u16 indxText, u8 conv)
{
  if (indx < SETT_BUFF_LEN) {
    if (pnt != NULL) {
      pSettReg[indx] = pnt;
    }

    SettParam[indx].pText = indxText;

    SettParam[indx].min = min;
    SettParam[indx].max = max;
    if (step == SETT_PROT) {
      SettParam[indx].step       = 1;
      SettParam[indx].flag.bProt = 1;
    } else {
      SettParam[indx].step       = step;
      SettParam[indx].flag.bProt = 0;
    }
    SettParam[indx].conv = conv;
  }
}

/**
 * @brief  Set default value of parameters.
 *
 * @param indx     	   index in Settings buffer. Use SETREGINDX enum
 * @param def_value	   default value
 */
void SettSetDef(u16 indx, u16 def_value)
{
	if (indx < SETT_BUFF_LEN) {
		SettParam[indx].def = def_value;
	}
}

u16 SettCrcCalc(u16 *pnt, u16 len)
{
  u16 Crc;
  u8 BitCnt;
  u16 cnt;

  Crc = 0xFFFF;

  cnt = 0;
  while (cnt < len) {
    Crc ^= (*pnt);

    BitCnt = 0;
    while (BitCnt < 16) {
      if (Crc & 0x0001) {
        Crc >>= 1;
        Crc ^= 0xA001;
      } else {
        Crc >>= 1;
      }
      BitCnt++;
    }
    cnt++;
    pnt++;
  }
  return Crc;
}

void Flash_ErasePage(uint32_t addr)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = addr;
    EraseInitStruct.NbPages     = 1;

    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
}

void FlashConfigWrite(void)
{
	uint16_t index;
	if (settings.flash_write_flag) {
		settings.flash_write_flag = 0;

		HAL_FLASH_Unlock();

		Flash_ErasePage(ADDR_FLASH);

		index = 0;
		while (index < SETT_MAIN_BUFF_LEN) {
			if (pSettReg[index] != NULL) {
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (ADDR_FLASH + (index * 2)), *pSettReg[index]);
			}
			index++;
		}

		HAL_FLASH_Lock();
	}
}


/**
 * @brief Initialization of settings:
 * Preset[]    - set all defined value
 * *pSettReg[] - set pointer to working registers
 * SettParam[] - set adjusting parameters (min,max,step) and name of parameters
 * - Flash reading
 *    When flash is reading *pSettReg[] is used to transfer Flash data into working registers
 * - Check read data about allowed limit and correct it if it is necessary
 */
void SettInit(void)
{
  u16 cnt = 0;
  u32 addr;

  cnt = 0;
  while (cnt < SETT_BUFF_LEN) {
    pSettReg[cnt] = NULL;
    cnt++;
  }
  memset((void *)&SettCtrl, 0x00, sizeof(SettCtrlDef));
  memset((void *)&Preset, 0x00, sizeof(Preset));
  memset((void *)&SettParam, 0x00, sizeof(SettParam));
  memset((void *)&SettUnit, 0x00, sizeof(SettUnit));

  SettCtrl.tbLen           = SETT_TEXT_NUM;
  SettCtrl.flag.bTtlProtEn = ENABLE;

  cnt = 0;
  while (cnt < RB_MDL_TTL_NUM) {///<GATE_MDL_TTL_NUM
    // common presets for all types of turnstiles
    Preset[cnt].settDef[SETT_CRC_INDX] = SETT_CRC_DEF;

    // structure of settings. It is copied for each preset
    Preset[cnt].settDef[SETT_SETT_NUM_INDX] = SETT_BUFF_LEN;
    Preset[cnt].settDef[SETT_UNIT_NUM_INDX] = SETT_UNIT_NUM;
    Preset[cnt].settDef[SETT_UNIT0_INDX]    = SETT_M_CMD_INTERVAL;
    Preset[cnt].settDef[SETT_UNIT1_INDX]    = SETT_M_MINUTES;
    Preset[cnt].settDef[SETT_UNIT2_INDX]    = SETT_M_START_TEST_MINUTES;
    Preset[cnt].settDef[SETT_UNIT3_INDX]    = SETT_M_STOP_TEST_MINUTES;
    Preset[cnt].settDef[SETT_UNIT4_INDX]    = SETT_M_MONDAY;

    cnt++;
  }


  SettSetParam(SETT_CRC_INDX,             &SettCtrl.crcRd, SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_SETT_NUM_INDX,        &SettUnit.settNum, SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_UNIT_NUM_INDX,        &SettUnit.unitNum, SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_UNIT0_INDX,           &SettUnit.unitIndx[0], SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_UNIT1_INDX,           &SettUnit.unitIndx[1], SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_UNIT2_INDX,           &SettUnit.unitIndx[2], SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_UNIT3_INDX,           &SettUnit.unitIndx[3], SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_UNIT4_INDX,           &SettUnit.unitIndx[4], SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);

  SettSetParam(SETT_DUMMY,                &Menu.paramDummy, SETT_LIM_MIN, SETT_LIM_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);

  SettSetParam(SETT_M_CMD_INTERVAL,       &settings.rs485_command_interval_s, RS485_INTERVAL_MIN_S, RS485_INTERVAL_MAX_S, RS485_INTERVAL_STEP_S, SETT_TEXT_NO, SETT_CONV_NO);

  SettSetParam(SETT_M_CURRENT_SECONDS,    NULL, 0, 59, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_CURRENT_MINUTES,    NULL, 0, 59, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_CURRENT_HOURS,      NULL, 0, 23, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_CURRENT_DAY,        NULL, 1, 31, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_CURRENT_MONTH,      NULL, 1, 12, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_CURRENT_YEAR,       NULL, 2000, 2099, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_WEEK_DAY,           NULL, 0, 6, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);

  SettSetParam(SETT_M_TIME,		          NULL, 0, 0, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_ACTIVE_DAY,         NULL, 0, 0, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_TEST_MODE,          &settings.test_mode, TEST_MODE_OFF, TEST_MODE_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);
  SettSetParam(SETT_M_START_TEST,         NULL, 0, 0, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_STOP_TEST,          NULL, 0, 0, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_MINUTES,            NULL, 0, 59, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_HOURS,              NULL, 0, 23, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_DAY,                NULL, 1, 31, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_MONTH,              NULL, 1, 12, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_YEAR,               NULL, 2000, 2099, 1, SETT_TEXT_NO, SETT_CONV_NO);

  SettSetParam(SETT_M_TODAY_IS,           &settings.active_today_is, 0, 6, 1, SETT_TEXT_WEEK_DAY_MONDAY, SETT_CONV_NO);
  SettSetParam(SETT_M_MONDAY,             &settings.active_monday, ACTIVE_DAY_OFF, ACTIVE_DAY_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);
  SettSetParam(SETT_M_TUESDAY,            &settings.active_tuesday, ACTIVE_DAY_OFF, ACTIVE_DAY_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);
  SettSetParam(SETT_M_WEDNESDAY,          &settings.active_wednesday, ACTIVE_DAY_OFF, ACTIVE_DAY_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);
  SettSetParam(SETT_M_THURSDAY,           &settings.active_thursday, ACTIVE_DAY_OFF, ACTIVE_DAY_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);
  SettSetParam(SETT_M_FRIDAY,             &settings.active_friday, ACTIVE_DAY_OFF, ACTIVE_DAY_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);
  SettSetParam(SETT_M_SETURDAY,           &settings.active_seturday, ACTIVE_DAY_OFF, ACTIVE_DAY_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);
  SettSetParam(SETT_M_SUNDAY,             &settings.active_sunday, ACTIVE_DAY_OFF, ACTIVE_DAY_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);

  SettSetParam(SETT_M_START_TEST_MINUTES, &settings.test_start_minutes, 0, 59, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_START_TEST_HOURS,   &settings.test_start_hours, 0, 23, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_STOP_TEST_MINUTES,  &settings.test_stop_minutes, 0, 59, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_STOP_TEST_HOURS,    &settings.test_stop_hours, 0, 23, 1, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_TODAY_EPOCH_DAY,    &settings.active_today_epoch_day, 0, ACTIVE_TODAY_EPOCH_DAY_MAX, SETT_PROT, SETT_TEXT_NO, SETT_CONV_NO);
  SettSetParam(SETT_M_RB_STATUS_CHECK_MODE, &settings.rb_status_check_mode, RB_STATUS_CHECK_MODE_OFF, RB_STATUS_CHECK_MODE_ON, 1, SETT_TEXT_OFF, SETT_CONV_NO);

  //set default settings value
  SettSetDef(SETT_M_CMD_INTERVAL, RS485_INTERVAL_DEF_S);
  SettSetDef(SETT_M_TEST_MODE, TEST_MODE_OFF);
  SettSetDef(SETT_M_START_TEST_MINUTES, TEST_START_MINUTES_DEF);
  SettSetDef(SETT_M_START_TEST_HOURS, TEST_START_HOURS_DEF);
  SettSetDef(SETT_M_STOP_TEST_MINUTES, TEST_STOP_MINUTES_DEF);
  SettSetDef(SETT_M_STOP_TEST_HOURS, TEST_STOP_HOURS_DEF);
  SettSetDef(SETT_M_TODAY_IS, ACTIVE_TODAY_IS_DEF);
  SettSetDef(SETT_M_MONDAY, ACTIVE_DAY_ON);
  SettSetDef(SETT_M_TUESDAY, ACTIVE_DAY_ON);
  SettSetDef(SETT_M_WEDNESDAY, ACTIVE_DAY_ON);
  SettSetDef(SETT_M_THURSDAY, ACTIVE_DAY_ON);
  SettSetDef(SETT_M_FRIDAY, ACTIVE_DAY_ON);
  SettSetDef(SETT_M_SETURDAY, ACTIVE_DAY_ON);
  SettSetDef(SETT_M_SUNDAY, ACTIVE_DAY_ON);
  SettSetDef(SETT_M_TODAY_EPOCH_DAY, ACTIVE_TODAY_EPOCH_DAY_DEF);
  SettSetDef(SETT_M_RB_STATUS_CHECK_MODE, RB_STATUS_CHECK_MODE_DEF);


#define SettMemGetData(a) (*(__IO u16 *)(addr + a * 2))
#define SettMemGetAddr(a) ((u16 *)(addr + a * 2))

  addr = ADDR_FLASH;
  cnt = 0;
  while (cnt < SETT_MAIN_BUFF_LEN) {
	  if (pSettReg[cnt] != NULL) {
		  (*pSettReg[cnt]) = SettMemGetData(cnt);
		  if ((*pSettReg[cnt]) > SettParam[cnt].max){
			  (*pSettReg[cnt]) = SettParam[cnt].def;
			  settings.flash_write_flag = 1;
		  }
		  if ((*pSettReg[cnt]) < SettParam[cnt].min){
			  (*pSettReg[cnt]) = SettParam[cnt].def;
			  settings.flash_write_flag = 1;
		  }
	  }
	  cnt ++;
  }

  if (!RTC_IsTimeRangeValid(&settings)) {
	  settings.test_stop_hours = settings.test_start_hours;
	  settings.test_stop_minutes = settings.test_start_minutes;
	  settings.flash_write_flag = 1;
  }

}

/**
 * @brief Reset all settings or Model settings to defined state
 *
 * @param type type of turnstile GATETYPE
 * @param mdl  model of turnstile
 * @param mode mode of reseting:
 *  #define SETT_RST_ALL		0		// all settings will be resetted
 *  #define SETT_RST_MODEL	1		// only settings that related to model will be resetted
 */
void SettResetToDef(u8 type, u8 mdl, u8 mode)
{

}

/**
 * @brief Reset single parameter to default value
 * @param indx index of parameter
 */
void SettResetSingleToDef(u8 indx)
{

}

/**
 * @brief Settings Check Save Task.
 * Call it from main cycle	when no one important process is running (upper level decides about it).
 * Because FlashWrite can takes up to 1-2 seconds (FLASH_EraseSector is very time-heavy operation)
 * @param None
 * @retval 1 - Settings was written, 0 - was not (it is NOT error)
 */
u8 SettSaveTask(void)
{

}

/**
 * @brief Change value of variable in settings buffer under "indx" position - for external access
 * Step of changes equals to "step"-value from SettParam[]
 * @param dir SETT_REG_UP or SETT_REG_DN.
 * @retval None
 */
void SettRegChange(u16 indx, u8 dir)
{
  // if "step"=0 it is forbidden to change variables
  if ((indx < SETT_BUFF_LEN) && (pSettReg[indx] != NULL) && (!SettParam[indx].flag.bProt || !SettCtrl.flag.bTtlProtEn)) {
    SettParam[indx].flag.bChng = 1;
    SettCtrl.flag.bWasChng     = 1;

    if (dir == SETT_REG_UP) {
      (*pSettReg[indx]) += SettParam[indx].step;
      if ((*pSettReg[indx]) > SettParam[indx].max) {
        (*pSettReg[indx]) = SettParam[indx].max;
      }

    } else {  // dir==SETT_REG_DN
      if ((*pSettReg[indx]) >= (SettParam[indx].min + SettParam[indx].step)) {
        (*pSettReg[indx]) -= SettParam[indx].step;
      } else {
        (*pSettReg[indx]) = SettParam[indx].min;
      }
    }

    SettingsValue = (*pSettReg[indx]);

    SettRfrParam();
  }
}

/**
 * @brief Change specific bit in selected settings.
 * @param indx index of settings
 * @param bit selected bit
 * @retval none
 */
void SettBitChange(u16 indx, u8 bit)
{
  if ((indx < SETT_BUFF_LEN) && (bit < 16U) && (pSettReg[indx] != NULL) && (SettParam[indx].conv == SETT_CONV_BITMAP)) {
    u16 mask = (u16)(1U << bit);

    SettParam[indx].flag.bChng = 1;
    SettCtrl.flag.bWasChng     = 1;
    *pSettReg[indx] ^= mask;

    SettRfrParam();
  }
}

/**
 * @brief Copy "indx" parameter (val, limits, etc) to Dummy register.
 * It makes possibility for user to change dummy parameters instead Read parameter
 * and if changes are accepted -> copy Dummy parameters back into Real parameters
 * @param indx index of parameter from SETREGINDX
 */
void SettCopyToDummy(u16 indx)
{
	//u16 val = 0;
	if ((indx < SETT_BUFF_LEN) && (pSettReg[indx] != NULL)) {
		(*pSettReg[SETT_DUMMY]) = (*pSettReg[indx]);  // value
		memcpy((void *)&SettParam[SETT_DUMMY], (void *)&SettParam[indx], sizeof(SettParamDef));
	}
}

/**
 * @brief read data from settings buffer
 * @param indx - index of parameter(SETREGINDX) in pSettReg
 * @retval value of parameter
 */
u16 SettGetData(u16 indx)
{
  if ((indx < SETT_BUFF_LEN) && (pSettReg[indx] != NULL)) {
    return *pSettReg[indx];
  } else {
    return 0;
  }
}

/**
 * @brief read defined data from settings for current GateType and Model
 * @param indx index of parameter(SETREGINDX) in pSettReg
 * @retval value of defined parameter
 */
u16 SettGetBldcDef(u16 indx)
{
//  if (indx < SETT_BLDC_NUM) {
//    return Preset[SettGetModelPos(Gate.gateType, Gate.gateModel)].bldcDef[indx];
//  } else {
//    return 0;
//  }
}

/**
 * @brief Set value of variable in settings buffer under "indx" position - for external access
 * @param indx index of parameter(SETREGINDX) in pSettReg
 * @param data value of parameter
 */
void SettSetData(u16 indx, u16 data)
{
  u16 tempData;

  if ((indx < SETT_BUFF_LEN) && (pSettReg[indx] != NULL) && (!SettParam[indx].flag.bProt || !SettCtrl.flag.bTtlProtEn)) {
    if ((data >= SettParam[indx].min) && (data <= SettParam[indx].max)) {
      tempData = data;
    } else if (data < SettParam[indx].min) {
      tempData = SettParam[indx].min;
    } else if (data > SettParam[indx].max) {
      tempData = SettParam[indx].max;
    }

    if (SettParam[indx].step != 0) {
      if ((SettParam[indx].step > 1) && (tempData % SettParam[indx].step) >= (SettParam[indx].step / 2)) {
        tempData += SettParam[indx].step / 2;
      }

      tempData /= SettParam[indx].step;
      tempData *= SettParam[indx].step;
    }

    // new data is arrived
    if (*pSettReg[indx] != tempData) {
      (*pSettReg[indx])          = tempData;
      SettParam[indx].flag.bChng = 1;
      SettCtrl.flag.bWasChng     = 1;
      SettRfrParam();
    }  // else - the same data
  }
}

/**
 * @brief Need to save settings
 * @param str - who calls function (for debug)
 */
void SettNeedSave(char *str)
{
  if (!SettCtrl.flag.bNeedSave) {
    //LogNewRecord(LOG_I_SETT, LOG_SETT, LogPutStr("NeedSave:", str), " ", 0, 0, 0);
  }

  SettCtrl.flag.bNeedSave = 1;
}

/**
 * @brief Is save-bit set
 * @retval 1 - set, settings is not saved yet, 0 - not set
 */
u8 SettIsNeedSave(void)
{
  return SettCtrl.flag.bNeedSave;
}

/**
 * @brief Settings task
 * Place it in main cycle
 * @param none
 */
void SettTask(void) {}

/**
 * @brief Refreshing of external parameters that are not placed in setting but it depends on it.
 * Lock and unlock some settings
 * It depends on type of turnstile
 * Call it after total change of settings
 * @param none
 */
void SettRfrParam(void)
{
  u8 cnt;

  cnt = 0;
  // set all settings as available for edit
  while (cnt < SETT_BUFF_LEN) {
    SettSetAvail(cnt);
    cnt++;
  }

  // refresh settings about usage of feedback pin
  //GateLockSettRfr();
}


/**
 * @brief Get Type position of selected Model in Preset buffer
 */
u16 SettGetModelPos(u8 type, u8 mdl)
{
  u8 cnt  = 0;
  u16 res = 0;
  while (cnt < type) {
    res += SettCtrl.mdlNum[cnt];
    cnt++;
  }
  return res + mdl;
}

void SettClearErr(void)
{
  SettCtrl.errMap = 0;
  SettCtrl.wrnMap = 0;
}

void SettProtEn(FunctionalState state)
{
  SettCtrl.flag.bTtlProtEn = state;
}
