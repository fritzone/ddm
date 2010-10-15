#include "Workspace.h"

Workspace* Workspace::m_instance = 0;

Workspace::Workspace() : m_currentSolution(0), m_solutions()
{}

Workspace* Workspace::getInstance()
{
    if(m_instance == 0)
    {
        m_instance = new Workspace();
    }

    return m_instance;
}
