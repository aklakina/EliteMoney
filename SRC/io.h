#ifndef API_H
#define API_H

#include "containerobject.h"


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
class IO: public QObject
{
    Q_OBJECT
public:
    explicit IO(QObject *parent = nullptr);

    QStringList originalContent;

private:

    bool GettingMissions=false;

    json *DockedStation=nullptr;

    set<QString> events;

    QTime LastEventDate;

    void LoadUIConfig(QString path);

    void GetMissions();

    void OnEvent(json event);

public:

    void HandleEvent(json event);

    void init();

public slots:

    void OnNewEvent(const QString &file);

    void OnNewFile(const QString &file);

    void SaveData(QString file);

    void LoadData(QString filePath);

    void SaveOverlayConfig(std::string conf);

    void SendUIConfig(std::string * conf);

signals:

    void missionCompleted(unsigned ID, bool remove=false);

    void Signal_Event(std::string processed);

    void addMission(string dest,int kills, double reward, unsigned ID, bool wing,QString Sfaction, QString Tfaction, QDateTime AcceptanceTime, QDateTime Expiry);

    void requestStatistics(techlevi::Statistics* input);

    void requestJson(std::string * input);

    void provideJson(std::string & input);

    void MissionRedirection(unsigned ID,QString newStation);

    void Docked(QString System,QString Station);

    void TargetsFaction(QString Faction);

    void BountyCollected(QString Faction,unsigned Reward);

    void ShipChanged(QString newShip);

    void AddFileToFSWatcher(QString path);

    void unDocked();

    void JumpedToSystem(QString name);

};
}
#endif // API_H
