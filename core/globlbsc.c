   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.24  06/05/06            */
   /*                                                     */
   /*         DEFGLOBAL BASIC COMMANDS HEADER FILE        */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements core commands for the defglobal       */
/*   construct such as clear, reset, save, undefglobal,      */
/*   ppdefglobal, list-defglobals, and get-defglobals-list.  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Dantes                                      */
/*                                                           */
/* Revision History:                                         */
/*      6.23: Correction for FalseSymbol/TrueSymbol. DR0859  */
/*                                                           */
/*            Corrected compilation errors for files         */
/*            generated by constructs-to-c. DR0861           */
/*                                                           */
/*            Changed name of variable log to logName        */
/*            because of Unix compiler warnings of shadowed  */
/*            definitions.                                   */
/*                                                           */
/*      6.24: Renamed BOOLEAN macro type to intBool.         */
/*                                                           */
/*************************************************************/

#define _GLOBLBSC_SOURCE_

#include "setup.h"

#if DEFGLOBAL_CONSTRUCT

#include "constrct.h"
#include "extnfunc.h"
#include "watch.h"
#include "envrnmnt.h"

#include "globlcom.h"
#include "globldef.h"

#if BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE
#include "globlbin.h"
#endif

#if CONSTRUCT_COMPILER && (! RUN_TIME)
#include "globlcmp.h"
#endif

#include "globlbsc.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                    SaveDefglobals(void *,void *,const char *);
   static void                    ResetDefglobalAction(void *,struct constructHeader *,void *);
#if DEBUGGING_FUNCTIONS && (! RUN_TIME)
   static unsigned                DefglobalWatchAccess(void *,int,unsigned,struct expr *);
   static unsigned                DefglobalWatchPrint(void *,const char *,int,struct expr *);
#endif

/****************************************/
/* GLOBAL INTERNAL VARIABLE DEFINITIONS */
/****************************************/

#if DEBUGGING_FUNCTIONS
   globle unsigned              WatchGlobals = OFF;
#endif

/*****************************************************************/
/* DefglobalBasicCommands: Initializes basic defglobal commands. */
/*****************************************************************/
globle void DefglobalBasicCommands(
  void *theEnv)
  {
   AddSaveFunction(theEnv,"defglobal",SaveDefglobals,40);
   EnvAddResetFunction(theEnv,"defglobal",ResetDefglobals,50);

#if ! RUN_TIME
   EnvDefineFunction2(theEnv,"get-defglobal-list",'m',PTIEF GetDefglobalListFunction,"GetDefglobalListFunction","01w");
   EnvDefineFunction2(theEnv,"undefglobal",'v',PTIEF UndefglobalCommand,"UndefglobalCommand","11w");
   EnvDefineFunction2(theEnv,"defglobal-module",'w',PTIEF DefglobalModuleFunction,"DefglobalModuleFunction","11w");

#if DEBUGGING_FUNCTIONS
   EnvDefineFunction2(theEnv,"list-defglobals",'v', PTIEF ListDefglobalsCommand,"ListDefglobalsCommand","01w");
   EnvDefineFunction2(theEnv,"ppdefglobal",'v',PTIEF PPDefglobalCommand,"PPDefglobalCommand","11w");
   AddWatchItem(theEnv,"globals",0,&WatchGlobals,0,DefglobalWatchAccess,DefglobalWatchPrint);
#endif

#if (BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE)
   DefglobalBinarySetup(theEnv);
#endif

#if CONSTRUCT_COMPILER && (! RUN_TIME)
   DefglobalCompilerSetup(theEnv);
#endif

#endif
  }

/*************************************************************/
/* ResetDefglobals: Defglobal reset routine for use with the */
/*   reset command. Restores the values of the defglobals.   */
/*************************************************************/
globle void ResetDefglobals(
  void *theEnv)
  {
   if (! EnvGetResetGlobals(theEnv)) return;
   DoForAllConstructs(theEnv,ResetDefglobalAction,DefglobalData(theEnv)->DefglobalModuleIndex,TRUE,NULL);
  }

/******************************************************/
/* ResetDefglobalAction: Action to be applied to each */
/*   defglobal construct during a reset command.      */
/******************************************************/
static void ResetDefglobalAction(
  void *theEnv,
  struct constructHeader *theConstruct,
  void *buffer)
  {
#if MAC_XCD
#pragma unused(buffer)
#endif
   struct defglobal *theDefglobal = (struct defglobal *) theConstruct;
   DATA_OBJECT assignValue;

   if (EvaluateExpression(theEnv,theDefglobal->initial,&assignValue))
     {
      assignValue.type = SYMBOL;
      assignValue.value = EnvFalseSymbol(theEnv);
     }

   QSetDefglobalValue(theEnv,theDefglobal,&assignValue,FALSE);
  }

/******************************************/
/* SaveDefglobals: Defglobal save routine */
/*   for use with the save command.       */
/******************************************/
static void SaveDefglobals(
  void *theEnv,
  void *theModule,
  const char *logicalName)
  {
   SaveConstruct(theEnv,theModule,logicalName,DefglobalData(theEnv)->DefglobalConstruct); 
  }

/********************************************/
/* UndefglobalCommand: H/L access routine   */
/*   for the undefglobal command.           */
/********************************************/
globle void UndefglobalCommand(
  void *theEnv)
  {
   UndefconstructCommand(theEnv,"undefglobal",DefglobalData(theEnv)->DefglobalConstruct); 
  }

/************************************/
/* EnvUndefglobal: C access routine */
/*   for the undefglobal command.   */
/************************************/
globle intBool EnvUndefglobal(
  void *theEnv,
  void *theDefglobal)
  {
   return(Undefconstruct(theEnv,theDefglobal,DefglobalData(theEnv)->DefglobalConstruct)); 
  }

/**************************************************/
/* GetDefglobalListFunction: H/L access routine   */
/*   for the get-defglobal-list function.         */
/**************************************************/
globle void GetDefglobalListFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  { 
   GetConstructListFunction(theEnv,"get-defglobal-list",returnValue,DefglobalData(theEnv)->DefglobalConstruct); 
  }

/******************************************/
/* EnvGetDefglobalList: C access routine  */
/*   for the get-defglobal-list function. */
/******************************************/
globle void EnvGetDefglobalList(
  void *theEnv,
  DATA_OBJECT_PTR returnValue,
  void *theModule)
  {
   GetConstructList(theEnv,returnValue,DefglobalData(theEnv)->DefglobalConstruct,(struct defmodule *) theModule); 
  }

/*************************************************/
/* DefglobalModuleFunction: H/L access routine   */
/*   for the defglobal-module function.          */
/*************************************************/
globle void *DefglobalModuleFunction(
  void *theEnv)
  { 
   return(GetConstructModuleCommand(theEnv,"defglobal-module",DefglobalData(theEnv)->DefglobalConstruct)); 
  }

#if DEBUGGING_FUNCTIONS

/********************************************/
/* PPDefglobalCommand: H/L access routine   */
/*   for the ppdefglobal command.           */
/********************************************/
globle void PPDefglobalCommand(
  void *theEnv)
  {
   PPConstructCommand(theEnv,"ppdefglobal",DefglobalData(theEnv)->DefglobalConstruct); 
  }

/*************************************/
/* PPDefglobal: C access routine for */
/*   the ppdefglobal command.        */
/*************************************/
globle int PPDefglobal(
  void *theEnv,
  const char *defglobalName,
  const char *logicalName)
  {
   return(PPConstruct(theEnv,defglobalName,logicalName,DefglobalData(theEnv)->DefglobalConstruct)); 
  }

/***********************************************/
/* ListDefglobalsCommand: H/L access routine   */
/*   for the list-defglobals command.          */
/***********************************************/
globle void ListDefglobalsCommand(
  void *theEnv)
  {
   ListConstructCommand(theEnv,"list-defglobals",DefglobalData(theEnv)->DefglobalConstruct);
  }

/***************************************/
/* EnvListDefglobals: C access routine */
/*   for the list-defglobals command.  */
/***************************************/
globle void EnvListDefglobals(
  void *theEnv,
  const char *logicalName,
  void *vTheModule)
  {
   struct defmodule *theModule = (struct defmodule *) vTheModule;

   ListConstruct(theEnv,DefglobalData(theEnv)->DefglobalConstruct,logicalName,theModule);
  }

/*********************************************************/
/* EnvGetDefglobalWatch: C access routine for retrieving */
/*   the current watch value of a defglobal.             */
/*********************************************************/
globle unsigned EnvGetDefglobalWatch(
  void *theEnv,
  void *theGlobal)
  { 
#if MAC_XCD
#pragma unused(theEnv)
#endif

   return(((struct defglobal *) theGlobal)->watch); 
  }

/********************************************************/
/* EnvSetDeftemplateWatch: C access routine for setting */
/*   the current watch value of a deftemplate.          */
/********************************************************/
globle void EnvSetDefglobalWatch(
  void *theEnv,
  unsigned newState,
  void *theGlobal)
  {  
#if MAC_XCD
#pragma unused(theEnv)
#endif

   ((struct defglobal *) theGlobal)->watch = newState; 
  }

#if ! RUN_TIME

/********************************************************/
/* DefglobalWatchAccess: Access routine for setting the */
/*   watch flag of a defglobal via the watch command.   */
/********************************************************/
static unsigned DefglobalWatchAccess(
  void *theEnv,
  int code,
  unsigned newState,
  EXPRESSION *argExprs)
  {
#if MAC_XCD
#pragma unused(code)
#endif

   return(ConstructSetWatchAccess(theEnv,DefglobalData(theEnv)->DefglobalConstruct,newState,argExprs,
                                  EnvGetDefglobalWatch,EnvSetDefglobalWatch));
  }

/*********************************************************************/
/* DefglobalWatchPrint: Access routine for printing which defglobals */
/*   have their watch flag set via the list-watch-items command.     */
/*********************************************************************/
static unsigned DefglobalWatchPrint(
  void *theEnv,
  const char *logName,
  int code,
  EXPRESSION *argExprs)
  {
#if MAC_XCD
#pragma unused(code)
#endif

   return(ConstructPrintWatchAccess(theEnv,DefglobalData(theEnv)->DefglobalConstruct,logName,argExprs,
                                    EnvGetDefglobalWatch,EnvSetDefglobalWatch));
  }

#endif

#endif /* DEBUGGING_FUNCTIONS */

#endif /* DEFGLOBAL_CONSTRUCT */


