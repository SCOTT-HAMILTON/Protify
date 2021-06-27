#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include <QObject>
#include <atomic>
#include <QTcpSocket>
#include "qzeroconf.h"

class CommunicationManager : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationManager(QObject *parent);
    constexpr static const char* SERVER_PORT = "SERVER_PORT";
    void initializeConnection();

signals:
	void newConnectedClient();

public slots:
    void sendProcessDied(const QString& name);
    void sendProcessus(const QStringList& processus);
    void sendName();
    void changeName(const QString& newName);

private:
    std::atomic_bool isConnected;
    std::atomic_bool isTryingToConnect;
    bool connectionInitialized;
    QTcpSocket* server_socket;
    QString m_name;

	void connectToService(const QZeroConfService s);
};

#endif // COMMUNICATIONMANAGER_H
