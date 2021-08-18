#include "communicationmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QDataStream>
#include <QSettings>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QUrlQuery>
#include <QNetworkReply>

CommunicationManager::CommunicationManager(
        QObject *parent) :
    QObject(parent),
    m_processus(),
    m_uuid(QUuid::createUuid())
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
            [](QZeroConfService s){
        qDebug() << "Service updated : "
                 << "name: " << s->name()
                 << "type: " << s->type()
                 << "domain: " << s->domain()
                 << "host: " << s->host()
                 << "ip: " << s->ip().toString()
                 << "port: " << s->port();
    });
    zeroConf->startBrowser("_examplednssd._tcp");
    qDebug() << "Started Browsing...";
}
void CommunicationManager::sendProcessDied(const QString& name) {
    QSettings settings;
    auto token = strip(settings.value("token", "XXXXXX").toString());
    auto gotifyAddress = strip(settings.value("gotify_address", "http://127.0.0.1").toString());
    auto gotifyPort = settings.value("gotify_port", 80).toInt();
    qDebug() << "gotifyAddress: " << gotifyAddress;
    QUrl url(gotifyAddress);
    url.setPort(gotifyPort);
    url.setPath("/message");
    url.setQuery(QUrlQuery({QPair(QString("token"), token)}));
    qDebug() << "[log] query url: " << url.toString();
    auto manager = new QNetworkAccessManager(this);
    auto request = new QHttpMultiPart(QHttpMultiPart::FormDataType, manager);
    {
        QHttpPart title, message, priority;
        title.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
        title.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"title\""));
        title.setBody("Protifydroid");
        message.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
        message.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"message\""));
        message.setBody(QString(tr("Process %1 finished running.")).arg(name).toUtf8());
        priority.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
        priority.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"priority\""));
        priority.setBody("5");
        request->append(title);
        request->append(message);
        request->append(priority);
    }
    auto answer = manager->post(QNetworkRequest(url), request);
    QObject::connect(answer, &QNetworkReply::errorOccurred, this, [](QNetworkReply::NetworkError e){
        qDebug() << "[error] network error occured: " << e;
    });
    qDebug() << "[log] sent post request.";
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
        QJsonObject object({
            QPair("alive", QJsonValue(true)),
            QPair("name", QJsonValue(m_name)),
            QPair("processus", QJsonValue(QJsonArray::fromStringList(m_processus))),
            QPair("uuid", QJsonValue(m_uuid.toString(QUuid::StringFormat::WithoutBraces)))
        });
        qDebug() << "Sending processus: " << m_processus;
        QJsonDocument jsonDoc(object);
        QString jsonMessage = jsonDoc.toJson(QJsonDocument::Compact);
        zmq::const_buffer buffer = zmq::buffer(jsonMessage.toStdString());
        publisher.send(buffer);
        zmq_sleep(2);
    }
    publisher.close();
    ctx.close();
}

QString CommunicationManager::strip(const QString &str)
{
    auto stdStr = str.toStdString();
    stdStr.erase(std::remove(stdStr.begin(), stdStr.end(), '\n'), stdStr.end());
    return QString::fromStdString(stdStr);
}
