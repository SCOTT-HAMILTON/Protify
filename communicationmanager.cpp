#include "communicationmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QDataStream>
#include <QSettings>

CommunicationManager::CommunicationManager(
        QObject *parent) :
    QObject(parent),
    connectionInitialized(false),
    server_socket(nullptr)
{
    QSettings settings;
    m_name = settings.value("hostname", "MY-PC").toString();
}

void CommunicationManager::initializeConnection()
{
    if (connectionInitialized) {
        qDebug() << "[log] connection already initialized";
        return;
    }
    isConnected.store(false);

    auto zeroConf = new QZeroConf(this);
    QObject::connect(zeroConf, &QZeroConf::servicePublished, [](){qDebug() << "Service published !";});
    QObject::connect(zeroConf, &QZeroConf::error, [](QZeroConf::error_t e){
        switch (e) {
        case QZeroConf::noError:
            qDebug() << "No Error";
            break;
        case QZeroConf::serviceRegistrationFailed:
            qDebug() << "serviceRegistrationFailed";
            break;
        case QZeroConf::serviceNameCollision:
            qDebug() << "serviceNameCollision";
            break;
        case QZeroConf::browserFailed:
            qDebug() << "browserFailed";
            break;
        default:
            qDebug() << "Unknown error code: " << e;
        }
    });
    QObject::connect(zeroConf, &QZeroConf::serviceAdded,
            [this](QZeroConfService s){
        qDebug() << "Service added : "
                 << "name: " << s->name()
                 << "type: " << s->type()
                 << "domain: " << s->domain()
                 << "host: " << s->host()
                 << "ip: " << s->ip().toString()
                 << "port: " << s->port();
        connectToService(s);
    });
    QObject::connect(zeroConf, &QZeroConf::serviceRemoved, [](QZeroConfService s){
        qDebug() << "Service removed : "
                 << "name: " << s->name()
                 << "type: " << s->type()
                 << "domain: " << s->domain()
                 << "host: " << s->host()
                 << "ip: " << s->ip().toString()
                 << "port: " << s->port();
    });
    QObject::connect(zeroConf, &QZeroConf::serviceUpdated,
            [this](QZeroConfService s){
        qDebug() << "Service updated : "
                 << "name: " << s->name()
                 << "type: " << s->type()
                 << "domain: " << s->domain()
                 << "host: " << s->host()
                 << "ip: " << s->ip().toString()
                 << "port: " << s->port();
		connectToService(s);
    });
    zeroConf->startBrowser("_examplednssd._tcp");
    connectionInitialized = true;
    qDebug() << "Started Browsing...";
}
void CommunicationManager::sendProcessDied(const QString& name) {
	if (server_socket && isConnected.load()) {
		server_socket->write("_\n");
        server_socket->write(QString("notif="+tr("Process %1 finished.\n")).arg(name)
                             .toStdString().c_str());
	}
}
void CommunicationManager::sendProcessus(const QStringList &processus)
{
	if (server_socket && isConnected.load()) {
		qDebug() << "[log] Sending Processus";
		auto jsonArray = QJsonArray::fromStringList(processus);
		QJsonDocument jsonDoc(jsonArray);
		server_socket->write("_\n");
		server_socket->write("processus="+jsonDoc.toJson(QJsonDocument::Compact)+"\n");
	}
}

void CommunicationManager::sendName()
{
	if (server_socket && isConnected.load()) {
		server_socket->write("_\n");
		server_socket->write(QString("name="+m_name).toUtf8()+"\n");
    }
}

void CommunicationManager::changeName(const QString &newName)
{
    m_name = newName;
}

void CommunicationManager::connectToService(const QZeroConfService s) {
	if (isConnected.load() || isTryingToConnect.load()) {
		return;
	}
	isTryingToConnect.store(true);
	auto server_port = QString(s->txt()[SERVER_PORT]).toInt(nullptr, 10);
	server_socket = new QTcpSocket(this);
	QObject::connect(server_socket, &QTcpSocket::errorOccurred,
			[this]() {
		isTryingToConnect.store(false);
		qDebug() << "[error] server_socket error : " << server_socket->errorString();
	});
	QObject::connect(server_socket, &QTcpSocket::connected,
			[this]() {
		qDebug() << "\n\n\t[log] NEW CONNECTION\n";
		isTryingToConnect.store(false);
		isConnected.store(true);
		sendName();
		emit newConnectedClient();
		qDebug() << "[log] server : connected, wrote message";
	});
	QObject::connect(server_socket, &QIODevice::readyRead, [this]() {
		QByteArray b;
		QDataStream in(server_socket);
		in >> b;
		qDebug() << "[log] server : received new message : " << QString::fromUtf8(b);
	});
	QObject::connect(server_socket, &QTcpSocket::disconnected, [this]() {
		qDebug() << "[log] server : client "
				 << server_socket->peerAddress().toString() << "disconnected.";
	});
	qDebug() << "[log] Trying to connect to host on port" << server_port;
	server_socket->connectToHost(s->ip(), server_port);
}


