#include "api.h"

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
    Statistics res;
    for (auto i=files.end()-13;i!=files.end();i++) {
        if (i->contains(".log") && i->contains("Journal.")) {
            OnNewEvent(*i);
            res=Data->getUnifiedStatistics();
            if (res.totalNumberOfMissions-res.completedMissions==0) {
                i-=2;
            }
        } else {
            if (res.totalNumberOfMissions-res.completedMissions==0) {
                i-=2;
            }
        }
    }
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

void API::refreshdata(int depth,json *SavedData, techlevi::Input * input) {
    if (SavedData==nullptr)
        SavedData=&config;
    if (input==nullptr)
        input=new techlevi::Input();
    QString key;
    QString alma;
    json ChildObject;
    for (auto i=SavedData->begin();i!=SavedData->end();i++) {
        key=QString::fromStdString((string)i.key());
        if ((string)i.key()=="Total kills so far") {
            continue;
        }
        if (depth==0) {
            addTreeItem();
            ui->treeWidget_3->currentItem()->setText(0,key);
            input->TSystem=key;
        } else {
            addTreeItem();
            ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->child(ui->treeWidget_3->currentItem()->childCount()-1));
            ui->treeWidget_3->currentItem()->setText(0,key);
            if (depth==2) {
                input->SStation=key;
            } else if (depth==1) {
                input->SSystem=key;
            }
        }
        if (depth!=3) {
            ChildObject=i.value();
            refreshdata(depth+1,&ChildObject,input);
        } else {
            ChildObject=i.value();
            if (((string)i.key()).find("] ")!=string::npos) {
                input->SFaction=QString::fromStdString((string)i.key()).section("] ",1,1);
            } else {
                input->SFaction=QString::fromStdString((string)i.key());
            }
            for (auto k=ChildObject.begin();k!=ChildObject.end();k++) {
                QString miss=QString::fromStdString((string)k.value());
                if (miss.section(";",3,3)=="0") {
                    input->completed=false;

                } else {
                    input->completed=true;
                    ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
                }
                input->MID=QString::fromStdString((string)k.key()).toUInt();
                input->kills=miss.section(";",0,0).toInt();
                input->killsSoFar=miss.section(";",1,1).toInt();
                input->reward=miss.section(";",2,2).toDouble();
                input->TFaction=miss.section(";",4,4);
                Data->parseData(*input);
            }
        }
        ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->parent());
    }
    ui->treeWidget_3->expandAll();
}
