
#include "CLIPSJNI_Environment.h"

#include "clips.h"

#include <cstddef>
#include <cstring>
#include <cstdio>

/* These error numbers are for the PrintErrorID function. Calls to it in the CLIPS core
 * just use integer literals, so there's no explanation for what they should be.
 * This value is chosen to avoid conflict with existing ones
 * (and so masking with 0xff will give a "java" error number.)*/

#define JAVA_UNKNOWN_ERROR 257

#define JAVA_MODULE "JAVA"

#define CLIPSJNI_DATA 67

struct clipsJNIData
  { 
   int javaExternalAddressID;

   jclass objectClass;

   jclass classClass;
   jmethodID classGetCanonicalNameMethod;
   jmethodID objectToStringMethod;
   
   jclass booleanClass;
   jmethodID booleanValueMethod;
   
   jclass byteClass;
   jmethodID byteInitMethod;
   jclass shortClass;
   jmethodID shortInitMethod;
   jclass intClass;
   jmethodID intInitMethod;
   jmethodID intValueMethod;
   jclass longClass;
   jmethodID longInitMethod;
   jmethodID longValueMethod;

   jclass floatClass;
   jmethodID floatInitMethod;
   jmethodID floatValueMethod;
   jclass doubleClass;
   jmethodID doubleInitMethod;
   jmethodID doubleValueMethod;

   jclass stringClass;

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

   jclass javaObjectValueClass;
   jmethodID javaObjectValueInitMethod;

   jclass expressionClass;
   jmethodID expressionInitMethod;
   jmethodID expressionExecuteMethod;
   jmethodID expressionGetValueMethod;
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
static void       PrintJavaObject(void *,const char *,void *);
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
  
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->objectClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->classClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->booleanClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->byteClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->shortClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->intClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->longClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->floatClass);
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
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->javaObjectValueClass);
   env->DeleteGlobalRef(CLIPSJNIData(theEnv)->expressionClass);
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
  __attribute__((unused))
  void *theEnv,
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
  jobject obj)
  {
   void *theEnv;
   jclass theObjectClass; 
   jclass theClassClass; 
   jmethodID theClassGetCanonicalNameMethod;
   jmethodID theObjectToStringMethod;
   jclass theBooleanClass;
   jmethodID theBooleanValueMethod;
   jclass theByteClass;
   jmethodID theByteInitMethod;
   jclass theShortClass;
   jmethodID theShortInitMethod;
   jclass theIntClass;
   jmethodID theIntInitMethod;
   jmethodID theIntValueMethod;
   jclass theLongClass; 
   jmethodID theLongInitMethod;
   jmethodID theLongValueMethod;
   jclass theFloatClass;
   jmethodID theFloatInitMethod;
   jmethodID theFloatValueMethod;
   jclass theDoubleClass;
   jmethodID theDoubleInitMethod;
   jmethodID theDoubleValueMethod;
   jclass theStringClass; 
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
   jclass theJavaObjectValueClass;
   jmethodID theJavaObjectValueInitMethod;
   jclass theExpressionClass;
   jmethodID theExpressionInitMethod, theExpressionExecuteMethod, theExpressionGetValueMethod;
   
   struct externalAddressType javaPointer = { "java", PrintJavaAddress, PrintJavaObject, DiscardJavaAddress, NewJavaAddress, CallJavaMethod };

   /*===========================*/
   /* Look up the Java classes. */
   /*===========================*/

   theObjectClass = env->FindClass("java/lang/Object"); 
   theClassClass = env->FindClass("java/lang/Class"); 
   theBooleanClass = env->FindClass("java/lang/Boolean");
   theByteClass = env->FindClass("java/lang/Byte");
   theShortClass = env->FindClass("java/lang/Short");
   theIntClass = env->FindClass("java/lang/Integer");
   theLongClass = env->FindClass("java/lang/Long");
   theFloatClass = env->FindClass("java/lang/Float");
   theDoubleClass = env->FindClass("java/lang/Double");
   theStringClass = env->FindClass("java/lang/String"); 
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
   theJavaObjectValueClass = env->FindClass("CLIPSJNI/JavaObjectValue");
   theExpressionClass = env->FindClass("java/beans/Expression");
                
   /*=========================================*/
   /* If the Java classes could not be found, */
   /* abort creation of the environment.      */
   /*=========================================*/
   
   if ((theObjectClass == NULL) ||
       (theClassClass == NULL) ||
     (theBooleanClass == NULL) ||
     (theByteClass == NULL) ||
     (theShortClass == NULL) ||
     (theIntClass == NULL) ||
     (theLongClass == NULL) ||
     (theFloatClass == NULL) ||
       (theDoubleClass == NULL) ||
       (theStringClass == NULL) ||
       (theArrayListClass == NULL) ||
       (theVoidValueClass == NULL) ||
       (theIntegerValueClass == NULL) || (theFloatValueClass == NULL) ||
       (theSymbolValueClass == NULL) || (theStringValueClass == NULL) || 
       (theInstanceNameValueClass == NULL) ||
       (theMultifieldValueClass == NULL) ||
       (theFactAddressValueClass == NULL) ||
       (theInstanceAddressValueClass == NULL) ||
       (theJavaObjectValueClass == NULL) ||
       (theExpressionClass == NULL))
     { return((jlong) NULL); }
     
   /*===========================*/
   /* Look up the Java methods. */
   /*===========================*/
   
   theClassGetCanonicalNameMethod = env->GetMethodID(theClassClass,"getCanonicalName","()Ljava/lang/String;");
   theObjectToStringMethod = env->GetMethodID(theClassClass,"toString","()Ljava/lang/String;");
   theBooleanValueMethod = env->GetMethodID(theBooleanClass,"booleanValue","()Z");
   theByteInitMethod = env->GetMethodID(theByteClass,"<init>","(B)V");
   theShortInitMethod = env->GetMethodID(theShortClass,"<init>","(S)V");
   theIntInitMethod = env->GetMethodID(theIntClass,"<init>","(I)V");
   theIntValueMethod = env->GetMethodID(theIntClass,"intValue","()I");
   theLongInitMethod = env->GetMethodID(theLongClass,"<init>","(J)V");
   theLongValueMethod = env->GetMethodID(theLongClass,"longValue","()J");
   theFloatInitMethod = env->GetMethodID(theFloatClass,"<init>","(F)V");
   theFloatValueMethod = env->GetMethodID(theFloatClass,"floatValue","()F");
   theDoubleInitMethod = env->GetMethodID(theDoubleClass,"<init>","(D)V");
   theDoubleValueMethod = env->GetMethodID(theDoubleClass,"doubleValue","()D");
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
   theJavaObjectValueInitMethod = env->GetMethodID(theJavaObjectValueClass,"<init>","(Ljava/lang/Object;)V");
   theExpressionInitMethod = env->GetMethodID(theExpressionClass,"<init>","(Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/Object;)V");
   theExpressionExecuteMethod = env->GetMethodID(theExpressionClass,"execute","()V");
   theExpressionGetValueMethod = env->GetMethodID(theExpressionClass,"getValue","()Ljava/lang/Object;");

   /*==============================================*/
   /* If the Java init methods could not be found, */
   /* abort creation of the enviroment.            */
   /*==============================================*/
   
   if ((theClassGetCanonicalNameMethod == NULL) ||
       (theObjectToStringMethod == NULL) ||
       (theBooleanValueMethod == NULL) ||
       (theByteInitMethod == NULL) ||
       (theShortInitMethod == NULL) || (theFloatInitMethod == NULL) || 
       (theFloatValueMethod == NULL) ||
       (theLongInitMethod == NULL) || (theDoubleInitMethod == NULL) || 
       (theLongValueMethod == NULL) || (theDoubleValueMethod == NULL) ||
       (theArrayListInitMethod == NULL) || (theArrayListAddMethod == NULL) ||
       (theVoidValueInitMethod == NULL) ||
       (theIntegerValueInitMethod == NULL) || (theFloatValueInitMethod == NULL) ||
       (theSymbolValueInitMethod == NULL) || (theStringValueInitMethod == NULL) ||
       (theInstanceNameValueInitMethod == NULL) ||
       (theMultifieldValueInitMethod == NULL) ||
       (theFactAddressValueInitMethod == NULL) ||
       (theInstanceAddressValueInitMethod == NULL) ||
       (theJavaObjectValueInitMethod == NULL) ||
       (theExpressionInitMethod == NULL) ||
       (theExpressionExecuteMethod == NULL) ||
       (theExpressionGetValueMethod == NULL))
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

   CLIPSJNIData(theEnv)->objectClass = static_cast<jclass>(env->NewGlobalRef(theObjectClass));
   CLIPSJNIData(theEnv)->classClass = static_cast<jclass>(env->NewGlobalRef(theClassClass));
   CLIPSJNIData(theEnv)->classGetCanonicalNameMethod = theClassGetCanonicalNameMethod;
   CLIPSJNIData(theEnv)->objectToStringMethod = theObjectToStringMethod;

   CLIPSJNIData(theEnv)->booleanClass = static_cast<jclass>(env->NewGlobalRef(theBooleanClass));
   CLIPSJNIData(theEnv)->booleanValueMethod = theBooleanValueMethod;
   CLIPSJNIData(theEnv)->byteClass = static_cast<jclass>(env->NewGlobalRef(theByteClass));
   CLIPSJNIData(theEnv)->byteInitMethod = theByteInitMethod;
   CLIPSJNIData(theEnv)->shortClass = static_cast<jclass>(env->NewGlobalRef(theShortClass));
   CLIPSJNIData(theEnv)->shortInitMethod = theShortInitMethod;
   CLIPSJNIData(theEnv)->intClass = static_cast<jclass>(env->NewGlobalRef(theIntClass));
   CLIPSJNIData(theEnv)->intInitMethod = theIntInitMethod;
   CLIPSJNIData(theEnv)->intValueMethod = theIntValueMethod;
   CLIPSJNIData(theEnv)->longClass = static_cast<jclass>(env->NewGlobalRef(theLongClass));
   CLIPSJNIData(theEnv)->longInitMethod = theLongInitMethod;
   CLIPSJNIData(theEnv)->longValueMethod = theLongValueMethod;
   CLIPSJNIData(theEnv)->floatClass = static_cast<jclass>(env->NewGlobalRef(theFloatClass));
   CLIPSJNIData(theEnv)->floatInitMethod = theFloatInitMethod;
   CLIPSJNIData(theEnv)->floatValueMethod = theFloatValueMethod;
   CLIPSJNIData(theEnv)->doubleClass = static_cast<jclass>(env->NewGlobalRef(theDoubleClass));
   CLIPSJNIData(theEnv)->doubleInitMethod = theDoubleInitMethod;
   CLIPSJNIData(theEnv)->doubleValueMethod = theDoubleValueMethod;
   CLIPSJNIData(theEnv)->stringClass = static_cast<jclass>(env->NewGlobalRef(theStringClass));
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

   CLIPSJNIData(theEnv)->javaObjectValueClass = static_cast<jclass>(env->NewGlobalRef(theJavaObjectValueClass));
   CLIPSJNIData(theEnv)->javaObjectValueInitMethod = theJavaObjectValueInitMethod;
   
   CLIPSJNIData(theEnv)->expressionClass = static_cast<jclass>(env->NewGlobalRef(theExpressionClass));
   CLIPSJNIData(theEnv)->expressionInitMethod = theExpressionInitMethod;
   CLIPSJNIData(theEnv)->expressionExecuteMethod = theExpressionExecuteMethod;
   CLIPSJNIData(theEnv)->expressionGetValueMethod = theExpressionGetValueMethod;

   /*======================================*/
   /* Store the java environment for later */
   /* access by the CLIPS environment.     */
   /*======================================*/
   
   SetEnvironmentContext(theEnv,(void *) env);
     
   /*=======================================*/
   /* Deallocate the local Java references. */
   /*=======================================*/
   
   env->DeleteLocalRef(theObjectClass);
   env->DeleteLocalRef(theClassClass);
   env->DeleteLocalRef(theBooleanClass);
   env->DeleteLocalRef(theByteClass);
   env->DeleteLocalRef(theShortClass);
   env->DeleteLocalRef(theIntClass);
   env->DeleteLocalRef(theLongClass);
   env->DeleteLocalRef(theFloatClass);
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
   env->DeleteLocalRef(theJavaObjectValueClass);
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
      case EXTERNAL_ADDRESS:
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

     case EXTERNAL_ADDRESS:
          result = env->NewObject(
                                     CLIPSJNIData(clipsEnv)->javaObjectValueClass,
                                     CLIPSJNIData(clipsEnv)->javaObjectValueInitMethod,
                                     reinterpret_cast<jobject>(ValueToExternalAddress(value)));
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
  __attribute__((unused))
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
  __attribute__((unused))
  jclass javaClass, 
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
  jclass javaClass, 
  __attribute__((unused))
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
  __attribute__((unused))
  jclass javaClass, 
  __attribute__((unused))
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
  __attribute__((unused))
  jclass javaClass, 
  __attribute__((unused))
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
  __attribute__((unused))
  jclass javaClass, 
  __attribute__((unused))
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
  __attribute__((unused))
  jclass javaClass, 
  __attribute__((unused))
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
  __attribute__((unused))
  jclass javaClass, 
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
  __attribute__((unused))
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
/* PrintJavaObject:  */
/*******************************************************/
static void PrintJavaObject(
  void *theEnv,
  const char *logicalName,
  void *theValue)
  {
   jobject theObject = reinterpret_cast<jobject>(ValueToExternalAddress(theValue));
   if (theObject != NULL) {
    JNIEnv *env = reinterpret_cast<JNIEnv*>(GetEnvironmentContext(theEnv));
    jstring str = static_cast<jstring>(env->CallObjectMethod(theObject,CLIPSJNIData(theEnv)->objectToStringMethod));
    const char *cStr = env->GetStringUTFChars(str,NULL);

    EnvPrintRouter(theEnv,logicalName,cStr);
    env->ReleaseStringUTFChars(str,cStr);
   }
   else {
    EnvPrintRouter(theEnv,logicalName,"null");
   }
  }

static jclass FindClass(const char* className, void* theEnv, JNIEnv* env) {
	/*=============================================*/
	/* Construct the class descriptor by replacing */
	/* any periods (.) in the class name with a    */
	/* forward slash (/).                          */
	/*=============================================*/
	size_t length = strlen(className);
	char* classDescriptor = (char*) (genalloc(theEnv, length + 1));
	for (size_t i = 0; i < length; i++) {
		if (className[i] != '.') {
			classDescriptor[i] = className[i];
		} else {
			classDescriptor[i] = '/';
		}
	}
	classDescriptor[length] = 0;
	/*=======================*/
	/* Search for the class. */
	/*=======================*/
	jclass theClass = env->FindClass(classDescriptor);
	/*========================================*/
	/* Free the constructed class descriptor. */
	/*========================================*/
	genfree(theEnv, classDescriptor, length + 1);
	return theClass;
}

/*******************************************************/
/* NewJavaAddress:  */
/*******************************************************/
static void NewJavaAddress(
  void *theEnv,
  DATA_OBJECT *rv)
  {
   int numberOfArguments;
   JNIEnv *env;
   DATA_OBJECT theValue;
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
   
   const char* className = DOToString(theValue);

   jclass theClass = FindClass(className, theEnv, env);
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
      for (unsigned int i = 0; i < (unsigned) numberOfArguments - 2; i++)
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

   mid = env->GetMethodID(CLIPSJNIData(theEnv)->classClass,"getConstructors","()[Ljava/lang/reflect/Constructor;");
   
   /*=======================================================*/
   /* Get the list of constructors for the specified class. */
   /*=======================================================*/
     
   constructorList = (jobjectArray) env->CallObjectMethod(theClass,mid);

   /*======================================================*/
   /* Get the method index of the getParameterTypes method */
   /* from the java.lang.reflect.Constructor class.        */
   /*======================================================*/

   jclass ctorClass = env->FindClass("java/lang/reflect/Constructor");
   mid = env->GetMethodID(ctorClass,"getParameterTypes","()[Ljava/lang/Class;");
   env->DeleteLocalRef(ctorClass);

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
   
         jclass tempClass = (jclass) env->GetObjectArrayElement(parameterList,p);
         
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

static jobject ConvertToJava(void* theEnv, DATA_OBJECT_PTR dataObj) {
  JNIEnv *env = reinterpret_cast<JNIEnv*>(GetEnvironmentContext(theEnv));
  jobject result;
  switch (GetpType(dataObj)) {
   case INTEGER:
    /* It might be nice to support byte and short, but Java won't
       dynamically cast them up to Integer. */
    {
     long intValue = DOToLong(*dataObj);
#ifdef USE_SMALL_INTS
     if (intValue < (1 << 7) && intValue >= -(1 << 7))
      {
       result = env->NewObject(CLIPSJNIData(theEnv)->byteClass,
         CLIPSJNIData(theEnv)->byteInitMethod, (jbyte) intValue);
      }
     else if (intValue < (1<<15) && intValue >= -(1<<15))
      {
       result = env->NewObject(CLIPSJNIData(theEnv)->shortClass,
         CLIPSJNIData(theEnv)->shortInitMethod, (jshort) intValue);
      }
     else
#endif
      if (intValue < (1L<<31) && intValue >= -(1L<<31))
      {
       result = env->NewObject(CLIPSJNIData(theEnv)->intClass,
         CLIPSJNIData(theEnv)->intInitMethod, (jint) intValue);
      }
     else {
       result = env->NewObject(CLIPSJNIData(theEnv)->longClass,
         CLIPSJNIData(theEnv)->longInitMethod, (jlong) intValue);
     }
    }
    break;
   case FLOAT:
    {
     float floatValue = DOToFloat(*dataObj);
     double doubleValue = DOToDouble(*dataObj);
     if (floatValue == doubleValue) {
       result = env->NewObject(CLIPSJNIData(theEnv)->floatClass,
         CLIPSJNIData(theEnv)->floatInitMethod, (jfloat) floatValue);
     }
     else {
       result = env->NewObject(CLIPSJNIData(theEnv)->doubleClass,
         CLIPSJNIData(theEnv)->doubleInitMethod, (jdouble) doubleValue);
     }
    }
    break;
    /* Convert SYMBOL and STRING to String */
   case SYMBOL:
   case STRING:
    {
     char* stringValue = DOToString(*dataObj);
     result = env->NewStringUTF(stringValue);
    }
    break;
   case EXTERNAL_ADDRESS:
    {
     struct externalAddressHashNode *theEA =
       reinterpret_cast<struct externalAddressHashNode *>(dataObj->value);
     if (theEA->type == CLIPSJNIData(theEnv)->javaExternalAddressID)
      {
       result = reinterpret_cast<jobject>(theEA->externalAddress);
      }
     else {
       result = NULL;
     }
    }
    break;
   case FACT_ADDRESS:
    {
     result = env->NewObject(CLIPSJNIData(theEnv)->factAddressValueClass,
       CLIPSJNIData(theEnv)->factAddressValueInitMethod, PointerToJLong(GetpValue(dataObj)));
    }
    break;
   case INSTANCE_NAME:
    {
     result = env->NewObject(CLIPSJNIData(theEnv)->instanceNameValueClass,
       CLIPSJNIData(theEnv)->instanceNameValueInitMethod, PointerToJLong(GetpValue(dataObj)));
    }
    break;
   case INSTANCE_ADDRESS:
    {
     result = env->NewObject(CLIPSJNIData(theEnv)->instanceAddressValueClass,
       CLIPSJNIData(theEnv)->instanceAddressValueInitMethod, PointerToJLong(GetpValue(dataObj)));
    }
    break;
   default:
    result = NULL;
    break;
  }
  return result;
}

void ConvertJavaToDO(JNIEnv* env, jobject result, void* theEnv, DATA_OBJECT* rv)
 {
  jclass klass = env->GetObjectClass(result);
  if (env->IsInstanceOf(result,CLIPSJNIData(theEnv)->booleanClass))
   {
    jboolean jbool = env->CallBooleanMethod(result,CLIPSJNIData(theEnv)->booleanValueMethod);
    EnvSetpType(theEnv,rv,SYMBOL);
    EnvSetpValue(theEnv,rv,jbool != JNI_FALSE ? EnvTrueSymbol(theEnv) : EnvFalseSymbol(theEnv));
   }
  else if (env->IsInstanceOf(result,CLIPSJNIData(theEnv)->intClass))
   {
    jint intValue = env->CallIntMethod(result,CLIPSJNIData(theEnv)->intValueMethod);
    EnvSetpType(theEnv,rv,INTEGER);
    EnvSetpValue(theEnv,rv,EnvAddLong(theEnv,intValue));
   }
  else if (env->IsInstanceOf(result,CLIPSJNIData(theEnv)->longClass))
   {
    jlong longValue = env->CallLongMethod(result,CLIPSJNIData(theEnv)->longValueMethod);
    EnvSetpType(theEnv,rv,INTEGER);
    EnvSetpValue(theEnv,rv,EnvAddLong(theEnv,longValue));
   }
  else if (env->IsInstanceOf(result,CLIPSJNIData(theEnv)->floatClass))
   {
    jfloat floatValue = env->CallFloatMethod(result,CLIPSJNIData(theEnv)->floatValueMethod);
    EnvSetpType(theEnv,rv,FLOAT);
    EnvSetpValue(theEnv,rv,EnvAddDouble(theEnv,floatValue));
   }
  else if (env->IsInstanceOf(result,CLIPSJNIData(theEnv)->doubleClass))
   {
    jdouble doubleValue = env->CallDoubleMethod(result,CLIPSJNIData(theEnv)->doubleValueMethod);
    EnvSetpType(theEnv,rv,FLOAT);
    EnvSetpValue(theEnv,rv,EnvAddDouble(theEnv,doubleValue));
   }
  else if (env->IsSameObject(klass, CLIPSJNIData(theEnv)->stringClass))
   {
    jstring javaString = static_cast<jstring>(result);
    EnvSetpType(theEnv, rv, STRING);
    jboolean isCopy = JNI_FALSE;
    const char *resultString = env->GetStringUTFChars(javaString, &isCopy);
    EnvSetpValue(theEnv, rv, EnvAddSymbol(theEnv, resultString));
    env->ReleaseStringUTFChars(javaString, resultString);
   }
  else
   {
   void* externalAddr;
   /* EnvAddExternalAddress doesn't tell us if the value was already in the internal
    * CLIPS hash table, so we don't know for sure if we need a new global reference.
    * But we'll make one because it's better to have it and waste some memory later
    * than for it to get deallocated too soon. */
    result = env->NewGlobalRef(result);
    externalAddr = EnvAddExternalAddress(theEnv, result,
      CLIPSJNIData(theEnv)->javaExternalAddressID);
    EnvSetpType(theEnv, rv, EXTERNAL_ADDRESS);
    EnvSetpValue(theEnv, rv, externalAddr);
   }
 }

static void JavaError(
  void *theEnv,
  int errorID,
  const char* message
  )
  {
   PrintErrorID(theEnv,JAVA_MODULE,errorID,FALSE);
   EnvPrintRouter(theEnv,WERROR,message);
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
   JNIEnv *env;
   DATA_OBJECT theValue;
   const char *methodName;
   //int found = FALSE, matches;
   int i;
   jobject result = NULL;
   
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

   /*===============================================*/
   /* If the target is an external address, then we */
   /* should be invoking a method of an instance.   */
   /*===============================================*/

   int methodArgOffset;
   jobject theObject;
   jclass theClass = NULL;
   switch (GetpType(target))
    {
    case EXTERNAL_ADDRESS:
     theObject = reinterpret_cast<jobject>(DOPToExternalAddress(target));
     methodArgOffset = 3;
     break;
    case SYMBOL:
     printf("looking for '%s'\n", methodName);
     theObject = theClass = FindClass(methodName, theEnv, env);
     if (theObject == NULL) {
      SetEvaluationError(theEnv, TRUE);
      JavaError(theEnv, JAVA_UNKNOWN_ERROR, "unknown class name\n");
     }
     else {
      DATA_OBJECT tempDO;
      if (EnvArgTypeCheck(theEnv,"call (with type Java)",3,SYMBOL,&tempDO) == TRUE) {
       methodName = DOToString(tempDO);
       methodArgOffset = 4;
       numberOfArguments--;
      }
      else {
       SetEvaluationError(theEnv, TRUE);
       JavaError(theEnv, JAVA_UNKNOWN_ERROR, "expecting method or field name\n");
       theObject = NULL;
      }
     }
     break;
    default:
     theObject = NULL;
     break;
    }
   jobject javaMethodName = env->NewStringUTF(methodName);

   if (theObject != NULL)
   {
    DATA_OBJECT_PTR newArgs;

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
        EnvRtnUnknown(theEnv,i+methodArgOffset,&newArgs[i]);
        if (GetEvaluationError(theEnv))
         { return FALSE; }
       }
     }

    unsigned int methodArgumentCount = numberOfArguments - 2;
    if (methodArgumentCount == 0) {
     // check for field access instead of method call
     jclass targetClass = env->GetObjectClass(theObject);
     jclass classClass = CLIPSJNIData(theEnv)->classClass;
     jmethodID getFieldID = env->GetMethodID(classClass,"getField","(Ljava/lang/String;)Ljava/lang/reflect/Field;");
     jobject fieldOwner = (theClass == NULL) ? targetClass : theObject;
     jobject field = env->CallObjectMethod(fieldOwner,getFieldID,javaMethodName);
     if (field != NULL) {
      // field exists
      jclass fieldClass = env->GetObjectClass(field);
      jmethodID fieldGetID = env->GetMethodID(fieldClass,"get","(Ljava/lang/Object;)Ljava/lang/Object;");
      result = env->CallObjectMethod(field,fieldGetID,theObject);
      env->DeleteLocalRef(fieldClass);
     }
     else {
      env->ExceptionClear();
     }
     env->DeleteLocalRef(targetClass);
    }

    if (result == NULL) {

     jobjectArray methodArgs = env->NewObjectArray(methodArgumentCount,
                                                   CLIPSJNIData(theEnv)->objectClass, NULL);
     /*====================================*/
     /* Convert the params to java objects */
     /*====================================*/

     for (unsigned int i = 0; i < methodArgumentCount; ++i)
      {
       env->SetObjectArrayElement(methodArgs, i,
                                  ConvertToJava(theEnv, &newArgs[i]));
      }
    
     /*========================================================*/
     /* We'll try to call the method via java.beans.Expression */
     /*========================================================*/
     jvalue expressionArgs[3];
     expressionArgs[0].l = theObject;
     expressionArgs[1].l = javaMethodName;
     expressionArgs[2].l = methodArgs;

     jobject expression = env->NewObjectA(CLIPSJNIData(theEnv)->expressionClass,
                                          CLIPSJNIData(theEnv)->expressionInitMethod, expressionArgs);
     if (env->ExceptionCheck() == JNI_TRUE)
      {
       SetEvaluationError(theEnv, TRUE);
       JavaError(theEnv, JAVA_UNKNOWN_ERROR,
                 "could not translate parameters to Java\n");
      }
     else {
      env->CallVoidMethod(expression, CLIPSJNIData(theEnv)->expressionExecuteMethod);
      if (env->ExceptionCheck() == JNI_TRUE)
       {
        SetEvaluationError(theEnv, TRUE);
        JavaError(theEnv, JAVA_UNKNOWN_ERROR, "could not invoke Java method\n");
       }
      else {
       result = env->CallObjectMethod(expression,
                                      CLIPSJNIData(theEnv)->expressionGetValueMethod);

      }
     }

     env->DeleteLocalRef(methodArgs);
    }
    if (newArgs != NULL)
     { genfree(theEnv,newArgs,sizeof(DATA_OBJECT) * (numberOfArguments - 2)); }

   }
   if (GetEvaluationError(theEnv) == 0) {
    if (result != NULL)
     {
      ConvertJavaToDO(env, result, theEnv, rv);
     }
   }

    return GetEvaluationError(theEnv) == 0;
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

/*
 * Class:     CLIPSJNI_Environment
 * Method:    setGlobal
 * Signature: (JLjava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_CLIPSJNI_Environment_setGlobal
  (JNIEnv *env,
   __attribute__((unused))
   jobject obj,
   jlong clipsEnv, jstring name, jobject value)
 {
  void* theEnv = JLongToPointer(clipsEnv);
  const char* cName = env->GetStringUTFChars(name, NULL);
  DATA_OBJECT data;
  ConvertJavaToDO(env, value, theEnv, &data);
  EnvSetDefglobalValue(theEnv, cName, &data);
 }


// Local Variables:
// c-basic-offset: 1
// End:
