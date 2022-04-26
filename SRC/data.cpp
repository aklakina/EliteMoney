#include "data.h"

using namespace techlevi;

data::data(QObject* parent):QObject(parent) {
    globalFactions=new GlobalFactions("");
    CompleteData=new HuntedSystems();

}

void data::Init(Ui::MainWindow *Mui, API *parent_api) {
    this->api=parent_api;
    this->ui=Mui;
}


void data::KillsperFaction() {
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(globalFactions->getSize());
    int curr_row=0;
    int curr_col=0;
    vector<int> frags;
    for (auto factions:*globalFactions) {
        if (ui->tableWidget->columnCount()<(int)factions->getMissionsbyID().size()+2) {
            ui->tableWidget->setColumnCount(factions->getMissionsbyID().size()+2);
        }
    }
    for (unsigned k=0;k<globalFactions->getSize();k++) {
        for (unsigned j=ui->tableWidget->columnCount();j>0;j--) {
            ui->tableWidget->setItem(k,j,new QTableWidgetItem(""));
        }
    }
    for (auto faction:*globalFactions) {
        int frags_needed=0;
        curr_col=0;
        for (auto Mission:(faction->getMissionsbyDate())) {
            if (!Mission->Completed) {
                int kills_needed=0;
                if (curr_col>0) {
                    kills_needed=Mission->overallKillsNeeded;
                } else {
                    kills_needed=Mission->overallKillsNeeded-Mission->killsSoFar;
                }
                ui->tableWidget->item(curr_row,curr_col+1)->setText(QString::number(kills_needed));
                frags_needed+=Mission->overallKillsNeeded-Mission->killsSoFar;
                curr_col++;
            }
        }
        frags.push_back(frags_needed);
        auto temp=new QTableWidgetItem();
        temp->setText(faction->name);
        ui->tableWidget->setItem(curr_row,0,temp);
        curr_row++;
    }
    curr_row=0;
    for (auto i:frags) {
        ui->tableWidget->item(curr_row,ui->tableWidget->columnCount()-1)->setText(QString::number(i));
        curr_row++;
    }
    QStringList labels;
    labels.push_back("Factions");
    for (auto i=0;i<ui->tableWidget->columnCount()-2;i++) {
        labels.push_back("Mission Layer "+QString::number(i+1));
    }
    labels.push_back("Total kills needed");
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    ui->tableWidget->resizeColumnsToContents();
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
        emit Refresh(*globalFactions,*CompleteData);
        KillsperFaction();
        emit UpdateTree((AdvancedContainer<ContainerObject>*) CompleteData);
        ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
    } else {
        ui->treeWidget->clear();
        RemoveMission(&ID);
        emit Refresh(*globalFactions,*CompleteData,true,mission);
        KillsperFaction();
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
}

void data::unDocked()
{
    if (CurrentStation!=nullptr) {
        delete CurrentStation;
        CurrentStation=nullptr;
    }
}

void data::RemoveMission(unsigned *ID) {
    auto res=globalFactions->findMission(*ID);
    (*res.factionPlace)->removeMission(*res.missionPlace);
    if (!(*res.factionPlace)->hasMissions()) {
        globalFactions->remove(res.factionPlace);
    }
}

void data::addMission(string dest,int kills, double reward, unsigned ID, bool wing,QString Sfaction, QString Tfaction, QDateTime AcceptanceTime, QDateTime Expiry) {
    ui->treeWidget->clear();
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
    parseData(input);
    KillsperFaction();
    globalFactions->RefreshStatistics();
    emit UpdateTree((AdvancedContainer<ContainerObject>*)CompleteData);
    emit RefreshUI(false,*globalFactions);
}

void data::MissionRedirection(unsigned ID, QString newDest)
{
    auto res=globalFactions->findMission(ID);
    if ((*res.missionPlace)->sourceStation->name==newDest) {
        missionCompleted(ID);
    }
}

Result data::parseData(Input input,Result *Prev)
{
    Result parsedData;
#pragma check if TargetSystem exists{
    if (CompleteData==nullptr) {
        CompleteData=new HuntedSystems();
    }
    bool TSystemNotSame=false;
    if (Prev==nullptr || input.TSystem!=Prev->TSystem->name) {
        TSystemNotSame=true;
        huntedSystem* Huntedtemp=new huntedSystem(input.TSystem);
        CompleteData->add(Huntedtemp);
        parsedData.TSystem=Huntedtemp;
    } else {
        parsedData.TSystem=Prev->TSystem;
    }
#pragma}
#pragma Check if Targeted faction exists{
    if (Prev==nullptr || input.TFaction!=Prev->TFaction->name || TSystemNotSame) {
        TargetedFaction * TFactionTemp=new TargetedFaction(input.TFaction);
        (parsedData.TSystem)->TargetedFactions.insert(TFactionTemp);
        parsedData.TFaction=TFactionTemp;
    } else {
        parsedData.TFaction=Prev->TFaction;
    }
#pragma}
    bool SSystemNotSame=false;
#pragma check if source system exists and create if not {
    if (Prev==nullptr || input.SSystem!=Prev->SSystem->name || TSystemNotSame) {
        SSystemNotSame=true;
        System *SSysTemp=new System(input.SSystem);
        auto SSysRes=(parsedData.TSystem)->add(SSysTemp);
        parsedData.SSystem=SSysTemp;
    } else {
        parsedData.SSystem=Prev->SSystem;
    }
#pragma}
    bool StationNotSame=false;
#pragma check if source station exists and create if not {
    if (Prev==nullptr || input.SStation!=Prev->SStation->name || SSystemNotSame) {
        StationNotSame=true;
        station* SStatTemp=new station(input.SStation);
        auto SStatRes=(parsedData.SSystem)->add(SStatTemp);
        parsedData.SStation=SStatTemp;
    } else {
        parsedData.SStation=Prev->SStation;
    }
#pragma}
#pragma check if source faction exists and is in snycron with the globalFactions container and fix if not {
    if ( (Prev==nullptr || input.SFaction!=Prev->SFaction->name || StationNotSame) ) {
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

map<huntedSystem*,Statistics> data::getStatistics() {
    map<huntedSystem*,Statistics> output;
    for (auto hunted:*CompleteData) {
        output.insert({hunted,(hunted)->getStats(*globalFactions)});
    }
    return output;
}

void data::getUnifiedStatistics(Statistics* input)
{
    auto res=this->getStatistics();
    for (auto hunted:res) {
        (*input)+=hunted.second;
    }
}

void data::getJsonFormattedData(string* input,QTreeWidgetItem* item) {
    *input="{ \""+item->text(0).toStdString()+"\":";
    RecursiveDataReader(item,*input);
    QString b=QString::fromStdString(*input);
    b.remove("\\");
    b.remove(0,0);
    b.remove(b.length(),0);
    //b+=",\"Total kills so far\":{\""+QString::fromStdString(MissionTarget)+"\":\""+QString::number(total_kills_so_far)+"\"}"
    b+="}";
    b.remove("\\");
    *input=b.toStdString();
}

void data::LoadDataFromJson(json &input)
{
    auto temp=new json(input);
    refreshdata(0,temp);
    delete temp;
    KillsperFaction();
    globalFactions->reCalcStackHeight();
    globalFactions->reCalcTotalKills();
    globalFactions->reCalcTotalPayout();
    emit UpdateTree((AdvancedContainer<ContainerObject>*)CompleteData);
    emit Refresh(*globalFactions,*CompleteData);
    emit RefreshUI(false,*globalFactions);
}

void data::RecursiveDataReader(QTreeWidgetItem *item, string &a) {
    auto GetFactionName=[](QTreeWidgetItem* a)->QString {
        if (a->text(0).contains("] ")) {
            return a->text(0).section("] ",1,1);
        } else {
            return a->text(0);
        }
    };
    if (item->childCount()>0) {
        a+="{";
    } else {
        a+="\"0\"";
    }
    for (int i=0; i<item->childCount();i++) {
        if (item->child(i)->text(0).contains("] ") && item->child(i)->text(0).contains("[")) {
            a+="\""+item->child(i)->text(0).section("] ",1,1).toStdString()+"\":";
        } else {
            a+="\""+item->child(i)->text(0).toStdString()+"\":";
        }
        if (item->child(i)->childCount()!=0) {
            RecursiveDataReader(item->child(i),a);
            if (i!=item->childCount()-1)
                a+=",";
        } else {
            a+="{";
            auto faction=globalFactions->find(GetFactionName(item->child(i)));
            if (faction==globalFactions->end()) {
                a+="\""+QString::number(i).toStdString()+"\":\""+
                        "0"
                        +";"+
                        "0;0\"";
            } else {
                set<mission*> CollectedMissions;
                for (auto mission:((*faction)->getMissionsbyID())) {
                    if ((mission->sourceStation)->name==item->text(0)) {
                        CollectedMissions.insert(mission);
                    }
                }
                for (auto mission:CollectedMissions) {
                    int tempor;
                    if (!mission->Completed) {
                        tempor=0;
                    } else {
                        tempor=1;
                    }
                    a+="\""+QString::number(mission->MID).toStdString()+"\":\""+
                            QString::number(mission->overallKillsNeeded).toStdString()
                            +";"+
                            QString::number(mission->killsSoFar).toStdString()
                            +";"+
                            QString::number(mission->payout).toStdString()
                            +";"+
                            QString::number(tempor).toStdString()
                            +";"+
                            mission->targetFaction->name.toStdString()
                            +"\"";
                    if (mission!=(*(--CollectedMissions.end()))) {
                        a+=",";
                    }
                }
            }
            a+="}";
            if (item->childCount()-1>i) {
                a+=",";
            }
        }
    }
    a+="}";
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
        if ((string)i.key()=="Total kills so far") {
            continue;
        }
        if (depth==0) {
            bool found=false;
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
                if (Prev==nullptr) {
                    Prev=new techlevi::Result();
                    *Prev=parseData(*input);
                } else {
                    *Prev=parseData(*input,Prev);
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
