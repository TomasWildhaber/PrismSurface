#pragma once

#include <string>

#include "PrismSurface/PrismSurface.h"

namespace PrismSurface
{
	enum class ErrorCode : int8_t
	{
		WindowCreationFailed,
		ModuleLoadingFailed,
	};

	using ErrorCallbackFn = void(*)(ErrorCode, const char*);

	class PRISM_API ErrorHandler
	{
	public:
		static void SetErrorCallback(ErrorCallbackFn callback);
		static void Error(ErrorCode code, const char* message);
	private:
		static std::string GetPlatformErrorMessage();

		static inline ErrorCallbackFn s_Callback = nullptr;
	};
}
