#include "main.h"
//#include "menu.h"
#include "menu_text.h"

//#define LOG_SETT LOG_S_SETT

SettCtrlDef SettCtrl;          // control registers of settings
SettUnitDef SettUnit;          // unit parameters definition read from settings
u16 *pSettReg[SETT_BUFF_LEN];  // pointer to settings value
// u16 SettBuffTemp[SETT_BUFF_LEN_MAX];		// for temporary calculation
PresetDef Preset[RB_MDL_TTL_NUM]; ///PresetDef Preset[GATE_MDL_TTL_NUM];     // preset buffer def
SettParamDef SettParam[SETT_BUFF_LEN];  // min,max,def,step of parameters


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

    if (indxText != NULL) {
      SettParam[indx].pText = indxText;
    }

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
  u16 cnt    = 0;
  u16 cntMax = 0;
  //u16 weight;
  u16 weightMax;
  u16 weightLimit;
  u16 weightTemp;
  u16 cntBlock;
  u16 cntBlockMax;
  u32 addr;
  u16 mdlPos;
  u16 errMap;
  u16 cntBlockExit;
  u16 lenEst;
  u16 cntOld;
  u16 cntUnit;

  union {
    u8 byte;

    struct
    {
      u8 bRestoreOldSett: 1;
      u8 bRestoreNewSett: 1;
    } bit;
  } flag;

  flag.byte = 0;

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

// set types number for each model
// and makes gate preset initialization (define values)
//---------------------ROAD_BLOCKER--------------------
  //SettCtrl.mdlNum[INTERNAL_HPU] = INT_HPU_NUM;



  SettPresetRBIntHPU();
  SettPresetRBExtHPU();
  SettPresetRBMobile();

  cnt = 0;
  while (cnt < RB_MDL_TTL_NUM) {///<GATE_MDL_TTL_NUM
    // common presets for all types of turnstiles
    Preset[cnt].settDef[SETT_CRC_INDX]    = SETT_CRC_DEF;
    Preset[cnt].settDef[SETT_WEIGHT_INDX] = SETT_WEIGHT_DEF;

    // structure of settings. It is copied for each preset
    Preset[cnt].settDef[SETT_SETT_NUM_INDX] = SETT_BUFF_LEN;
    Preset[cnt].settDef[SETT_UNIT_NUM_INDX] = SETT_UNIT_NUM;
    Preset[cnt].settDef[SETT_UNIT0_INDX]    = SETT_CRC_INDX;
//    Preset[cnt].settDef[SETT_UNIT1_INDX]    = SETT_RB_TYPE;
//    Preset[cnt].settDef[SETT_UNIT2_INDX]    = SETT_ANG_SENS_CHNG_ADDR;  //SETT_LOCKA_TYPE;
//    Preset[cnt].settDef[SETT_UNIT3_INDX]    = SETT_HS_TOP_SLOW_ANGLE;   //SETT_CURR_TTL;
//    Preset[cnt].settDef[SETT_UNIT4_INDX]    = SETT_IND_TYPE;            //SETT_BLDC_PID_P;
//    Preset[cnt].settDef[SETT_UNIT5_INDX]    = SETT_I_RS1_MODE;
//    Preset[cnt].settDef[SETT_UNIT6_INDX]    = SETT_CAL_PARAM_1;
//    Preset[cnt].settDef[SETT_UNIT7_INDX]    = AUTO_CLOS_PAUSE_RB1;

    cnt++;
  }


  SettSetParam(SETT_M_KG_S1,			&weight[0].kg, 0, 1, 1, SETT_TEXT_KG_S1, SETT_CONV_NO);
  SettSetParam(SETT_M_KG_MAX_S1,		&weight[0].max_kg, 0, 1, 1, SETT_TEXT_KG_MAX_S1, SETT_CONV_NO);
  SettSetParam(SETT_M_KG_S2,			&weight[1].kg, 0, 1, 1, SETT_TEXT_KG_S2, SETT_CONV_NO);
  SettSetParam(SETT_M_KG_MAX_S2,		&weight[1].max_kg, 0, 1, 1, SETT_TEXT_KG_MAX_S2, SETT_CONV_NO);
  SettSetParam(SETT_M_RAW_S1,			&weight[0].raw_data, 0, 1, 1, SETT_TEXT_RAW_S1, SETT_CONV_NO);
  SettSetParam(SETT_M_OFFSETT_S1,	    &weight[0].raw_zero_offset, 0, 1, 1, SETT_TEXT_OFFSETT_S1, SETT_CONV_NO);
  SettSetParam(SETT_M_RAW_S2,			&weight[1].raw_data, 0, 1, 1, SETT_TEXT_RAW_S2, SETT_CONV_NO);
  SettSetParam(SETT_M_OFFSETT_S2,	    &weight[1].raw_zero_offset, 0, 1, 1, SETT_TEXT_OFFSETT_S2, SETT_CONV_NO);

  SettSetParam(SETT_M_SYNCHRO_MODE,		&settings.mod_config, ALARM_ST_ALONE, ALARM_SYNCHRO, 1, SETT_TEXT_SYNCHRO_MODE, SETT_CONV_NO);
  SettSetParam(SETT_M_THRESHOLD_WEIGHT, &settings.alarm_treshold_kg, 0, 1, 1, SETT_TEXT_THRESHOLD_WEIGHT, SETT_CONV_NO);

  // --- UNIT-1. General settings
//  SettSetParam(SETT_RB_TYPE,                &road_blocker.RB_type, SETT_LIM_MIN, RB_TYPE_NUM - 1, 1, SETT_TEXT_RB_TYPE, SETT_CONV_NO);
//  SettSetParam(SETT_RB_MODEL,               &road_blocker.RB_model, SETT_LIM_MIN, INT_HPU_NUM + EXT_HPU_NUM + MOBILE_RB_NUM, 1, SETT_TEXT_RB_MDL, SETT_CONV_NO);

  /*
  cntBlockExit = FLACH_SETT_BLOCK_NUM * 2;

  // settings will be searching in weightMax range
  weightLimit = SETT_WEIGHT_MAX;
  weightMax   = 0;
  while (1) {
    // --- find block of settings with higher weight
    // Settings are located in FLASH_SETT1_SECTOR and FLASH_SETT2_SECTOR, which is stand one after another
    cntBlock       = 0;
    cntBlockMax    = FLACH_SETT_BLOCK_NUM * 2;
    weight         = 0;
    SettCtrl.block = 0;
    while (cntBlock < cntBlockMax) {
      addr       = FLASH_SETT1_SECTOR_ADDR + cntBlock * FLACH_SETT_BLOCK_LEN + SETT_WEIGHT_INDX * 2;
      weightTemp = *(__IO u16 *)addr;
      static u8 i = 0;

      if (weightTemp < weightLimit) {
        if (weight < weightTemp) {
          weight         = weightTemp;
          SettCtrl.block = cntBlock;
        }
        if (weightMax < weightTemp) {
          weightMax = weightTemp;
        }
      }
      cntBlock++;
    }
    LogNewRecord(LOG_I_SETT, LOG_SETT, "DBG", ".-WL-W_OK", 0, weightLimit, weight);
    // there is normal weight - some block with settings was found
    if (weight) {
#define SettMemGetData(a) (*(__IO u16 *)(addr + a * 2))
#define SettMemGetAddr(a) ((u16 *)(addr + a * 2))

      // --- load settings data into temporary buffer
      addr = FLASH_SETT1_SECTOR_ADDR + SettCtrl.block * FLACH_SETT_BLOCK_LEN;  // start address

      // estimate of settings length based on the presence of words 0xFFFF
      lenEst = 32;  // skip first 32 bytes
      while (lenEst < (FLACH_SETT_BLOCK_LEN / 2)) {
        if (SettMemGetData(lenEst) == 0xFFFF) {
          break;
        }

        lenEst++;
      }

      //DBG
      u16 _crc = SettCrcCalc(SettMemGetAddr(SETT_CRC_INDX + 1), SETT_BUFF_LEN - 1);
      if(SettMemGetData(SETT_CRC_INDX) != _crc) {
        LogNewRecord(LOG_I_SETT, LOG_SETT, "DbgCRCFAIL", "crc-crc_blk-.", _crc, SettMemGetData(SETT_CRC_INDX), 0);}
      if(lenEst != SETT_BUFF_LEN) {
        LogNewRecord(LOG_I_SETT, LOG_SETT, "dbg", "lenEst-BUFF_l-.", lenEst, SETT_BUFF_LEN, 0);}
      if(SettMemGetData(SETT_SETT_NUM_INDX) != SETT_BUFF_LEN) {
        LogNewRecord(LOG_I_SETT, LOG_SETT, "dbg", "SET_N_I-B_L-.", SettMemGetData(SETT_SETT_NUM_INDX), SETT_BUFF_LEN, 0);}
      //DBG

      // --- check settings
      errMap = 0;  // pretend like no errors are exist
      // check CRC of settings...
      // ...as for new settings set
      if ((SettMemGetData(SETT_CRC_INDX) == SettCrcCalc(SettMemGetAddr(SETT_CRC_INDX + 1), SETT_BUFF_LEN - 1)) &&
          (lenEst == SETT_BUFF_LEN) &&
          (SettMemGetData(SETT_SETT_NUM_INDX) == SETT_BUFF_LEN)) {
            LogNewRecord(LOG_I_SETT, LOG_SETT, "DbgCRCOK", ".-.-.", 0, 0, 0);
        // settings have the same units number
        if ((SettMemGetData(SETT_UNIT_NUM_INDX) == Preset[0].settDef[SETT_UNIT_NUM_INDX]) &&
            (SettMemGetData(SETT_UNIT_NUM_INDX) <= SETT_UNIT_MAX) &&
            SettMemGetData(SETT_UNIT_NUM_INDX)) {
          // compare units index
          cnt = SETT_UNIT0_INDX;
          while (cnt <= SETT_UNIT7_INDX) {
            // compare Preset[0] because all presets are the same
            if (SettMemGetData(cnt) != Preset[0].settDef[cnt])
              break;
            cnt++;
          }

          // all units have the same indexes
          if (cnt > SETT_UNIT7_INDX) {
            LogNewRecord(LOG_I_IMP, LOG_SETT, "SettComp-OK", "Block-Len-.", SettCtrl.block, lenEst, 0);
            // copy settings into work buffer
            cnt = 0;
            while (cnt < SETT_BUFF_LEN) {
              if (pSettReg[cnt] != NULL) {
                (*pSettReg[cnt]) = SettMemGetData(cnt);
              }

              cnt++;
            }
          } else {  // some units have different indexes
            LogNewRecord(LOG_I_IMP, LOG_SETT, "SettDiffUnitIndex", "Block-Len-.", SettCtrl.block, lenEst, 0);
            flag.bit.bRestoreNewSett = 1;
          }
        } else {  // settings have different units number
          LogNewRecord(LOG_I_IMP, LOG_SETT, "SettDiffUnitNum", "Block-Len-.", SettCtrl.block, lenEst, 0);
          flag.bit.bRestoreNewSett = 1;
        }
      }
      // ...as for old settings set (v1.00)
      else if ((SettMemGetData(SETT_CRC_INDX) == SettCrcCalc(SettMemGetAddr(SETT_CRC_INDX + 1), SETT_OLD_BUFF_LEN - 1)) && (lenEst == SETT_OLD_BUFF_LEN)) {
        LogNewRecord(LOG_I_IMP, LOG_SETT, "Sett v1.00 detected", "", 0, 0, 0);
        LogNewRecord(LOG_I_IMP, LOG_SETT, "RestoreFromV1.00", "Block-Len-.", SettCtrl.block, lenEst, 0);
        flag.bit.bRestoreOldSett = 1;
      }
      // ...as some type of settings after (v1.01) and later
      else if ((lenEst > SETT_OLD_BUFF_LEN) && (SettMemGetData(SETT_CRC_INDX) == SettCrcCalc(SettMemGetAddr(SETT_CRC_INDX + 1), lenEst - 1))) {
        LogNewRecord(LOG_I_IMP, LOG_SETT, "SettHasDiffSize", "LenRd-Len-.", lenEst, SETT_BUFF_LEN, 0);
        LogNewRecord(LOG_I_IMP, LOG_SETT, "RestoreMainSett", "Block-Len-", SettCtrl.block, lenEst, 0);
        flag.bit.bRestoreNewSett = 1;
      } else {  // CRC-error
        LogNewRecord(LOG_I_IMP, LOG_SETT, "SettCrcErr", "Block-Len-.", SettCtrl.block, lenEst, 0);
        SetU16Bit(&errMap, SETT_ERR_CRC);
      }

      // need to restore settings from v1.00
      if (flag.bit.bRestoreOldSett) {
        flag.bit.bRestoreOldSett = 0;

        // Set all settings as default and Copy old settings into new
        SettResetToDef(SettMemGetData(SETT_OLD_G_TYPE), SettMemGetData(SETT_OLD_G_MODEL), SETT_RST_ALL);
        // and copy important settings from
        // --- UNIT-0. Internal  parameters of settings.
        (*pSettReg[SETT_WEIGHT_INDX]) = SettMemGetData(SETT_OLD_WEIGHT_INDX);
        (*pSettReg[SETT_GATE_GR])     = SettMemGetData(SETT_OLD_GATE_GR);
        (*pSettReg[SETT_CNFG0])       = SettMemGetData(SETT_OLD_CNFG0);
        // --- UNIT-1. Gate settings
        (*pSettReg[SETT_RB_TYPE])      = SettMemGetData(SETT_OLD_G_TYPE);
        (*pSettReg[SETT_RB_MODEL])     = SettMemGetData(SETT_OLD_G_MODEL);
        // (*pSettReg[SETT_G_BR_LVL])    = SettMemGetData(SETT_OLD_G_BR_LVL);
        // (*pSettReg[SETT_G_POS_A])     = SettMemGetData(SETT_OLD_G_POS_A);
        // (*pSettReg[SETT_G_POS_B])     = SettMemGetData(SETT_OLD_G_POS_B);
        // (*pSettReg[SETT_HS_TOP_SLOW_ANGLE])   = SettMemGetData(SETT_OLD_G_SPD_MAX);
        // // --- UNIT-2. lock magnet settings
        // (*pSettReg[SETT_LOCKA_TYPE])  = SettMemGetData(SETT_OLD_LOCKA_TYPE);
        // (*pSettReg[SETT_LOCKA_MODE])  = SettMemGetData(SETT_OLD_LOCKA_MODE);
        // // --- UNIT-3. Current consumption definition
        // (*pSettReg[SETT_CURR_TTL])    = SettMemGetData(SETT_OLD_CURR_TTL);
        // --- UNIT-6. Calibration parameters
        cntOld                        = SETT_OLD_CAL_PNTMAP;
        cnt                           = SETT_CAL_PARAM_1;
        while (cntOld < SETT_OLD_BUFF_LEN) {
          if (pSettReg[cnt] != NULL) {
            (*pSettReg[cnt]) = SettMemGetData(cntOld);
          }

          cntOld++;
          cnt++;
        }
        SetU16Bit(&SettCtrl.wrnMap, SETT_ERR_RANGE);
      }
      // need to restore settings from v1.01 and upper
      else if (flag.bit.bRestoreNewSett) {
        flag.bit.bRestoreNewSett = 0;

        // Set all settings as default and Copy old settings into new
        SettResetToDef(SettMemGetData(SETT_RB_TYPE), SettMemGetData(SETT_RB_MODEL), SETT_RST_ALL);

        // --- UNIT-1. Copy some Gate settings
        cntOld = SettMemGetData(SETT_UNIT1_INDX);
        cnt    = SETT_RB_TYPE;
        while (cnt <= SETT_GEN_SAFE_SIGN_EFO) {
          if (pSettReg[cnt] != NULL) {
            (*pSettReg[cnt]) = SettMemGetData(cntOld);
          }

          cntOld++;
          cnt++;
        }
        // --- UNIT-2. Copy some Lock magnet settings
        cntOld = SettMemGetData(SETT_UNIT2_INDX);
        cnt    = SETT_ANG_SENS_CHNG_ADDR;
        while (cnt <= SETT_ANG_PARAM_2) {
          if (pSettReg[cnt] != NULL) {
            (*pSettReg[cnt]) = SettMemGetData(cntOld);
          }

          cntOld++;
          cnt++;
        }
        // --- UNIT-3. 
        cntOld = SettMemGetData(SETT_UNIT3_INDX);
        cnt    = SETT_HS_TOP_SLOW_ANGLE;
        while (cnt <= SETT_HS_UPS_LOWERING_TIME) {
          if (pSettReg[cnt] != NULL) {
            (*pSettReg[SETT_HS_UPS_LOWERING_TIME]) = SettMemGetData(cntOld);
          }

          cntOld++;
          cnt++;
        }
        // search of calibration unit in settings (It must be the last unit)
        cntUnit = SETT_UNIT1_INDX;
        while (cntUnit <= SETT_UNIT7_INDX) {
          if (!SettMemGetData(cntUnit) || (cntUnit == SETT_UNIT7_INDX))
            break;
          cntUnit++;
        }
        cntUnit--;
        // --- UNIT-6. Calibration parameters
        cntOld = SettMemGetData(cntUnit);
        cnt    = SETT_CAL_PARAM_1;
        while (cnt < SETT_CAL_PARAM_8) {
          if (pSettReg[cnt] != NULL) {
            (*pSettReg[cnt]) = SettMemGetData(cntOld);
          }

          cntOld++;
          cnt++;
        }
        SetU16Bit(&SettCtrl.wrnMap, SETT_ERR_RANGE);
      }

      // check gate type and model about correct limits before main checking about limits
      // because they are used before it
      if ((Gate.gateType >= GATE_TYPE_NUM) ||
          (Gate.gateModel >= SettCtrl.mdlNum[Gate.gateType])) {
        SetU16Bit(&errMap, SETT_ERR_CRC);
      }

      if (!errMap) {                          // no errors before
        if (Gate.gateGroup != GATE_GR_DEF) {  // fail
          SetU16Bit(&errMap, SETT_ERR_GR);
        } else {                              // OK
          // refresh MotorSettings (depends on Gate and Model settings)
          if (BldcInitParamCalc()) {
            // fill some settings limits (which depend on MotorSettings).
            // --- speed parameters
            cnt    = SETT_ANG_PARAM_1;
            cntMax = SETT_ANG_PARAM_2;
            while (cnt <= cntMax) {
              SettSetParam(cnt, NULL, BldcMotor.spdMin, BldcMotor.spdMax, GATE_SPD_STEP, NULL, SETT_CONV_DIV_100);
              cnt++;
            }
            cnt    = SETT_HS_TOP_SLOW_ANGLE;
            cntMax = SETT_HS_BOT_SLOW_ANGLE;
            while (cnt <= cntMax) {
              SettSetParam(cnt, NULL, BldcMotor.spdMin, BldcMotor.spdMax, GATE_SPD_STEP, NULL, SETT_CONV_DIV_100);
              cnt++;
            }

            // --- calibration parameters (hall position)
            // cnt    = SETT_CAL_HPOS_1;
            // cntMax = SETT_CAL_HPOS_6;
            // while (cnt <= cntMax) {
            //   SettSetParam(cnt, NULL, SETT_LIM_MIN, BldcMotor.e360 - 1, SETT_PROT, NULL, SETT_CONV_NO);
            //   cnt++;
            // }
            // // --- calibration parameters (zero offset)
            // SettSetParam(SETT_CAL_ZERO_OFF_CCW, NULL, SETT_LIM_MIN, BldcMotor.e360 - 1, SETT_PROT, NULL, SETT_CONV_NO);
            // SettSetParam(SETT_CAL_ZERO_OFF_CW, NULL, SETT_LIM_MIN, BldcMotor.e360 - 1, SETT_PROT, NULL, SETT_CONV_NO);
            // check all buffer and correct all values that fall out from allowed range
            cnt = 0;
            while (cnt < SETT_BUFF_LEN) {
              // value is out of range - set it to default
              if (pSettReg[cnt] != NULL) {
                SettParam[cnt].flag.bErrMax = 0;
                SettParam[cnt].flag.bErrMin = 0;

                if ((*pSettReg[cnt]) > SettParam[cnt].max) {
                  SettParam[cnt].flag.bErrMax = 1;
                  LogNewRecord(LOG_I_ERR, LOG_SETT, "MaxErr:", "Indx-Max-Rd", cnt, SettParam[cnt].max, *pSettReg[cnt]);
                } else if ((*pSettReg[cnt]) < SettParam[cnt].min) {
                  SettParam[cnt].flag.bErrMin = 1;
                  LogNewRecord(LOG_I_ERR, LOG_SETT, "MinErr:", "Indx-Min-Rd", cnt, SettParam[cnt].min, *pSettReg[cnt]);
                }

                // there are range errors were captured
                if (SettParam[cnt].flag.bErrMin || SettParam[cnt].flag.bErrMax) {
                  SettResetSingleToDef(cnt);
                  SetU16Bit(&errMap, SETT_ERR_RANGE);  // set error for next conditional

                  // only some Settings are sensetive to range errors
                  // some problem with work area definition
                  // if ((cnt == SETT_GEN_RISING_TIME) || (cnt == SETT_GEN_LOWERING_TIME)) {
                  //   SettResetSingleToDef(SETT_SRCH_STAT);  // reset search info
                  //   SetU16Bit(&errMap, SETT_ERR_CAL);      // set error for next conditional
                  // }
                  // calibration data are lost
                  // else if ((cnt >= SETT_CAL_PNTMAP) && (cnt <= SETT_CAL_SYNC)) {
                  //   SettResetSingleToDef(SETT_CAL_STAT);   // reset cal. info
                  //   SettResetSingleToDef(SETT_SRCH_STAT);  // reset search info
                  //   SetU16Bit(&errMap, SETT_ERR_CAL);      // set error for next conditional
                  // }
                }
              }
              cnt++;
            }
          } else {                              // BldcInitParamCalc()==false
            SetU16Bit(&errMap, SETT_ERR_BLDC);  // set error for next conditional
          }
        }
      }  // else - err!=0

      // add current errors as warning into SettCtrl
      SettCtrl.wrnMap |= errMap;
      ClearU16Bit(&errMap, SETT_ERR_RANGE);  // it is not critical error. It is only warning
      if (errMap) {                          // errors were captured
        if (ReadU16Bit(&errMap, SETT_ERR_CRC)) LogNewRecord(LOG_I_ERR, LOG_SETT, "CrcErr", "Block-.-.", SettCtrl.block, 0, 0);
        if (ReadU16Bit(&errMap, SETT_ERR_CAL)) LogNewRecord(LOG_I_ERR, LOG_SETT, "CalibrErr", "Block-.-.", SettCtrl.block, 0, 0);
        if (ReadU16Bit(&errMap, SETT_ERR_GR)) LogNewRecord(LOG_I_ERR, LOG_SETT, "WrongGroup", "Block-.-.", SettCtrl.block, 0, 0);
        if (ReadU16Bit(&errMap, SETT_ERR_BLDC)) LogNewRecord(LOG_I_ERR, LOG_SETT, "BldcErr", "Block-.-.", SettCtrl.block, 0, 0);

        // reduce range of searching. Block with errors is not available anymore for searching
        // but weightMax contains maximum weight of current settings
        weightLimit = weight;
      } else {  // no errors
        // model position in Preset buffer
        mdlPos = SettGetModelPos(Gate.gateType, Gate.gateModel);

        // copy Preset to SettParam -> it is for external access to data
        cnt = 0;
        while (cnt < SETT_BUFF_LEN) {
          // Use preset value
          if (cnt < SETT_PRESET_LEN) {
            SettParam[cnt].def = Preset[mdlPos].settDef[cnt];
          } else {  // over preset range all settings have typical default value
            // fall out of Unit5
            if (cnt == SETT_CAL_PARAM_8) {
              SettParam[cnt].def = BLDC_HALL_DEF;
            } else {
              SettParam[cnt].def = 0;
            }
          }
          cnt++;
        }
        LogNewRecord(LOG_I_SETT, LOG_SETT, "OK", "Block-Weight-.", SettCtrl.block, weight, 0);
        // set maximal weight of settings
        // when Settings will be saved next time -> maximal weight will be used and all other block of settings
        // will have lower weight
        SettCtrl.weight = weightMax;
        break;                            // go out from cycle
      }
    } else {                              // ==0, settings are lost
      SettCtrl.errMap = SettCtrl.wrnMap;  // copy all previously found warning into error map
      SettCtrl.wrnMap = 0;                // clean warning map. It is no sense to keep it
      if (!SettCtrl.errMap) {             // no errors -> this is case of empty Flash
        SetU16Bit(&SettCtrl.errMap, SETT_ERR_EMPTY);
        LogNewRecord(LOG_I_ERR, LOG_SETT, "EmptyFlash", "", 0, 0, 0);
      } else {
        LogNewRecord(LOG_I_ERR, LOG_SETT, "NoCorrectSett", "", 0, 0, 0);
      }

      SettCtrl.flag.bSectErase = 1;  // when user click "save settings" -> both sectors will be erased
      SettCtrl.weight          = 1;  // new weight
      SettCtrl.block           = 0;
      // try to reset all values to defined state for GateType & Model loaded from flash (Gate.gateType, Model)
      // if Gate.gateModel or Type are incorrect -> all values will be reset as for GATE_TYPE_DEF and GATE_MDL_DEF turnstile
      SettResetToDef(Gate.gateType, Gate.gateModel, SETT_RST_ALL);
      break;  // go out from cycle
    }
    cntBlockExit--;
    if (!cntBlockExit) {
      SetU16Bit(&SettCtrl.errMap, SETT_ERR_EMPTY);
      LogNewRecord(LOG_I_ERR, LOG_SETT, "ProgrammFail", "", 0, 0, 0);
      break;
    }
  }  // while(1)

  LogNewRecord(LOG_I_SETT, LOG_SETT, "FinishLoad", "", 0, 0, 0);

  if (!SettCtrl.errMap) {
    FlashErrLog(BERR_GR_SYS, BERR_S_SETT_LOAD_OK);
  } else {
    FlashErrLog(BERR_GR_SYS, BERR_S_SETT_LOAD_FAIL);
  }

  SettRfrParam();
  */
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
  if ((indx < SETT_BUFF_LEN) && (pSettReg[indx] != NULL) && (SettParam[indx].conv == SETT_CONV_BITMAP)) {
    SettParam[indx].flag.bChng = 1;
    SettCtrl.flag.bWasChng     = 1;

    if (!ReadU16Bit(pSettReg[indx], bit)) {
      SetU16Bit(pSettReg[indx], bit);
    } else {
      ClearU16Bit(pSettReg[indx], bit);
    }

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
