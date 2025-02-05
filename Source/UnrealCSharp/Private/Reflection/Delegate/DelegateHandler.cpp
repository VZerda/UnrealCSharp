﻿#include "Reflection/Delegate/DelegateHandler.h"
#include "Macro/FunctionMacro.h"
#include "Dynamic/FDynamicGeneratorCore.h"

void UDelegateHandler::ProcessEvent(UFunction* Function, void* Parms)
{
	if (Function != nullptr && Function->GetName() == FUNCTION_CSHARP_CALLBACK)
	{
		if (DelegateDescriptor != nullptr)
		{
			DelegateDescriptor->CallDelegate(DelegateWrapper.Object.Get(), DelegateWrapper.Method, Parms);
		}
	}
	else
	{
		UObject::ProcessEvent(Function, Parms);
	}
}

void UDelegateHandler::CSharpCallBack()
{
}

void UDelegateHandler::Initialize(FScriptDelegate* InScriptDelegate, UFunction* InSignatureFunction)
{
	bNeedFree = InScriptDelegate == nullptr;

	ScriptDelegate = InScriptDelegate != nullptr ? InScriptDelegate : new FScriptDelegate();

	DelegateDescriptor = new FCSharpDelegateDescriptor(InSignatureFunction);

	FunctionInfo = InSignatureFunction;
}

void UDelegateHandler::Deinitialize()
{
	if (ScriptDelegate != nullptr)
	{
		if (bNeedFree)
		{
			ScriptDelegate->Unbind();

			delete ScriptDelegate;
		}

		ScriptDelegate = nullptr;
	}

	if (DelegateDescriptor != nullptr)
	{
		delete DelegateDescriptor;

		DelegateDescriptor = nullptr;
	}

	FunctionInfo = nullptr;
}

void UDelegateHandler::Bind(UObject* InObject, MonoMethod* InMonoMethod)
{
	if (ScriptDelegate != nullptr)
	{
		UpdateDelegateDescriptorWithMonoMethod(InMonoMethod, (!!InObject)?InObject->GetClass(): UDelegateHandler::StaticClass());
		if (!ScriptDelegate->IsBound())
		{
			ScriptDelegate->BindUFunction(this, *FUNCTION_CSHARP_CALLBACK);
		}
	}

	DelegateWrapper = {InObject, InMonoMethod};
}

bool UDelegateHandler::IsBound() const
{
	return ScriptDelegate != nullptr ? ScriptDelegate->IsBound() : false;
}

void UDelegateHandler::UnBind() const
{
	if (ScriptDelegate != nullptr)
	{
		ScriptDelegate->Unbind();
	}
}

void UDelegateHandler::Clear() const
{
	if (ScriptDelegate != nullptr)
	{
		ScriptDelegate->Clear();
	}
}

UObject* UDelegateHandler::GetUObject() const
{
	return ScriptDelegate != nullptr ? ScriptDelegate->GetUObject() : nullptr;
}

FName UDelegateHandler::GetFunctionName() const
{
	return ScriptDelegate != nullptr ? ScriptDelegate->GetFunctionName() : NAME_None;
}

UFunction* UDelegateHandler::GetCallBack() const
{
	return FindFunction(*FUNCTION_CSHARP_CALLBACK);
}

void UDelegateHandler::UpdateDelegateDescriptorWithMonoMethod(MonoMethod* InMethod, UClass* InClass)
{
	FunctionInfo = nullptr;
	const char* MethodName = nullptr;
	if (!FDynamicGeneratorCoreExports::CreateUFunctionForMonoMethod(InClass, InMethod, FunctionInfo, MethodName))
	{
		return;
	}


	if (DelegateDescriptor != nullptr)
	{
		delete DelegateDescriptor;
	}

	DelegateDescriptor = new FCSharpDelegateDescriptor(FunctionInfo);
}
