#include "Configuration.h"

Configuration::Configuration() : useDefaultLengths(true), m_allowForeignKeyPropagation(false), m_drawTableTypes(false)
{
}

Configuration::~Configuration()
{
}
