/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "platform/win/windows_dlls.h"

#include "base/platform/win/base_windows_safe_library.h"

#include <VersionHelpers.h>
#include <QtCore/QSysInfo>

#define LOAD_SYMBOL(lib, name) ::base::Platform::LoadMethod(lib, #name, name)

bool DirectXResolveCompiler();

namespace Platform {
namespace Dlls {

using base::Platform::SafeLoadLibrary;
using base::Platform::LoadMethod;

void init() {
	static bool inited = false;
	if (inited) return;
	inited = true;

	// Remove the current directory from the DLL search order.
	::SetDllDirectory(L"");

	const auto list = {
		u"dbghelp.dll"_q,
		u"dbgcore.dll"_q,
		u"propsys.dll"_q,
		u"winsta.dll"_q,
		u"textinputframework.dll"_q,
		u"uxtheme.dll"_q,
		u"igdumdim32.dll"_q,
		u"amdhdl32.dll"_q,
		u"wtsapi32.dll"_q,
		u"propsys.dll"_q,
		u"combase.dll"_q,
		u"dwmapi.dll"_q,
		u"rstrtmgr.dll"_q,
		u"psapi.dll"_q,
		u"user32.dll"_q,
		u"d3d9.dll"_q,
		u"d3d11.dll"_q,
		u"dxgi.dll"_q,
	};
	for (const auto &lib : list) {
		SafeLoadLibrary(lib);
	}
}

void start() {
	init();

	const auto LibShell32 = LoadLibrary(L"shell32.dll");
	LOAD_SYMBOL(LibShell32, SHAssocEnumHandlers);
	LOAD_SYMBOL(LibShell32, SHCreateItemFromParsingName);
	LOAD_SYMBOL(LibShell32, SHOpenWithDialog);
	LOAD_SYMBOL(LibShell32, OpenAs_RunDLL);
	LOAD_SYMBOL(LibShell32, SHQueryUserNotificationState);
	LOAD_SYMBOL(LibShell32, SHChangeNotify);

	//if (IsWindows10OrGreater()) {
	//	static const auto kSystemVersion = QOperatingSystemVersion::current();
	//	static const auto kMinor = kSystemVersion.minorVersion();
	//	static const auto kBuild = kSystemVersion.microVersion();
	//	if (kMinor > 0 || (kMinor == 0 && kBuild >= 17763)) {
	//		const auto LibUxTheme = LoadLibrary(L"uxtheme.dll");
	//		if (kBuild < 18362) {
	//			LOAD_SYMBOL(LibUxTheme, AllowDarkModeForApp, 135);
	//		} else {
	//			LOAD_SYMBOL(LibUxTheme, SetPreferredAppMode, 135);
	//		}
	//		LOAD_SYMBOL(LibUxTheme, AllowDarkModeForWindow, 133);
	//		LOAD_SYMBOL(LibUxTheme, RefreshImmersiveColorPolicyState, 104);
	//		LOAD_SYMBOL(LibUxTheme, FlushMenuThemes, 136);
	//	}
	//}

	if (IsWindowsVistaOrGreater()) {
		const auto LibWtsApi32 = SafeLoadLibrary(u"wtsapi32.dll"_q);
		LOAD_SYMBOL(LibWtsApi32, WTSRegisterSessionNotification);
		LOAD_SYMBOL(LibWtsApi32, WTSUnRegisterSessionNotification);

		const auto LibPropSys = SafeLoadLibrary(u"propsys.dll"_q);
		LOAD_SYMBOL(LibPropSys, PropVariantToString);
		LOAD_SYMBOL(LibPropSys, PSStringFromPropertyKey);

		const auto LibDwmApi = SafeLoadLibrary(u"dwmapi.dll"_q);
		LOAD_SYMBOL(LibDwmApi, DwmIsCompositionEnabled);
		LOAD_SYMBOL(LibDwmApi, DwmSetWindowAttribute);
	}

	const auto LibPsApi = SafeLoadLibrary(u"psapi.dll"_q);
	LOAD_SYMBOL(LibPsApi, GetProcessMemoryInfo);

	const auto LibUser32 = LoadLibrary(L"user32.dll");
	LOAD_SYMBOL(LibUser32, SetWindowCompositionAttribute);
}

SafeIniter kSafeIniter;

} // namespace

void CheckLoadedModules() {
	if (DirectXResolveCompiler()) {
		auto LibD3DCompiler = HMODULE();
		if (GetModuleHandleEx(0, L"d3dcompiler_47.dll", &LibD3DCompiler)) {
			constexpr auto kMaxPathLong = 32767;
			auto path = std::array<WCHAR, kMaxPathLong + 1>{ 0 };
			const auto length = GetModuleFileName(
				LibD3DCompiler,
				path.data(),
				kMaxPathLong);
			if (length > 0 && length < kMaxPathLong) {
				LOG(("Using DirectX compiler '%1'."
					).arg(QString::fromWCharArray(path.data())));
			} else {
				LOG(("Error: Could not resolve DirectX compiler path."));
			}
		} else {
			LOG(("Error: Could not resolve DirectX compiler module."));
		}
	} else {
		LOG(("Error: Could not resolve DirectX compiler library."));
	}
}

} // namespace Dlls
} // namespace Platform
