#include "followedprocessitem.h"
#include "processlister.h"

#include <QDebug>

FollowedProcessItem::FollowedProcessItem(
        const QString &name,
        int pid, bool followed,
        QObject *parent) :
    ProcessItem(name, pid, followed, parent), m_running(true),
    watcher(new QTimer(this))
{
    initWatcher();
}
FollowedProcessItem::FollowedProcessItem(const ProcessItem &copyItem) :
    ProcessItem(copyItem),
    m_running(true),
    watcher(new QTimer(this))
{
    initWatcher();
}
FollowedProcessItem::~FollowedProcessItem()
{
    qDebug() << "[log-pid-" << m_pid << "] Stopping the timer";
    watcher->stop();
    qDebug() << "[log-pid-" << m_pid << "] Exitted !";
}
bool FollowedProcessItem::running() const
{
    return m_running;
}

void FollowedProcessItem::setRunning(bool running)
{
    m_running = running;
    emit runningChanged(m_running);
}
void FollowedProcessItem::initWatcher() {
    watcher->setInterval(1000);
    QObject::connect(watcher, &QTimer::timeout,[this](){
		auto processes = ProcessLister::getRunningProcesses();
        auto res = std::find_if(processes.begin(), processes.end(),
                                [this](const ProcessLister::Process& p){
            return p.pid == m_pid;
        });
        if (res == processes.end()) {
            emit processDied(m_pid);
			watcher->stop();
        }
    });
    watcher->start();
}
