#ifndef CORE_LOCKABLEELEMENT_H
#define CORE_LOCKABLEELEMENT_H

class LockableElement
{
public:

    enum LockType
    {
        NEVER_LOCKED = 0,
        LOCKED = 1,
        UNLOCKED = 2
    };

    LockableElement() : m_lockState(NEVER_LOCKED), m_wasLocked(false) {}

    void lock()
    {
        m_lockState = LOCKED;
        m_wasLocked = true;
    }

    void unlock()
    {
        m_lockState = UNLOCKED;
    }

    bool isLocked() const
    {
        return m_lockState == LOCKED;
    }

    bool wasLocked() const
    {
        return m_wasLocked;
    }

    LockType state() const
    {
        return m_lockState;
    }

    virtual void updateGui() = 0;

private:
    LockType m_lockState;
    bool m_wasLocked;
};

#endif // CORE_LOCKABLEELEMENT_H
