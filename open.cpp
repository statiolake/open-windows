#include <windows.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>

std::wstring from_lpstr(LPSTR lpstr) {
    auto needed_size = MultiByteToWideChar(
            CP_ACP, 0, lpstr, -1, NULL, 0
    );

    std::unique_ptr<wchar_t> to(new wchar_t[needed_size]);
    MultiByteToWideChar(
            CP_ACP, 0, lpstr, -1, to.get(), needed_size
    );

    return std::wstring(to.get());
}

std::vector<std::wstring> parse_args(std::wstring const &arg) {
    bool in_double_quote = false;
    bool in_single_quote = false;
    std::vector<std::wstring> result;
    std::wstring current;
    for (auto it = arg.begin(); it != arg.end(); ++it) {
        auto c = *it;
        if (c == L' ' && !in_double_quote && !in_single_quote) {
            if (!current.empty()) {
                result.push_back(std::move(current));
            }
            current = std::wstring();
        } else if (c == L'\"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
        } else if (c == L'\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        result.push_back(std::move(current));
    }

    return result;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow) {
    std::wstring arg = from_lpstr(pCmdLine);
    auto args = parse_args(arg);

    if (args.empty()) {
        args.push_back(L"-h");
    }

    if (args[0] == L"-h") {
        MessageBox(NULL, L"normal usage: open.exe [filename] (open file with associated application)", L"help (1/3)", MB_OK);
        MessageBox(NULL, L"explorer usage: open.exe -e [foldername] (open explorer pointing the file)", L"help (2/3)", MB_OK);
        MessageBox(NULL, L"show help: open.exe -h (show this help)", L"help (3/3)", MB_OK);
        return 0;
    }

    std::wstring verb = L"open";
    if (args[0] == L"-e") {
        verb = L"explore";
        args.erase(args.begin());
    }

    if (args.empty()) {
        MessageBox(NULL, L"Please specify filename.", L"Missing argument", MB_OK | MB_ICONSTOP);
        return 1;
    }

    // when ShellExecute fails, value less than or equal to 32 is returned.
    // that's the form of HINSTACE * for backward compatibility, but can be casted to integer.
    if (reinterpret_cast<size_t>(ShellExecute(NULL, verb.c_str(), args[0].c_str(), NULL, NULL, SW_SHOWNORMAL)) <= 32) {
        LPVOID lpMsgBuf;
        FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
                (LPTSTR) &lpMsgBuf, 0, NULL );
        MessageBox(NULL, (LPCWSTR)lpMsgBuf, L"Error", MB_OK | MB_ICONSTOP);
        // バッファを解放する。
        LocalFree(lpMsgBuf);
    }
    return 0;
}
