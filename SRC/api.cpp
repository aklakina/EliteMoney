#include "api.h"
#include "data.h"

API::API(QObject * parent) : QObject(parent)
{
    QDir saves(QDir::currentPath()+"/System/");
    if (!saves.exists()) {
        QDir().mkdir(saves.path());
    }
    QDateTime a=saves.entryInfoList().begin()->lastModified();
    QString c=saves.entryInfoList().begin()->canonicalFilePath();
    for (auto const & i:saves.entryInfoList()) {
        if (a<i.lastModified()) {
            a=i.lastModified();
            c=i.canonicalFilePath();
        }
    }
    LoadData(c);
}

void API::OnNewFile(const QString &file) {
    QDir directory(file);
    QString full;
    bool found=false;
    for (auto & i:directory.entryList()) {
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
    emit AddFileToFSWatcher(full);
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
        qDebug()<<QString::fromStdString(event.dump());
        json test=json::parse("{\"DestinationStation\":\"Bolton Gateway\",\"DestinationSystem\":\"HIP 114040\",\"Expiry\":\"2022-05-01T12:28:09Z\",\"Faction\":\"Aristocrats of HIP 114614\",\"Influence\":\"++\",\"KillCount\":36,\"LocalisedName\":\"Massacre HIP 114040 Crimson Drug Empire faction Pirates\",\"MissionID\":862486764,\"Name\":\"Mission_MassacreWing\",\"Reputation\":\"++\",\"Reward\":4347401,\"TargetFaction\":\"HIP 114040 Crimson Drug Empire\",\"TargetType\":\"$MissionUtil_FactionTag_Pirate;\",\"TargetType_Localised\":\"Pirates\",\"Wing\":true,\"event\":\"MissionAccepted\",\"timestamp\":\"2022-04-24T12:29:48Z\"}");
        if (test==event) {
            qDebug()<<"STOP";
        }
        if ((string)event["event"]=="MissionAccepted" && ((string)event["Name"]).find("Massacre")!=string::npos) {
            /*{ "timestamp":"2021-03-10T14:58:54Z"
             *  , "event":"MissionAccepted"
             *  , "Faction":"People's MET 20 Liberals"
             *  , "Name":"Mission_MassacreWing"
             *  , "LocalisedName":"Massacre Qi Yomisii Council faction Pirates"
             *  , "TargetType":"$MissionUtil_FactionTag_Pirate;"
             *  , "TargetType_Localised":"Pirates"
             *  , "TargetFaction":"Qi Yomisii Council"
             *  , "KillCount":20
             *  , "DestinationSystem":"Qi Yomisii"
             *  , "DestinationStation":"Seddon Terminal"
             *  , "Expiry":"2021-03-17T14:26:09Z"
             *  , "Wing":true
             *  , "Influence":"++"
             *  , "Reputation":"++"
             *  , "Reward":2428060
             *  , "MissionID":726287843 }
             */
            emit addMission(
                    (string)event["DestinationSystem"]
                    ,(int)event["KillCount"]
                    ,(double)event["Reward"]/1000
                    ,(unsigned)event["MissionID"]
                    ,(bool)event["Wing"]
                    ,QString::fromStdString((string)event["Faction"])
                    ,QString::fromStdString((string)event["TargetFaction"])
                    ,QDateTime::fromString(QString::fromStdString((string)event["timestamp"]),"yyyy'-'MM'-'dd'T'hh:mm:ss'Z'")
                    ,QDateTime::fromString(QString::fromStdString((string)event["Expiry"]),"yyyy'-'MM'-'dd'T'hh:mm:ss'Z'")
                    );
        } else if ((string)event["event"]=="MissionRedirected" && ((string)event["Name"]).find("Massacre")!=string::npos) {
            /*{ "timestamp":"2021-03-14T18:10:43Z"
             *  , "event":"MissionRedirected"
             *  , "MissionID":726287843
             *  , "Name":"Mission_MassacreWing"
             *  , "NewDestinationStation":"Tesla Terminal"
             *  , "NewDestinationSystem":"Njulngan"
             *  , "OldDestinationStation":""
             *  , "OldDestinationSystem":"Qi Yomisii" }
             */
            emit MissionRedirection((unsigned)event["MissionID"],QString::fromStdString((string)event["NewDestinationStation"]));
        } else if ((string)event["event"]=="MissionCompleted") {
            //missionCompleted((unsigned)event["MissionID"],true);
            //Mission Completed and handed in
            emit missionCompleted((unsigned)event["MissionID"],true);
        } else if ((string)event["event"]=="Docked") {
            //Docked
            /*
             *{ "timestamp":"2021-03-14T18:56:42Z"
             *, "event":"Docked"
             *, "StationName":"XNB-55Z"
             *, "StationType":"FleetCarrier"
             *, "StarSystem":"Qi Yomisii"
             *, "SystemAddress":22953915599624
             *, "MarketID":3705556992
             *, "StationFaction":{ "Name":"FleetCarrier" }
             *, "StationGovernment":"$government_Carrier;"
             *, "StationGovernment_Localised":"Private Ownership "
             *, "StationServices":[ "dock", "autodock", "commodities", "contacts", "exploration", "outfitting", "crewlounge", "rearm", "refuel", "repair", "shipyard", "engineer", "flightcontroller", "stationoperations", "stationMenu", "carriermanagement", "carrierfuel", "voucherredemption" ]
             *, "StationEconomy":"$economy_Carrier;"
             *, "StationEconomy_Localised":"Private Enterprise"
             *, "StationEconomies":[ { "Name":"$economy_Carrier;", "Name_Localised":"Private Enterprise", "Proportion":1.000000 } ]
             *, "DistFromStarLS":610.551176 }
             */
            if (DockedStation==nullptr) DockedStation=new json(event);
            emit Docked(QString::fromStdString((string)event["StarSystem"]),QString::fromStdString((string)event["StationName"]));
        } else if ((string)event["event"]=="Undocked") {
            /*{ "timestamp":"2021-03-14T18:59:39Z"
             *  , "event":"Undocked"
             *  , "StationName":"XNB-55Z"
             *  , "StationType":"FleetCarrier"
             *  , "MarketID":3705556992 }
            */
            if (DockedStation!=nullptr) {
                delete DockedStation;
                DockedStation=nullptr;
            }
            emit unDocked();
        } else if ((string)event["event"]=="ShipTargeted" && (bool)event["TargetLocked"]) {
            if ((int)event["ScanStage"]==3) {
                emit TargetsFaction(QString::fromStdString((string)event["Faction"]));
            }
        } else if ((string)event["event"]=="Bounty") {
            //Bounty Collected
            emit BountyCollected(QString::fromStdString((string)event["VictimFaction"]),(unsigned)event["TotalReward"]);
        } else if ((string)event["event"]=="MissionAbandoned") {
            //Mission Cancelled
            emit missionCompleted((unsigned)event["MissionID"]);
            emit missionCompleted((unsigned)event["MissionID"],true);
        } else if ((string)event["event"]=="ShipyardSwap") {
            /*{ "timestamp":"2022-04-24T12:02:04Z"
             * , "event":"ShipyardSwap"
             * , "ShipType":"krait_mkii"
             * , "ShipType_Localised":"Krait Mk II"
             * , "ShipID":9
             * , "StoreOldShip":"Anaconda"
             * , "StoreShipID":12
             * , "MarketID":3221679616 }
             */
            emit ShipChanged(QString::fromStdString((string)event["ShipType"]));
        }
    } catch (const std::exception& e) {
        qDebug()<<QString::fromStdString(event.dump());
        qDebug()<<e.what();
    }
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

