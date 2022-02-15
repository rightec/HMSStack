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
** COPYRIGHT NOTIFICATION (c) 2013 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, this code can be   **
** modified, reproduced and distributed in binary form without any            **
** restrictions.                                                              **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
*/
#include "windows.h"
#include "process.h"
#include "tp.h"
#include "imp_tp.h"

#include "abcc_drv_cfg.h"
#include "abcc_port.h"
#include "abcc.h"

#include "abcc_sys_adapt.h"
#include "abcc_sys_adapt_spi.h"
#include "abcc_sys_adapt_par.h"
#include "abcc_sys_adapt_ser.h"


BOOL ABCC_StartTransportProvider( void );
void ABCC_CloseTransportProvider( void );

EXTFUNC void ( *ABCC_ISR )( void );
unsigned __stdcall ISR( void *pMyID );

HANDLE hThread;
unsigned threadID;
unsigned runISR = 0;

#define TP_USB2_SPECIFIC_CMD_GET_PORT_C ( 0x04 )
#define USB2_PORT_C_MI_MASK 0x03
#define USB2_PORT_C_MD_MASK 0x0C

#define TP_USB2_SPECIFIC_CMD_GET_PORT_E ( 0x06 )
#define USB2_PORT_E_IRQ 0x01


/* The ACI external memory map is 16 kB. */
#define ACI_MEMORY_MAP_SIZE 16384

static ABCC_SYS_SpiDataReceivedCbfType pnDataReadyCbf;
static ABCC_SYS_SerDataReceivedCbfType pnSerDataReadyCbf;

static UINT8    sys_abReadProcessData[ ABCC_CFG_MAX_PROCESS_DATA_SIZE ];  /* Process data byte array. */
static UINT8    sys_abWriteProcessData[ ABCC_CFG_MAX_PROCESS_DATA_SIZE ]; /* Process data byte array. */

static    TP_Path xPathHandle = NULL;
static    UINT32 lPathId = 0;

static   UINT8 sys_bOpmode = 0;
static    TP_InterfaceType eInterface = TP_ANY;


/* Global to ease debugging: This is the last received status code from the TP. */
static    TP_StatusType eLastReceivedTpPariStatus;

void TP_Shutdown( void )
{
   ABCC_CloseTransportProvider();
}

/*
** Explicitly set which TP Path ID to use. It is optional to call this
** function. lPathId is initialised to '0' which means "let the user select
** the path ID manually at startup".
*/
void TP_vSetPathId( UINT32 lValue )
{
   lPathId = lValue;
   return;
}

static UINT8 TP_Command( UINT8 bCommand )
{
   TP_StatusType eStatus;
   TP_MessageType  sMsg;
   sMsg.sReq.eCommand = TP_CMD_USB2_SPECIFIC;
   sMsg.sReq.bDataSize = 1;
   sMsg.sReq.abData[0] = bCommand;

   ABCC_PORT_EnterCritical();
   eStatus = TP_ProviderSpecificCommand( xPathHandle, &sMsg );
   ABCC_PORT_ExitCritical();

   if ( eStatus != TP_ERR_NONE )
   {
      ABCC_DEBUG_ERR( ( "Transport provider error %d\n", eStatus ) );
      ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_SYS_ADAPTION_ERR, (UINT32)eStatus );
   }
   return( sMsg.sRsp.abData[0] );
}


#if( ABCC_CFG_INT_ENABLED )
unsigned __stdcall ISR( void *pMyID )
{
   UINT8 bTpPortE;
   (void) pMyID;

   while( runISR )
   {
      Sleep(1);
      bTpPortE = TP_Command( TP_USB2_SPECIFIC_CMD_GET_PORT_E );
      if ( ( bTpPortE & USB2_PORT_E_IRQ ) != USB2_PORT_E_IRQ )
      {
         ABCC_PORT_EnterCritical();
         ABCC_ISR();
         ABCC_PORT_ExitCritical();
      }
   }
   _endthreadex( 0 );
   return( 0 );
}
#endif


BOOL ABCC_SYS_HwInit( void )
{
   if( ABCC_StartTransportProvider() )
   {
      ABCC_SYS_HWReset();
      return( TRUE );
   }
   return( FALSE );
}

BOOL ABCC_SYS_Init( void )
{
   return( TRUE );
}

void ABCC_SYS_Close( void )
{
}

#ifdef ABCC_SYS_ADAPT_SPI_
void ABCC_SYS_SpiRegDataReceived( ABCC_SYS_SpiDataReceivedCbfType pnDataReceived  )
{
   pnDataReadyCbf = pnDataReceived;
}

void ABCC_SYS_SpiSendReceive( void* pxSendDataBuffer, void* pxReceiveDataBuffer, UINT16 iLength )
{
   TP_StatusType eStatus;
   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   eStatus = TP_SpiTransaction( xPathHandle, pxSendDataBuffer, pxReceiveDataBuffer, iLength );

   ABCC_PORT_ExitCritical();

   if (eStatus == TP_ERR_NONE )
   {
      pnDataReadyCbf();
   }
   else
   {
      ABCC_DEBUG_ERR( ( "ERROR in SPI transaction: ERR: 0x%x\n", eStatus ) );
      ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_SYS_ADAPTION_ERR, (UINT32)eStatus );
   }
}
#endif


#if( ( ABCC_CFG_DRV_PARALLEL || ABCC_CFG_DRV_PARALLEL_30 ) && !ABCC_CFG_MEMORY_MAPPED_ACCESS )
void ABCC_SYS_ParallelRead( UINT16 iMemOffset, void* pxData, UINT16 iLength )
{
   if( iLength == 0 )
   {
      return;
   }

   ABCC_PORT_EnterCritical();
   eLastReceivedTpPariStatus = TP_ParallelRead( xPathHandle, iMemOffset, pxData, iLength );
   ABCC_PORT_ExitCritical();
   if( eLastReceivedTpPariStatus == TP_ERR_NONE )
   {
   }
   else
   {
      ABCC_DEBUG_ERR( ( "Read failed with error code %x\n", eLastReceivedTpPariStatus ) );
      ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_SYS_ADAPTION_ERR, (UINT32)eLastReceivedTpPariStatus );
   }
}

#if( ABCC_CFG_DRV_PARALLEL_30 )
UINT8 ABCC_SYS_ParallelRead8( UINT16 iMemOffset )
{
   UINT8 bData;
   ABCC_SYS_ParallelRead( iMemOffset, &bData, sizeof( UINT8 ) );
   return( bData );
}
#endif

#if( ABCC_CFG_DRV_PARALLEL )
UINT16 ABCC_SYS_ParallelRead16( UINT16 iMemOffset )
{
   UINT16 iData;
   ABCC_SYS_ParallelRead( iMemOffset, (UINT8*)&iData, sizeof( UINT16 ) );
   return( iData );
}
#endif


void ABCC_SYS_ParallelWrite( UINT16 iMemOffset, void* pxData, UINT16 iLength )
{

   ABCC_PORT_EnterCritical();
   eLastReceivedTpPariStatus = TP_ParallelWrite( xPathHandle, iMemOffset, pxData, iLength );
   ABCC_PORT_ExitCritical();

   if( eLastReceivedTpPariStatus != TP_ERR_NONE )
   {
      ABCC_DEBUG_ERR( ( "Write failed with error code %x\n", eLastReceivedTpPariStatus ) );
      ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_SYS_ADAPTION_ERR, (UINT32)eLastReceivedTpPariStatus );
   }
}

#if( ABCC_CFG_DRV_PARALLEL_30 )
void ABCC_SYS_ParallelWrite8( UINT16 iMemOffset, UINT8 pbData )
{
   ABCC_SYS_ParallelWrite( iMemOffset, (UINT8*)&pbData, sizeof( UINT8 ) );
}
#endif

#if( ABCC_CFG_DRV_PARALLEL )
void ABCC_SYS_ParallelWrite16( UINT16 iMemOffset, UINT16 piData )
{
   ABCC_SYS_ParallelWrite( iMemOffset, (UINT8*)&piData, sizeof( UINT16 ) );
}
#endif
#endif

#if( ABCC_CFG_OP_MODE_SETTABLE )
void ABCC_SYS_SetOpmode( UINT8 bOpmode )
{
   /*
   ** This is done already in ABCC_SYS_init. Otherwise we cannot read the MD or MI
   ** values from the USB2 board.
   */
   (void)bOpmode;
}
#endif


#if( ABCC_CFG_OP_MODE_GETTABLE )
UINT8 ABCC_SYS_GetOpmode( void )
{
   return( sys_bOpmode );
}
#endif


void ABCC_SYS_HWReset( void )
{
   TP_StatusType eStatus;
   TP_MessageType  sMsg;

   sMsg.sReq.eCommand = TP_CMD_RESET;
   sMsg.sReq.bDataSize = 1;
   sMsg.sReq.abData[0] = 0;

   ABCC_PORT_EnterCritical();
   eStatus = TP_ProviderSpecificCommand( xPathHandle, &sMsg );
   ABCC_PORT_ExitCritical();
}


void ABCC_SYS_HWReleaseReset( void )
{
   TP_StatusType eStatus;
   TP_MessageType  sMsg;
   sMsg.sReq.eCommand = TP_CMD_RESET;
   sMsg.sReq.bDataSize = 1;
   sMsg.sReq.abData[0] = 1;

   ABCC_PORT_EnterCritical();
   eStatus = TP_ProviderSpecificCommand( xPathHandle, &sMsg );
   ABCC_PORT_ExitCritical();

}


#ifndef ABCC_CFG_ABCC_MODULE_ID
UINT8 ABCC_SYS_ReadModuleId( void )
{
   UINT8 bTpPortC;
   bTpPortC = TP_Command( TP_USB2_SPECIFIC_CMD_GET_PORT_C );
   return( bTpPortC & USB2_PORT_C_MI_MASK );
}
#endif


#if( ABCC_CFG_MOD_DETECT_PINS_CONN )
BOOL ABCC_SYS_ModuleDetect( void )
{
   UINT8 bTpPortC;
   bTpPortC = TP_Command( TP_USB2_SPECIFIC_CMD_GET_PORT_C );

   return( ( bTpPortC & USB2_PORT_C_MD_MASK ) == 0 );
}
#endif

#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
void* ABCC_SYS_ParallelGetRdPdBuffer( void )
{
   return( sys_abReadProcessData );
}


void* ABCC_SYS_ParallelGetWrPdBuffer( void )
{
   return( sys_abWriteProcessData );
}
#endif

#ifdef ABCC_CFG_DRV_SERIAL
void ABCC_SYS_SerRegDataReceived( ABCC_SYS_SerDataReceivedCbfType pnDataReceived  )
{
   pnSerDataReadyCbf = pnDataReceived;
}


void ABCC_SYS_SerSendReceive( void* pxTxDataBuffer, void* pxRxDataBuffer, UINT16 iTxSize, UINT16 iRxSize )
{

   TP_StatusType  eStatus;
   UINT16         iRdOffset;
   UINT16         iTemp;

   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   eStatus = TP_SerialWriteBlocking( xPathHandle, pxTxDataBuffer, iTxSize );
   ABCC_PORT_ExitCritical();

   if (eStatus != TP_ERR_NONE )
   {
      ABCC_DEBUG_ERR( ( "Serial TX error: ERR: 0x%x", eStatus ) );
      ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_SYS_ADAPTION_ERR, (UINT32)eStatus );
      return;
   }

   iRdOffset = 0;
   ABCC_PORT_EnterCritical();
   while( iRdOffset < iRxSize )
   {
      iTemp = iRxSize - iRdOffset;
      /*
      ** The timeout is only used in the RX direction, so the longest
      ** listed "Tsend" (175ms) plus some arbitrary margin ought to be
      ** enough for the TP_SerialRead() call.
      */
      eStatus = TP_SerialRead( xPathHandle, (UINT8*)pxRxDataBuffer + iRdOffset, &iTemp, 200 );
      if( eStatus == TP_ERR_NONE )
      {
         if( iTemp > 0 )
         {
            iRdOffset += iTemp;
         }
         else
         {
            ABCC_DEBUG_ERR( ( "Serial RX timeout!\n" ) );
            break;
         }
      }
      else
      {
         break;
      }
   }
   ABCC_PORT_ExitCritical();

   if ( eStatus != TP_ERR_NONE )
   {
      ABCC_DEBUG_ERR( ( "Serial RX error: ERR: 0x%x\n", eStatus ) );
      ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_SYS_ADAPTION_ERR, (UINT32)eStatus );
      return;
   }

   pnSerDataReadyCbf();

}


void ABCC_SYS_SerRestart( void )
{
   UINT8          bTemp;
   TP_StatusType  eStatus;
   UINT16         iSize;

   /*
   ** Empty the RX buffer in the TP system.
   */
   do
   {
      iSize = 1;
      eStatus = TP_SerialRead( xPathHandle, &bTemp, &iSize, 0 );
   }
   while( ( eStatus == TP_ERR_NONE ) && ( iSize > 0 ) );
}
#endif

#if( ABCC_CFG_INT_ENABLED )
void ABCC_SYS_AbccInterruptEnable( void )
{
   runISR  = TRUE;
   hThread = (HANDLE)_beginthreadex( NULL, 0, &ISR, NULL, 0, &threadID );
}
#endif


#if( ABCC_CFG_INT_ENABLED )
void ABCC_SYS_AbccInterruptDisable( void )
{
   if(  runISR )
   {
      runISR  = FALSE;
      WaitForSingleObject( hThread, INFINITE );
      CloseHandle( hThread );
   }
}
#endif

#if( ABCC_CFG_POLL_ABCC_IRQ_PIN )
BOOL ABCC_SYS_IsAbccInterruptActive( void )
{
   UINT8 bTpPortE;
   BOOL fIrq;

   fIrq = FALSE;
   bTpPortE = TP_Command( TP_USB2_SPECIFIC_CMD_GET_PORT_E );
   if( ( bTpPortE & USB2_PORT_E_IRQ ) != USB2_PORT_E_IRQ )
   {
      fIrq = TRUE;
   }

   return( fIrq );
}
#endif


/*
 ** This function will start the transport provider connection
 ** Note! This function is called by the application before
 ** the driver is accessed.
 */
BOOL ABCC_StartTransportProvider( void )
{
   TP_StatusType eStatus;

   if ( xPathHandle !=  NULL )
   {
      return( TRUE );
   }

   eStatus = TP_Initialise( "HMSTPRTR.DLL", 0x200 );

   if ( eStatus != TP_ERR_NONE )
   {
      ABCC_DEBUG_ERR( (" TP_Init failed: %d\n", eStatus ) );
      return( FALSE );
   }

   if( lPathId == 0 )
   {
      /*
      ** lPathId == 0 -> no path has been set, let the user select one
      ** manually.
      */
      eStatus = TP_UserSelectPath( &eInterface, &lPathId, &xPathHandle );
   }
   else
   {
      /*
      ** lPathId != 0 -> a Path ID has been set explicitly by someone.
      ** Use that.
      */
      eStatus = TP_SelectPath( &eInterface, lPathId, &xPathHandle );
   }

   if( eStatus != TP_ERR_NONE )
   {
      ABCC_DEBUG_ERR( (" TP_UserSelectPath: %d\n", eStatus ) );
      return( FALSE );
   }


   switch( eInterface )
   {
   case TP_SPI:

      eStatus = TP_SpiOpen(xPathHandle, 12000000, TP_SPI_4WIRE );

      if( eStatus != TP_ERR_NONE )
      {
         ABCC_DEBUG_ERR( (" TP_SpiOpen: %d\n", eStatus ) );
         return( FALSE );
      }
      sys_bOpmode = ABP_OP_MODE_SPI;

      break;

   case TP_PARALLEL:

      eStatus = TP_ParallelOpen( xPathHandle, ACI_MEMORY_MAP_SIZE );

      if( eStatus != TP_ERR_NONE )
      {
         ABCC_DEBUG_ERR( (" TP_ParallelOpen: %d\n", eStatus ) );
         return( FALSE );
      }



      if( ( TP_Command( 0x17 ) & 0x03 ) == 0x01 )
      {
         sys_bOpmode = ABP_OP_MODE_16_BIT_PARALLEL;
      }
      else
      {
         sys_bOpmode = ABP_OP_MODE_8_BIT_PARALLEL;
      }

      break;

     case TP_SERIAL:

       eStatus = TP_SerialOpen( xPathHandle, 57600, 8, TP_PARITY_NONE, TP_STOPBIT_ONE );

       if( eStatus != TP_ERR_NONE )
       {
          ABCC_DEBUG_ERR( (" TP_SerialOpen: %d\n", eStatus ) );
          return( FALSE );
       }
        sys_bOpmode = ABP_OP_MODE_SERIAL_57_6;
       break;

   default:

      ABCC_DEBUG_ERR( (" TP_ParallelOpen: %d\n", eStatus ) );
      sys_bOpmode = 0;
      return( FALSE );
      break;
   }

   ABCC_SYS_HWReset();

   return( TRUE );
}


/*
 ** This function will close the transport provider connection.
 ** This function is called by the application at system shutdown.
 ** to release tranport provider recources
 */
void ABCC_CloseTransportProvider( void )
{
   if ( xPathHandle )
   {
      switch( eInterface )
      {
      case TP_SPI:
         TP_SpiClose( xPathHandle );
         break;

      case TP_PARALLEL:
         TP_ParallelClose( xPathHandle );
         break;
      case TP_SERIAL:
         TP_SerialClose( xPathHandle );

      default:
         /* ERROR: Unexpected interface. Throw an exception? */
         break;
      }
   }

   TP_Close();
   xPathHandle = NULL;
}



/*******************************************************************************
** End of abcc_sys_adapt.c
********************************************************************************
*/
