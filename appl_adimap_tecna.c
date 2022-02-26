/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.07.02 (2020-12-09)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.76.01 (2020-10-19)                                     **
**    ABCC Driver    5.07.01 (2020-10-12)                                     **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2022 TECNA                                      **
**                                                                            **
********************************************************************************
********************************************************************************
** Example of an ADI setup with: TO DO
@tag_1602_01 - CREATION OF THIS FILE
********************************************************************************
********************************************************************************
*/

#include "appl_adi_config.h"
#include "abcc.h"
#include "../abcc_drv/src/abcc_debug_err.h"  // @tag_1702_01

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_TECNA )

/*TO VERIFY
#if (  ABCC_CFG_STRUCT_DATA_TYPE || ABCC_CFG_ADI_GET_SET_CALLBACK )
#error ABCC_CFG_ADI_GET_SET_CALLBACK must be set to FALSE and ABCC_CFG_STRUCT_DATA_TYPE set to FALSE in order to run this example
#endif
*/
/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Access descriptor for the ADIs
**------------------------------------------------------------------------------
*/
#define APPL_READ_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_READ_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_SET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_WRITE_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

#define APPL_NOT_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS )

#define APPL_NOT_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_SET_ACCESS )



/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/
#define TECNA_MAX_ARRAY_FOR_32_BIT      255 // @tag_2502_01
#define TECNA_MAX_ARRAY_FOR_16_BIT      230 // @tag_2502_01

static UINT16 appl_iSpeed;
static UINT16 appl_iRefSpeed;
static UINT16 appl_iWelCur;
static UINT16 appl_iRefWelCur;
static UINT16 appl_aiUint16[TECNA_MAX_ARRAY_FOR_16_BIT];  // @tag_1802_00
static UINT32 appl_aiUint32[TECNA_MAX_ARRAY_FOR_32_BIT];  // @tag_2502_01
static UINT16  appl_iLastIn;    // @tag_2502_00
static UINT16  appl_iLastOut;    // @tag_2502_00

static UINT16 appl_aiUint16Ghost[TECNA_MAX_ARRAY_FOR_16_BIT];  // @tag_1802_00

/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
static AD_UINT16Type appl_sUint16Prop = { { 0, 0xFFFF, 0 } };
static AD_UINT32Type appl_sUint32Prop = { { 0, 0xFFFFFFFF, 0 } }; // @tag_2502_00

/*******************************************************************************
** Public Globals
********************************************************************************
*/
#define ADI_DESCR_ALL_ACCESS  ( ABP_APPD_DESCR_GET_ACCESS | ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_MAPPABLE_WRITE_PD | ABP_APPD_DESCR_MAPPABLE_READ_PD )

/*-------------------------------------------------------------------------------------------------------------
** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
**--------------------------------------------------------------------------------------------------------------
*/
/* @tag_1602_01: 
Speed is mappable on write process data: ABP_APPD_DESCR_MAPPABLE_WRITE_PD
RefSpeed is mappable on read process data: ABP_APPD_DESCR_MAPPABLE_READ_PD
*/
/*@tag_1602_02:
The ADI access descriptor values cannot be changed :
Input ADIs : 09h(Get access + Write process data mapping possible).
 ABP_APPD_DESCR_GET_ACCESS | ABP_APPD_DESCR_MAPPABLE_WRITE_PD
Output ADIs : 11h(Get access + Read process data mapping possible)
ABP_APPD_DESCR_GET_ACCESS | ABP_APPD_DESCR_MAPPABLE_READ_PD
*/
// #define ABP_ADI_INPUT_TECNA              ABP_APPD_DESCR_GET_ACCESS | ABP_APPD_DESCR_MAPPABLE_WRITE_PD  // @tag_1602_02
#define ABP_ADI_OUTPUT_TECNA             ABP_APPD_DESCR_GET_ACCESS | ABP_APPD_DESCR_MAPPABLE_READ_PD  // @tag_1602_02
#define ABP_ADI_INPUT_TECNA              ABP_APPD_DESCR_SET_ACCESS |  \
                                            ABP_APPD_DESCR_GET_ACCESS | ABP_APPD_DESCR_MAPPABLE_WRITE_PD  // @tag_1602_03

#define USE_OF_MAP_ADI
#define USE_OF_SIMPLE_TEST
#undef USE_OF_MAP_ADI
// #undef USE_OF_SIMPLE_TEST

#ifdef USE_OF_SIMPLE_TEST
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   {  0x1,  "SPEED",     ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_iSpeed,    &appl_sUint16Prop } } },
   {  0x2,  "REF_SPEED", ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &appl_iRefSpeed, &appl_sUint16Prop } } },
   {  0x3,  "WEL_CUR",     ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_iWelCur,    &appl_sUint16Prop } } },
   {  0x4,  "REF_WEL_CUR", ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &appl_iRefWelCur, &appl_sUint16Prop } } },
   {  0x5,  "ABP_UINT16_WRITE",   ABP_UINT16,   TECNA_MAX_ARRAY_FOR_16_BIT, APPL_WRITE_MAP_READ_ACCESS_DESC, { { appl_aiUint16, &appl_sUint16Prop } } },  // @tag_1802_00
   {  0x6,  "ABP_UINT16_READ",    ABP_UINT16,   TECNA_MAX_ARRAY_FOR_16_BIT, APPL_READ_MAP_WRITE_ACCESS_DESC,  { { appl_aiUint16, &appl_sUint16Prop } } },  // @tag_1802_00
   {  0x7,  "ABP_UINT32_WRITE",   ABP_UINT32,   TECNA_MAX_ARRAY_FOR_32_BIT, APPL_WRITE_MAP_READ_ACCESS_DESC, { { appl_aiUint32, &appl_sUint32Prop } } },  // @tag_2502_00
   {  0x8,  "ABP_UINT32_READ",    ABP_UINT32,   TECNA_MAX_ARRAY_FOR_32_BIT, APPL_READ_MAP_WRITE_ACCESS_DESC,  { { appl_aiUint32, &appl_sUint32Prop } } },  // @tag_2502_00
   {  0x9,  "LAST_IN",     ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_iLastIn,    &appl_sUint16Prop } } },
   {  0xa,  "LAST_OUT", ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &appl_iLastOut, &appl_sUint16Prop } } },
};

#else
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   {  0x1,  "SPEED",     ABP_UINT16,   1, ABP_ADI_OUTPUT_TECNA, { { &appl_iSpeed,    &appl_sUint16Prop } } },
   {  0x2,  "REF_SPEED", ABP_UINT16,   1, ABP_ADI_INPUT_TECNA, { { &appl_iRefSpeed, &appl_sUint16Prop } } }
};
#endif

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/

#ifdef USE_OF_MAP_ADI
/*@tag_1702_00*/
const AD_MapType APPL_asAdObjDefaultMap[] =
{
   { AD_MAP_PAD_ADI, PD_WRITE, 16, 0 },
   { AD_MAP_PAD_ADI, PD_READ,  16, 0 },
   { AD_MAP_END_ENTRY }
};
#else
#ifdef USE_OF_SIMPLE_TEST
// @tag_1702_01: Restore simple example changing nameand number of variables
const AD_MapType APPL_asAdObjDefaultMap[] =
{
   { 1, PD_WRITE, AD_MAP_ALL_ELEM, 0 },
   { 2, PD_READ,  AD_MAP_ALL_ELEM, 0 },
   { 3, PD_WRITE, AD_MAP_ALL_ELEM, 0 },
   { 4, PD_READ,  AD_MAP_ALL_ELEM, 0 },
   { 5, PD_WRITE, AD_MAP_ALL_ELEM, 0 },
   { 6, PD_READ,  AD_MAP_ALL_ELEM, 0 },
   { 7, PD_WRITE, AD_MAP_ALL_ELEM, 0 },
   { 8, PD_READ,  AD_MAP_ALL_ELEM, 0 },
   { 9, PD_WRITE, AD_MAP_ALL_ELEM, 0 },
   { 10, PD_READ,  AD_MAP_ALL_ELEM, 0 },
   { AD_MAP_END_ENTRY }
};
#else
const AD_MapType APPL_asAdObjDefaultMap[] =
{
   { 1, PD_WRITE, AD_MAP_ALL_ELEM, 0 },
   { 2, PD_READ,  AD_MAP_ALL_ELEM, 0 },
   { AD_MAP_END_ENTRY }
};
#endif
#endif





/*******************************************************************************
** Private Services
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

UINT16 APPL_GetNumAdi(void)
{
    int a = sizeof(APPL_asAdiEntryList);
    int b = sizeof(APPL_asAdObjDefaultMap);
    int c = sizeof(AD_AdiEntryType);
    int d = sizeof(AD_MapType);
    return(sizeof(APPL_asAdiEntryList) / sizeof(AD_AdiEntryType));
    /// -1 is because of  AD_MAP_END_ENTRY 
    /// return((sizeof(APPL_asAdObjDefaultMap) / sizeof(AD_MapType)) -1 );

}

void APPL_CyclicalProcessing(void)
{
    if (ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE)
    {
        /*
        ** An example of ADI data handling.
        */

        for (int i = 0; i < TECNA_MAX_ARRAY_FOR_16_BIT; i++) {
            if (appl_aiUint16Ghost[i] != appl_aiUint16[i]) {
                /// Align internal array as soon as external var is changed
                appl_aiUint16Ghost[i] = appl_aiUint16[i];
                DEBUG_EVENT(("Update %d element of the ghost array\n",i)); // @tag_1802_00
            } /// else
        }


        for (int i = 0; i < TECNA_MAX_ARRAY_FOR_32_BIT; i++) {
            if (0 != appl_aiUint32[i]) {
                DEBUG_EVENT(("Update %d element of the UINT32 array with value: %x\n", i, appl_aiUint32[i])); // @tag_2502_09
            } /// else
        }


        if (appl_iLastIn != appl_iLastOut) {
            DEBUG_EVENT(("appl_iLastIn is 0x%x and appl_iLastOut is 0x%x\n", appl_iLastIn, appl_iLastOut)); // @tag_2502_05
            appl_iLastIn = appl_iLastOut;
        } /// else

        if (appl_iSpeed > appl_iRefSpeed)
        {
            /*
            ** Do something that lowers speed.
            */
            DEBUG_EVENT(("appl_iSpeed > appl_iRefSpeed\n")); // @tag_1702_01 @tag_1802_01
            DEBUG_EVENT(("appl_iSpeed is %d and appl_iRefSpeed is %d\n", appl_iSpeed,appl_iRefSpeed)); // @tag_1802_00

            appl_iSpeed -= 1;
        }
        else if (appl_iSpeed < appl_iRefSpeed)
        {
            /*
            ** Do something that increases speed.
            */
            DEBUG_EVENT(("appl_iSpeed < appl_iRefSpeed\n")); // @tag_1702_01
            DEBUG_EVENT(("appl_iSpeed is %d and appl_iRefSpeed is %d\n", appl_iSpeed, appl_iRefSpeed)); // @tag_1802_01

            appl_iSpeed += 1;
            appl_iWelCur += 1;
        }
    }
    else
    {
        /*
        ** We are not in process active, the default should be that the motor
        ** should not run.
        */
        appl_iSpeed = 0;
    }
}

/*******************************************************************************
** Tasks
********************************************************************************
*/

#endif
