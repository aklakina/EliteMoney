#include "data.h"

using namespace techlevi;

data::data(QObject* parent):QObject(parent) {
    globalFactions=new GlobalFactions("");
    CompleteData=new HuntedSystems();

}

void data::Init(Ui::MainWindow *Mui) {
    this->ui=Mui;
}

void data::missionCompleted(unsigned ID,bool remove) {
    double kill_diff=0;
    double rew_diff=0;
    auto res=globalFactions->findMission(ID);
    auto mission=(*res.missionPlace);
    auto faction=(*res.factionPlace);
    if (!remove) {
        mission->Completed=true;
        kill_diff=mission->overallKillsNeeded;
        rew_diff=mission->payout;
        faction->totalKillsNeeded-=kill_diff;
        globalFactions->totalKills-=kill_diff;
        faction->totalReward-=rew_diff;
        globalFactions->totalMissionCount-=1;
        auto oldStackHeight=globalFactions->stackHeight;
        globalFactions->totalKillsSoFar-=oldStackHeight-globalFactions->reCalcStackHeight();
        emit RefreshTable(*globalFactions);
        emit UpdateTree((AdvancedContainer<ContainerObject>*) CompleteData);
        ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
    } else {
        RemoveMission(&ID);
        emit RefreshTable(*globalFactions);
        ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()-1));
    }
}

void data::BountyCollected(QString Faction, unsigned Reward)
{
    auto temp=new TargetedFaction(Faction);
    for (auto session:*CompleteData) {
        auto res=session->TargetedFactions.find(temp);
        if (res!=session->TargetedFactions.end()) {
            for (auto system:*session) {
                for (auto station:*system) {
                    for (auto faction:*station) {
                        for (auto mission:faction->getMissionsbyDate()) {
                            if (!mission->Completed) {
                                mission->killsSoFar++;
                                if (mission->killsSoFar==mission->overallKillsNeeded) {
                                    missionCompleted(mission->MID);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    delete temp;
}

void data::calculateTheoreticalCompletion(int theoreticalKills, TheoreticalResults & _ret)
{
    for (auto faction:*globalFactions) {
        int TheorKilled=theoreticalKills;
        for (auto mission:(faction->getMissionsbyDate())) {
            if (TheorKilled>=mission->overallKillsNeeded) {
                TheorKilled=TheorKilled-mission->overallKillsNeeded;
                _ret.TheoreticallyCompletedMissions++;
                _ret.TheoreticallyGainedPayout+=mission->payout;
            }
            _ret.allMissions++;
            _ret.totalPossiblePayout+=mission->payout;
        }
    }
}

void data::Docked(QString System, QString Station)
{
    CurrentStation=new currentStation(Station);
    CurrentStation->systemName=System;
    auto missions=getStationMissionData();
    missions.shrink_to_fit();
    if (missions.size()>0) {
        emit BuildStationMissionData(missions);
    }
}

void data::unDocked()
{
    if (CurrentStation!=nullptr) {
        delete CurrentStation;
        CurrentStation=nullptr;
    }
    auto missions=getCompletedMissionData();
    missions.shrink_to_fit();
    if (missions.size()>0) {
        emit BuildCompletedMissionData(missions);
    }
}

void data::JumpedToSystem(QString system)
{
    bool found=false;
    for (auto TSystem:*CompleteData) {
        if (TSystem->name==system) {
            Session=TSystem;
            emit RefreshTable(*globalFactions);
            found=true;
            break;
        }
        for (auto SSystem:*TSystem) {
            if (SSystem->name==system) {
                found=true;
                Session=TSystem;
                break;
            }
        }
        if (found) break;
    }
    if (!found) {
        Session=nullptr;
    }
}

void data::GetSession(huntedSystem *&input)
{
    if (Session!=nullptr) {
        input=Session;
    } else {
        map<huntedSystem*,Statistics>* res1=nullptr;
        auto res=getStatistics(res1);
        unsigned prevStack=0;
        for (auto TSystem:res) {
            if (prevStack<TSystem.second.StackHeight) {
                prevStack=TSystem.second.StackHeight;
                input=TSystem.first;
            }
        }
        delete res1;
    }
}

void data::RemoveMission(unsigned *ID) {
    auto res=globalFactions->findMission(*ID);
    ui->label_8->setText(QString::number(ui->label_8->text().toDouble()-(*res.missionPlace)->payout));
    (*res.factionPlace)->removeMission(*res.missionPlace);
    if (!(*res.factionPlace)->hasMissions()) {
        globalFactions->remove(res.factionPlace);
    }
}

void data::addMission(string dest,int kills, double reward, unsigned ID, bool wing,QString Sfaction, QString Tfaction, QDateTime AcceptanceTime, QDateTime Expiry) {
    Input input;
    input.MID=ID;
    input.SFaction=Sfaction;
    input.SStation=CurrentStation->name;
    input.SSystem=(CurrentStation->systemName);
    input.kills=kills;
    input.reward=reward;
    input.TSystem=QString::fromStdString(dest);
    input.TFaction=Tfaction;
    input.AcceptanceTime=AcceptanceTime;
    input.Expiry=Expiry;
    input.Winged=wing;
    auto res=parseData(input);
    emit RefreshTable(*globalFactions);
    globalFactions->RefreshStatistics();
    emit UpdateTree((AdvancedContainer<ContainerObject>*)CompleteData);
    emit RefreshUI(false);
}

void data::MissionRedirection(unsigned ID, QString newDest)
{
    auto res=globalFactions->findMission(ID);
    if ((*res.missionPlace)->sourceStation->name==newDest) {
        missionCompleted(ID);
    }
}

Result data::parseData(Input input,Result *Prev, bool SingleInsert)
{
    Result parsedData;
#pragma check if TargetSystem exists{
    if (CompleteData==nullptr) {
        CompleteData=new HuntedSystems();
    }
    bool TSystemNotSame=false;
    bool temp=false;
    if (Prev!=nullptr) {
        temp=input.TSystem!=Prev->TSystem->name;
    }
    if (temp || SingleInsert) {
        TSystemNotSame=temp;
        huntedSystem* Huntedtemp=new huntedSystem(input.TSystem);
        auto res=CompleteData->add(Huntedtemp);
        if (!res.second) {
            delete Huntedtemp;
            Huntedtemp=*res.first;
        }
        parsedData.TSystem=Huntedtemp;
    } else {
        parsedData.TSystem=Prev->TSystem;
    }
#pragma}
#pragma Check if Targeted faction exists{
    temp=Prev!=nullptr ? input.TFaction!=Prev->TFaction->name || TSystemNotSame : false;
    if (SingleInsert || temp) {
        TargetedFaction * TFactionTemp=new TargetedFaction(input.TFaction);
        auto res=(parsedData.TSystem)->TargetedFactions.insert(TFactionTemp);
        if (!res.second) {
            delete TFactionTemp;
            TFactionTemp=*res.first;
        }
        parsedData.TFaction=TFactionTemp;
    } else {
        parsedData.TFaction=Prev->TFaction;
    }
#pragma}
#pragma check if source system exists and create if not {
    bool SSystemNotSame=false;
    temp=Prev!=nullptr ? input.SSystem!=Prev->SSystem->name || TSystemNotSame : false;
    if (SingleInsert || temp) {
        SSystemNotSame=temp;
        System *SSysTemp=new System(input.SSystem);
        auto SSysRes=(parsedData.TSystem)->add(SSysTemp);
        if (!SSysRes.second) {
            delete SSysTemp;
            SSysTemp=*SSysRes.first;
        }
        parsedData.SSystem=SSysTemp;
    } else {
        parsedData.SSystem=Prev->SSystem;
    }
#pragma}
#pragma check if source station exists and create if not {
    bool StationNotSame=false;
    temp=Prev!=nullptr ? input.SStation!=Prev->SStation->name || SSystemNotSame : false;
    if (temp || SingleInsert) {
        StationNotSame=temp;
        station* SStatTemp=new station(input.SStation);
        auto SStatRes=(parsedData.SSystem)->add(SStatTemp);
        if (!SStatRes.second) {
            delete SStatTemp;
            SStatTemp=*SStatRes.first;
        }
        parsedData.SStation=SStatTemp;
    } else {
        parsedData.SStation=Prev->SStation;
    }
#pragma}
#pragma check if source faction exists and is in snycron with the globalFactions container and fix if not {
    temp=Prev!=nullptr ? input.SFaction!=Prev->SFaction->name || StationNotSame : false;
    if (  temp || SingleInsert ) {
        faction* SFactionTemp=new faction(input.SFaction);
        if (globalFactions==nullptr) globalFactions=new GlobalFactions("main");
        auto SFactionGlobRes=globalFactions->add(SFactionTemp);
        if (!SFactionGlobRes.second) {
            delete SFactionTemp;
            SFactionTemp=*SFactionGlobRes.first;
        }
        auto SFactionStatRes=(parsedData.SStation)->add(SFactionTemp);
        if (SFactionGlobRes.second && !SFactionStatRes.second) {
            SFactionTemp=globalFactions->pop(SFactionTemp).first;
            delete SFactionTemp;
            SFactionTemp=*SFactionStatRes.first;
            SFactionGlobRes.first=SFactionStatRes.first;
            globalFactions->add(SFactionTemp);
        }
        if (SFactionStatRes.second) {
            auto fact=(*SFactionGlobRes.first);
            sysStat *temp2=new sysStat({parsedData.SSystem,parsedData.SStation});
            fact->InsertHome(temp2);
        }
        parsedData.SFaction=SFactionTemp;
    } else {
        parsedData.SFaction=Prev->SFaction;
    }
#pragma}
#pragma check if mission already exists and create if not {
    auto Mission=new class mission(
                input.MID
                ,parsedData.TFaction
                ,parsedData.TSystem
                ,parsedData.SSystem
                ,parsedData.SFaction
                ,parsedData.SStation
                ,input.kills
                ,input.reward
                ,input.AcceptanceTime
                ,input.Expiry
                ,input.Winged
                ,input.completed
                ,input.killsSoFar
                );
    auto MissionRes=(parsedData.SFaction)->add(Mission);
    if (!MissionRes.second) {
        delete Mission;
        Mission=MissionRes.first;
    }
    parsedData.Mission=Mission;
    return parsedData;
#pragma}
}

map<huntedSystem*,Statistics> data::getStatistics(map<huntedSystem*,Statistics> *&res) {
    map<huntedSystem*,Statistics> *output=new map<huntedSystem*,Statistics>();
    for (auto hunted:*CompleteData) {
        output->insert({hunted,(hunted)->getStats(*globalFactions)});
    }
    res=output;
    return *output;
}

void data::getUnifiedStatistics(Statistics* input)
{
    map<huntedSystem*,Statistics>* res1=nullptr;
    auto res=this->getStatistics(res1);
    unsigned stackHeight=0;
    for (auto hunted:res) {
        (*input)+=hunted.second;
        if (stackHeight<hunted.second.StackHeight) {
            input->StackHeight=hunted.second.StackHeight;
        }
        input->totalKillsNeeded+=hunted.second.totalKillsNeeded;
    }
    for (auto fact:*globalFactions) {
        if (fact->getMissionsbyID().size()>0) {
            input->StackWidth++;
        }
    }
    delete res1;
}

void data::getJsonFormattedData(string* input) {
    if (CompleteData==nullptr) {
        return;
    }
    json Data;
    RecursiveDataReader((AdvancedContainer<ContainerObject>*)CompleteData,Data);
    if (CurrentStation!=nullptr) {
        Data["Current Station"]["System"]=CurrentStation->systemName.toStdString();
        Data["Current Station"]["Station"]=CurrentStation->name.toStdString();
    }
    *input=Data.dump(4);
}

void data::LoadDataFromJson(json &input)
{
    auto temp=new json(input);
    refreshdata(0,temp);
    delete temp;
    emit RefreshTable(*globalFactions);
    globalFactions->reCalcStackHeight();
    globalFactions->reCalcTotalKills();
    globalFactions->reCalcTotalPayout();
    emit UpdateTree((AdvancedContainer<ContainerObject>*)CompleteData);
    emit RefreshUI(false);
}

void data::RecursiveDataReader(AdvancedContainer<ContainerObject>* item, json &a, unsigned depth) {
    for (auto childItem:*item) {
        a[childItem->name.toStdString()]=json::object();
        if (depth!=3) {
            RecursiveDataReader((AdvancedContainer<ContainerObject>*)childItem,a[childItem->name.toStdString()],depth+1);
        } else {
            json & fact=a[childItem->name.toStdString()];
            for (auto m:((faction*)childItem)->getMissionsbyID()) {
                fact[QString::number(m->MID).toStdString()]=json::object();
                json & mission=fact[QString::number(m->MID).toStdString()];
                mission["kills"]=m->overallKillsNeeded;
                mission["KillsSoFar"]=m->killsSoFar;
                mission["Completed"]=m->Completed;
                mission["Shareble"]=m->Winged;
                mission["Time of acquiring"]=m->AcceptanceTime.toString("yyyy'-'MM'-'dd' 'hh':'mm':'ss").toStdString();
                mission["Time of expiry"]=m->Expiry.toString("yyyy'-'MM'-'dd' 'hh':'mm':'ss").toStdString();
                mission["Reward"]=m->payout;
                mission["Target Faction"]=m->targetFaction->name.toStdString();
            }
        }
    }
}

void data::refreshdata(int depth,json *SavedData, techlevi::Input * input, techlevi::Result * Prev) {
    if (SavedData==nullptr) {
        try {
            string* temp=new string();
            getJsonFormattedData(temp);
            auto res=new json(*temp);
            delete temp;
            SavedData=res;
        }  catch (exception & e) {
            qDebug()<<e.what();
        }
    }
    if (input==nullptr)
        input=new techlevi::Input();
    QString key;
    json ChildObject;
    for (auto i=SavedData->begin();i!=SavedData->end();i++) {
        key=QString::fromStdString((string)i.key());
        if (depth==0) {
            bool found=false;
            if (i.key()=="Current Station") {
                CurrentStation=new currentStation();
                CurrentStation->name=QString::fromStdString(SavedData->at("Current Station")["Station"]);
                CurrentStation->systemName=QString::fromStdString((SavedData->at("Current Station")["System"]));
                continue;
            }
            for (auto itemIndex=0;itemIndex<ui->treeWidget_3->topLevelItemCount();itemIndex++) {
                if (ui->treeWidget_3->topLevelItem(itemIndex)->text(0)==key) {
                    found=true;
                    break;
                }
            }
            if (!found) {
                auto temp=new QTreeWidgetItem();
                ui->treeWidget_3->addTopLevelItem(temp);
                temp->setText(0,key);
                ui->treeWidget_3->setCurrentItem(temp);
            }
            input->TSystem=key;
        } else {
            emit addTreeItem(key);
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
                json miss=k.value();
                if (QDateTime::fromString(QString::fromStdString(miss["Time of expiry"]),"yyyy'-'MM'-'dd' 'hh':'mm':'ss")<QDateTime::currentDateTime()) {
                    continue;
                }
                if (miss["Completed"]) {
                    input->completed=true;
                    ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
                } else {
                    input->completed=false;
                }
                input->MID=QString::fromStdString((string)k.key()).toUInt();
                input->kills=miss["kills"];
                input->killsSoFar=miss["KillsSoFar"];
                input->reward=miss["Reward"];
                input->TFaction=QString::fromStdString(miss["Target Faction"]);
                input->AcceptanceTime=QDateTime::fromString(QString::fromStdString(miss["Time of acquiring"]),"yyyy'-'MM'-'dd' 'hh':'mm':'ss");
                input->Expiry=QDateTime::fromString(QString::fromStdString(miss["Time of expiry"]),"yyyy'-'MM'-'dd' 'hh':'mm':'ss");
                input->Winged=miss["Shareble"];
                if (Prev==nullptr) {
                    Prev=new techlevi::Result();
                    *Prev=parseData(*input);
                } else {
                    *Prev=parseData(*input,Prev,true);
                }
            }
        }
        ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->parent());
    }
    ui->treeWidget_3->expandAll();
    if (depth==0) {
        delete input;
        if (Prev!=nullptr) {
            delete Prev;
        }
    }
}

vector<mission*> data::getStationMissionData()
{
    for (auto TSystem:*CompleteData) {
        for (auto SSystem:*TSystem) {
            if (SSystem->name==CurrentStation->systemName) {
                for (auto SStation:*SSystem) {
                    if (SStation->name==CurrentStation->name) {
                        vector<mission*> _ret;
                        for (auto faction:*SStation) {
                            for (auto mission:faction->getMissionsbyDate()) {
                                if (mission->Completed) _ret.push_back(mission);
                            }
                        }
                        return _ret;
                    }
                }
            }
        }
    }
    vector<mission*> _ret;
    return _ret;
}

vector<mission *> data::getCompletedMissionData()
{
    vector<mission*> _ret;
    for (auto faction:*globalFactions) {
        for (auto mission:faction->getMissionsbyDate()) {
            if (mission->Completed) _ret.push_back(mission);
        }
    }
    return _ret;
}
