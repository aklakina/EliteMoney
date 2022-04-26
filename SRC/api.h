#ifndef API_H
#define API_H

#include "containerobject.h"

#include "json.hpp"

#include <QObject>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <set>
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QTime>

using json=nlohmann::json;
using namespace std;
using namespace techlevi;

namespace techlevi {
class API : public QObject
{
    Q_OBJECT
public:
    explicit API(QObject *parent = nullptr);

    QStringList originalContent;

private:

    bool GettingMissions=false;

    json *DockedStation=nullptr;

    set<QString> events;

    QTime LastEventDate;



    void GetMissions();

    void OnEvent(json event);

public:

    void HandleEvent(json event);

    void SaveData(QString file);

    void LoadData(QString filePath);

public slots:

    void OnNewEvent(const QString &file);

    void OnNewFile(const QString &file);


signals:

    void missionCompleted(unsigned ID, bool remove=false);

    void Signal_Event(json processed);

    void addMission(string dest,int kills, double reward, unsigned ID, bool wing,QString Sfaction, QString Tfaction, QDateTime AcceptanceTime, QDateTime Expiry);

    void requestStatistics(Statistics* input);

    void requestJson(string * input,QTreeWidgetItem* item=nullptr);

    void provideJson(json & input);

    void MissionRedirection(unsigned ID,QString newStation);

    void Docked(QString System,QString Station);

    void TargetsFaction(QString Faction);

    void BountyCollected(QString Faction,unsigned Reward);

    void ShipChanged(QString newShip);

    void AddFileToFSWatcher(QString path);

    void unDocked();
};
}
#endif // API_H
