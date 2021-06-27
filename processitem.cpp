#include "processitem.h"

ProcessItem::ProcessItem(const QString &name, int pid, bool followed, QObject *parent):
    QObject(parent), m_name(name), m_pid(pid), m_followed(followed)
{
}

ProcessItem::ProcessItem(const ProcessItem &copyItem) :
    QObject(copyItem.parent()),
    m_name(copyItem.name()),
    m_pid(copyItem.pid()),
    m_followed(copyItem.followed())
{
}

QString ProcessItem::name() const
{
    return m_name;
}

int ProcessItem::pid() const
{
    return m_pid;
}

bool ProcessItem::followed() const
{
    return m_followed;
}

void ProcessItem::setFollowed(bool followed)
{
    m_followed = followed;
}
