   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.24  06/05/06            */
   /*                                                     */
   /*          DEFTEMPLATE UTILITIES HEADER FILE          */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.24: Renamed BOOLEAN macro type to intBool.         */
/*                                                           */
/*            Added additional arguments to                  */
/*            InvalidDeftemplateSlotMessage function.        */
/*                                                           */
/*            Added additional arguments to                  */
/*            PrintTemplateFact function.                    */
/*                                                           */
/*************************************************************/

#ifndef _H_tmpltutl

#define _H_tmpltutl

#ifndef _H_expressn
#include "expressn.h"
#endif
#ifndef _H_evaluatn
#include "evaluatn.h"
#endif
#ifndef _H_factmngr
#include "factmngr.h"
#endif
#ifndef _H_constrnt
#include "constrnt.h"
#endif
#ifndef _H_symbol
#include "symbol.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _TMPLTUTL_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           InvalidDeftemplateSlotMessage(void *,const char *,const char *,int);
   LOCALE void                           SingleFieldSlotCardinalityError(void *,const char *);
   LOCALE void                           MultiIntoSingleFieldSlotError(void *,struct templateSlot *,struct deftemplate *);
   LOCALE void                           CheckTemplateFact(void *,struct fact *);
   LOCALE intBool                        CheckRHSSlotTypes(void *,struct expr *,struct templateSlot *,const char *);
   LOCALE struct templateSlot           *GetNthSlot(struct deftemplate *,int);
   LOCALE int                            FindSlotPosition(struct deftemplate *,struct symbolHashNode *);
   LOCALE void                           PrintTemplateFact(void *,const char *,struct fact *,int,int);
   LOCALE void                           UpdateDeftemplateScope(void *);
   LOCALE struct templateSlot           *FindSlot(struct deftemplate *,struct symbolHashNode *,short *);
   LOCALE struct deftemplate            *CreateImpliedDeftemplate(void *,SYMBOL_HN *,int);

#endif



