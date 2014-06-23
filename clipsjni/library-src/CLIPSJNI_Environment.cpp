
#include "CLIPSJNI_Environment.h"

#include "clips.h"

#define CLIPSJNI_DATA 67

struct clipsJNIData
  { 
   int javaExternalAddressID;

   jclass classClass;
   jmethodID classGetCanonicalNameMethod;
   
   jclass longClass;
   jmethodID longInitMethod;

   jclass doubleClass;
   jmethodID doubleInitMethod;
   
   jclass arrayListClass;
   jmethodID arrayListInitMethod;
   jmethodID arrayListAddMethod;

   jclass voidValueClass;
   jmethodID voidValueInitMethod;

   jclass integerValueClass;
   jmethodID integerValueInitMethod;
   jclass floatValueClass;
   jmethodID floatValueInitMethod;

   jclass symbolValueClass;
   jmethodID symbolValueInitMethod;
   jclass stringValueClass;
   jmethodID stringValueInitMethod;
   jclass instanceNameValueClass;
   jmethodID instanceNameValueInitMethod;

   jclass multifieldValueClass;
   jmethodID multifieldValueInitMethod;

   jclass factAddressValueClass;
   jmethodID factAddressValueInitMethod;

   jclass instanceAddressValueClass;
   jmethodID instanceAddressValueInitMethod;
  };

#define CLIPSJNIData(theEnv) ((struct clipsJNIData *) GetEnvironmentData(theEnv,CLIPSJNI_DATA))

static int        QueryJNIRouter(void *,const char *);
static int        ExitJNIRouter(void *,int);
static int        PrintJNIRouter(void *,const char *,const char *);
static int        GetcJNIRouter(void *,const char *);
static int        UngetcJNIRouter(void *,int,const char *);
static void       DeallocateJNIData(void *);
static jobject    ConvertSingleFieldValue(JNIEnv *,jobject,void *,int,void *);
static jobject    ConvertDataObject(JNIEnv *,jobject,void *,DATA_OBJECT *);
static void      *JLongToPointer(jlong);
static jlong      PointerToJLong(void *);
static void       PrintJavaAddress(void *,const char *,void *);
static void       NewJavaAddress(void *,DATA_OBJECT *);
static intBool    CallJavaMethod(void *,DATA_OBJECT *,DATA_OBJECT *);
static intBool    DiscardJavaAddress(void *,void *);

/**********************************************/
/* DeallocateJNIData: Deallocates environment */
/*    data for the JNI functionality.         */
/**********************************************/
static void DeallocateJNIData(
  void *theEnv)
  {
   JNIEnv *env;
   
   env = (JNIEnv *) GetEnvironmentContext(theEnv);
  
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->classClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->longClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->doubleClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->arrayListClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->voidValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->integerValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->floatValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->symbolValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->stringValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->instanceNameValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->multifieldValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->factAddressValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->instanceAddressValueClass);
  }

/*************************************************/
/* FindJNIRouter: Query routine for JNI routers. */
/*************************************************/
static int QueryJNIRouter(
  void *theEnv,
  const char *logicalName)
  {
   jobject context;
   jclass cls;
   JNIEnv *env;
   jmethodID mid;
   jboolean rv;
   jstring str;
  
   env = (JNIEnv *) GetEnvironmentContext(theEnv);

   context = reinterpret_cast<jobject>(GetEnvironmentRouterContext(theEnv));
  
   cls = env->GetObjectClass(context);

   mid = env->GetMethodID(cls,"query","(Ljava/lang/String;)Z");

   env->DeleteLocalRef(cls);

   if (mid == NULL)
     { return FALSE; }

   str = env->NewStringUTF(logicalName);

   rv = env->CallBooleanMethod(context,mid,str);
      
   env->DeleteLocalRef(str);
      
   return(rv);
  }

/*************************************************/
/* ExitJNIRouter:  Exit routine for JNI routers. */
/*************************************************/
#if IBM_TBC
#pragma argsused
#endif
static int ExitJNIRouter(
  void *theEnv,
  int num)
  {
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(num)
#endif
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#endif
   /* TBD deallocate global context reference */
   return(1);
  }

/**************************************************/
/* PrintJNIRouter: Print routine for JNI routers. */
/**************************************************/
static int PrintJNIRouter(
  void *theEnv,
  const char *logicalName,
  const char *str)
  {
   jobject context;
   JNIEnv *env;
   jmethodID mid;
   jclass cls;
   jstring str1, str2;

   env = (JNIEnv *) GetEnvironmentContext(theEnv);

   context = reinterpret_cast<jobject>(GetEnvironmentRouterContext(theEnv));

   cls = env->GetObjectClass(context);

   mid = env->GetMethodID(cls,"print","(Ljava/lang/String;Ljava/lang/String;)V");

   env->DeleteLocalRef(cls);

   if (mid == NULL)
     { return FALSE; }

   str1 = env->NewStringUTF(logicalName);
   str2 = env->NewStringUTF(str);

   env->CallVoidMethod(context,mid,str1,str2);

   env->DeleteLocalRef(str1);
   env->DeleteLocalRef(str2);
   
   return(1);
  }

/************************************************/
/* GetcJNIRouter: Getc routine for JNI routers. */
/************************************************/
static int GetcJNIRouter(
  void *theEnv,
  const char *logicalName)
  {
   jint theChar;
   jobject context;
   JNIEnv *env;
   jmethodID mid;
   jclass cls;
   jstring str;

   env = (JNIEnv *) GetEnvironmentContext(theEnv);

   context = reinterpret_cast<jobject>(GetEnvironmentRouterContext(theEnv));

   cls = env->GetObjectClass(context);

   mid = env->GetMethodID(cls,"getchar","(Ljava/lang/String;)I");

   env->DeleteLocalRef(cls);

   if (mid == NULL)
     { return -1; }

   str = env->NewStringUTF(logicalName);

   theChar = env->CallIntMethod(context,mid,str);

   env->DeleteLocalRef(str);

   return((int) theChar);
  }

/****************************************************/
/* UngetcJNIRouter: Ungetc routine for JNI routers. */
/****************************************************/
static int UngetcJNIRouter(
  void *theEnv,
  int ch,
  const char *logicalName)
  {
   jint theChar;
   jobject context;
   JNIEnv *env;
   jmethodID mid;
   jclass cls;
   jstring str;

   env = (JNIEnv *) GetEnvironmentContext(theEnv);

   context = reinterpret_cast<jobject>(GetEnvironmentRouterContext(theEnv));

   cls = env->GetObjectClass(context);

   mid = env->GetMethodID(cls,"ungetchar","(Ljava/lang/String;I)I");

   env->DeleteLocalRef(cls);

   if (mid == NULL)
     { return -1; }

   str = env->NewStringUTF(logicalName);

   theChar = env->CallIntMethod(context,mid,(jint) ch,str);

   env->DeleteLocalRef(str);

   return((int) theChar);
  }

/*******************************************************/
/* Java_CLIPSJNI_Environment_getCLIPSVersion: Native   */
/*   function for the CLIPSJNI getCLIPSVersion method. */
/* Class:     CLIPSJNI_Environment                     */
/* Method:    getCLIPSVersion                          */
/* Signature: ()Ljava/lang/String;                     */
/*******************************************************/
JNIEXPORT jstring JNICALL Java_CLIPSJNI_Environment_getCLIPSVersion(
  JNIEnv *env, 
  jclass cls) 
  {
   return env->NewStringUTF(VERSION_STRING);
  }

/************************************************/
/* Java_CLIPSJNI_Environment_createEnvironment: */
/*                                              */
/*    Class:     CLIPSJNI_Environment           */
/*    Method:    createEnvironment              */
/*    Signature: ()J                            */
/************************************************/
JNIEXPORT jlong JNICALL Java_CLIPSJNI_Environment_createEnvironment(
  JNIEnv *env, 
  jobject obj)
  {
   void *theEnv;
   jclass theClassClass; 
   jmethodID theClassGetCanonicalNameMethod;
   jclass theLongClass; 
   jmethodID theLongInitMethod;
   jclass theDoubleClass; 
   jmethodID theDoubleInitMethod;
   jclass theArrayListClass; 
   jmethodID theArrayListInitMethod, theArrayListAddMethod;
   jclass theVoidValueClass;
   jmethodID theVoidValueInitMethod;
   jclass theIntegerValueClass, theFloatValueClass;
   jmethodID theIntegerValueInitMethod, theFloatValueInitMethod;
   jclass theSymbolValueClass, theStringValueClass, theInstanceNameValueClass;
   jmethodID theSymbolValueInitMethod, theStringValueInitMethod, theInstanceNameValueInitMethod;
   jclass theMultifieldValueClass;
   jmethodID theMultifieldValueInitMethod;
   jclass theFactAddressValueClass;
   jmethodID theFactAddressValueInitMethod;
   jclass theInstanceAddressValueClass;
   jmethodID theInstanceAddressValueInitMethod;
   struct externalAddressType javaPointer = { "java", PrintJavaAddress, PrintJavaAddress, DiscardJavaAddress, NewJavaAddress, CallJavaMethod };

   /*===========================*/
   /* Look up the Java classes. */
   /*===========================*/

   theClassClass = env->FindClass("java/lang/Class"); 
   theLongClass = env->FindClass("java/lang/Long"); 
   theDoubleClass = env->FindClass("java/lang/Double"); 
   theArrayListClass = env->FindClass("java/util/ArrayList"); 
   theVoidValueClass = env->FindClass("CLIPSJNI/VoidValue");
   theIntegerValueClass = env->FindClass("CLIPSJNI/IntegerValue");
   theFloatValueClass = env->FindClass("CLIPSJNI/FloatValue");
   theSymbolValueClass = env->FindClass("CLIPSJNI/SymbolValue");
   theStringValueClass = env->FindClass("CLIPSJNI/StringValue");
   theInstanceNameValueClass = env->FindClass("CLIPSJNI/InstanceNameValue");
   theMultifieldValueClass = env->FindClass("CLIPSJNI/MultifieldValue");
   theFactAddressValueClass = env->FindClass("CLIPSJNI/FactAddressValue");
   theInstanceAddressValueClass = env->FindClass("CLIPSJNI/InstanceAddressValue");
                
   /*=========================================*/
   /* If the Java classes could not be found, */
   /* abort creation of the environment.      */
   /*=========================================*/
   
   if ((theClassClass == NULL) ||
       (theLongClass == NULL) || (theDoubleClass == NULL) ||
       (theArrayListClass == NULL) ||
       (theVoidValueClass == NULL) ||
       (theIntegerValueClass == NULL) || (theFloatValueClass == NULL) ||
       (theSymbolValueClass == NULL) || (theStringValueClass == NULL) || 
       (theInstanceNameValueClass == NULL) ||
       (theMultifieldValueClass == NULL) ||
       (theFactAddressValueClass == NULL) ||
       (theInstanceAddressValueClass == NULL))
     { return((jlong) NULL); }
     
   /*================================*/
   /* Look up the Java init methods. */
   /*================================*/
   
   theClassGetCanonicalNameMethod = env->GetMethodID(theClassClass,"getCanonicalName","()Ljava/lang/String;");
   theLongInitMethod = env->GetMethodID(theLongClass,"<init>","(J)V");
   theDoubleInitMethod = env->GetMethodID(theDoubleClass,"<init>","(D)V");
   theArrayListInitMethod = env->GetMethodID(theArrayListClass,"<init>","(I)V");
   theArrayListAddMethod = env->GetMethodID(theArrayListClass,"add","(Ljava/lang/Object;)Z");
   theVoidValueInitMethod = env->GetMethodID(theVoidValueClass,"<init>","()V");
   theIntegerValueInitMethod = env->GetMethodID(theIntegerValueClass,"<init>","(Ljava/lang/Long;)V");
   theFloatValueInitMethod = env->GetMethodID(theFloatValueClass,"<init>","(Ljava/lang/Double;)V");
   theSymbolValueInitMethod = env->GetMethodID(theSymbolValueClass,"<init>","(Ljava/lang/String;)V");
   theStringValueInitMethod = env->GetMethodID(theStringValueClass,"<init>","(Ljava/lang/String;)V");
   theInstanceNameValueInitMethod = env->GetMethodID(theInstanceNameValueClass,"<init>","(Ljava/lang/String;)V");
   theMultifieldValueInitMethod = env->GetMethodID(theMultifieldValueClass,"<init>","(Ljava/util/List;)V");
   theFactAddressValueInitMethod = env->GetMethodID(theFactAddressValueClass,"<init>","(JLCLIPSJNI/Environment;)V");
   theInstanceAddressValueInitMethod = env->GetMethodID(theInstanceAddressValueClass,"<init>","(JLCLIPSJNI/Environment;)V");

   /*==============================================*/
   /* If the Java init methods could not be found, */
   /* abort creation of the enviroment.            */
   /*==============================================*/
   
   if ((theClassGetCanonicalNameMethod == NULL) ||
       (theLongInitMethod == NULL) || (theDoubleInitMethod == NULL) || 
       (theArrayListInitMethod == NULL) || (theArrayListAddMethod == NULL) ||
       (theVoidValueInitMethod == NULL) ||
       (theIntegerValueInitMethod == NULL) || (theFloatValueInitMethod == NULL) ||
       (theSymbolValueInitMethod == NULL) || (theStringValueInitMethod == NULL) ||
       (theInstanceNameValueInitMethod == NULL) ||
       (theMultifieldValueInitMethod == NULL) ||
       (theFactAddressValueInitMethod == NULL) ||
       (theInstanceAddressValueInitMethod == NULL))
     { return((jlong) NULL); }
     
   /*=========================*/
   /* Create the environment. */
   /*=========================*/
   
   theEnv = CreateEnvironment();
   if (theEnv == NULL) return((jlong) NULL);
   
   /*====================================*/
   /* Allocate the JNI environment data. */
   /*====================================*/
   
   AllocateEnvironmentData(theEnv,CLIPSJNI_DATA,sizeof(struct clipsJNIData),DeallocateJNIData);
   
   /*===================================================*/
   /* Cache the class and method references (converting */
   /* the local class references to global references   */
   /* so they won't be garbage collected.               */
   /*===================================================*/

   CLIPSJNIData(theEnv)->classClass = static_cast<jclass>(env->NewGlobalRef(theClassClass));
   CLIPSJNIData(theEnv)->classGetCanonicalNameMethod = theClassGetCanonicalNameMethod;

   CLIPSJNIData(theEnv)->longClass = static_cast<jclass>(env->NewGlobalRef(theLongClass));
   CLIPSJNIData(theEnv)->longInitMethod = theLongInitMethod;
   CLIPSJNIData(theEnv)->doubleClass = static_cast<jclass>(env->NewGlobalRef(theDoubleClass));
   CLIPSJNIData(theEnv)->doubleInitMethod = theDoubleInitMethod;
   CLIPSJNIData(theEnv)->arrayListClass = static_cast<jclass>(env->NewGlobalRef(theArrayListClass));
   CLIPSJNIData(theEnv)->arrayListInitMethod = theArrayListInitMethod;
   CLIPSJNIData(theEnv)->arrayListAddMethod = theArrayListAddMethod;

   CLIPSJNIData(theEnv)->voidValueClass = static_cast<jclass>(env->NewGlobalRef(theVoidValueClass));
   CLIPSJNIData(theEnv)->voidValueInitMethod = theVoidValueInitMethod;
   
   CLIPSJNIData(theEnv)->integerValueClass = static_cast<jclass>(env->NewGlobalRef(theIntegerValueClass));
   CLIPSJNIData(theEnv)->integerValueInitMethod = theIntegerValueInitMethod;
   CLIPSJNIData(theEnv)->floatValueClass = static_cast<jclass>(env->NewGlobalRef(theFloatValueClass));
   CLIPSJNIData(theEnv)->floatValueInitMethod = theFloatValueInitMethod;
      
   CLIPSJNIData(theEnv)->symbolValueClass = static_cast<jclass>(env->NewGlobalRef(theSymbolValueClass));
   CLIPSJNIData(theEnv)->symbolValueInitMethod = theSymbolValueInitMethod;
   CLIPSJNIData(theEnv)->stringValueClass = static_cast<jclass>(env->NewGlobalRef(theStringValueClass));
   CLIPSJNIData(theEnv)->stringValueInitMethod = theStringValueInitMethod;
   CLIPSJNIData(theEnv)->instanceNameValueClass = static_cast<jclass>(env->NewGlobalRef(theInstanceNameValueClass));
   CLIPSJNIData(theEnv)->instanceNameValueInitMethod = theInstanceNameValueInitMethod;

   CLIPSJNIData(theEnv)->multifieldValueClass = static_cast<jclass>(env->NewGlobalRef(theMultifieldValueClass));
   CLIPSJNIData(theEnv)->multifieldValueInitMethod = theMultifieldValueInitMethod;

   CLIPSJNIData(theEnv)->factAddressValueClass = static_cast<jclass>(env->NewGlobalRef(theFactAddressValueClass));
   CLIPSJNIData(theEnv)->factAddressValueInitMethod = theFactAddressValueInitMethod;

   CLIPSJNIData(theEnv)->instanceAddressValueClass = static_cast<jclass>(env->NewGlobalRef(theInstanceAddressValueClass));
   CLIPSJNIData(theEnv)->instanceAddressValueInitMethod = theInstanceAddressValueInitMethod;
   
   /*======================================*/
   /* Store the java environment for later */
   /* access by the CLIPS environment.     */
   /*======================================*/
   
   SetEnvironmentContext(theEnv,(void *) env);
     
   /*=======================================*/
   /* Deallocate the local Java references. */
   /*=======================================*/
   
   env->DeleteLocalRef(theClassClass);
   env->DeleteLocalRef(theLongClass);
   env->DeleteLocalRef(theDoubleClass);
   env->DeleteLocalRef(theArrayListClass);
   env->DeleteLocalRef(theVoidValueClass);
   env->DeleteLocalRef(theIntegerValueClass);
   env->DeleteLocalRef(theFloatValueClass);
   env->DeleteLocalRef(theSymbolValueClass);
   env->DeleteLocalRef(theStringValueClass);
   env->DeleteLocalRef(theInstanceNameValueClass);
   env->DeleteLocalRef(theMultifieldValueClass);
   env->DeleteLocalRef(theFactAddressValueClass);
   env->DeleteLocalRef(theInstanceAddressValueClass);

   /*=================================*/
   /* Set up Java External Addresses. */
   /*=================================*/
   
   CLIPSJNIData(theEnv)->javaExternalAddressID = InstallExternalAddressType(theEnv,&javaPointer);
   
   /*=========================*/
   /* Return the environment. */
   /*=========================*/
   
   return (PointerToJLong(theEnv));
  }

/*********************************************/
/* Java_CLIPSJNI_Environment_clear: Native   */ 
/*   function for the CLIPSJNI clear method. */
/*                                           */
/* Class:     CLIPSJNI_Environment           */
/* Method:    clear                          */
/* Signature: (J)V                           */
/*********************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_clear(
  JNIEnv *env, 
  jobject obj,
  jlong clipsEnv)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);
   
   EnvClear(JLongToPointer(clipsEnv));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/*********************************************/
/* Java_CLIPSJNI_Environment_reset: Native   */ 
/*   function for the CLIPSJNI reset method. */
/*                                           */
/* Class:     CLIPSJNI_Environment           */
/* Method:    reset                          */
/* Signature: (J)V                           */
/*********************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_reset(
  JNIEnv *env, 
  jobject obj,
  jlong clipsEnv)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);
   
   EnvReset(JLongToPointer(clipsEnv));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/********************************************/
/* Java_CLIPSJNI_Environment_load: Native   */ 
/*   function for the CLIPSJNI load method. */
/*                                          */
/* Class:     CLIPSJNI_Environment          */
/* Method:    load                          */
/* Signature: (JLjava/lang/String;)V        */
/********************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_load(
  JNIEnv *env, 
  jobject obj,
  jlong clipsEnv,
  jstring fileName)
  {
   const char *cFileName = env->GetStringUTFChars(fileName,NULL);
   
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   EnvLoad(JLongToPointer(clipsEnv),(const char *) cFileName);
   
   env->ReleaseStringUTFChars(fileName,cFileName);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/*************************************************/
/* Java_CLIPSJNI_Environment_loadFacts: Native   */ 
/*   function for the CLIPSJNI loadFacts method. */
/*                                               */
/* Class:     CLIPSJNI_Environment               */
/* Method:    loadFact                           */
/* Signature: (JLjava/lang/String;)Z             */
/*************************************************/
JNIEXPORT jboolean JNICALL Java_CLIPSJNI_Environment_loadFacts(
  JNIEnv *env, 
  jobject obj,
  jlong clipsEnv,
  jstring fileName)
  {
   jboolean rv;
   const char *cFileName = env->GetStringUTFChars(fileName,NULL);
   
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   rv = EnvLoadFacts(JLongToPointer(clipsEnv),(const char *) cFileName);
   
   env->ReleaseStringUTFChars(fileName,cFileName);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
   
   return rv;
  }

/*********************************************/
/* Java_CLIPSJNI_Environment_watch: Native   */ 
/*   function for the CLIPSJNI watch method. */
/*                                           */
/* Class:     CLIPSJNI_Environment           */
/* Method:    watch                          */
/* Signature: (JLjava/lang/String;)Z         */
/*********************************************/
JNIEXPORT jboolean JNICALL Java_CLIPSJNI_Environment_watch(
  JNIEnv *env, 
  jobject obj,
  jlong clipsEnv,
  jstring watchItem)
  {
   jboolean rv;
   const char *cWatchItem = env->GetStringUTFChars(watchItem,NULL);
   
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   rv = EnvWatch(JLongToPointer(clipsEnv),(const char *) cWatchItem);
   
   env->ReleaseStringUTFChars(watchItem,cWatchItem);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
   
   return rv;
  }

/***********************************************/
/* Java_CLIPSJNI_Environment_unwatch: Native   */ 
/*   function for the CLIPSJNI unwatch method. */
/*                                             */
/* Class:     CLIPSJNI_Environment             */
/* Method:    unwatch                          */
/* Signature: (JLjava/lang/String;)Z           */
/***********************************************/
JNIEXPORT jboolean JNICALL Java_CLIPSJNI_Environment_unwatch(
  JNIEnv *env, 
  jobject obj,
  jlong clipsEnv,
  jstring watchItem)
  {
   jboolean rv;
   char *cWatchItem = const_cast<char*>(env->GetStringUTFChars(watchItem,NULL));

   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   rv = EnvUnwatch(JLongToPointer(clipsEnv),cWatchItem);
   
   env->ReleaseStringUTFChars(watchItem,cWatchItem);

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/**************************************************/
/* Java_CLIPSJNI_Environment_run: Native function */ 
/*   for the CLIPSJNI run method.                 */
/*                                                */
/* Class:     CLIPSJNI_Environment                */
/* Method:    run                                 */
/* Signature: (JJ)J                               */
/**************************************************/
JNIEXPORT jlong JNICALL Java_CLIPSJNI_Environment_run(
  JNIEnv *env, 
  jobject obj, 
  jlong clipsEnv,
  jlong runLimit)
  {
   jlong rv;
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   rv = EnvRun(JLongToPointer(clipsEnv),runLimit);

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/*************************************************************/
/* Java_CLIPSJNI_Environment_eval: Native function for the   */
/*   CLIPSJNI eval method.                                   */
/*                                                           */
/* Class:     CLIPSJNI_Environment                           */
/* Method:    eval                                           */
/* Signature: (JLjava/lang/String;)LCLIPSJNI/PrimitiveValue; */
/*                                                           */
/*************************************************************/
JNIEXPORT jobject JNICALL Java_CLIPSJNI_Environment_eval(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv,
  jstring evalStr)
  {
   DATA_OBJECT theDO;
   jobject result = NULL;
   char *cEvalStr;
   void *theCLIPSEnv = JLongToPointer(clipsEnv);

   void *oldContext = SetEnvironmentContext(theCLIPSEnv,(void *) env);
   
   cEvalStr = const_cast<char*>(env->GetStringUTFChars(evalStr,NULL));
   
   EnvEval(theCLIPSEnv,cEvalStr,&theDO);

   env->ReleaseStringUTFChars(evalStr,cEvalStr);
   
   result = ConvertDataObject(env, obj,theCLIPSEnv,&theDO);

   SetEnvironmentContext(theCLIPSEnv,oldContext);

   return result;  
  }

/**********************/
/* ConvertDataObject: */
/**********************/
static jobject ConvertDataObject(
  JNIEnv *env,
  jobject javaEnv,
  void *clipsEnv,
  DATA_OBJECT *theDO)
  {
   jobject result = NULL, tresult;
   jint mfLength;
   struct multifield *theList;
   long i;
   
   switch(GetpType(theDO))
     {
      case MULTIFIELD:
        mfLength = GetpDOLength(theDO);

        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->arrayListClass,
                                   CLIPSJNIData(clipsEnv)->arrayListInitMethod,
                                   (jint) mfLength);
                                   
        if (result == NULL)
          { return result; }
          
        theList = (struct multifield *) DOPToPointer(theDO);
        
        for (i = GetpDOBegin(theDO); i <= GetpDOEnd(theDO); i++)
         {
           tresult = ConvertSingleFieldValue(env,javaEnv,clipsEnv,GetMFType(theList,i),GetMFValue(theList,i));
          
          if (tresult != NULL)
             { env->CallBooleanMethod(result,CLIPSJNIData(clipsEnv)->arrayListAddMethod,tresult); }

          env->DeleteLocalRef(tresult);
         }
       
        tresult = result;
         
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->multifieldValueClass,
                                   CLIPSJNIData(clipsEnv)->multifieldValueInitMethod,
                                   tresult);
        break;
        
      case RVOID:
      case SYMBOL:
      case STRING:
      case INSTANCE_NAME:
      case INTEGER:
      case FLOAT:
      case FACT_ADDRESS:
      case INSTANCE_ADDRESS:
        result = ConvertSingleFieldValue(env,javaEnv,clipsEnv,GetpType(theDO),GetpValue(theDO));
        break;

      default: 
        break;
     }

   return result;  
  }
  
/****************************/
/* ConvertSingleFieldValue: */
/****************************/
static jobject ConvertSingleFieldValue(
  JNIEnv *env,
  jobject javaEnv,
  void *clipsEnv,
  int type,
  void  *value)
  {
   jobject result = NULL, tresult;
   jstring sresult = NULL;
   
   switch(type)
     {
      case RVOID:
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->voidValueClass,
                                   CLIPSJNIData(clipsEnv)->voidValueInitMethod,
                                   sresult);
        break;

      case SYMBOL:
        sresult = env->NewStringUTF(ValueToString(value));
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->symbolValueClass,
                                   CLIPSJNIData(clipsEnv)->symbolValueInitMethod,
                                   sresult);
        env->DeleteLocalRef(sresult);
        break;
        
        
      case STRING:
        sresult = env->NewStringUTF(ValueToString(value));
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->stringValueClass,
                                   CLIPSJNIData(clipsEnv)->stringValueInitMethod,
                                   sresult);
        env->DeleteLocalRef(sresult);
        break;
        
      case INSTANCE_NAME:
        sresult = env->NewStringUTF(ValueToString(value));
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->instanceNameValueClass,
                                   CLIPSJNIData(clipsEnv)->instanceNameValueInitMethod,
                                   sresult);
        env->DeleteLocalRef(sresult);
        break;
        
      case INTEGER:
        tresult = env->NewObject(
                                    CLIPSJNIData(clipsEnv)->longClass,
                                    CLIPSJNIData(clipsEnv)->longInitMethod,
                                    (jlong) ValueToLong(value));
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->integerValueClass,
                                   CLIPSJNIData(clipsEnv)->integerValueInitMethod,
                                   tresult);
        env->DeleteLocalRef(tresult);
        break;

      case FLOAT:
        tresult = env->NewObject(
                                    CLIPSJNIData(clipsEnv)->doubleClass,
                                    CLIPSJNIData(clipsEnv)->doubleInitMethod,
                                    (jdouble) ValueToDouble(value));

        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->floatValueClass,
                                   CLIPSJNIData(clipsEnv)->floatValueInitMethod,
                                   tresult);
        env->DeleteLocalRef(tresult);
        break;

      case FACT_ADDRESS:
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->factAddressValueClass,
                                   CLIPSJNIData(clipsEnv)->factAddressValueInitMethod,
                                   PointerToJLong(value),javaEnv);
        break;

      case INSTANCE_ADDRESS:
        result = env->NewObject(
                                   CLIPSJNIData(clipsEnv)->instanceAddressValueClass,
                                   CLIPSJNIData(clipsEnv)->instanceAddressValueInitMethod,
                                   PointerToJLong(value),javaEnv);
        break;

      default: 
        break;
     }

   return result;  
  }

/****************************************************/
/* Java_CLIPSJNI_Environment_build: Native function */
/*   for the CLIPSJNI build method.                 */
/*                                                  */
/* Class:     CLIPSJNI_Environment                  */
/* Method:    build                                 */
/* Signature: (JLjava/lang/String;)Z                */
/****************************************************/
JNIEXPORT jboolean JNICALL Java_CLIPSJNI_Environment_build(
  JNIEnv *env, 
  jobject obj, 
  jlong clipsEnv,
  jstring buildStr)
  {
   jboolean rv;
   void *theCLIPSEnv = JLongToPointer(clipsEnv);
   char *cBuildStr = const_cast<char*>(env->GetStringUTFChars(buildStr,NULL));

   void *oldContext = SetEnvironmentContext(theCLIPSEnv,(void *) env);
   
   rv = (jboolean) EnvBuild(theCLIPSEnv,cBuildStr);

   env->ReleaseStringUTFChars(buildStr,cBuildStr);

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/***************************************************************/
/* Java_CLIPSJNI_Environment_assertString: Native function for */
/*   the CLIPSJNI assertString method.                         */
/*                                                             */
/* Class:     CLIPSJNI_Environment                             */
/* Method:    assertString                                     */
/* Signature: (JLjava/lang/String;)LCLIPSJNI/FactAddressValue; */
/***************************************************************/
JNIEXPORT jobject JNICALL Java_CLIPSJNI_Environment_assertString(
  JNIEnv *env,
  jobject obj, 
  jlong clipsEnv, 
  jstring factStr)
  {
   jobject rv;
   void *theFact;
   void *theCLIPSEnv = JLongToPointer(clipsEnv);
   char *cFactStr = const_cast<char*>(env->GetStringUTFChars(factStr,NULL));
   
   void *oldContext = SetEnvironmentContext(theCLIPSEnv,(void *) env);

   theFact = EnvAssertString(theCLIPSEnv,cFactStr);

   env->ReleaseStringUTFChars(factStr,cFactStr);
   
   if (theFact == NULL)
     { return(NULL); }
     
   rv = ConvertSingleFieldValue(env,obj,theCLIPSEnv,FACT_ADDRESS,theFact);

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/*************************************************/
/* Java_CLIPSJNI_Environment_factIndex: Native   */
/*   function for the CLIPSJNI factIndex method. */
/*                                               */
/* Class:     CLIPSJNI_Environment               */
/* Method:    factIndex                          */
/* Signature: (LCLIPSJNI/Environment;JJ)J        */
/*************************************************/
JNIEXPORT jlong JNICALL Java_CLIPSJNI_Environment_factIndex(
  JNIEnv *env, 
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv, 
  jlong clipsFact)
  {
   jlong rv;
   
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   rv = EnvFactIndex(JLongToPointer(clipsEnv),JLongToPointer(clipsFact));

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/**************************************************************/
/* Java_CLIPSJNI_Environment_getFactSlot: Native function     */
/*   for the CLIPSJNI getFactSlot method.                     */
/*                                                            */
/* Class:     CLIPSJNI_Environment                            */
/* Method:    getFactSlot                                     */
/* Signature: (LCLIPSJNI/Environment;JJLjava/lang/String;)    */
/*            LCLIPSJNI/PrimitiveValue;                       */
/**************************************************************/
JNIEXPORT jobject JNICALL Java_CLIPSJNI_Environment_getFactSlot(
  JNIEnv *env,
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv,
  jlong clipsFact,
  jstring slotName)
  {
   jobject rv;
   DATA_OBJECT theDO;
   void *theCLIPSEnv = JLongToPointer(clipsEnv);
   const char *cSlotName = env->GetStringUTFChars(slotName,NULL);

   void *oldContext = SetEnvironmentContext(theCLIPSEnv,(void *) env);
   
   EnvGetFactSlot(JLongToPointer(clipsEnv),JLongToPointer(clipsFact),(const char *) cSlotName,&theDO);

   env->ReleaseStringUTFChars(slotName,cSlotName);
   
   rv = ConvertDataObject(env,javaEnv,theCLIPSEnv,&theDO);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
   
   return rv;
  }

/**********************************************************/
/* Java_CLIPSJNI_Environment_incrementFactCount: Native   */
/*   function for the CLIPSJNI incrementFactCount method. */
/*                                                        */
/*                                                        */
/* Class:     CLIPSJNI_Environment                        */
/* Method:    incrementFactCount                          */
/* Signature: (LCLIPSJNI/Environment;JJ)V                 */
/**********************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_incrementFactCount(
  JNIEnv *env, 
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv, 
  jlong clipsFact)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   EnvIncrementFactCount(JLongToPointer(clipsEnv),JLongToPointer(clipsFact));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }
  
/**********************************************************/
/* Java_CLIPSJNI_Environment_decrementFactCount: Native   */
/*   function for the CLIPSJNI decrementFactCount method. */
/*                                                        */
/*                                                        */
/* Class:     CLIPSJNI_Environment                        */
/* Method:    decrementFactCount                          */
/* Signature: (LCLIPSJNI/Environment;JJ)V                 */
/**********************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_decrementFactCount(
  JNIEnv *env, 
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv, 
  jlong clipsFact)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   EnvDecrementFactCount(JLongToPointer(clipsEnv),JLongToPointer(clipsFact));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }
  
/*******************************************************************/
/* Java_CLIPSJNI_Environment_makeInstance: Native function for the */
/*   CLIPSJNI makeInstance method.                                 */
/*                                                                 */
/* Class:     CLIPSJNI_Environment                                 */
/* Method:    makeInstance                                         */
/* Signature: (JLjava/lang/String;)LCLIPSJNI/InstanceAddressValue; */
/*******************************************************************/
JNIEXPORT jobject JNICALL Java_CLIPSJNI_Environment_makeInstance(
  JNIEnv *env,
  jobject obj, 
  jlong clipsEnv, 
  jstring instanceStr)
  {
   jobject rv;
   void *theInstance;
   void *theCLIPSEnv = JLongToPointer(clipsEnv);
   char *cInstanceStr = const_cast<char*>(env->GetStringUTFChars(instanceStr,NULL));
   
   void *oldContext = SetEnvironmentContext(theCLIPSEnv,(void *) env);

   theInstance = EnvMakeInstance(theCLIPSEnv,cInstanceStr);

   env->ReleaseStringUTFChars(instanceStr,cInstanceStr);
   
   if (theInstance == NULL)
     {
      SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
      return(NULL); 
     }
     
   rv = ConvertSingleFieldValue(env,obj,theCLIPSEnv,INSTANCE_ADDRESS,theInstance);
      
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/***********************************************************/
/* Java_CLIPSJNI_Environment_getInstanceName: Native       */
/*   function for the CLIPSJNI getInstanceName method.     */
/*                                                         */
/* Class:     CLIPSJNI_Environment                         */
/* Method:    getInstanceName                              */
/* Signature: (LCLIPSJNI/Environment;JJ)Ljava/lang/String; */
/***********************************************************/
JNIEXPORT jstring JNICALL Java_CLIPSJNI_Environment_getInstanceName(
  JNIEnv *env, 
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv, 
  jlong clipsInstance)
  {
   jstring rv;
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   rv = env->NewStringUTF(EnvGetInstanceName(JLongToPointer(clipsEnv),JLongToPointer(clipsInstance)));

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/**************************************************************/
/* Java_CLIPSJNI_Environment_incrementInstanceCount: Native   */
/*   function for the CLIPSJNI incrementInstanceCount method. */
/*                                                            */
/*                                                            */
/* Class:     CLIPSJNI_Environment                            */
/* Method:    incrementInstanceCount                          */
/* Signature: (LCLIPSJNI/Environment;JJ)V                     */
/**************************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_incrementInstanceCount(
  JNIEnv *env, 
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv, 
  jlong clipsInstance)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   EnvIncrementInstanceCount(JLongToPointer(clipsEnv),JLongToPointer(clipsInstance));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }
  
/**************************************************************/
/* Java_CLIPSJNI_Environment_decrementInstanceCount: Native   */
/*   function for the CLIPSJNI decrementInstanceCount method. */
/*                                                            */
/*                                                            */
/* Class:     CLIPSJNI_Environment                            */
/* Method:    decrementInstanceCount                          */
/* Signature: (LCLIPSJNI/Environment;JJ)V                     */
/**************************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_decrementInstanceCount(
  JNIEnv *env, 
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv, 
  jlong clipsInstance)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   EnvDecrementInstanceCount(JLongToPointer(clipsEnv),JLongToPointer(clipsInstance));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/**************************************************************/
/* Java_CLIPSJNI_Environment_directGetSlot: Native function   */
/*   for the CLIPSJNI directGetSlot method.                   */
/*                                                            */
/* Class:     CLIPSJNI_Environment                            */
/* Method:    directGetSlot                                   */
/* Signature: (LCLIPSJNI/Environment;JJLjava/lang/String;)    */
/*            LCLIPSJNI/PrimitiveValue;                       */
/**************************************************************/
JNIEXPORT jobject JNICALL Java_CLIPSJNI_Environment_directGetSlot(
  JNIEnv *env,
  jclass javaClass, 
  jobject javaEnv,
  jlong clipsEnv,
  jlong clipsInstance,
  jstring slotName)
  {
   jobject rv;
   DATA_OBJECT theDO;
   void *theCLIPSEnv = JLongToPointer(clipsEnv);
   const char *cSlotName = env->GetStringUTFChars(slotName,NULL);

   void *oldContext = SetEnvironmentContext(theCLIPSEnv,(void *) env);
   
   EnvDirectGetSlot(JLongToPointer(clipsEnv),JLongToPointer(clipsInstance),(const char *) cSlotName,&theDO);

   env->ReleaseStringUTFChars(slotName,cSlotName);
   
   rv = ConvertDataObject(env,javaEnv,theCLIPSEnv,&theDO);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
   
   return rv;
  }

/**********************************************************/
/* Java_CLIPSJNI_Environment_destroyEnvironment: Native   */
/*   function for the CLIPSJNI destroyEnvironment method. */
/*                                                        */
/* Class:     CLIPSJNI_Environment                        */
/* Method:    destroyEnvironment                          */
/* Signature: (J)V                                        */
/**********************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_destroyEnvironment(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv)
  {
   SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   DestroyEnvironment(JLongToPointer(clipsEnv));
  }

/***************************************************/
/* Java_CLIPSJNI_Environment_commandLoop: Native   */
/*   function for the CLIPSJNI commandLoop method. */
/*                                                 */
/* Class:     CLIPSJNI_Environment                 */
/* Method:    commandLoop                          */
/* Signature: (J)V                                 */
/***************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_commandLoop(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv)
  {
   SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   CommandLoop(JLongToPointer(clipsEnv));
  }

/****************************************************************/
/* Java_CLIPSJNI_Environment_commandLoopOnceThenBatch: Native   */
/*   function for the CLIPSJNI commandLoopOnceThenBatch method. */
/*                                                              */
/* Class:     CLIPSJNI_Environment                              */
/* Method:    commandLoopOnceThenBatch                          */
/* Signature: (J)V                                              */
/****************************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_commandLoopOnceThenBatch(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv)
  {
   SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   CommandLoopOnceThenBatch(JLongToPointer(clipsEnv));
  }

/***************************************************/
/* Java_CLIPSJNI_Environment_printBanner: Native   */
/*   function for the CLIPSJNI printBanner method. */
/*                                                 */
/* Class:     CLIPSJNI_Environment                 */
/* Method:    printBanner                          */
/* Signature: (J)V                                 */
/***************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_printBanner(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   PrintBanner(JLongToPointer(clipsEnv));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/***************************************************/
/* Java_CLIPSJNI_Environment_printPrompt: Native   */
/*   function for the CLIPSJNI printPrompt method. */
/*                                                 */
/* Class:     CLIPSJNI_Environment                 */
/* Method:    printPrompt                          */
/* Signature: (J)V                                 */
/***************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_printPrompt(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   PrintPrompt(JLongToPointer(clipsEnv));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/******************************************************/
/* Java_CLIPSJNI_Environment_getInputBuffer: Native   */
/*   function for the CLIPSJNI getInputBuffer method. */
/*                                                    */
/* Class:     CLIPSJNI_Environment                    */
/* Method:    getInputBuffer                          */
/* Signature: (J)Ljava/lang/String;                   */
/******************************************************/
JNIEXPORT jstring JNICALL Java_CLIPSJNI_Environment_getInputBuffer(
  JNIEnv *env, 
  jobject obj, 
  jlong clipsEnv)
  {
   jstring rv;
   
   const char *command;
   
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   command = GetCommandString(JLongToPointer(clipsEnv));
   
   if (command == NULL)
     { 
      rv = env->NewStringUTF(""); 
      SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
      return rv;
     }
     
   rv = env->NewStringUTF(GetCommandString(JLongToPointer(clipsEnv)));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
   
   return rv;
  }

/******************************************************/
/* Java_CLIPSJNI_Environment_setInputBuffer: Native   */
/*   function for the CLIPSJNI setInputBuffer method. */
/*                                                    */
/* Class:     CLIPSJNI_Environment                    */
/* Method:    setInputBuffer                          */
/* Signature: (JLjava/lang/String;)V                  */
/******************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_setInputBuffer(
  JNIEnv *env, 
  jobject obj,
  jlong clipsEnv,
  jstring command)
  {
   const char *cCommand = env->GetStringUTFChars(command,NULL);

   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   SetCommandString(JLongToPointer(clipsEnv),(const char *) cCommand);
   
   env->ReleaseStringUTFChars(command,cCommand);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/***********************************************************/
/* Java_CLIPSJNI_Environment_getInputBufferCount: Native   */
/*   function for the CLIPSJNI getInputBufferCount method. */
/*                                                         */
/* Class:     CLIPSJNI_Environment                         */
/* Method:    getInputBufferCount                          */
/* Signature: (J)J                                         */
/***********************************************************/
JNIEXPORT jlong JNICALL Java_CLIPSJNI_Environment_getInputBufferCount(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv)
  {
   jlong rv;
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);
   
   rv = RouterData(JLongToPointer(clipsEnv))->CommandBufferInputCount;

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/***********************************************************/
/* Java_CLIPSJNI_Environment_setInputBufferCount: Native   */
/*   function for the CLIPSJNI setInputBufferCount method. */
/*                                                         */
/* Class:     CLIPSJNI_Environment                         */
/* Method:    setInputBufferCount                          */
/* Signature: (JJ)J                                        */
/***********************************************************/
JNIEXPORT jlong JNICALL Java_CLIPSJNI_Environment_setInputBufferCount(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv,
  jlong theValue)
  {
   jlong rv;
   
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);

   rv = RouterData(JLongToPointer(clipsEnv))->CommandBufferInputCount;
   
   RouterData(JLongToPointer(clipsEnv))->CommandBufferInputCount = theValue;

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/*********************************************************/
/* Java_CLIPSJNI_Environment_appendInputBuffer: Native   */
/*   function for the CLIPSJNI appendInputBuffer method. */
/*                                                       */
/* Class:     CLIPSJNI_Environment                       */
/* Method:    appendInputBuffer                          */
/* Signature: (JLjava/lang/String;)V                     */
/*********************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_appendInputBuffer(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv,
  jstring commandString)
  {
   const char *cCommandString = env->GetStringUTFChars(commandString,NULL);
   
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);
   
   AppendCommandString(JLongToPointer(clipsEnv),(const char *) cCommandString);

   env->ReleaseStringUTFChars(commandString,cCommandString);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/*********************************************************/
/* Java_CLIPSJNI_Environment_expandInputBuffer: Native   */
/*   function for the CLIPSJNI expandInputBuffer method. */
/*                                                       */
/* Class:     CLIPSJNI_Environment                       */
/* Method:    expandInputBuffer                          */
/* Signature: (JC)V                                      */
/*********************************************************/
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_expandInputBuffer(
  JNIEnv *env,
  jobject obj,
  jlong clipsEnv,
  jchar theChar)
  {
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);
   
   ExpandCommandString(JLongToPointer(clipsEnv),(int) theChar);
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
  }

/******************************************************************/
/* Java_CLIPSJNI_Environment_inputBufferContainsCommand: Native   */
/*   function for the CLIPSJNI inputBufferContainsCommand method. */
/*                                                                */
/* Class:     CLIPSJNI_Environment                                */
/* Method:    inputBufferContainsCommand                          */
/* Signature: (J)Z                                                */
/******************************************************************/
JNIEXPORT jboolean JNICALL Java_CLIPSJNI_Environment_inputBufferContainsCommand(
  JNIEnv *env,
  jobject obj, 
  jlong clipsEnv)
  {
   jboolean rv;
   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);
  
   rv = CommandCompleteAndNotEmpty(JLongToPointer(clipsEnv));
   
   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);
   
   return rv;
  }

/*******************************************************/
/* Java_CLIPSJNI_Environment_addRouter: Native         */
/*   function for the CLIPSJNI addRouter method.       */
/*                                                     */
/* Class:     CLIPSJNI_Environment                     */
/* Method:    addRouter                                */
/* Signature: (JLjava/lang/String;ILCLIPSJNI/Router;)Z */
/*******************************************************/
JNIEXPORT jboolean JNICALL Java_CLIPSJNI_Environment_addRouter(
  JNIEnv *env, 
  jobject obj, 
  jlong clipsEnv, 
  jstring routerName, 
  jint priority, 
  jobject context)
  {
   int rv;
   jclass cls;
   jobject nobj;   
   const char *cRouterName = env->GetStringUTFChars(routerName,NULL);

   void *oldContext = SetEnvironmentContext(JLongToPointer(clipsEnv),(void *) env);
      
   nobj = env->NewGlobalRef(context); /* TBD Need to deallocate when environment or router destroyed */
   
   cls = env->GetObjectClass(context);

   rv = EnvAddRouterWithContext(JLongToPointer(clipsEnv),(const char *) cRouterName,(int) priority,
                                QueryJNIRouter,PrintJNIRouter,GetcJNIRouter,
                                UngetcJNIRouter,ExitJNIRouter,(void *) nobj);
   
   env->ReleaseStringUTFChars(routerName,cRouterName);

   SetEnvironmentContext(JLongToPointer(clipsEnv),oldContext);

   return rv;
  }

/******************/
/* JLongToPointer */
/******************/
static void *JLongToPointer(
  jlong value)
  {
   return (void *) value;
  }


/******************/
/* PointerToJLong */
/******************/
static jlong PointerToJLong(
  void *value)
  {
   return (jlong) value;
  }

/*******************************************************/
/* PrintJavaAddress:  */
/*******************************************************/
static void PrintJavaAddress(
  void *theEnv,
  const char *logicalName,
  void *theValue)
  {
   jobject theObject;
   jclass cls;
   jstring str;
   const char *cStr;
   JNIEnv *env;
   char buffer[20];

   EnvPrintRouter(theEnv,logicalName,"<Pointer-");
        
   theObject = (jobject) ValueToExternalAddress(theValue);
   
   if (theObject != NULL)
     {
      env = (JNIEnv *) GetEnvironmentContext(theEnv);
      
      cls = env->GetObjectClass(theObject);
      
      str = (jstring) env->CallObjectMethod(cls,CLIPSJNIData(theEnv)->classGetCanonicalNameMethod);

      cStr = (const char *) env->GetStringUTFChars(str,NULL);
      
      EnvPrintRouter(theEnv,logicalName,cStr);
      EnvPrintRouter(theEnv,logicalName,"-");
   
      env->ReleaseStringUTFChars(str,cStr);
     }
   else
     { EnvPrintRouter(theEnv,logicalName,"java-"); }
   
   gensprintf(buffer,"%p",(void *) theObject);
   EnvPrintRouter(theEnv,logicalName,buffer);
   EnvPrintRouter(theEnv,logicalName,">");
  }

/*******************************************************/
/* NewJavaAddress:  */
/*******************************************************/
static void NewJavaAddress(
  void *theEnv,
  DATA_OBJECT *rv)
  {
   jclass theClass, tempClass;
   int numberOfArguments;
   JNIEnv *env;
   const char *className;
   char *classDescriptor;
   DATA_OBJECT theValue;
   size_t i, length;
   jmethodID mid;
   jobjectArray constructorList, parameterList;
   jsize theSize, c; 
   jsize paramCount, p; 
   jobject theConstructor, theObject, oldObject; 
   int found = FALSE, matches;
   DATA_OBJECT_PTR newArgs;
   jvalue *javaArgs;
   
   /*=============================================*/
   /* Retrieve the JNI environment pointer stored */
   /* in the CLIPS environment structure.         */
   /*=============================================*/
   
   env = (JNIEnv *) GetEnvironmentContext(theEnv);

   /*======================================================================*/
   /* If the Java external address type is used, additional arguments must */
   /* at least include the Java class name of the object to be created.    */
   /*======================================================================*/
   
   if ((numberOfArguments = EnvArgCountCheck(theEnv,"new (with type Java)",AT_LEAST,2)) == -1) 
     { return; }
   
   /*=======================================*/
   /* The Java class name must be a symbol. */
   /*=======================================*/
   
   if (EnvArgTypeCheck(theEnv,"new (with type Java)",2,SYMBOL,&theValue) == FALSE) 
     { return; }
   
   className = DOToString(theValue);
   
   /*=============================================*/
   /* Construct the class descriptor by replacing */
   /* any periods (.) in the class name with a    */
   /* forward slash (/).                          */
   /*=============================================*/
   
   length = strlen(className);
   classDescriptor = (char*) genalloc(theEnv,length + 1);
   for (i = 0; i < length; i++)
     {
      if (className[i] != '.')
        { classDescriptor[i] = className[i]; }
      else 
        { classDescriptor[i] = '/'; }
     }
   classDescriptor[i] = 0;
   
   /*=======================*/
   /* Search for the class. */
   /*=======================*/
   
   theClass = env->FindClass(classDescriptor); 
   
   /*========================================*/
   /* Free the constructed class descriptor. */
   /*========================================*/
   
   genfree(theEnv,classDescriptor,length + 1);

   /*============================================*/
   /* Signal an error if the class wasn't found. */
   /*============================================*/
   
   if (theClass == NULL)
     {
      if (env->ExceptionOccurred())
        { env->ExceptionClear(); }
      SetEvaluationError(theEnv,TRUE);
      ExpectedTypeError1(theEnv,"new (with type Java)",2,"Java class name");
      return;
     }
      
   /*===========================================================================*/
   /* Evaluate the CLIPS arguments that will be passed to the java constructor. */
   /*===========================================================================*/
   
   if (numberOfArguments - 2 == 0)
     { newArgs = NULL; }
   else
     {
      newArgs = (DATA_OBJECT_PTR) genalloc(theEnv,sizeof(DATA_OBJECT) * (numberOfArguments - 2));
      for (i = 0; i < numberOfArguments - 2; i++)
        {
         EnvRtnUnknown(theEnv,i+3,&newArgs[i]);
         if (GetEvaluationError(theEnv))
           {   
            env->DeleteLocalRef(theClass);
            return;
           }
        }
     }

   /*========================================================================*/
   /* Construct an array in which to store the corresponding java arguments. */
   /*========================================================================*/
   
   if (numberOfArguments - 2 == 0)
     { javaArgs = NULL; }
   else
     { javaArgs = (jvalue *) genalloc(theEnv,sizeof(jvalue) * (numberOfArguments - 2)); }
   
   /*=============================================*/
   /* Get the method index of the getConstructors */
   /* method from the java.lang.Class class.      */
   /*=============================================*/

   tempClass = env->FindClass("java/lang/Class");
   mid = env->GetMethodID(tempClass,"getConstructors","()[Ljava/lang/reflect/Constructor;"); 
   env->DeleteLocalRef(tempClass);
   
   /*=======================================================*/
   /* Get the list of constructors for the specified class. */
   /*=======================================================*/
     
   constructorList = (jobjectArray) env->CallObjectMethod(theClass,mid);

   /*======================================================*/
   /* Get the method index of the getParameterTypes method */
   /* from the java.lang.reflect.Constructor class.        */
   /*======================================================*/

   tempClass = env->FindClass("java/lang/reflect/Constructor"); 
   mid = env->GetMethodID(tempClass,"getParameterTypes","()[Ljava/lang/Class;"); 
   env->DeleteLocalRef(tempClass);

   /*===============================================*/
   /* Search the constructor list for a constructor */
   /* with matching arguments.                      */
   /*===============================================*/
      
   theSize = env->GetArrayLength(constructorList); 
   for (c = 0; c < theSize; c++) 
     { 
      theConstructor = env->GetObjectArrayElement(constructorList,c); 
      
      parameterList = (jobjectArray) env->CallObjectMethod(theConstructor,mid);
      
      paramCount = env->GetArrayLength(parameterList); 
      
      if (paramCount != (numberOfArguments - 2))
        { continue; }
        
      matches = TRUE;
      
      for (p = 0; (p < paramCount) && matches; p++)
        {
         jstring str;
         const char *cStr;
   
         tempClass = (jclass) env->GetObjectArrayElement(parameterList,p);
         
         str = (jstring) env->CallObjectMethod(tempClass,CLIPSJNIData(theEnv)->classGetCanonicalNameMethod);

         cStr = (const char *) env->GetStringUTFChars(str,NULL);
                  
         if (GetType(newArgs[p]) == INTEGER)
           {
            if (strcmp(cStr,"long") == 0)
              { 
               printf("p[%d] = %s\n",(int) p,cStr);
               javaArgs[p].j = DOToLong(newArgs[p]);
              }
            else if (strcmp(cStr,"int") == 0)  
              { 
               printf("p[%d] = %s\n",(int) p,cStr);
               javaArgs[p].i = DOToLong(newArgs[p]);
              }
            else
              { matches = FALSE; }
           }
         else
           { matches = FALSE; }
         
         env->ReleaseStringUTFChars(str,cStr);
      
         env->DeleteLocalRef(tempClass);
        }
      
      if (matches)
        { 
         found = TRUE;
         break; 
        }
     } 

   /*==========================================*/
   /* If an appropriate constructor was found, */
   /* invoke it to create a new java object.   */
   /*==========================================*/
   
   theObject = NULL;
   if (found)
     {
      if (paramCount == 0)
        {
         mid = env->FromReflectedMethod(theConstructor);
         theObject = env->NewObject(theClass,mid);
        }
      else
        {
         mid = env->FromReflectedMethod(theConstructor);
         theObject = env->NewObjectA(theClass,mid,javaArgs);
        }
     }
 
   /*========================================================*/
   /* Delete the local reference to the class of the object. */
   /*========================================================*/
   
   env->DeleteLocalRef(theClass);

   /*=========================================================*/
   /* If the object was created, add a global reference to it */
   /* and delete the local reference. This will prevent the   */
   /* object from being garbage collected until CLIPS deletes */
   /* the global reference.                                   */
   /*=========================================================*/
   
   if (theObject != NULL)
     {
      oldObject = theObject;
      theObject = env->NewGlobalRef(theObject);
      env->DeleteLocalRef(oldObject);
     }

   /*=============================================*/
   /* Return the array containing the DATA_OBJECT */
   /* arguments to the new function.              */
   /*=============================================*/
   
   if (newArgs != NULL)
     { genfree(theEnv,newArgs,sizeof(DATA_OBJECT) * (numberOfArguments - 2)); }
     
   if (javaArgs != NULL)
     { genfree(theEnv,javaArgs,sizeof(jvalue) * (numberOfArguments - 2)); }
   
   /*=============================================*/
   /* If a java exception occurred, set the CLIPS */
   /* error flag and clear the java exception.    */
   /*=============================================*/
   
   if (env->ExceptionOccurred())
     { 
      SetEvaluationError(theEnv,TRUE);
      env->ExceptionClear(); 
     }

   /*==========================================*/
   /* Return the newly created java object if  */
   /* it was successfully created, otherwise   */
   /* leave the default return value of FALSE. */
   /*==========================================*/
   
   if (theObject != NULL)
     {
      SetpType(rv,EXTERNAL_ADDRESS);
      SetpValue(rv,EnvAddExternalAddress(theEnv,theObject,CLIPSJNIData(theEnv)->javaExternalAddressID));
     }
  }

/*******************************************************/
/* CallJavaMethod:  */
/*******************************************************/
static intBool CallJavaMethod(
  void *theEnv,
  DATA_OBJECT *target,
  DATA_OBJECT *rv)
  {
   int numberOfArguments;
   jobject theObject, theMethod;
   jclass objectClass, tempClass;
   jmethodID mid, getNameID, getReturnTypeID;
   JNIEnv *env;
   jobjectArray methodList, parameterList;
   jsize theSize, c; 
   jsize paramCount, p; 
   DATA_OBJECT theValue;
   const char *methodName;
   jstring str;
   const char *cStr;
   int found = FALSE, matches;
   DATA_OBJECT_PTR newArgs;
   jvalue *javaArgs;
   int i;
   
   /*=============================================*/
   /* Retrieve the JNI environment pointer stored */
   /* in the CLIPS environment structure.         */
   /*=============================================*/
   
   env = (JNIEnv *) GetEnvironmentContext(theEnv);

   /*=================================================================*/
   /* If the Java external address type is used, additional arguments */
   /* must at least include the name of the method being called.      */
   /*=================================================================*/
   
   if ((numberOfArguments = EnvArgCountCheck(theEnv,"call (with type Java)",AT_LEAST,2)) == -1) 
     { return FALSE; }

   /*========================================*/
   /* The Java method name must be a symbol. */
   /*========================================*/
   
   if (EnvArgTypeCheck(theEnv,"call (with type Java)",2,SYMBOL,&theValue) == FALSE) 
     { return FALSE; }
   
   methodName = DOToString(theValue);

   /*===========================================================================*/
   /* Evaluate the CLIPS arguments that will be passed to the java constructor. */
   /*===========================================================================*/
   
   if (numberOfArguments - 2 == 0)
     { newArgs = NULL; }
   else
     {
      newArgs = (DATA_OBJECT_PTR) genalloc(theEnv,sizeof(DATA_OBJECT) * (numberOfArguments - 2));
      for (i = 0; i < numberOfArguments - 2; i++)
        {
         EnvRtnUnknown(theEnv,i+3,&newArgs[i]);
         if (GetEvaluationError(theEnv))
           { return FALSE; }
        }
     }

   /*========================================================================*/
   /* Construct an array in which to store the corresponding java arguments. */
   /*========================================================================*/
   
   if (numberOfArguments - 2 == 0)
     { javaArgs = NULL; }
   else
     { javaArgs = (jvalue *) genalloc(theEnv,sizeof(jvalue) * (numberOfArguments - 2)); }

   /*===============================================*/
   /* If the target is an external address, then we */
   /* should be invoking a method of an instance.   */
   /*===============================================*/

   if (GetpType(target) == EXTERNAL_ADDRESS)
     {
      theObject = reinterpret_cast<jobject>(DOPToExternalAddress(target));

      /*=========================================*/
      /* Determine the class of the java object. */
      /*=========================================*/
      
      objectClass = env->GetObjectClass(theObject);

      /*=============================================*/
      /* Get the method index of the getConstructors */
      /* method from the java.lang.Class class.      */
      /*=============================================*/

      tempClass = env->FindClass("java/lang/Class"); /* TBD Cache this Value */
      mid = env->GetMethodID(tempClass,"getMethods","()[Ljava/lang/reflect/Method;"); 
      env->DeleteLocalRef(tempClass);

      /*==================================================*/
      /* Get the list of methods for the specified class. */
      /*==================================================*/
     
      methodList = (jobjectArray) env->CallObjectMethod(objectClass,mid);
      env->DeleteLocalRef(objectClass);

      /*======================================================*/
      /* Get the method index of the getParameterTypes method */
      /* from the java.lang.reflect.Method class.             */
      /*======================================================*/

      tempClass = env->FindClass("java/lang/reflect/Method"); /* TBD Cache this Value */
      mid = env->GetMethodID(tempClass,"getParameterTypes","()[Ljava/lang/Class;"); 
      getNameID = env->GetMethodID(tempClass,"getName","()Ljava/lang/String;"); 
      getReturnTypeID = env->GetMethodID(tempClass,"getReturnType","()Ljava/lang/Class;"); 
      env->DeleteLocalRef(tempClass);

      /*=====================================*/
      /* Search the method list for a method */
      /* with matching arguments.            */
      /*=====================================*/

      theSize = env->GetArrayLength(methodList); 
      for (c = 0; c < theSize; c++) 
        { 
         theMethod = env->GetObjectArrayElement(methodList,c); 
         str = (jstring) env->CallObjectMethod(theMethod,getNameID);
         cStr = (const char *) env->GetStringUTFChars(str,NULL);
         
         /*===================================*/
         /* If the method name doesn't match, */
         /* move on to the next method.       */
         /*===================================*/
         
         if (strcmp(methodName,cStr) != 0)
           {
            env->ReleaseStringUTFChars(str,cStr);
            continue;
           }
         env->ReleaseStringUTFChars(str,cStr);
         
         /*==========================================*/
         /* Get the parameter list of the method and */
         /* determine the number of parameters.      */
         /*==========================================*/
         
         parameterList = (jobjectArray) env->CallObjectMethod(theMethod,mid);
      
         paramCount = env->GetArrayLength(parameterList); 
      
         if (paramCount != (numberOfArguments - 2))
           { 
            env->ReleaseStringUTFChars(str,cStr);
            continue; 
           }

         matches = TRUE;
      
         for (p = 0; (p < paramCount) && matches; p++)
           {
            tempClass = (jclass) env->GetObjectArrayElement(parameterList,p);
         
            str = (jstring) env->CallObjectMethod(tempClass,CLIPSJNIData(theEnv)->classGetCanonicalNameMethod);

            cStr = (const char *) env->GetStringUTFChars(str,NULL);
             
            printf("p[%d] = %s\n",(int) p,cStr);
            
            if (GetType(newArgs[p]) == INTEGER)
              {
               if (strcmp(cStr,"long") == 0)
                 { 
                  /* printf("p[%d] = %s\n",(int) p,cStr); */
                  javaArgs[p].j = DOToLong(newArgs[p]);
                 }
               else if (strcmp(cStr,"int") == 0)  
                 { 
                  /* printf("p[%d] = %s\n",(int) p,cStr); */
                  javaArgs[p].i = DOToLong(newArgs[p]);
                 }
               else
                 { matches = FALSE; }
              }
            else
              { matches = FALSE; }

            env->ReleaseStringUTFChars(str,cStr);
         
            env->DeleteLocalRef(tempClass);
           }
      
         if (matches)
           {
            jobject returnType = env->CallObjectMethod(theMethod,getReturnTypeID); 
            jstring returnTypeNameStr = (jstring) env->CallObjectMethod(returnType,CLIPSJNIData(theEnv)->classGetCanonicalNameMethod);
            const char* returnTypeName = (const char*) env->GetStringUTFChars(returnTypeNameStr,NULL);
            if (strcmp(returnTypeName, "boolean") == 0) {
             jboolean result = env->CallBooleanMethodA(theObject, env->FromReflectedMethod(theMethod), javaArgs);
             EnvSetpType(theEnv,rv,SYMBOL);
             EnvSetpValue(theEnv,rv,result ? EnvTrueSymbol(theEnv) : EnvFalseSymbol(theEnv));
            }
            else if (strcmp(returnTypeName, "byte") == 0) {
             jint result = env->CallByteMethodA(theObject, env->FromReflectedMethod(theMethod), javaArgs);
             EnvSetpType(theEnv,rv,INTEGER);
             EnvSetpValue(theEnv,rv,EnvAddLong(theEnv,result));
            }
            else if (strcmp(returnTypeName, "char") == 0) {
             jchar result = env->CallCharMethodA(theObject, env->FromReflectedMethod(theMethod), javaArgs);
             char asString[] = { result, '\0' };
             EnvSetpType(theEnv,rv,STRING);
             EnvSetpValue(theEnv,rv,EnvAddSymbol(theEnv,asString));
            }
            else if (strcmp(returnTypeName, "double") == 0) {
             jdouble result = env->CallDoubleMethodA(theObject, env->FromReflectedMethod(theMethod), javaArgs);
             EnvSetpType(theEnv,rv,FLOAT);
             EnvSetpValue(theEnv,rv,EnvAddDouble(theEnv,result));
            }
            else if (strcmp(returnTypeName, "float") == 0) {
             jfloat result = env->CallFloatMethodA(theObject,
                                                   env->FromReflectedMethod(theMethod),
                                                   javaArgs);
             EnvSetpType(theEnv,rv,FLOAT);
             EnvSetpValue(theEnv,rv,EnvAddDouble(theEnv,result));
            }
            else if (strcmp(returnTypeName, "int") == 0) {
             jint result = env->CallIntMethodA(theObject,
                                               env->FromReflectedMethod(theMethod),
                                               javaArgs);
             EnvSetpType(theEnv,rv,INTEGER);
             EnvSetpValue(theEnv,rv,EnvAddLong(theEnv,result));
            }
            else if (strcmp(returnTypeName, "long") == 0) {
             jlong result = env->CallLongMethodA(theObject,
                                                 env->FromReflectedMethod(theMethod),
                                                 javaArgs);
             EnvSetpType(theEnv,rv,INTEGER);
             EnvSetpValue(theEnv,rv,EnvAddLong(theEnv,result));
            }
            else if (strcmp(returnTypeName, "short") == 0) {
             jshort result = env->CallShortMethodA(theObject,
                                                   env->FromReflectedMethod(theMethod),
                                                   javaArgs);
             EnvSetpType(theEnv,rv,INTEGER);
             EnvSetpValue(theEnv,rv,EnvAddLong(theEnv,result));
            }
            else if (strcmp(returnTypeName, "void") == 0) {
             env->CallVoidMethodA(theObject, env->FromReflectedMethod(theMethod), javaArgs);
             // there doesn't seem to be a void type, so just return TRUE
             EnvSetpType(theEnv,rv,SYMBOL);
             EnvSetpValue(theEnv,rv,EnvTrueSymbol(theEnv));
            }
            else {
             // returns a Java object
             jobject result = env->CallObjectMethodA(theObject,
                                                     env->FromReflectedMethod(theMethod),
                                                     javaArgs);
             void* externalAddr;
             if (result != NULL) {
              result = env->NewGlobalRef(result);
              externalAddr = EnvAddExternalAddress
               (theEnv,result, CLIPSJNIData(theEnv)->javaExternalAddressID);
             }
             else {
              externalAddr = NULL;
             }
             EnvSetpType(theEnv,rv,EXTERNAL_ADDRESS);
             EnvSetpValue(theEnv,rv,externalAddr);
            }
            found = TRUE;
            break; 
           }
        }
     }

   if (newArgs != NULL)
     { genfree(theEnv,newArgs,sizeof(DATA_OBJECT) * (numberOfArguments - 2)); }

   if (javaArgs != NULL)
     { genfree(theEnv,javaArgs,sizeof(jvalue) * (numberOfArguments - 2)); }
     
   return TRUE;
  }
  
/*******************************************************/
/* DiscardJavaAddress: */
/*******************************************************/
static intBool DiscardJavaAddress(
  void *theEnv,
  void* theValue)
  {
   JNIEnv *env;

   printf("Discarding Java Address %p\n",theValue);
   
   if (theValue != NULL)
     {
      env = (JNIEnv *) GetEnvironmentContext(theEnv);
      env->DeleteGlobalRef((jobject) theValue);
     }
   
   return TRUE;
  }
