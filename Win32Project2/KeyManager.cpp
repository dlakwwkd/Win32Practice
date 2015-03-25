#include "KeyManager.h"

KeyManager::KeyManager()
{
}


KeyManager::~KeyManager()
{
}

void KeyManager::KeyInput()
{
    // ------------------ Init -------------------
    // 매 프레임 한번만 처리하면 됨.
    // 입력감지
    ZeroMemory(m_ByOldKey, 256);
    ZeroMemory(m_ByKey, 256);

    if (GetKeyboardState(m_ByKey))
    {
        // DOWN 및 UP계산
        for (int i = 0; i < 256; i++)
        {
            if (m_ByKey[i] & HOLDKEY)
            {
                if (!m_ByOldKey[i])
                {
                    m_ByOldKey[i] = 1;
                    m_ByKey[i] |= PUSHKEY;
                }
            }
            else
            {
                if (m_ByOldKey[i])
                {
                    m_ByOldKey[i] = 0;
                    m_ByKey[i] = PULLKEY;
                }
            }
        }
    }
}

bool KeyManager::KeyCheck(BYTE key, BYTE state)
{
    if (m_ByKey[key] & state)
        return true;
    return false;
}
