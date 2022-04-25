#ifndef API_H
#define API_H

#include "containerobject.h"
#include <QObject>
#include <QFileSystemWatcher>
#include "json.hpp"
#include "QDir"
#include <QStandardPaths>
#include <QFile>
#include <set>
#include <QFileDialog>
#include <QTreeWidgetItem>

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

    //techlevi::data * Data;

    bool GettingMissions=false;

    set<QString> events;

    QFileSystemWatcher *notifier;

    QStringList originalContent;

    void GetMissions();

    void OnEvent(json event);

public:

    void HandleEvent(json event);

    void SaveData(QString file);

    void LoadData(QString filePath);

public slots:



signals:

    void missionCompleted(unsigned ID);

    void Signal_Event(json processed);

    void addMission(string dest,int kills, double reward, unsigned ID,QString Sfaction, QString Tfaction);

    void requestStatistics(Statistics* input);

    void requestJson(string * input,QTreeWidgetItem* item=nullptr);

    void provideJson(json & input);
};

#endif // API_H
