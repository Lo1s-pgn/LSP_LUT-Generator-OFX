// Windows: folder chooser for export path (IFileOpenDialog). Resolve runs UI on the main thread.
#include "../LSPLutGeneratorDialogs.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <shobjidl.h>
#include <objbase.h>
#include <string>

static std::string lutGenWideToUtf8(const wchar_t* wide) {
    if (!wide || !*wide)
        return {};
    int needed = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    if (needed <= 1)
        return {};
    std::string out(static_cast<size_t>(needed - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, out.data(), needed, nullptr, nullptr);
    return out;
}

static std::wstring lutGenUtf8ToWide(const char* utf8) {
    if (!utf8 || !*utf8)
        return {};
    int needed = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
    if (needed <= 1)
        return {};
    std::wstring out(static_cast<size_t>(needed - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, out.data(), needed);
    return out;
}

struct LutGenComApartmentScope {
    bool uninit = false;
    LutGenComApartmentScope() {
        const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (hr == S_OK)
            uninit = true;
    }
    ~LutGenComApartmentScope() {
        if (uninit)
            CoUninitialize();
    }
};

std::string LSPLutGenShowChooseFolderDialog(const char* p_DefaultDir) {
    LutGenComApartmentScope com;
    std::string result;

    IFileOpenDialog* dialog = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog,
                                  reinterpret_cast<void**>(&dialog));
    if (FAILED(hr) || !dialog)
        return result;

    DWORD opt = 0;
    if (SUCCEEDED(dialog->GetOptions(&opt))) {
        dialog->SetOptions(opt | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);
    }
    dialog->SetTitle(L"Choose export folder");

    if (p_DefaultDir && p_DefaultDir[0] != '\0') {
        std::wstring wdir = lutGenUtf8ToWide(p_DefaultDir);
        if (!wdir.empty()) {
            IShellItem* item = nullptr;
            if (SUCCEEDED(SHCreateItemFromParsingName(wdir.c_str(), nullptr, IID_IShellItem,
                                                      reinterpret_cast<void**>(&item)))
                && item) {
                dialog->SetFolder(item);
                item->Release();
            }
        }
    }

    if (SUCCEEDED(dialog->Show(nullptr))) {
        IShellItem* item = nullptr;
        if (SUCCEEDED(dialog->GetResult(&item)) && item) {
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) {
                result = lutGenWideToUtf8(path);
                CoTaskMemFree(path);
            }
            item->Release();
        }
    }

    dialog->Release();
    return result;
}
