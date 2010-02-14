//===---------- JnjvmModule.h - Definition of a J3 module -----------------===//
//
//                            The VMKit project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef JNJVM_MODULE_H
#define JNJVM_MODULE_H

#include <map>
#include <vector>

#include "mvm/Allocator.h"
#include "mvm/JIT.h"
#include "mvm/UTF8.h"

#include "JavaCompiler.h"

namespace llvm {
  class BasicBlock;
  class Constant;
  class ConstantInt;
  class Function;
  class FunctionPassManager;
  class FunctionType;
  class GlobalVariable;
  class GCFunctionInfo;
  class GVMaterializer;
  class MDNode;
  class Module;
  class Type;
  class Value;
}

namespace j3 {

class ArrayObject;
class Attribut;
class CommonClass;
class Class;
class ClassArray;
class ClassPrimitive;
class JavaConstantPool;
class JavaField;
class JavaMethod;
class JavaObject;
class JavaString;
class JavaVirtualTable;
class Jnjvm;
class JnjvmClassLoader;
class JnjvmModule;
class Typedef;
class Signdef;

using mvm::UTF8;

class LLVMAssessorInfo {
public:
  const llvm::Type* llvmType;
  const llvm::Type* llvmTypePtr;
  uint8_t logSizeInBytesConstant;
  
  static void initialise();
  static std::map<const char, LLVMAssessorInfo> AssessorInfo;

};


class LLVMClassInfo : public mvm::JITInfo {
  friend class JavaAOTCompiler;
  friend class JavaJITCompiler;
  friend class JavaLLVMCompiler;
private:
  Class* classDef;
  /// virtualSizeLLVM - The LLVM constant size of instances of this class.
  ///
  llvm::Constant* virtualSizeConstant;
  /// virtualType - The LLVM type of instance of this class.
  ///
  const llvm::Type * virtualType;

  /// staticType - The LLVM type of the static instance of this class.
  ///
  const llvm::Type * staticType;
public:
  
  llvm::Value* getVirtualSize();
  const llvm::Type* getVirtualType();
  const llvm::Type* getStaticType();
  
  LLVMClassInfo(Class* cl) :
    classDef(cl),
    virtualSizeConstant(0),
    virtualType(0),
    staticType(0) {}

  virtual void clear() {
    virtualType = 0;
    staticType = 0;
    virtualSizeConstant = 0;
  }
};

class LLVMMethodInfo : public mvm::JITInfo {
private:
  JavaMethod* methodDef;

  llvm::Function* methodFunction;
  llvm::Constant* offsetConstant;
  const llvm::FunctionType* functionType;
  llvm::MDNode* DbgSubprogram;
  
  
public:
  llvm::GCFunctionInfo* GCInfo;
  llvm::Function* getMethod();
  llvm::Constant* getOffset();
  const llvm::FunctionType* getFunctionType();
    
  LLVMMethodInfo(JavaMethod* M) :  methodDef(M), methodFunction(0),
    offsetConstant(0), functionType(0), DbgSubprogram(0), GCInfo(0) {}
 
  void setDbgSubprogram(llvm::MDNode* node) { DbgSubprogram = node; }
  llvm::MDNode* getDbgSubprogram() { return DbgSubprogram; }

  virtual void clear() {
    GCInfo = 0;
    methodFunction = 0;
    offsetConstant = 0;
    functionType = 0;
    DbgSubprogram = 0;
  }
};


class LLVMFieldInfo : public mvm::JITInfo {
private:
  JavaField* fieldDef;
  
  llvm::Constant* offsetConstant;

public:
  llvm::Constant* getOffset();

  LLVMFieldInfo(JavaField* F) : 
    fieldDef(F), 
    offsetConstant(0) {}

  virtual void clear() {
    offsetConstant = 0;
  }
};

class LLVMSignatureInfo : public mvm::JITInfo {
private:
  const llvm::FunctionType* staticType;
  const llvm::FunctionType* virtualType;
  const llvm::FunctionType* nativeType;
  
  const llvm::FunctionType* virtualBufType;
  const llvm::FunctionType* staticBufType;

  const llvm::PointerType* staticPtrType;
  const llvm::PointerType* virtualPtrType;
  const llvm::PointerType* nativePtrType;
  
  llvm::Function* virtualBufFunction;
  llvm::Function* virtualAPFunction;
  llvm::Function* staticBufFunction;
  llvm::Function* staticAPFunction;
  
  llvm::Function* staticStubFunction;
  llvm::Function* specialStubFunction;
  llvm::Function* virtualStubFunction;
  
  Signdef* signature;

  llvm::Function* createFunctionCallBuf(bool virt);
  llvm::Function* createFunctionCallAP(bool virt);
  llvm::Function* createFunctionStub(bool special, bool virt);
   
  
public:
  const llvm::FunctionType* getVirtualType();
  const llvm::FunctionType* getStaticType();
  const llvm::FunctionType* getNativeType();

  const llvm::FunctionType* getVirtualBufType();
  const llvm::FunctionType* getStaticBufType();

  const llvm::PointerType*  getStaticPtrType();
  const llvm::PointerType*  getNativePtrType();
  const llvm::PointerType*  getVirtualPtrType();
   
  llvm::Function* getVirtualBuf();
  llvm::Function* getVirtualAP();
  llvm::Function* getStaticBuf();
  llvm::Function* getStaticAP();
  
  llvm::Function* getStaticStub();
  llvm::Function* getSpecialStub();
  llvm::Function* getVirtualStub();
  
  LLVMSignatureInfo(Signdef* sign) : 
    staticType(0),
    virtualType(0),
    nativeType(0),
    virtualBufType(0),
    staticBufType(0),
    staticPtrType(0),
    virtualPtrType(0),
    nativePtrType(0),
    virtualBufFunction(0),
    virtualAPFunction(0),
    staticBufFunction(0),
    staticAPFunction(0),
    staticStubFunction(0),
    specialStubFunction(0),
    virtualStubFunction(0),
    signature(sign) {}

};

class JnjvmModule : public mvm::MvmModule {

public:

  static const llvm::Type* JavaArrayUInt8Type;
  static const llvm::Type* JavaArraySInt8Type;
  static const llvm::Type* JavaArrayUInt16Type;
  static const llvm::Type* JavaArraySInt16Type;
  static const llvm::Type* JavaArrayUInt32Type;
  static const llvm::Type* JavaArraySInt32Type;
  static const llvm::Type* JavaArrayLongType;
  static const llvm::Type* JavaArrayFloatType;
  static const llvm::Type* JavaArrayDoubleType;
  static const llvm::Type* JavaArrayObjectType;
  
  static const llvm::Type* VTType;
  static const llvm::Type* JavaObjectType;
  static const llvm::Type* JavaArrayType;
  static const llvm::Type* JavaCommonClassType;
  static const llvm::Type* JavaClassType;
  static const llvm::Type* JavaClassArrayType;
  static const llvm::Type* JavaClassPrimitiveType;
  static const llvm::Type* ConstantPoolType;
  static const llvm::Type* CodeLineInfoType;
  static const llvm::Type* UTF8Type;
  static const llvm::Type* JavaMethodType;
  static const llvm::Type* JavaFieldType;
  static const llvm::Type* AttributType;
  static const llvm::Type* JavaThreadType;
  static const llvm::Type* MutatorThreadType;
  
#ifdef ISOLATE_SHARING
  static const llvm::Type* JnjvmType;
#endif
  
  llvm::Function* EmptyTracerFunction;
  llvm::Function* JavaObjectTracerFunction;
  llvm::Function* JavaArrayTracerFunction;
  llvm::Function* ArrayObjectTracerFunction;
  llvm::Function* RegularObjectTracerFunction;
  
  llvm::Function* StartJNIFunction;
  llvm::Function* EndJNIFunction;
  llvm::Function* InterfaceLookupFunction;
  llvm::Function* VirtualFieldLookupFunction;
  llvm::Function* StaticFieldLookupFunction;
  llvm::Function* PrintExecutionFunction;
  llvm::Function* PrintMethodStartFunction;
  llvm::Function* PrintMethodEndFunction;
  llvm::Function* InitialiseClassFunction;
  llvm::Function* InitialisationCheckFunction;
  llvm::Function* ForceInitialisationCheckFunction;
  llvm::Function* ForceLoadedCheckFunction;
  llvm::Function* ClassLookupFunction;
  llvm::Function* StringLookupFunction;
  
  llvm::Function* ResolveVirtualStubFunction;
  llvm::Function* ResolveSpecialStubFunction;
  llvm::Function* ResolveStaticStubFunction;

#ifndef WITHOUT_VTABLE
  llvm::Function* VirtualLookupFunction;
#endif
  llvm::Function* IsAssignableFromFunction;
  llvm::Function* IsSecondaryClassFunction;
  llvm::Function* GetDepthFunction;
  llvm::Function* GetDisplayFunction;
  llvm::Function* GetVTInDisplayFunction;
  llvm::Function* GetStaticInstanceFunction;
  llvm::Function* AquireObjectFunction;
  llvm::Function* ReleaseObjectFunction;
  llvm::Function* GetConstantPoolAtFunction;
  llvm::Function* MultiCallNewFunction;
  llvm::Function* GetArrayClassFunction;

#ifdef ISOLATE_SHARING
  llvm::Function* GetCtpClassFunction;
  llvm::Function* GetJnjvmExceptionClassFunction;
  llvm::Function* GetJnjvmArrayClassFunction;
  llvm::Function* StaticCtpLookupFunction;
  llvm::Function* SpecialCtpLookupFunction;
#endif

#ifdef SERVICE
  llvm::Function* ServiceCallStartFunction;
  llvm::Function* ServiceCallStopFunction;
#endif

  llvm::Function* GetClassDelegateeFunction;
  llvm::Function* RuntimeDelegateeFunction;
  llvm::Function* ArrayLengthFunction;
  llvm::Function* GetVTFunction;
  llvm::Function* GetIMTFunction;
  llvm::Function* GetClassFunction;
  llvm::Function* GetVTFromClassFunction;
  llvm::Function* GetVTFromClassArrayFunction;
  llvm::Function* GetVTFromCommonClassFunction;
  llvm::Function* GetObjectSizeFromClassFunction;
  llvm::Function* GetBaseClassVTFromVTFunction;

  llvm::Function* GetLockFunction;
  llvm::Function* OverflowThinLockFunction;
  
  llvm::Function* GetFinalInt8FieldFunction;
  llvm::Function* GetFinalInt16FieldFunction;
  llvm::Function* GetFinalInt32FieldFunction;
  llvm::Function* GetFinalLongFieldFunction;
  llvm::Function* GetFinalFloatFieldFunction;
  llvm::Function* GetFinalDoubleFieldFunction;
  llvm::Function* GetFinalObjectFieldFunction;
  
  llvm::Constant* JavaArraySizeOffsetConstant;
  llvm::Constant* JavaArrayElementsOffsetConstant;
  llvm::Constant* JavaObjectLockOffsetConstant;
  llvm::Constant* JavaObjectVTOffsetConstant;

  llvm::Constant* OffsetAccessInCommonClassConstant;
  llvm::Constant* IsArrayConstant;
  llvm::Constant* IsPrimitiveConstant;
  llvm::Constant* OffsetObjectSizeInClassConstant;
  llvm::Constant* OffsetVTInClassConstant;
  llvm::Constant* OffsetTaskClassMirrorInClassConstant;
  llvm::Constant* OffsetVirtualMethodsInClassConstant;
  llvm::Constant* OffsetStaticInstanceInTaskClassMirrorConstant;
  llvm::Constant* OffsetInitializedInTaskClassMirrorConstant;
  llvm::Constant* OffsetStatusInTaskClassMirrorConstant;
  
  llvm::Constant* OffsetDoYieldInThreadConstant;
  llvm::Constant* OffsetIsolateInThreadConstant;
  llvm::Constant* OffsetJNIInThreadConstant;
  llvm::Constant* OffsetJavaExceptionInThreadConstant;
  llvm::Constant* OffsetCXXExceptionInThreadConstant;
  
  llvm::Constant* OffsetClassInVTConstant;
  llvm::Constant* OffsetDepthInVTConstant;
  llvm::Constant* OffsetDisplayInVTConstant;
  llvm::Constant* OffsetBaseClassVTInVTConstant;
  llvm::Constant* OffsetIMTInVTConstant;
  
  llvm::Constant* OffsetBaseClassInArrayClassConstant;
  llvm::Constant* OffsetLogSizeInPrimitiveClassConstant;
  
  llvm::Constant* ClassReadyConstant;

  llvm::Constant* JavaObjectNullConstant;
  llvm::Constant* MaxArraySizeConstant;
  llvm::Constant* JavaArraySizeConstant;

  llvm::Function* ThrowExceptionFunction;
  llvm::Function* NullPointerExceptionFunction;
  llvm::Function* IndexOutOfBoundsExceptionFunction;
  llvm::Function* ClassCastExceptionFunction;
  llvm::Function* OutOfMemoryErrorFunction;
  llvm::Function* StackOverflowErrorFunction;
  llvm::Function* NegativeArraySizeExceptionFunction;
  llvm::Function* ArrayStoreExceptionFunction;
  llvm::Function* ArithmeticExceptionFunction;
  llvm::Function* ThrowExceptionFromJITFunction;
  

  JnjvmModule(llvm::Module*);
  
  static void initialise();

};

}

#endif
