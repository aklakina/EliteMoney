#ifndef API_H
#define API_H

#include <data.h>
#include <QObject>
#include <QFileSystemWatcher>
#include "json.hpp"
#include "QDir"
#include <QStandardPaths>
#include <QFile>
#include <set>

using json=nlohmann::json;
using namespace std;

class API : public QObject
{
    Q_OBJECT
public:
    explicit API(QObject *parent = nullptr);

private slots:

    void OnNewEvent(const QString &file);

    void OnNewFile(const QString &file);

private:

    techlevi::data * Data;

    bool GettingMissions=false;

    set<QString> events;

    QFileSystemWatcher *notifier;

    QStringList originalContent;

    void GetMissions();

    void OnEvent(json event);

    void refreshdata(int depth=0, json *SavedData=nullptr, techlevi::Input * input=nullptr);
public:

    void setData(techlevi::data* input) {Data=input;}

    void HandleEvent(json event);

signals:

    void missionCompleted(unsigned ID);

    void Signal_Event(json processed);

    void addMission(string dest,int kills, double reward, unsigned ID,QString Sfaction, QString Tfaction);
};

#endif // API_H
