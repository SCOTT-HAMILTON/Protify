#ifndef FOLLOWEDPROCESSITEM_H
#define FOLLOWEDPROCESSITEM_H

#include "processitem.h"

#include <QTimer>

class FollowedProcessItem : public ProcessItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
public:
    explicit FollowedProcessItem(const QString& name,
                         int pid,
                         bool followed,
                         QObject *parent = nullptr);
    FollowedProcessItem(const ProcessItem& copyItem);
    ~FollowedProcessItem();
    bool running() const;
    void setRunning(bool running);

signals:
    void runningChanged(bool);
    void processDied(int);

private:
    bool m_running;
    void initWatcher();
    QTimer* watcher;
};

#endif // FOLLOWEDPROCESSITEM_H
