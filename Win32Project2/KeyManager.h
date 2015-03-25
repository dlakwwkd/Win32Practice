#pragma once
#include <windows.h>

#define FREEKEY	0x10	// ������ ���� ����.
#define PULLKEY	0x20	// ������ �������
#define PUSHKEY	0x40	// ������ ����
#define HOLDKEY	0x80	// ������ �ִ� ��

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

