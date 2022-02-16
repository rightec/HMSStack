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
static UINT16 appl_iSpeed;
static UINT16 appl_iRefSpeed;

/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
static AD_UINT16Type appl_sUint16Prop = { { 0, 0xFFFF, 0 } };

/*******************************************************************************
** Public Globals
********************************************************************************
*/
#define ADI_DESCR_ALL_ACCESS  ( ABP_APPD_DESCR_GET_ACCESS | ABP_APPD_DESCR_SET_ACCESS | ABP_APPD_DESCR_MAPPABLE_WRITE_PD | ABP_APPD_DESCR_MAPPABLE_READ_PD )

/*-------------------------------------------------------------------------------------------------------------
** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
**--------------------------------------------------------------------------------------------------------------
*/
// @tag_1602_01: 
// Speed is mappable on write process data: ABP_APPD_DESCR_MAPPABLE_WRITE_PD
// RefSpeed is mappable on read process data: ABP_APPD_DESCR_MAPPABLE_READ_PD
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   {  0x1,  "SPEED",     ABP_UINT16,   1, ABP_APPD_DESCR_MAPPABLE_WRITE_PD, { { &appl_iSpeed,    &appl_sUint16Prop } } },
   {  0x2,  "REF_SPEED", ABP_UINT16,   1, ABP_APPD_DESCR_MAPPABLE_READ_PD, { { &appl_iRefSpeed, &appl_sUint16Prop } } }
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_MapType APPL_asAdObjDefaultMap[] =
{
   { 1, PD_WRITE, AD_MAP_ALL_ELEM, 0 },
   { 2, PD_READ,  AD_MAP_ALL_ELEM, 0 },
   { AD_MAP_END_ENTRY }
};



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
    return(sizeof(APPL_asAdiEntryList) / sizeof(AD_AdiEntryType));
}

void APPL_CyclicalProcessing(void)
{
    if (ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE)
    {
        /*
        ** An example of ADI data handling.
        */
        if (appl_iSpeed > appl_iRefSpeed)
        {
            /*
            ** Do something that lowers speed.
            */
            appl_iSpeed -= 1;
        }
        else if (appl_iSpeed < appl_iRefSpeed)
        {
            /*
            ** Do something that increases speed.
            */
            appl_iSpeed += 1;
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
