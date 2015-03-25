#pragma once
#include <windows.h>

#define FREEKEY	0x10	// 눌르지 않은 상태.
#define PULLKEY	0x20	// 눌렀다 띄었을때
#define PUSHKEY	0x40	// 누르자 마자
#define HOLDKEY	0x80	// 누르고 있는 중

class KeyManager
{
public:
    static KeyManager* getInstance()
    {
        static KeyManager keyManager;
        return &keyManager;
    }

    void KeyInput();
    bool KeyCheck(BYTE key, BYTE state);


private:
    KeyManager();
    ~KeyManager();

    BYTE m_ByOldKey[256];
    BYTE m_ByKey[256];
};

