#ifndef PROCESSITEM_H
#define PROCESSITEM_H

#include <QObject>

class ProcessItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int pid READ pid)
    Q_PROPERTY(bool followed READ followed)


public:
    explicit ProcessItem(const QString& name,
                         int pid,
                         bool followed,
                         QObject *parent = nullptr);
    ProcessItem(const ProcessItem& copyItem);
    QString name() const;
    int pid() const;
    bool followed() const;

    void setFollowed(bool followed);

protected:
    const QString m_name;
    const int m_pid;
    bool m_followed;
};

#endif // PROCESSITEM_H
