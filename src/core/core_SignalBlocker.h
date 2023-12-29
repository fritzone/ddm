#ifndef CORE_SIGNALBLOCKER_H
#define CORE_SIGNALBLOCKER_H

#include <QObject>

template<class T>
class SignalBlocker final
{
public:
    SignalBlocker(T o, bool block = true): m_object(o), m_alreadyBlocked(m_object->signalsBlocked())
    {
        if(block)
        {
            if (!m_alreadyBlocked)
            {
                m_object->blockSignals(true);
            }
        }
    }
    ~SignalBlocker()
    {
        if (!m_alreadyBlocked)
        {
            m_object->blockSignals(false);
        }
    }

    T operator -> () const
    {
        return m_object;
    }

private:
    T m_object;
    bool m_alreadyBlocked;
};

#define blocked(o) (SignalBlocker<decltype (o)>(o))

#endif // CORE_SIGNALBLOCKER_H
