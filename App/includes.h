/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                              (c) Copyright 1992-2004, Micrium, Weston, FL
*                                           All Rights Reserved
*
*                                           MASTER INCLUDE FILE
*********************************************************************************************************
*/

#ifndef  _INCLUDES_H
#define  _INCLUDES_H

#include    <app_cfg.h>
#include	<os_cfg_app.h>
#include    <os.h>
#include	<posix-pthread.h>
#include 	<api-wrapper.h>

#if(HW_TYPE == HW_EMBEDDED_LPC2129)
#include 	<LPC21xx.H>
#include    <bsp.h>
#define NULL	0
#else /* HW_VIRTUAL_LINUX */
#include  <stdio.h>
#include  <lib_mem.h>
#include  <lib_math.h>
#endif

#if(HW_TYPE == HW_VIRTUAL_LINUX)
	#define PRINT	printf
#else
	#define PRINT	sendstr
#endif


#endif /* _INCLUDES_H */
