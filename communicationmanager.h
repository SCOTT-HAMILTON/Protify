#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include <QObject>
#include <atomic>
#include <QUuid>
#include <memory>
#include <QTcpSocket>
#include <zmq_addon.hpp>
#include "QtZeroConf/qzeroconf.h"
#include <thread>
#include <QStack>

struct ConnectionArgs {
    const QString address;
    int port;
};

class CommunicationManager : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationManager(QObject *parent);
    ~CommunicationManager();
    constexpr static const char* SERVER_PORT_ZERO_CONF_KEY = "SRV_PORT";
    void initializeConnection();

signals:
    void newConnectedClient();

public slots:
    void sendProcessDied(const QString& name);
    void sendPing();
    void updateProcessus(const QStringList& processus);
    void changeName(const QString& newName);

private:
    std::unique_ptr<std::thread> server_thread;
    QStringList m_processus;
    std::atomic_bool running;
    QString m_name;
    QUuid m_uuid;

    void connectToService(const QZeroConfService s);
    void connectToService(const QString& address, int port);
    void server_loop(const ConnectionArgs& args);

    void sendMessage(const QString& name);
    static QString strip(const QString& str);
};

#endif // COMMUNICATIONMANAGER_H
