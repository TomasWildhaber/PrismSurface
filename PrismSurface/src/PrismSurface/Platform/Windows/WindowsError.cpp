#include "pch.h"
#if defined(PRISM_PLATFORM_WINDOWS)

#include <windows.h>

#include "PrismSurface/Error.h"

namespace PrismSurface
{
	// Reading error string is limited to english to match the user error messages	 

	std::string ErrorHandler::GetPlatformErrorMessage()
	{
		std::string message;

		DWORD errorMessageID = GetLastError();
		if (errorMessageID == 0)
			return message;

		LPWSTR messageBuffer = nullptr;

		size_t size = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorMessageID,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			reinterpret_cast<LPWSTR>(&messageBuffer),
			0,
			NULL);

		if (size > 0 && messageBuffer)
		{
			int utf8Length = WideCharToMultiByte(CP_UTF8, 0, messageBuffer, (int)size, NULL, 0, NULL, NULL);

			if (utf8Length > 0)
			{
				message = "Win32 error: ";
				std::string errorMessage(utf8Length, '\0');

				WideCharToMultiByte(CP_UTF8, 0, messageBuffer, (int)size, &errorMessage[0], utf8Length, NULL, NULL);

				while (!errorMessage.empty() && (errorMessage.back() == '\n' || errorMessage.back() == '\r'))
					errorMessage.pop_back();

				message += errorMessage;
			}

			LocalFree(messageBuffer);
		}

		return message;
	}
}

#endif
