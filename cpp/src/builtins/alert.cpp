// alert.cpp
// Implements: alert.h

#include "builtins/alert.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace builtins {

int Alert::show_dialog(const std::string& title, const std::string& message, int button_type) {
#ifdef _WIN32
    UINT uType;
    switch (button_type) {
        case 0:  // OK only
            uType = MB_OK | MB_ICONINFORMATION;
            break;
        case 1:  // OK/Cancel
            uType = MB_OKCANCEL | MB_ICONINFORMATION;
            break;
        case 2:  // Yes/No/Cancel
            uType = MB_YESNOCANCEL | MB_ICONQUESTION;
            break;
        default:
            return -1;
    }

    int result = MessageBoxA(NULL, message.c_str(), title.c_str(), uType);

    switch (button_type) {
        case 0:
            return 0;
        case 1:
            return (result == IDOK) ? 1 : 0;
        case 2:
            if (result == IDYES) return 0;
            if (result == IDNO) return 1;
            if (result == IDCANCEL) return 2;
            return 2;
        default:
            return -1;
    }

#elif __linux__
    // TODO
    return -1;

#elif __APPLE__
    CFStringRef cf_title = CFStringCreateWithCString(NULL, title.c_str(), kCFStringEncodingUTF8);
    CFStringRef cf_message =
        CFStringCreateWithCString(NULL, message.c_str(), kCFStringEncodingUTF8);

    CFOptionFlags response;
    CFUserNotificationDisplayAlert(
        0,                                  // timeout (0 = no timeout)
        kCFUserNotificationNoteAlertLevel,  // flags
        NULL,                               // iconURL
        NULL,                               // soundURL
        NULL,                               // localizationURL
        cf_title, cf_message,
        button_type == 0 ? CFSTR("OK") : (button_type == 1 ? CFSTR("OK") : CFSTR("Yes")),
        button_type == 0 ? NULL : (button_type == 1 ? CFSTR("Cancel") : CFSTR("No")),
        button_type == 2 ? CFSTR("Cancel") : NULL, &response);

    CFRelease(cf_title);
    CFRelease(cf_message);

    switch (button_type) {
        case 0:
            return 0;
        case 1:
            return (response == kCFUserNotificationDefaultResponse) ? 1 : 0;
        case 2:
            if (response == kCFUserNotificationDefaultResponse) return 0;    // Yes
            if (response == kCFUserNotificationAlternateResponse) return 1;  // No
            return 2;                                                        // Cancel
        default:
            return -1;
    }
    return -1;

#else
    // Fallback: no GUI available
    return -1;
#endif
}

Result<RuntimeValue> Alert::show_ok(const std::string& title, const std::string& message) {
    show_dialog(title, message, 0);
    RuntimeValue result;
    result.value = RuntimeValue::Null{};
    return ok(result);
}

Result<RuntimeValue> Alert::show_ok_cancel(const std::string& title, const std::string& message) {
    int dialog_result = show_dialog(title, message, 1);
    RuntimeValue result;
    result.value = RuntimeValue::Bool{dialog_result == 1};
    return ok(result);
}

Result<RuntimeValue> Alert::show_yes_no_cancel(const std::string& title,
                                               const std::string& message) {
    int dialog_result = show_dialog(title, message, 2);
    RuntimeValue result;
    result.value = RuntimeValue::Int{dialog_result};
    return ok(result);
}

}  // namespace builtins