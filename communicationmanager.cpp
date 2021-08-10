#include "communicationmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QDataStream>
#include <QSettings>
#include <QJsonObject>

CommunicationManager::CommunicationManager(
        QObject *parent) :
    QObject(parent),
    m_uuid(QUuid::createUuid()),
    m_processus()
{
    m_name = QSettings().value("hostname", "MY-PC").toString();
}

CommunicationManager::~CommunicationManager()
{
    running.store(false);
    if (server_thread) {
        server_thread->join();
    }
}

void CommunicationManager::initializeConnection()
{
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
//        connectToService(s);
    });
    zeroConf->startBrowser("_examplednssd._tcp");
//    connectToService("192.168.1.19", 7777);
    qDebug() << "Started Browsing...";
}
void CommunicationManager::sendProcessDied(const QString& name) {
    sendDiedProcessNotifStackMutex.lock();
    qDebug() << "Process Stack size before: " << sendDiedProcessNotifStack.size();
    sendDiedProcessNotifStack.push(name);
    qDebug() << "Process Stack size after: " << sendDiedProcessNotifStack.size();
    sendDiedProcessNotifStackMutex.unlock();
}

void CommunicationManager::updateProcessus(const QStringList &processus)
{
   m_processus = processus;
}

void CommunicationManager::changeName(const QString &newName)
{
    m_name = newName;
}

void CommunicationManager::connectToService(const QZeroConfService s) {
    qDebug() << "Connecting to Service";
    auto server_port = QString(s->txt()[SERVER_PORT_ZERO_CONF_KEY]).toInt(nullptr, 10);
    connectToService(s->ip().toString(), server_port);
}

void CommunicationManager::connectToService(const QString &address, int server_port)
{
    qDebug() << "Connecting to Service";
    if (!server_thread) {
        ConnectionArgs args = {address, server_port};
        server_thread = std::make_unique<std::thread>(&CommunicationManager::server_loop, this, args);
    } else {
        qDebug() << "Server is already running!";
    }
}

void CommunicationManager::server_loop(const ConnectionArgs& args) {
    zmq::context_t ctx;
    zmq::socket_t publisher(ctx, zmq::socket_type::pub);
    auto server_address = "tcp://"+args.address+":"+QString::number(args.port);
    qDebug() << "Connecting to server: " << server_address;
    publisher.connect(server_address.toStdString());
    running.store(true);
    while(running.load()) {
        qDebug() << "Process Stack size: " << sendDiedProcessNotifStack.size();
        while (sendDiedProcessNotifStack.size() > 0) {
            sendDiedProcessNotifStackMutex.lock();
            auto process = sendDiedProcessNotifStack.pop();
            sendDiedProcessNotifStackMutex.unlock();
            QString message("notifdied="+process);
            qDebug() << "Publisher sent message: " << message;
            publisher.send(message.toStdString().c_str(), message.size());
        }
        QJsonObject object({
            QPair("alive", QJsonValue(true)),
            QPair("name", QJsonValue(m_name)),
            QPair("processus", QJsonValue(QJsonArray::fromStringList(m_processus))),
            QPair("uuid", QJsonValue(m_uuid.toString(QUuid::StringFormat::WithoutBraces)))
        });
        qDebug() << "Sending processus: " << m_processus;
        QJsonDocument jsonDoc(object);
        QString jsonMessage = jsonDoc.toJson(QJsonDocument::Compact);
        publisher.send(jsonMessage.toStdString().c_str(), jsonMessage.size());
        zmq_sleep(2);
    }
    publisher.close();
    ctx.close();
}

