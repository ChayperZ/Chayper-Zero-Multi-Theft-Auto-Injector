#include <windows.h>
#include <tlhelp32.h>
#include <shobjidl.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>


enum ConsoleColor {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7,
    GRAY = 8,
    BRIGHT_BLUE = 9,
    BRIGHT_GREEN = 10,
    BRIGHT_CYAN = 11,
    BRIGHT_RED = 12,
    BRIGHT_MAGENTA = 13,
    BRIGHT_YELLOW = 14,
    BRIGHT_WHITE = 15
};

class ConsoleUI {
private:
    HANDLE hConsole;

public:
    ConsoleUI() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(CP_UTF8);

        SetConsoleCP(CP_UTF8);
    }

    void SetColor(ConsoleColor color) {
        SetConsoleTextAttribute(hConsole, color);
    }

    void ResetColor() {
        SetConsoleTextAttribute(hConsole, WHITE);
    }

    void PrintColored(const std::wstring& text, ConsoleColor color) {
        SetColor(color);
        std::wcout << text;
        ResetColor();
    }

    void ClearScreen() {
        system("cls");
    }

    void SetWindowTitle(const std::wstring& title) {
        SetConsoleTitleW(title.c_str());
    }

    void MakeWindowTransparent() {
        HWND hwnd = GetConsoleWindow();
        if (hwnd != 0) {
            LONG style = GetWindowLongW(hwnd, GWL_EXSTYLE);
            SetWindowLongW(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hwnd, 0, 230, LWA_ALPHA);
        }
    }

    HANDLE GetConsoleHandle() const {
        return hConsole;
    }
};

class AnimatedTitle {
private:
    bool running;
    std::thread titleThread;

public:
    void Start() {
        running = true;
        titleThread = std::thread([this]() {
            std::vector<std::wstring> titles = {
                L"Chayper Zero DLL Injector",
                L"By Chayper Zero Team",
                L"https://discord.gg/jMPCKfQaAM"
            };

            while (running) {
                for (const auto& title : titles) {
                    for (size_t i = 1; i <= title.length(); i++) {
                        SetConsoleTitleW(title.substr(0, i).c_str());
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            }
            });
    }

    void Stop() {
        running = false;
        if (titleThread.joinable()) {
            titleThread.join();
        }
    }
};

class GradientEffect {
private:
    ConsoleUI ui;

public:
    void PrintGradientLine() {
        int width = 80;
        for (int i = 0; i < width; ++i) {
            int red = 255 - static_cast<int>(static_cast<float>(i) / width * 200);
            int green = static_cast<int>(static_cast<float>(i) / width * 255);

            WORD color;
            if (red > 128 && green > 128) {
                color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            else if (red > 128) {
                color = FOREGROUND_RED | FOREGROUND_INTENSITY;
            }
            else if (green > 128) {
                color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            else {
                color = FOREGROUND_RED | FOREGROUND_GREEN;
            }

            SetConsoleTextAttribute(ui.GetConsoleHandle(), color);
            std::wcout << L"-";
        }
        ui.ResetColor();
        std::wcout << L"\n";
    }
};

void ShowBanner() {
    ConsoleUI ui;
    ui.ClearScreen();
    ui.SetColor(BRIGHT_MAGENTA);

    std::wcout << LR"(  
    Tool By Chayper Zero Team | https://discord.gg/jMPCKfQaAM
)" << std::endl;

    ui.ResetColor();
}

void PrintStatus(const std::wstring& message, ConsoleColor color = WHITE, bool success = true) {
    ConsoleUI ui;

    ui.SetColor(WHITE);
    std::wcout << L"[";

    if (success) {
        ui.SetColor(BRIGHT_GREEN);
        std::wcout << L"SUCCESS";
    }
    else {
        ui.SetColor(BRIGHT_RED);
        std::wcout << L"ERROR";
    }

    ui.SetColor(WHITE);
    std::wcout << L"] ";

    ui.SetColor(color);
    std::wcout << message << std::endl;
    ui.ResetColor();
}

void PrintInfo(const std::wstring& message, ConsoleColor color = BRIGHT_CYAN) {
    ConsoleUI ui;

    ui.SetColor(WHITE);
    std::wcout << L"[";
    ui.SetColor(BRIGHT_CYAN);
    std::wcout << L"INFO";
    ui.SetColor(WHITE);
    std::wcout << L"] ";

    ui.SetColor(color);
    std::wcout << message << std::endl;
    ui.ResetColor();
}

void PrintWarning(const std::wstring& message) {
    ConsoleUI ui;

    ui.SetColor(WHITE);
    std::wcout << L"[";
    ui.SetColor(BRIGHT_YELLOW);
    std::wcout << L"WARNING";
    ui.SetColor(WHITE);
    std::wcout << L"] ";

    ui.SetColor(BRIGHT_YELLOW);
    std::wcout << message << std::endl;
    ui.ResetColor();
}

class Spinner {
private:
    std::vector<wchar_t> frames;
    size_t currentFrame;
    bool running;
    std::thread spinnerThread;
    ConsoleUI ui;

public:
    Spinner() : frames{ L'|', L'/', L'-', L'\\' }, currentFrame(0), running(false) {}

    void Start(const std::wstring& message) {
        running = true;
        spinnerThread = std::thread([this, message]() {
            while (running) {
                ui.SetColor(BRIGHT_CYAN);
                std::wcout << L"\r[" << frames[currentFrame] << L"] " << message;
                currentFrame = (currentFrame + 1) % frames.size();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            });
    }

    void Stop() {
        running = false;
        if (spinnerThread.joinable()) {
            spinnerThread.join();
        }
        std::wcout << L"\r" << std::wstring(50, L' ') << L"\r";
    }
};

std::wstring GetFileNameFromPath(const std::wstring& path) {
    size_t lastSlash = path.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        return path.substr(lastSlash + 1);
    }
    return path;
}

bool IsElevated() {
    BOOL isElevated = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return isElevated;
}

std::wstring GetMTAPath() {
    HKEY hKey;
    wchar_t path[MAX_PATH] = { 0 };
    DWORD size = sizeof(path);
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Multi Theft Auto: San Andreas All\\1.6",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"Last Run Path", NULL, NULL, (LPBYTE)path, &size);
        RegCloseKey(hKey);
        return std::wstring(path);
    }
    return L"";
}

DWORD GetProcessIdByName(const std::wstring& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    DWORD pid = 0;

    if (Process32FirstW(snapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName.c_str()) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return pid;
}

std::wstring SelectDLLFile() {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        PrintStatus(L"Failed to initialize COM.", BRIGHT_RED, false);
        return L"";
    }

    IFileOpenDialog* pFileOpen;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (FAILED(hr)) {
        PrintStatus(L"Failed to create file dialog instance.", BRIGHT_RED, false);
        CoUninitialize();
        return L"";
    }

    COMDLG_FILTERSPEC filter[] = { { L"DLL Files", L"*.dll" }, { L"All Files", L"*.*" } };
    pFileOpen->SetFileTypes(2, filter);
    pFileOpen->SetTitle(L"Select DLL File");

    hr = pFileOpen->Show(NULL);
    std::wstring dllPath;
    if (SUCCEEDED(hr)) {
        IShellItem* pItem;
        if (SUCCEEDED(pFileOpen->GetResult(&pItem))) {
            PWSTR pszFilePath;
            if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
                dllPath = pszFilePath;
                CoTaskMemFree(pszFilePath);
            }
            pItem->Release();
        }
    }
    else {
        PrintInfo(L"No DLL selected or dialog cancelled.");
    }

    pFileOpen->Release();
    CoUninitialize();
    return dllPath;
}

bool SetAutodialDLL(const std::wstring& dllPath, const std::wstring& targetProcess = L"gta_sa.exe") {
    PrintInfo(L"Waiting for " + targetProcess + L" to start...");

    Spinner spinner;
    spinner.Start(L"Searching for " + targetProcess + L"...");

    DWORD pid = 0;
    const int timeoutSeconds = 30;
    auto startTime = GetTickCount64();

    while (pid == 0 && (GetTickCount64() - startTime) < (timeoutSeconds * 1000)) {
        pid = GetProcessIdByName(targetProcess);
        if (pid == 0) Sleep(500);
    }

    spinner.Stop();

    if (pid == 0) {
        PrintStatus(L"Process not found within timeout period.", BRIGHT_RED, false);
        return false;
    }

    PrintStatus(L"Found " + targetProcess + L" with PID: " + std::to_wstring(pid), BRIGHT_GREEN);

    PrintInfo(L"Waiting for process initialization...");
    Sleep(4000);

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\ControlSet001\\Services\\WinSock2\\Parameters",
        0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        PrintStatus(L"Registry update failed: Access denied. Run as administrator.", BRIGHT_RED, false);
        return false;
    }

    DWORD dataSize = static_cast<DWORD>((dllPath.size() + 1) * sizeof(wchar_t));
    if (RegSetValueExW(hKey, L"AutodialDLL", 0, REG_SZ, (LPBYTE)dllPath.c_str(), dataSize) == ERROR_SUCCESS) {
        PrintStatus(L"DLL [" + GetFileNameFromPath(dllPath) + L"] injected successfully!", BRIGHT_GREEN);
        RegCloseKey(hKey);
        return true;
    }

    PrintStatus(L"Failed to set AutodialDLL in registry.", BRIGHT_RED, false);
    RegCloseKey(hKey);
    return false;
}

void ClearAutodialDLL() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\ControlSet001\\Services\\WinSock2\\Parameters",
        0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValueW(hKey, L"AutodialDLL");
        RegCloseKey(hKey);
    }
}


std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

int main() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);


    std::locale::global(std::locale(""));

    ConsoleUI ui;
    AnimatedTitle titleAnim;
    GradientEffect gradient;


    ui.MakeWindowTransparent();
    titleAnim.Start();


    ShowBanner();


    gradient.PrintGradientLine();


    if (!IsElevated()) {
        PrintStatus(L"This program requires administrator privileges.", BRIGHT_RED, false);
        std::wcout << L"Press any key to exit...\n";
        std::wcin.get();
        titleAnim.Stop();
        return 1;
    }

    PrintInfo(L"Initializing DLL Injector...", BRIGHT_MAGENTA);


    std::wstring mtaPath = GetMTAPath();
    if (mtaPath.empty()) {
        PrintStatus(L"Could not find MTA path in registry.", BRIGHT_RED, false);
        std::wcout << L"Press any key to exit...\n";
        std::wcin.get();
        titleAnim.Stop();
        return 1;
    }

    PrintStatus(L"MTA Path found: " + mtaPath, BRIGHT_GREEN);


    PrintInfo(L"Please select DLL file to inject...", BRIGHT_CYAN);
    std::wstring dllPath = SelectDLLFile();
    if (dllPath.empty()) {
        PrintStatus(L"No DLL selected!", BRIGHT_RED, false);
        std::wcout << L"Press any key to exit...\n";
        std::wcin.get();
        titleAnim.Stop();
        return 1;
    }

    PrintStatus(L"Selected DLL: " + GetFileNameFromPath(dllPath), BRIGHT_GREEN);


    PrintInfo(L"Launching MTA:SA...");
    ShellExecuteW(NULL, L"open", mtaPath.c_str(), NULL, NULL, SW_SHOWNORMAL);


    if (!SetAutodialDLL(dllPath)) {
        titleAnim.Stop();
        return 1;
    }


    PrintInfo(L"Cleaning up registry...");
    Sleep(10000);
    ClearAutodialDLL();

    PrintStatus(L"Registry cleaned successfully!", BRIGHT_GREEN);
    PrintInfo(L"Injection process completed!", BRIGHT_MAGENTA);

    std::wcout << L"Press any key to exit...\n";
    std::wcin.get();

    titleAnim.Stop();
    return 0;
}