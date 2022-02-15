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
** permission. When used together with a product from HMS, this code can be   **
** modified, reproduced and distributed in binary form without any            **
** restrictions.                                                              **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** file_description
** Definition of ABCC types
********************************************************************************
********************************************************************************
** Services:
.
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_TD_H
#define ABCC_TD_H

/*******************************************************************************
**
** Constants
**
********************************************************************************
*/
//#define ABCC_SYS_16_BIT_CHAR

/*---------------------------------------------------------------------------
**
** BOOL8
**
** INT8
** INT16
** INT32
**
** UINT8
** UINT16
** UINT32
**
** FLOAT32
**
** The standard boolean data type, 8-bit.
** The standard signed 8, 16, and 32 bit data types (respectively).
** The standard unsigned 8, 16, and 32 bit data types (respectively).
**---------------------------------------------------------------------------
*/

#ifdef ABCC_SYS_16_BIT_CHAR
#define UINT8              MY_UNT8         
#define INT8               MY_INT8
#define BOOL8              MY_BOOL


typedef int                BOOL;
typedef unsigned short     MY_BOOL;
typedef unsigned short     MY_UNT8;
typedef signed short       MY_INT8;
typedef unsigned short     UINT16;
typedef signed short       INT16;
typedef unsigned int       UINT32;
typedef signed int         INT32;
typedef unsigned __int64   UINT64;
typedef signed __int64     INT64;
typedef float              FLOAT32;
#else
typedef int                BOOL;
typedef unsigned char      BOOL8;
typedef unsigned char      UINT8;
typedef signed char        INT8;
typedef unsigned short     UINT16;
typedef signed short       INT16;
typedef unsigned int       UINT32;
typedef signed int         INT32;
typedef unsigned __int64   UINT64;
typedef signed __int64     INT64;
typedef float              FLOAT32;
#endif





/*------------------------------------------------------------------------------
** LeINT16
** LeINT32
** LeUINT16
** LeUINT32
**
** Little endian data types for words and longwords.
**------------------------------------------------------------------------------
*/
typedef INT16     LeINT16;
typedef INT32     LeINT32;
typedef INT64     LeINT64;
typedef UINT16    LeUINT16;
typedef UINT32    LeUINT32;
typedef UINT64    LeUINT64;


/*------------------------------------------------------------------------------
** BeINT16
** BeINT32
** BeUINT16
** BeUINT32
**
** Big endian data types for words and longwords.
**------------------------------------------------------------------------------
*/
typedef INT16     BeINT16;
typedef INT32     BeINT32;
typedef INT64     BeINT64;
typedef UINT16    BeUINT16;
typedef UINT32    BeUINT32;
typedef UINT64    BeUINT64;

/*---------------------------------------------------------------------------
**
** FALSE
** TRUE
**
** These are the symbolic constants for true and false used in boolean
** data type comparisons.
**
**---------------------------------------------------------------------------
*/
#ifndef FALSE
   #define FALSE     0
#endif

#ifndef TRUE
   #define TRUE      ( !FALSE )
#endif



/*---------------------------------------------------------------------------
**
** NULL
**
** Default value for invalid pointers.
**
**---------------------------------------------------------------------------
*/
#ifndef NULL
#define NULL 0
#endif



#ifdef __cplusplus
   #define CPLUSPLUS
#endif

#ifdef CPLUSPLUS
   #define EXTFUNC extern "C"
#else
   #define EXTFUNC extern
#endif



#endif  /* inclusion lock */

