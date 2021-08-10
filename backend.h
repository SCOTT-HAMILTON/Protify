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

public:
    explicit Backend(QObject *parent);
    ~Backend();
    QQmlObjectListModel<ProcessItem>* searchResults();
    QQmlObjectListModel<FollowedProcessItem>* followedItems();
    QString hostname() const;
    void setHostname(const QString& hostname);
    static constexpr int MAX_FOLLOWED_PROCESS_COUNT = 10;
    void startCommunications();

signals:
    void searchResultsChanged();
    void followedItemsChanged();
    void hostnameChanged(QString);

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
    QString m_lastSearch;
    CommunicationManager* communicationManager;
};

#endif // BACKEND_H
