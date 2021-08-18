#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "qt-qml-models/QQmlObjectListModel.h"

#include "processitem.h"
#include "followedprocessitem.h"
#include "communicationmanager.h"

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlObjectListModelBase* searchResults READ searchResults NOTIFY searchResultsChanged)
    Q_PROPERTY(QQmlObjectListModelBase* followedItems READ followedItems NOTIFY followedItemsChanged)
    Q_PROPERTY(QString hostname READ hostname WRITE setHostname NOTIFY hostnameChanged)
    Q_PROPERTY(QString token READ token WRITE setToken NOTIFY tokenChanged)
    Q_PROPERTY(QString gotifyAddress READ gotifyAddress WRITE setGotifyAddress NOTIFY gotifyAddressChanged)
    Q_PROPERTY(int gotifyPort READ gotifyPort WRITE setGotifyPort NOTIFY gotifyPortChanged)

public:
    explicit Backend(QObject *parent);
    ~Backend();
    QQmlObjectListModel<ProcessItem>* searchResults();
    QQmlObjectListModel<FollowedProcessItem>* followedItems();
    QString hostname() const;
    QString token() const;
    QString gotifyAddress() const;
    int gotifyPort() const;
    void setHostname(const QString& hostname);
    void setToken(const QString& token);
    void setGotifyAddress(const QString& address);
    void setGotifyPort(int port);
    static constexpr int MAX_FOLLOWED_PROCESS_COUNT = 10;
    void startCommunications();

signals:
    void searchResultsChanged();
    void followedItemsChanged();
    void hostnameChanged(QString);
    void tokenChanged(QString);
    void gotifyAddressChanged(QString);
    void gotifyPortChanged(int);

public slots:
    void updateSearchResults(const QString& search);
    void updateSearchResults();
    void addFollowedItem(int pid);
    void removeFollowedItem(int pid);
    void onProcessDied(int pid);
    void sendProcessus();

private:
    QQmlObjectListModel<ProcessItem> m_searchResults;
    QQmlObjectListModel<FollowedProcessItem> m_followedItems;
    QString m_hostname;
    QString m_token;
    QString m_gotifyAddress;
    int m_gotifyPort;
    QString m_lastSearch;
    CommunicationManager* communicationManager;
};

#endif // BACKEND_H
