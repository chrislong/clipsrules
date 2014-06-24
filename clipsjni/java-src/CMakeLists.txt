include(UseJava)

set(CMAKE_JAVA_JAR_ENTRY_POINT CLIPSJNI/Environment)
set(CMAKE_JAVA_TARGET_OUTPUT_NAME CLIPSJNI)
add_jar(CLIPSJNI_jar
  CLIPSJNI/CLIPSTerminal.java
  CLIPSJNI/ConsoleRouter.java
  CLIPSJNI/ConsoleUTF8Router.java
  CLIPSJNI/Environment.java
  CLIPSJNI/FactAddressValue.java
  CLIPSJNI/FloatValue.java
  CLIPSJNI/InstanceAddressValue.java
  CLIPSJNI/InstanceNameValue.java
  CLIPSJNI/InstanceValue.java
  CLIPSJNI/IntegerValue.java
  CLIPSJNI/JavaObjectValue.java
  CLIPSJNI/JTextAreaCommandPromptRouter.java
  CLIPSJNI/JTextAreaRouter.java
  CLIPSJNI/MultifieldValue.java
  CLIPSJNI/PrimitiveValue.java
  CLIPSJNI/Router.java
  CLIPSJNI/StringValue.java
  CLIPSJNI/SymbolValue.java
  CLIPSJNI/VoidValue.java
)
