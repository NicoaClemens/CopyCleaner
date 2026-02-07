// clipboard.cpp
// Implements builtins/clipboard.h

#include "builtins/clipboard.h"

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

namespace builtins {

Result<RuntimeValue> Clipboard::is_text() {
#ifdef _WIN32
    if (!OpenClipboard(nullptr)) {
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    bool has_text =
        IsClipboardFormatAvailable(CF_TEXT) || IsClipboardFormatAvailable(CF_UNICODETEXT);

    CloseClipboard();

    RuntimeValue result;
    result.value = RuntimeValue::Bool{has_text};
    return ok(result);
#elif defined(__APPLE__)
    // Use pbpaste to check if clipboard has content
    FILE* pipe = popen("pbpaste", "r");
    if (!pipe) {
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    char buffer[128];
    bool has_content = (fgets(buffer, sizeof(buffer), pipe) != nullptr);
    pclose(pipe);

    RuntimeValue result;
    result.value = RuntimeValue::Bool{has_content};
    return ok(result);
#else
    // For other platforms, return false for now
    // TODO: Implement for Linux
    RuntimeValue result;
    result.value = RuntimeValue::Bool{false};
    return ok(result);
#endif
}

Result<RuntimeValue> Clipboard::read() {
#ifdef _WIN32
    if (!OpenClipboard(nullptr)) {
        RuntimeValue result;
        result.value = RuntimeValue::String{""};
        return ok(result);
    }

    std::string text;

    // Try Unicode text first
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData != nullptr) {
        wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
        if (pszText != nullptr) {
            // Convert wide string to narrow string (simple conversion)
            int size = WideCharToMultiByte(CP_UTF8, 0, pszText, -1, nullptr, 0, nullptr, nullptr);
            if (size > 0) {
                text.resize(size - 1);  // -1 to exclude null terminator
                WideCharToMultiByte(CP_UTF8, 0, pszText, -1, &text[0], size, nullptr, nullptr);
            }
            GlobalUnlock(hData);
        }
    } else {
        // Fallback to ASCII text
        hData = GetClipboardData(CF_TEXT);
        if (hData != nullptr) {
            char* pszText = static_cast<char*>(GlobalLock(hData));
            if (pszText != nullptr) {
                text = pszText;
                GlobalUnlock(hData);
            }
        }
    }

    CloseClipboard();

    RuntimeValue result;
    result.value = RuntimeValue::String{text};
    return ok(result);
#elif defined(__APPLE__)
    // Use pbpaste to read clipboard
    FILE* pipe = popen("pbpaste", "r");
    if (!pipe) {
        RuntimeValue result;
        result.value = RuntimeValue::String{""};
        return ok(result);
    }

    std::string text;
    std::array<char, 1024> buffer;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        text += buffer.data();
    }
    pclose(pipe);

    RuntimeValue result;
    result.value = RuntimeValue::String{text};
    return ok(result);
#else
    // For other platforms, return empty string
    // TODO: Implement for Linux
    RuntimeValue result;
    result.value = RuntimeValue::String{""};
    return ok(result);
#endif
}

Result<RuntimeValue> Clipboard::write(const std::string& message) {
#ifdef _WIN32
    if (!OpenClipboard(nullptr)) {
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    EmptyClipboard();

    // Convert to wide string for Unicode support
    int size = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
    if (size == 0) {
        CloseClipboard();
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size * sizeof(wchar_t));
    if (hMem == nullptr) {
        CloseClipboard();
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hMem));
    if (pMem == nullptr) {
        GlobalFree(hMem);
        CloseClipboard();
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, pMem, size);
    GlobalUnlock(hMem);

    if (SetClipboardData(CF_UNICODETEXT, hMem) == nullptr) {
        GlobalFree(hMem);
        CloseClipboard();
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    CloseClipboard();

    RuntimeValue result;
    result.value = RuntimeValue::Bool{true};
    return ok(result);
#elif defined(__APPLE__)
    // Use pbcopy to write to clipboard
    FILE* pipe = popen("pbcopy", "w");
    if (!pipe) {
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    size_t written = fwrite(message.c_str(), 1, message.length(), pipe);
    int status = pclose(pipe);

    bool success = (written == message.length() && status == 0);

    RuntimeValue result;
    result.value = RuntimeValue::Bool{success};
    return ok(result);
#else
    // For other platforms, return false
    // TODO: Implement for Linux
    RuntimeValue result;
    result.value = RuntimeValue::Bool{false};
    return ok(result);
#endif
}

}  // namespace builtins
