#include <stdio.h>
#include <Windows.h>
#include <time.h> 
DWORD ReadRegistry(HKEY hKey, LPCTSTR keyPath, LPCTSTR valueName) {
    DWORD data = 0;
    HKEY regKey;
    
    if (RegOpenKeyEx(hKey, keyPath, 0, KEY_READ, &regKey) == ERROR_SUCCESS) {
        DWORD type;
        DWORD dataSize = sizeof(DWORD);

        if (RegQueryValueEx(regKey, valueName, NULL, &type, (LPBYTE)&data, &dataSize) != ERROR_SUCCESS) {
            data = 0; 
        }

        RegCloseKey(regKey);
    }

    return data;
}

void WriteRegistry(HKEY hKey, LPCTSTR keyPath, LPCTSTR valueName, DWORD data) {
    HKEY regKey;
    
    if (RegCreateKeyEx(hKey, keyPath, 0, NULL, 0, KEY_WRITE, NULL, &regKey, NULL) == ERROR_SUCCESS) {
        RegSetValueEx(regKey, valueName, 0, REG_DWORD, (const BYTE*)&data, sizeof(DWORD));
        RegCloseKey(regKey);
    }
}

DWORD GetVolumeSerial() {
    DWORD serial = 0;
    GetVolumeInformationW(L"C:\\", NULL, 0, &serial, NULL, NULL, NULL, 0); 
    return serial;
}
void GenerateLicense() {
    srand((unsigned int)time(NULL));
    int value1 = rand() % 256;
    int value2 = rand() % 256;
    int value3 = rand() % 256;

    char license[17];
    sprintf(license, "%02X%02X%02X7C%02X%02X%02X%02X%02X", value1, value2 ^ 0x7C, ~value1 & 0xFF, value2, value3 % 255, (value3 % 255) ^ 7, value1 ^ (value3 % 255) & 0xFF);
}
int ParseAppVersion(LPCTSTR appVersion) {
    const TCHAR* parsedVersion = appVersion + 14;
    int appVersionInt = 0;

    while (*parsedVersion != '\0') {
        if (*parsedVersion != '.')
            appVersionInt = appVersionInt * 10 + (*parsedVersion - '0');
        parsedVersion++;
    }

    return appVersionInt;
}

void GenerateRegistryKey(DWORD serial, int appVersion) {
    char key[20];
    int magicNumber = appVersion ^ ((~serial >> 1) + 736) ^ 0x590D4;
    sprintf(key, "SN%d", magicNumber);
}



int main() {
    srand((unsigned int)time(NULL));
    DWORD appVer = ParseAppVersion((LPCTSTR)ReadRegistry(HKEY_CURRENT_USER, (LPCTSTR)"Software\\MadeForNet\\HTTPDebuggerPro", (LPCTSTR)"AppVer"));
    DWORD serialNum = GetVolumeSerial();
    GenerateRegistryKey(serialNum, (int)appVer);

    GenerateLicense();
    return 0;
}
