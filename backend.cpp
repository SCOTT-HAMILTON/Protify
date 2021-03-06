#include "backend.h"
#include "processlister.h"

#include <algorithm>
#include <iostream>
#include <QList>
#include <QStringList>
#include <QSettings>

Backend::Backend(QObject *parent) :
    QObject(parent),
    communicationManager(new CommunicationManager(this))
{
    QSettings settings;
    setHostname(settings.value("hostname", "MY-PC").toString());
    setToken(settings.value("token", "").toString());
    setGotifyAddress(settings.value("gotify_address", "").toString());
    setGotifyPort(settings.value("gotify_port", 80).toInt());

    updateSearchResults("");
    QObject::connect(this, &Backend::followedItemsChanged,
                     this, &Backend::sendProcessus);
    QObject::connect(communicationManager, &CommunicationManager::newConnectedClient,
                     this, &Backend::sendProcessus);
    QObject::connect(this, &Backend::hostnameChanged,
                     communicationManager, &CommunicationManager::changeName);
}

Backend::~Backend()
{
    for (const auto it : m_searchResults) {
        it->deleteLater();
    }
    qDebug() << "Followed items : " << m_followedItems.size();
    std::cerr << "Followed items : " << m_followedItems.size();
    for (const auto it : m_followedItems) {
        qDebug() << "delete Later";
        it->deleteLater();
    }
    m_searchResults.clear();
    m_followedItems.clear();
}

QQmlObjectListModel<ProcessItem> *Backend::searchResults()
{
    return &m_searchResults;
}

QQmlObjectListModel<FollowedProcessItem> *Backend::followedItems()
{
    return &m_followedItems;
}

QString Backend::hostname() const
{
    return m_hostname;
}

QString Backend::token() const
{
    return m_token;
}

QString Backend::gotifyAddress() const
{
    return m_gotifyAddress;
}

int Backend::gotifyPort() const
{
    return m_gotifyPort;
}

void Backend::setHostname(const QString &hostname)
{
    m_hostname = hostname;
    QSettings settings;
    settings.setValue("hostname", hostname);
    emit hostnameChanged(m_hostname);
}

void Backend::setToken(const QString &token)
{
    m_token = token;
    QSettings settings;
    settings.setValue("token", token);
    emit tokenChanged(m_token);
}

void Backend::setGotifyAddress(const QString &address)
{
    m_gotifyAddress = address;
    QSettings settings;
    settings.setValue("gotify_address", address);
    emit gotifyAddressChanged(m_gotifyAddress);
}

void Backend::setGotifyPort(int port)
{

    m_gotifyPort = port;
    QSettings settings;
    qDebug() << "[log] new value for gotify port: " << port;
    settings.setValue("gotify_port", port);
    emit gotifyPortChanged(m_gotifyPort);
}

void Backend::startCommunications()
{
    communicationManager->initializeConnection();
}

void Backend::updateSearchResults(const QString& search)
{
    m_lastSearch = search;
    m_searchResults.clear();
    auto processes = ProcessLister::getRunningProcesses();
    for (const auto& it : processes) {
        const auto& name = it.name;
        if (name.toUpper().contains(search.toUpper())) {
            auto item_pid = it.pid;
            bool alreadyFollowed = false;
            for (const auto& it : m_followedItems) {
                if (it->pid() == item_pid) {
                    alreadyFollowed = true;
                }
            }
            m_searchResults.append(new ProcessItem(name, item_pid, alreadyFollowed, this));
        }
    }
    emit searchResultsChanged();
}

void Backend::updateSearchResults()
{
    updateSearchResults(m_lastSearch);
}

void Backend::addFollowedItem(int pid)
{
    if (m_followedItems.count() >= MAX_FOLLOWED_PROCESS_COUNT) {
        qDebug() << "[error] reached max followed process count, remove some before"
                   << "adding more";
        return;
    }
    // Checking if not already followed
    {
        auto it = std::find_if(m_followedItems.begin(), m_followedItems.end(),
                     [pid](const ProcessItem* it){
            return it->pid() == pid;
        });
        if (it != m_followedItems.end()){
            return;
        }
    }
    {
        auto it = std::find_if(m_searchResults.begin(), m_searchResults.end(),
                     [pid](const ProcessItem* it){
            return it->pid() == pid;
        });
        if (it != m_searchResults.end()){
            (*it)->setFollowed(true);
            auto followedProcess = new FollowedProcessItem(*(*it));
            QObject::connect(
                        followedProcess, &FollowedProcessItem::processDied,
                        this, &Backend::onProcessDied);
            m_followedItems.append(followedProcess);
            emit searchResultsChanged();
            emit followedItemsChanged();
        }
    }
}

void Backend::removeFollowedItem(int pid)
{
    {
        auto item = std::find_if(m_followedItems.begin(), m_followedItems.end(),
                     [pid](const ProcessItem* it){
            return it->pid() == pid;
        });
        if (item != m_followedItems.end()) {
            QObject::disconnect(
                        *item, &FollowedProcessItem::processDied,
                        this, &Backend::onProcessDied);
            m_followedItems.remove(*item);
            emit followedItemsChanged();
        }
    }
    {
        auto item = std::find_if(m_searchResults.begin(), m_searchResults.end(),
                     [pid](const ProcessItem* it){
            return it->pid() == pid;
        });
        if (item != m_searchResults.end()) {
            updateSearchResults();
        }
    }
}

void Backend::onProcessDied(int pid)
{
    {
        auto item = std::find_if(m_followedItems.begin(), m_followedItems.end(),
                     [pid](const ProcessItem* it){
            return it->pid() == pid;
        });
        if (item != m_followedItems.end()) {
            (*item)->setRunning(false);
            qDebug() << "Sending processDied";
            communicationManager->sendProcessDied((*item)->name());
        }
    }
}
void Backend::sendProcessus() {
    qDebug() << "Sending processus";
    QList<QString> processus;
    processus.reserve(m_followedItems.size());
    std::transform(m_followedItems.begin(), m_followedItems.end(),
            std::back_inserter(processus),
                   [](const ProcessItem* it) -> QString { return it->name(); });
    communicationManager->updateProcessus(QStringList(processus));
}
void Backend::pingDevices() {
    qDebug() << "Pinging devices";
    communicationManager->sendPing();
}
