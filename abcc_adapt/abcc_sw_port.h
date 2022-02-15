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
** COPYRIGHT NOTIFICATION (c) 2013 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, permission is      **
** granted to modify, reproduce and distribute the code in binary form        **
** without any restrictions.                                                  **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** Platform dependent macros and functions required by the ABCC driver and
** Anybus objects implementation to be platform independent.
** The description of the macros are found in abcc_port.h. Abcc_port.h is found
** in the public ABCC40 driver interface.
********************************************************************************
********************************************************************************
** Services:
**    ABCC_PORT_DebugPrint()           - Print debug info
**    ABCC_PORT_UseCritical()          - Used if any preparation is needed
**                                       before calling
**                                       "ABCC_PORT_EnterCritical()".
**    ABCC_PORT_EnterCritical()        - Disables all interrupts, if they are
**                                       not already disabled.
**    ABCC_PORT_ExitCritical()         - Restore interrupts to the state they
**                                       were in when
**                                       "ABCC_PORT_EnterCritical()" was called.
**    ABCC_PORT_MemCpy()               - Port of standard memcpy()
**    ABCC_PORT_StrCpyToNative()       - Copy native char string to octet string
**    ABCC_PORT_StrCpyToPacked()       - Copy octetstring to native char* string
**    ABCC_PORT_Uint8CpyToNative()     - Copy packed octet string to native UINT8 array
**    ABCC_PORT_Uint8CpyToPacked()     - Copy native UINT8 array to packed octet string
**    ABCC_PORT_CopyOctets()           - Copy octet aligned buffer.
**    ABCC_PORT_Copy8()                - Copy octet aligned 8 bit data.
**    ABCC_PORT_Copy16()               - Copy octet aligned 16 bit data.
**    ABCC_PORT_Copy32()               - Copy octet aligned 32 bit data.
**    ABCC_PORT_Copy64()               - Copy octet aligned 64 bit data.
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_SW_PORT_H_
#define ABCC_SW_PORT_H_

#include "stdio.h"
#include "string.h"
#include "abcc_td.h"
#include "abcc_drv_cfg.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

#define ABCC_PORT_DebugPrint( args )   printf args

#define ABCC_PORT_UseCritical() ABCC_PORT_UseCriticalImpl()
EXTFUNC void ABCC_PORT_UseCriticalImpl( void );

#define ABCC_PORT_EnterCritical() ABCC_PORT_EnterCriticalImpl()
EXTFUNC void ABCC_PORT_EnterCriticalImpl( void );

#define ABCC_PORT_ExitCritical() ABCC_PORT_ExitCriticalImpl()
EXTFUNC void ABCC_PORT_ExitCriticalImpl( void );

#ifdef ABCC_SYS_16_BIT_CHAR

void ABCC_PORT_StrCpyToPackedImpl( void* pxDest, UINT16 iDestOctetOffset,
                                   const void* pxSrc, UINT16 iNbrOfChars );

void ABCC_PORT_StrCpyToNativeImpl( void* pxDest,  void* pxSrc,
                                   UINT16 iSrcOctetOffset, UINT16 iNbrOfChars );

void ABCC_PORT_CopyOctetsImpl( void* pxDest, UINT16 iDestOctetOffset,
                               void* pxSrc, UINT16 iSrcOctetOffset,
                               UINT16 iNumOctets );

#define ABCC_PORT_MemCpy( pbDest, pbSource, iNbrOfOctets )                     \
        memcpy( pbDest, pbSource, ( (iNbrOfOctets) + 1 ) >> 1 );

#define ABCC_PORT_StrCpyToNative( pxDest, pxSrc, iSrcOctetOffset, iNbrOfChars ) \
        ABCC_PORT_StrCpyToNativeImpl( pxDest, pxSrc, iSrcOctetOffset, iNbrOfChars )

#define ABCC_PORT_StrCpyToPacked( pxDest, iDestOctetOffset, pxSrc, iNbrOfChars ) \
        ABCC_PORT_StrCpyToPackedImpl( pxDest, iDestOctetOffset, pxSrc, iNbrOfChars )

#define ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset,\
                              iNumOctets )                                     \
        ABCC_PORT_CopyOctetsImpl( pxDest, iDestOctetOffset, pxSrc,             \
                                  iSrcOctetOffset, UINT16 iNumOctets );

#define ABCC_PORT_Copy8( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )    \
        ABCC_PORT_CopyOctetsImpl( pxDest, iDestOctetOffset, pxSrc,             \
                                  iSrcOctetOffset, 1 )

#else

#define ABCC_PORT_MemCpy( pbDest, pbSource, iNbrOfOctets )                     \
        memcpy( pbDest, pbSource, iNbrOfOctets );

#define ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset,\
                              iNumOctets )                                     \
        ABCC_PORT_MemCpy( (void*)&( (UINT8*)(pxDest) )[ iDestOctetOffset ],    \
                          (void*)&( (UINT8*)(pxSrc) )[ iSrcOctetOffset ],      \
                          iNumOctets )

#define ABCC_PORT_StrCpyToNative( pxDest, pxSrc, iSrcOctetOffset, iNbrOfChars )\
        ABCC_PORT_MemCpy( (void*)(pxDest),                                     \
                          (void*)&( (UINT8*)(pxSrc) )[ iSrcOctetOffset ],      \
                          iNbrOfChars )

#define ABCC_PORT_StrCpyToPacked( pxDest, iDestOctetOffset, pxSrc,             \
                                  iNbrOfChars )                                \
        ABCC_PORT_MemCpy( (void*)&( (UINT8*)(pxDest) )[ iDestOctetOffset ],    \
                           pxSrc, iNbrOfChars )

#define ABCC_PORT_Copy8( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )    \
        ( (UINT8*)(pxDest) )[ iDestOctetOffset ] =                             \
        ( (UINT8*)(pxSrc) )[ iSrcOctetOffset ]
#endif

#define ABCC_PORT_Uint8CpyToNative( pxDest, pxSrc, iSrcOctetOffset, iNbrOfOctets )\
        ABCC_PORT_StrCpyToNative( pxDest, pxSrc, iSrcOctetOffset, iNbrOfOctets )

#define ABCC_PORT_Uint8CpyToPacked( pxDest, iDestOctetOffset, pxSrc, iNbrOfOctets ) \
        ABCC_PORT_StrCpyToPacked( pxDest, iDestOctetOffset, pxSrc, iNbrOfOctets )

#define ABCC_PORT_Copy16( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )   \
        ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset, 2 )

#define ABCC_PORT_Copy32( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )   \
        ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset, 4 )

#if( ABCC_CFG_64BIT_ADI_SUPPORT )
#define ABCC_PORT_Copy64( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset )   \
        ABCC_PORT_CopyOctets( pxDest, iDestOctetOffset, pxSrc, iSrcOctetOffset, 8 )
#endif

#endif  /* inclusion lock */
