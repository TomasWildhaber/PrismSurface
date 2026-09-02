#include "pch.h"

#include "PrismSurface/Error.h"

namespace PrismSurface
{
	void ErrorHandler::SetErrorCallback(ErrorCallbackFn callback)
	{
		s_Callback = callback;
	}

	void ErrorHandler::Error(ErrorCode code, const char* message)
	{
		if (!s_Callback)
			return;

		std::string finalMessage = message;
		finalMessage += " " + GetPlatformErrorMessage();

		s_Callback(code, finalMessage.c_str());
	}
}
