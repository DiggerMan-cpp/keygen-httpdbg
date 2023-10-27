import winreg
import ctypes
import random

def read_registry_value(hKey, keyPath, valueName):
    value_data = ""
    
    try:
        hRegistryKey = winreg.OpenKey(hKey, keyPath)
        dwType, _ = winreg.QueryValueEx(hRegistryKey, valueName)
        
        if dwType == winreg.REG_SZ:
            value_data = winreg.QueryValueEx(hRegistryKey, valueName)[0]
        
        winreg.CloseKey(hRegistryKey)
    except Exception as e:
        pass

    return value_data

def write_registry_value(hKey, keyPath, valueName, valueData):
    try:
        hRegistryKey, _ = winreg.CreateKey(hKey, keyPath)
        winreg.SetValueEx(hRegistryKey, valueName, 0, winreg.REG_SZ, valueData)
        winreg.CloseKey(hRegistryKey)
    except Exception as e:
        pass

def get_volume_serial_number():
    serial_number = 0
    ctypes.windll.kernel32.GetVolumeInformationW("C:\\", None, 0, ctypes.byref(serial_number), None, None, None, 0)
    return serial_number

def parse_app_ver(appVer):
    appVerParsed = appVer[14:]
    appVerParsed = appVerParsed.replace(".", "")
    return int(appVerParsed)

def gen_reg_key_name(serialNumber, appVer):
    reg_key = "SN"
    magic_num = appVer ^ ((~serialNumber >> 1) + 736) ^ 0x590D4
    reg_key += str(magic_num)
    return reg_key

def gen_license_key():
    v1 = random.randint(0, 255)
    v2 = random.randint(0, 255)
    v3 = random.randint(0, 255)
    buffer = f"{v1:02X}{v2 ^ 0x7C:02X}{~v1 & 0xFF:02X}7C{v2:02X}{v3 % 255:02X}{(v3 % 255) ^ 7:02X}{v1 ^ (v3 % 255) & 0xFF:02X}"
    return buffer

def main():
    random.seed()
    app_ver = parse_app_ver(read_registry_value(winreg.HKEY_CURRENT_USER, "Software\\MadeForNet\\HTTPDebuggerPro", "AppVer"))
    serial_number = get_volume_serial_number()
    reg_key = gen_reg_key_name(serial_number, app_ver)

    license_key = gen_license_key()
    write_registry_value(winreg.HKEY_CURRENT_USER, "Software\\MadeForNet\\HTTPDebuggerPro", reg_key, license_key)
    input()

if __name__ == "__main__":
    main()
