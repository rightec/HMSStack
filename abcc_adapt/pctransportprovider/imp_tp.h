/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.07.02 (2020-12-09)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.76.01 (2020-10-19)                                     **
**    ABCC Driver    5.07.01 (2020-10-12)                                     **
**                                                                            */
//******************************************************************************
// COPYRIGHT NOTIFICATION (c) 2013 HMS Industrial Networks AB
//
// This program is the property of HMS Industrial Networks AB.
// It may not be reproduced, distributed, or used without permission
// of an authorised company official.
//******************************************************************************

#ifndef IMP_TP_H
#define IMP_TP_H


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#ifndef TP_H
#error Please include TP.H prior to imp_tp.h
#endif


// TP_Initialise()
//
// This function loads the indicated DLL and sets up the function pointers.
// It should be called prior to calling any other function mentioned in this DLL.
// Before exiting the program, the DLL should be unloaded by a call to TP_Close().
//
// NOTE: The standard windows search order applies when giving the DLL-argument.
//
// Inputs:
//    aDllFileName - Null terminated string identifying the DLL.
//    aVersion     - ( majorVersion << 8 ) | minorVersion.
//                TP_DLL_VERSION can be used if the latest version is wanted.
//                0x0100 is the first version.
//                0x0102 includes GetProviderHandleAndPath()
//                0x0200 includes all SPI functionality, TP_UserSelectPathExt() and TP_PathNameW()
//                0x0201 added support for TP_GetSupportedBaudRates()
//                0x0202 added support for TP_SerialReopen()
//
// Usage:
//    stat = TP_Initialise( "HMSTPRTR.DLL", TP_DLL_VERSION );
extern TP_StatusType WINAPI TP_Initialise( const char* aDllFileName, const UINT16 aVersion );


// TP_Close()
//
// Closes the dll which was previously loaded by means of TP_Initialise().
//
// NOTE: Any call to the function pointers after the dll has been closed results in a memory exception error.
//
// Usage:
//    stat = TP_Close();
extern TP_StatusType WINAPI TP_Close( void );


// TP_UserSelectPath()
//
// Will cause the Transport Router to display a form prompting the user for the transport path to use.
// The user will also be able to configure and create new paths from this form.
//
// Inputs:
//    anInterface  - Pointer to enum of TP_InterfaceType which determines what
//                   type of interface the user should be able to select from.
//                   If the enum is set to ANY upon calling the function, it
//                   will contain the interface selected by the user if the
//                   function returns succesfully
//    aReturnPathId - Pointer to variable which will contain the PathId the
//                   user selected if the function returns successfully
//    aReturnPath  - Pointer to path identifier pointer
//
// Usage:
//    TP_InterfaceType tpInterface = TP_SERIAL; // want to select a serial path
//    stat = TP_UserSelectPath( &tpInterface, &pathId, &path );
typedef TP_StatusType ( WINAPI * TP_UserSelectPathType )( TP_InterfaceType* anInterface, UINT32* aReturnPathId, TP_Path* aReturnPath );
extern TP_UserSelectPathType TP_UserSelectPath;


// TP_UserSelectPathExt()
//
// same as TP_UserSelectPath() except:
// Inputs:
//    aLabel - a text caption that overrides the default "Transport Paths" text shown when selecting a path
//
// Usage:
//    TP_InterfaceType tpInterface = TP_SERIAL; // want to select a serial path
//    stat = TP_UserSelectPathExt( &tpInterface, &pathId, &path, "Pick path" );
typedef TP_StatusType ( WINAPI * TP_UserSelectPathExtType )( TP_InterfaceType* anInterface, UINT32* aReturnPathId, TP_Path* aReturnPath, const char* aLabel );
TP_StatusType WINAPI TP_UserSelectPathExt( TP_InterfaceType* anInterface, UINT32* aReturnPathId, TP_Path* aReturnPath, const char* aLabel );


// TP_SelectPath()
//
// This function could be called by the application when it knows of a PathId it would like to use.
// It will cause the Transport Router to load the associated dll and create and configure the path.
//
// Inputs:
//    anInterface - Identifies the type of path which was opened
//    aPathId     - Identifier of the path to use
//    aReturnPath - Pointer to path identifier pointer
//
// Usage:
//    stat = TP_SelectPath( &tpInterface, pathId, &path );
typedef TP_StatusType ( WINAPI * TP_SelectPathType )( TP_InterfaceType* anInterface, UINT32 aPathId, TP_Path* aReturnPath );
extern TP_SelectPathType TP_SelectPath;


// TP_DestroyPath()
//
// Frees previously allocated memory used to identify a transport path
// If the path is open, it should be closed prior to destroying it.
// Should be called after the path has been closed and is not intended to be reopened.
//
// NOTE: Should be supported by all transport providers
//
// Inputs:
//    aPath      - Path identifier pointer
//
// Usage:
//    stat = TP_DestroyPath( path );
typedef TP_StatusType ( WINAPI * TP_DestroyPathType )( TP_Path aPath );
extern TP_DestroyPathType TP_DestroyPath;


// TP_PathName()
//
// Returns a pointer to the name of the path supplied by the user
//
// Inputs:
//    aPath              - The path the user whishes to extract the name from
//    aReturnPathNamePtr - Pointer to char pointer which will be set to an array containing the name of the path
//
// Usage:
//    const char* pathName;
//    stat = TP_PathName( path, &pathName );
typedef TP_StatusType ( WINAPI * TP_PathNameType )( TP_Path aPath, const char** aReturnPathNamePtr );
extern TP_PathNameType TP_PathName;


// TP_PathNameW()
//
// Unicode version of TP_PathName()
//
// NOTE: only available when TP_DLL_VERSION >= 0x0201
//
// Usage:
//    const wchar_t* pathName;
//    stat = TP_PathNameW( path, &pathName );
typedef TP_StatusType ( WINAPI * TP_PathNameWType )( TP_Path aPath, const wchar_t** aReturnPathNamePtr );
extern TP_PathNameWType TP_PathNameW;


// TP_GetSupportedBaudRates()
//
// Used to get a list of supported baud rates for the serial or SPI path
//
// NOTE: only available when TP_DLL_VERSION >= 0x0201
//
// Inputs:
//    aPath                - Path identifier pointer
//    aReturnBaudRateList  - the list is filled with baud rates that are guaranteed to work with TP_SerialOpen() or TP_SpiOpen()
//    aBaudRateListLength  - on entry contains the maximum size of the list and on exit it contains the actual number of entries in the list
//
// Usage:
//    UINT32 baudRates[ 16 ];
//    UINT32 numBaudRates = sizeof( baudRates ) / sizeof( baudRates[ 0 ] );
//    stat = TP_GetSupportedBaudRates( path, baudRates, &numBaudRates );
typedef TP_StatusType ( WINAPI * TP_GetSupportedBaudRatesType )( TP_Path aPath, UINT32* aReturnBaudRateList, UINT32* aBaudRateListLength );
extern TP_GetSupportedBaudRatesType TP_GetSupportedBaudRates;


// TP_GetProviderHandleAndPath()
//
// Attempts to retrieve the handle to an opened transport provider dll used for the indicated path,
// and also the path pointer to use when communicating with the provider directly.
//
// Inputs:
//    aRouterPath       - Path identifier pointer to the ROUTER path
//    aReturnProvHandle - Pointer to handle of provider dll.
//    aReturnProvPath   - Pointer to provider path pointer.
//
// Usage:
//    stat = TP_GetProviderHandleAndPath( path, &provHand, &provPath );
typedef TP_StatusType ( WINAPI * TP_GetProviderHandleAndPathType )( TP_Path aRouterPath, HANDLE* aReturnProvHandle, TP_Path* aReturnProvPath );
extern TP_GetProviderHandleAndPathType TP_GetProviderHandleAndPath;


// TP_ProviderSpecificCommand()
//
// By a message based structure it should be possible to send requests and receive responses through this function.
// Used for all the stuff which may not be generic to all transport providers and as such are not implemented as standard functions.
// Also a last resort in case new things need to be added to the transport provider specification.
// Eventhough the name implies that the function takes care of Provider specific commands,
// a list of all the commands should be kept and two providers are not to use the same function code for different functionality.
// The end-user does not know which provider is used, remember?
//
// Therefore functions like Reset which may not be available on all transport providers shall be implemented as a provider specific command.
// BUT, it shall be the same provider specific command on all providers.
// and the ones that does not support the intended functionality responds with unknown command or something similar.
//
// NOTE: This function should be supported by all transport providers
//
// Inputs:
//    aPath      - Path identifier pointer
//    aMessage   - Pointer to a message buffer which contains the request
//                 and upon returning from the function contains the response
//
// Usage:
//    stat = TP_ProviderSpecificCommand( path, &msg );
typedef TP_StatusType ( WINAPI * TP_ProviderSpecificCommandType )( TP_Path aPath, TP_MessageType* aMessage );
extern TP_ProviderSpecificCommandType TP_ProviderSpecificCommand;


// TP_ParallelOpen()
//
// Opens a parallel path to a aSize big memory-window according to the configured path settings.
//
// NOTE: Should be supported by all parallel transport providers
//
// Inputs:
//    aPath       - Path identifier pointer
//    aSize       - Size of memory window
//
// Usage:
//    stat = TP_ParallelOpen( path, 4096 );
typedef TP_StatusType ( WINAPI * TP_ParallelOpenType )( TP_Path aPath, UINT16 aSize );
extern TP_ParallelOpenType TP_ParallelOpen;


// TP_ParallelClose()
//
// Closes the path to a previously opened memory-window so that the memory is made available to other paths/applications
// Please note that this does NOT destroy the path. It should be possible to open the memory-window again by calling ParallelOpen()
//
// NOTE: Should be supported by all parallel transport providers
//
// Inputs:
//    aPath       - Path identifier pointer which has an open memory-window
//
// Usage:
//    stat = TP_ParallelClose( path );
typedef TP_StatusType ( WINAPI * TP_ParallelCloseType )( TP_Path aPath );
extern TP_ParallelCloseType TP_ParallelClose;


// TP_ParallelRead()
//
// Reads anAmount of bytes at anOffset from a previously opened memory-window of the indicated path
//
// NOTE: Should be supported by all parallel transport providers
//
// Inputs:
//    aPath       - Path identifier pointer which has an open memory-window
//    anOffset    - Memory offset to start reading from
//    someData    - Pointer where the read data should be stored
//    anAmount    - The amount of data to be read
//
// Usage:
//    stat = TP_ParallelRead( path, 0, buf, 256 );
typedef TP_StatusType ( WINAPI * TP_ParallelReadType )( TP_Path aPath, UINT16 anOffset, UINT8* someData, UINT16 anAmount );
extern TP_ParallelReadType TP_ParallelRead;


// TP_ParallelVerifyRead()
//
// Reads anAmount of bytes at anOffset from a previously opened memory-window of the indicated path.
// Reads every byte until a matching value has been read on a per byte basis OR until
// the aMaxTries attempts to verify the read value has been performed and the function fails.
// Please note that if the function fails for a byte, it should return immediately without attempting to read the rest of the buffer.
// Useful for handshaking registers etc.
//
// NOTE: Should be supported by all parallel transport providers
//
// Inputs:
//    aPath      - Path identifier pointer which has an open memory-window
//    anOffset   - Memory offset to start reading from
//    someData   - Pointer where the read data should be stored
//    anAmount   - The amount of data to be read
//    aMaxTries  - Maximum times to try and re-read per byte
//
// Usage:
//    stat = TP_ParallelVerifyRead( path, 0, buf, 256, 3 );
typedef TP_StatusType ( WINAPI * TP_ParallelVerifyReadType )( TP_Path aPath, UINT16 anOffset, UINT8* someData, UINT16 anAmount, UINT16 aNbrMaxTries );
extern TP_ParallelVerifyReadType TP_ParallelVerifyRead;


// TP_ParallelWrite()
//
// Writes anAmount of bytes at anOffset to a previously opened memory-window of the indicated path
//
// NOTE: Should be supported by all parallel transport providers
//
// Inputs:
//    aPath      - Path identifier pointer which has an open memory-window
//    anOffset   - Memory offset to start writing to
//    someData   - Pointer to the data to be written
//    anAmount   - The amount of data to write
//
// Usage:
//    stat = TP_ParallelWrite( path, 0, buf, 256 );
typedef TP_StatusType ( WINAPI * TP_ParallelWriteType )( TP_Path aPath, UINT16 anOffset, const UINT8* someData, UINT16 anAmount );
extern TP_ParallelWriteType TP_ParallelWrite;


// TP_ParallelVerifyWrite()
//
// Writes anAmount of bytes at anOffset to a previously opened memory-window of the indicated path.
// Writes every byte until it can be verified that the value was written on a per byte basis OR until
// the aMaxTries attempts to verify the written value has been performed and the function fails.
// Please note that if the function fails for a byte, it should return immediately without attempting to write the rest of the buffer.
// Useful for handshaking registers etc.
//
// NOTE: Should be supported by all parallel transport providers
//
// Inputs:
//    aPath      - Path identifier pointer which has an open memory-window
//    anOffset   - Memory offset to start writing to
//    someData   - Pointer to the data to be written
//    anAmount   - The amount of data to write
//    aMaxTries  - Maximum times to try and write the byte
//
// Usage:
//    stat = TP_ParallelVerifyWrite( path, 0, buf, 128, 3 );
typedef TP_StatusType ( WINAPI * TP_ParallelVerifyWriteType )( TP_Path aPath, UINT16 anOffset, const UINT8* someData, UINT16 anAmount, UINT16 aNbrMaxTries );
extern TP_ParallelVerifyWriteType TP_ParallelVerifyWrite;


// TP_SerialOpen()
//
// Tries to open a serial port on the specified path with the indicated serial settings.
//
// NOTE: Should be supported by all serial transport providers
//
// Inputs:
//    aPath      - Path identifier pointer
//    aBaudRate  - The desired baud rate
//    aDataBits  - Amount of data bits
//    aParity    - The desired parity
//    aStopBits  - The amount of stop bits to use
//
// Usage:
//    stat = TP_SerialOpen( path, 57600, 8, TP_PARITY_NONE, TP_STOPBIT_ONE );
typedef TP_StatusType ( WINAPI * TP_SerialOpenType )( TP_Path aPath, UINT32 aBaudRate, UINT8 aDataBits, TP_SerialParityType aParity, TP_SerialStopBitType aStopBits );
extern TP_SerialOpenType TP_SerialOpen;


// TP_SerialClose()
//
// Closes the access to a previously opened serial channel so that the channel is made available to other paths/applications.
// Please note that this does NOT destroy the path. It should be possible to open the channel again by calling SerialOpen()
//
// NOTE: Should be supported by all serial transport providers
//
// Inputs:
//    aPath      - Path identifier pointer
//
// Usage:
//    stat = TP_SerialClose( path );
typedef TP_StatusType ( WINAPI * TP_SerialCloseType )( TP_Path aPath );
extern TP_SerialCloseType TP_SerialClose;


// TP_SerialReopen()
//
// Changes the serial settings on an already open serial path
//
// NOTE: only available when TP_DLL_VERSION >= 0x0202
//
// Inputs:
//    same as for TP_SerialOpen
//
// Usage:
//    stat = TP_SerialReopen( path, 115200, 8, TP_PARITY_NONE, TP_STOPBIT_ONE );
extern TP_SerialOpenType TP_SerialReopen;


// TP_SerialGetInAmount()
//
// Sets the amount of available bytes in the serial receive buffer of an open serial channel to a user specified variable.
//
// NOTE: Should be supported by all serial transport providers
//
// Inputs:
//    aPath         - Path identifier pointer
//    aReturnAmount - Pointer to variable to hold the Amount
//
// Usage:
//    stat = TP_SerialGetInAmount( path, &numBytes );
typedef TP_StatusType ( WINAPI * TP_SerialGetInAmountType )( TP_Path aPath, UINT16* aReturnAmount );
extern TP_SerialGetInAmountType TP_SerialGetInAmount;


// TP_SerialGetOutAmount()
//
// Sets the amount of bytes left to transfer in the serial transmit buffer of an open serial channel to a user specified variable.
//
// NOTE: Should be supported by all serial transport providers
//
// Inputs:
//    aPath         - Path identifier pointer
//    aReturnAmount - Pointer to variable to hold the Amount
//
// Usage:
//    stat = SerialGetOutAmount( path, &numBytes );
typedef TP_StatusType ( WINAPI * TP_SerialGetOutAmountType )( TP_Path aPath, UINT16* aReturnAmount );
extern TP_SerialGetOutAmountType TP_SerialGetOutAmount;


// TP_SerialRead()
//
// Tries to read anAmount of bytes from an open serial port on the specified path.
// Returns the amount of read bytes in the same variable.
// The function will be blocking until either the desired amount of characters has been read OR the specified max allowed time to wait has passed.
// Please note that the transport provider may in some cases HAVE to wait longer than the specified maximum allowed wait time due to hardware drivers etc.
// In general if the time has not yet run out, another look for newly received characters is allowed.
//
// NOTE: Should be supported by all serial transport providers
//
// Inputs:
//    aPath       - Path identifier pointer
//    someData    - Pointer to buffer where the read data is to be stored
//    anAmount    - Pointer to variable which upon entering the function
//                   contains the desired amount of bytes to be read, and upon
//                   exit is updated by the function with the amount actually read
//    aMaxWaitTime - Specifies the desired maximum time to wait (in ms)
//                   If set to zero, the function returns as fast as possible
//
// Usage:
//    stat = TP_SerialRead( path, dat, &numBytes, 100 );
typedef TP_StatusType ( WINAPI * TP_SerialReadType )( TP_Path aPath, UINT8* someData, UINT16* anAmount, UINT16 aMaxWaitTime );
extern TP_SerialReadType TP_SerialRead;


// TP_SerialReadBlocking()
//
// Works the same way as TP_SerialRead(), but will always block until all data has been read (or an error occurs)
TP_StatusType TP_SerialReadBlocking( TP_Path aPath, UINT8* someData, UINT16 anAmount );


// TP_SerialWrite()
//
// Tries to write anAmount of bytes to an open serial port on the specified path.
// Returns the amount of written (or buffered) bytes in the same variable.
// The function will be blocking until either the desired amount of characters has been read OR the specified max allowed time to wait has passed.
// Please note that the transport provider may in some cases HAVE to wait longer than the specified maximum allowed wait time due to hardware drivers etc.
// In general if the time has not yet run out, another attempt to write characters are made.
//
// NOTE: Should be supported by all serial transport providers
//
// Inputs:
//    aPath       - Path identifier pointer
//    someData    - Pointer to buffer containing the data to be sent
//    anAmount    - Pointer to variable which upon entering the function
//                   contains the desired amount of bytes to write, and upon
//                   exit is updated by the function with the amount actually
//                   written (or buffered)
//    aMaxWaitTime - Specifies the desired maximum time to wait (in ms)
//                   If set to zero, the function returns as fast as possible
//
// Usage:
//    stat = TP_SerialWrite( path, dat, &numBytes, 100 );
typedef TP_StatusType ( WINAPI * TP_SerialWriteType )( TP_Path aPath, const UINT8* someData, UINT16* anAmount, UINT16 aMaxWaitTime );
extern TP_SerialWriteType TP_SerialWrite;


// TP_SerialWriteBlocking()
//
// Works the same way as TP_SerialWrite(), but will always block until all data has been sent (or an error occurs)
TP_StatusType TP_SerialWriteBlocking( TP_Path aPath, const UINT8* someData, UINT16 anAmount );


// TP_SpiOpen()
//
// Opens a SPI path.
//
// NOTE: Should be supported by all SPI transport providers
//
// Inputs:
//    aPath     - Path identifier pointer
//    aBaudRate - Baud rate of the SPI
//    aWireMode - The wire mode
//
// Usage:
//    stat = TP_SpiOpen( path, 250000, TP_SPI_4WIRE );
typedef TP_StatusType ( WINAPI * TP_SpiOpenType )( TP_Path aPath, UINT32 aBaudRate, TP_SpiWireModeType aWireMode );
extern TP_SpiOpenType TP_SpiOpen;


// TP_SpiClose()
//
// Closes the path to a previously opened SPI path so that the memory is made available to other paths/applications
// Please note that this does NOT destroy the path.
// It should be possible to open the SPI path again by calling SpiOpen()
//
// NOTE: Should be supported by all SPI transport providers
//
// Inputs:
//    aPath     - Path identifier pointer
//
// Usage:
//    stat = TP_SpiClose( path );
typedef TP_StatusType ( WINAPI * TP_SpiCloseType )( TP_Path aPath );
extern TP_SpiCloseType TP_SpiClose;


// TP_SpiTransaction()
//
// Writes anAmount of bytes from someInData and then reads the same amount of data back into someOutData
//
// NOTE: Should be supported by all SPI transport providers
//
// Inputs:
//    aPath      - Path identifier pointer
//    someInData - Pointer where the data to write is stored
//    someOutData - Pointer where the data read should be stored
//    anAmount   - The amount of data to be written and read
//
// Usage:
//    stat = TP_SpiTransaction( path, writeBuf, resultBuf, 256 );
typedef TP_StatusType ( WINAPI * TP_SpiTransactionType )( TP_Path aPath, const UINT8* someInData, UINT8* someOutData, UINT16 anAmount );
extern TP_SpiTransactionType TP_SpiTransaction;


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


#endif // not IMP_TP_H
