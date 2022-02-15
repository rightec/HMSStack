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
********************************************************************************
********************************************************************************
*/

#include "abcc_sw_port.h"
#include "process.h"
#include "windows.h"

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
** Private Globals
********************************************************************************
*/

static HANDLE ghMutex = NULL;

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/
#ifdef ABCC_SYS_16_BIT_CHAR
void ABCC_PORT_CopyOctetsImpl( void* pxDest, UINT16 iDestOctetOffset,
                               void* pxSrc, UINT16 iSrcOctetOffset,
                               UINT16 iNumOctets )
{
   UINT16 i;
   UINT16 iData;
   BOOL fOddDestOctet;
   BOOL fOddSrcOctet;
   UINT16* piDest;
   UINT16* piSrc;

   fOddDestOctet = iDestOctetOffset & 1;
   fOddSrcOctet =  iSrcOctetOffset & 1;
   piDest =   (UINT16*)pxDest + ( iDestOctetOffset >> 1 );
   piSrc =    (UINT16*)pxSrc + ( iSrcOctetOffset >> 1 );

   for( i = 0; i < iNumOctets; i++ )
   {
      if( fOddSrcOctet )
      {
         iData = ABCC_GetHighAddrOct( *piSrc );
         piSrc++;
      }
      else
      {
         iData = ABCC_GetLowAddrOct( *piSrc );
      }
      fOddSrcOctet ^= 1;

      if( fOddDestOctet )
      {
         ABCC_SetHighAddrOct( *piDest, iData );
         piDest++;
      }
      else
      {
         ABCC_SetLowAddrOct( *piDest, iData );
      }
      fOddDestOctet ^= 1;
   }
}

void ABCC_PORT_StrCpyToNativeImpl( void* pxDest,  void* pxSrc,
                                   UINT16 iSrcOctetOffset, UINT16 iNbrOfChars )
{
   UINT16 i;
   UINT16* piDest;
   const UINT16* piSrc;
   BOOL fOddSrc;

   piDest = pxDest;
   piSrc =  pxSrc;
   fOddSrc = iSrcOctetOffset & 1;

   for( i = 0; i < iNbrOfChars; i++ )
   {
      if ( fOddSrc )
      {
         piDest[ i ] = ABCC_GetHighAddrOct( piSrc[ i >> 1 ] );
      }
      else
      {
         piDest[ i ] = ABCC_GetLowAddrOct( piSrc[ i >> 1 ] );
      }
      fOddSrc ^= 1;
   }
}

void ABCC_PORT_StrCpyToPackedImpl( void* pxDest, UINT16 iDestOctetOffset,
                                   const void* pxSrc, UINT16 iNbrOfChars )
{
   UINT16 i;
   UINT16* piDest;
   const UINT16* piSrc;
   BOOL fOddDest;

   piDest = pxDest;
   piSrc =  pxSrc;
   fOddDest = iDestOctetOffset & 1;

   for( i = 0; i < iNbrOfChars; i++ )
   {
      if ( fOddDest )
      {
         ABCC_SetHighAddrOct( piDest[ i >> 1 ], piSrc[ i ] );
      }
      else
      {
         ABCC_SetLowAddrOct( piDest[ i >> 1 ], piSrc[ i ] );
      }
      fOddDest ^= 1;
   }
}
#endif

void ABCC_PORT_UseCriticalImpl( void )
{
   if( ghMutex == NULL )
   {
      ghMutex = CreateMutex( NULL,       /* Default security attributes.  */
                          FALSE,      /* Initially not owned.          */
                          NULL );
   }
}

void ABCC_PORT_EnterCriticalImpl( void )
{
   WaitForSingleObject( 
            ghMutex,    /* Handle to the mutex. */
            INFINITE);  /* No time-out interval. */
}

void ABCC_PORT_ExitCriticalImpl( void )
{
   ReleaseMutex(ghMutex); 
}
