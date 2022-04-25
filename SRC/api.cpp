#include "api.h"
#include "data.h"

API::API(QObject * parent) : QObject(parent)
{

    QStringList path1=QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    notifier=new QFileSystemWatcher(this);
    notifier->addPath(path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous");
    QDir base(path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous");
    QStringList basenames=base.entryList();
    QStringList baseabsolute;
    for (auto i:basenames) {
        QString b=path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous/"+i;
        baseabsolute.push_back(b);
    }
    originalContent=basenames;
    notifier->addPaths(baseabsolute);
    connect(notifier,SIGNAL(directoryChanged(QString)),this,SLOT(OnNewFile(QString)));
    connect(notifier,SIGNAL(fileChanged(const QString &)),this,SLOT(OnNewEvent(const QString &)));
}

void API::OnNewFile(const QString &file) {
    QDir directory(file);
    QString full;
    bool found=false;
    for (auto i:directory.entryList()) {
        if (originalContent.contains(i)) {
            continue;
        } else {
            if (i.contains(".log")) {
                full=file+"/"+i;
                found=true;
                break;
            }
        }
    }
    if (!found)
        return;
    originalContent=directory.entryList();
    qDebug()<<full;
    notifier->addPath(full);
}

void API::OnNewEvent(const QString &file) {
    //Sleep(3000);
    QFile changed(file);
    qDebug()<<"event started file opened";
    if (changed.open(QIODevice::ReadOnly)) {
        QTextStream stream(&changed);
        QString lastEvent="";
        json event;
        while (!stream.atEnd()) {
            lastEvent=stream.readLine();
            bool found=false;
            if (events.find(lastEvent)!=events.end()) {
                qDebug()<<"skipping since event already loaded";
                found=true;
            }
            if (!found) {
                try {
                    event=json::parse(lastEvent.toStdString());
                    events.insert(lastEvent);
                    OnEvent(event);
                }  catch (const std::exception& e) {
                    qDebug()<<lastEvent;
                    qDebug()<<e.what();
                }
            }
        }
    }
}


void API::GetMissions() {
    GettingMissions=true;
    QStringList path1=QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QDir base(path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous");
    auto files=base.entryList();
    Statistics *res=new Statistics();
    for (auto i=files.end()-13;i!=files.end();i++) {
        if (i->contains(".log") && i->contains("Journal.")) {
            OnNewEvent(*i);
            emit requestStatistics(res);
            if (res->totalNumberOfMissions-res->completedMissions==0) {
                i-=2;
            }
        } else {
            if (res->totalNumberOfMissions-res->completedMissions==0) {
                i-=2;
            }
        }
    }
    delete res;
}

void API::OnEvent(json event) {
    try {
        if ((string)event["event"]=="MissionAccepted" && ((string)event["Name"]).find("Massacre")!=string::npos) {
            //Mission Accepted
            /*MissionTargetFactions.insert({(string)event["TargetFaction"],0});
            if (MissionTarget=="Nothing_yet") {
                MissionTarget=(string)event["TargetFaction"];
            }
            temp_mission_target=QString::fromStdString((string)event["TargetFaction"]);*/
            emit addMission(
                    (string)event["DestinationSystem"]
                    ,(int)event["KillCount"]
                    ,(double)event["Reward"]/1000
                    ,(unsigned)event["MissionID"]
                    ,QString::fromStdString((string)event["Faction"])
                    ,QString::fromStdString((string)event["TargetFaction"])
                    );
        } else if ((string)event["event"]=="MissionRedirected" && ((string)event["Name"]).find("Massacre")!=string::npos) {
            //missionCompleted((unsigned)event["MissionID"]);
            //Mission Completed but not handed in yet

        } else if ((string)event["event"]=="MissionCompleted") {
            //missionCompleted((unsigned)event["MissionID"],true);
            //Mission Completed and handed in

        } else if ((string)event["event"]=="Docked") {
            //Docked

        } else if ((string)event["event"]=="Bounty") {
            //Bounty Collected

        } else if ((string)event["event"]=="MissionAbandoned") {
            //Mission Cancelled

        }
    } catch (const std::exception& e) {
        qDebug()<<QString::fromStdString(event.dump());
        qDebug()<<e.what();
    }
    //emit AutoSave();
}

void API::SaveData(QString file)
{
    QFile Qfile(file);
    if (Qfile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&Qfile);
        string * temp=new string();
        *temp="";
        emit requestJson(temp);
        QString output=QString::fromStdString(*temp);
        delete temp;
        stream<<output;
    } else {
        throw "Could not open file for write";
    }
}

void API::LoadData(QString filePath)
{
        QFile f(filePath);
        if (f.open(QIODevice::ReadOnly)) {
            QTextStream stream(&f);
            json config= json::parse(stream.readAll().toStdString());
            emit provideJson(config);
        }
}

