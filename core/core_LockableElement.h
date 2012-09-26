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

    LockableElement() : m_lockState(NEVER_LOCKED), m_wasLocked(false), m_isDeleted(false)
    {}

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

    void forceSetWasLocked(bool b)
    {
        m_wasLocked = b;
    }

    virtual void updateGui() = 0;

    bool isDeleted()
    {
        return m_isDeleted;
    }

    bool deleteObject()
    {
        m_isDeleted = true;
    }

    bool undeleteObject()
    {
        m_isDeleted = false;
    }

private:
    LockType m_lockState;
    bool m_wasLocked;
    bool m_isDeleted;
};

#endif // CORE_LOCKABLEELEMENT_H
