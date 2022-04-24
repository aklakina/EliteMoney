#include "data.h"

using namespace techlevi;

data::data(QObject* parent):QObject(parent) {}

void data::Init(Ui::MainWindow *Mui, API *parent_api) {
    this->api=parent_api;
    this->ui=Mui;
}


void data::KillsperFaction() {
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(globalFactions->GetSize());
    int curr_row=0;
    int curr_col=0;
    vector<int> frags;
    for (auto factions:*globalFactions) {
        if (ui->tableWidget->columnCount()<(int)factions->missions.size()+2) {
            ui->tableWidget->setColumnCount(factions->missions.size()+2);
        }
        for (unsigned k=0;k<globalFactions->getSize();k++) {
            for (unsigned j=ui->tableWidget->columnCount();j>0;j--) {
                ui->tableWidget->setItem(k,j,new QTableWidgetItem(""));
            }
        }
    }
    for (auto faction:*globalFactions) {
        QTableWidgetItem* temp=new QTableWidgetItem(faction->name);
        int frags_needed=0;
        curr_col=0;
        for (auto Mission:faction->missions) {
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
        emit Refresh(*globalFactions,*CompleteData,*mission);
        KillsperFaction();
        emit UpdateTree(*globalFactions);
        ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
    } else {
        ui->treeWidget->clear();
        RemoveMission(&ID);
        emit Refresh(*globalFactions,*CompleteData,(*mission),true);
        KillsperFaction();
        ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()-1));
    }
}

void data::RemoveMission(unsigned *ID) {
    auto res=globalFactions->findMission(*ID);
    (*res.factionPlace)->missions.erase(res.missionPlace);
    if ((*res.factionPlace)->missions.empty()) {
        globalFactions->remove((*res.factionPlace));
    }
}

void data::addMission(string dest,int kills, double reward, unsigned ID,QString Sfaction, QString Tfaction) {
    if (ui->systemName->text()=="System") {
        emit addTreeItem();
        ui->treeWidget_3->topLevelItem(0)->setText(0,QString::fromStdString(dest));
        ui->systemName->setText(QString::fromStdString(dest));
    }
    ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->topLevelItem(0));
    //CheckCurrentStation(faction,nullptr,false,0,QString::fromStdString(dest));
    ui->treeWidget->clear();
    Input input;
    input.MID=ID;
    input.SFaction=Sfaction;
    input.SStation=CurrentStation->name;
    input.SSystem=(*CurrentStation->System)->name;
    input.kills=kills;
    input.reward=reward;
    input.TSystem=QString::fromStdString(dest);
    input.TFaction=Tfaction;
    parseData(input);
    KillsperFaction();
    emit UpdateTree(*globalFactions);
    emit RefreshUI(false,*globalFactions);
}

Result data::parseData(Input input)
{
    Result parsedData;
#pragma check if TargetSystem exists{
    if (CompleteData==nullptr) {
        CompleteData=new HuntedSystems();
    }
    huntedSystem* Huntedtemp=new huntedSystem(input.TSystem);
    auto HuntedRes=CompleteData->add(Huntedtemp);
    parsedData.TSystem=HuntedRes.first;
    TargetedFaction * TFactionTemp=new TargetedFaction(input.TFaction);
    (*parsedData.TSystem)->TargetedFactions.insert(TFactionTemp);
#pragma}
#pragma check if source system exists and create if not {
    System *SSysTemp=new System(input.SSystem);
    auto SSysRes=(*parsedData.TSystem)->add(SSysTemp);
    parsedData.SSystem=SSysRes.first;
#pragma}
#pragma check if source station exists and create if not {
    station* SStatTemp=new station(input.SStation);
    auto SStatRes=(*parsedData.SSystem)->add(SStatTemp);
    parsedData.SStation=SStatRes.first;
#pragma}
#pragma check if source faction exists and is in snycron with the globalFactions container and fix if not {
    faction* SFactionTemp=new faction(input.SFaction);
    auto SFactionGlobRes=globalFactions->add(SFactionTemp);
    auto SFactionGlobTemp=SFactionTemp;
    auto SFactionStatRes=(*parsedData.SStation)->add(SFactionTemp);
    if (SFactionGlobRes.second && !SFactionStatRes.second) {
        //New faction in global container
        globalFactions->remove(SFactionGlobTemp);
        delete SFactionGlobTemp;
        SFactionTemp=(*SFactionStatRes.first);
        globalFactions->add(SFactionTemp);
    }
    if (SFactionStatRes.second) {
        //New faction in station
        //If it is new in station it does not matter if it was new in global or not
        //We have to adjust it's homes container either way
        auto fact=(*SFactionGlobRes.first);
        sysStat *temp2=new sysStat({parsedData.SSystem,parsedData.SStation});
        fact->homes.insert(temp2);
    }
    parsedData.SFaction=SFactionStatRes.first;
#pragma}
#pragma check if mission already exists and create if not {
    auto Mission=new class mission(
                input.MID
                ,parsedData.TSystem
                ,parsedData.SSystem
                ,parsedData.SFaction
                ,parsedData.SStation
                ,input.kills
                ,input.reward
                ,input.completed
                ,input.killsSoFar
                );
    auto MissionRes=(*parsedData.SFaction)->add(Mission);
    if (MissionRes.second) {
#pragma Refresh UI with new data{
        unsigned max=globalFactions->stackHeight;
        unsigned total_kills=globalFactions->reCalcTotalKills();
        unsigned total_kills_so_far=globalFactions->totalKillsSoFar;
        double rew=globalFactions->payout;
        ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+rew));
        ui->horizontalSlider->setMaximum(max);
        ui->label_9->setText(QString::number(max));
        ui->label_10->setText(QString::number(total_kills/max, 'f', 10));
        ui->label_11->setText(QString::number((ui->label_8->text().toDouble()*1000/max)/1000000, 'f', 10)+"$ M");
        int temp_4=100*total_kills_so_far/max;
        ui->kills_made->setText(QString::number(total_kills_so_far)+" ("+QString::number(temp_4)+"%)");
        ui->kills_left->setText(QString::number(max-total_kills_so_far)+" ("+QString::number(100-temp_4)+"%)");
#pragma}
    }
    parsedData.Mission=MissionRes.first;
    return parsedData;
#pragma}
}

map<set<huntedSystem*>::iterator,Statistics> data::getStatistics() {
    map<set<huntedSystem*>::iterator,Statistics> output;
    for (auto hunted=CompleteData->begin();hunted!=CompleteData->end();hunted++) {
        output.insert({hunted,(*hunted)->getStats(*globalFactions)});
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
            //item->text(0);
            if (faction==globalFactions->end()) {
                a+="\""+QString::number(i).toStdString()+"\":\""+
                        "0"
                        +";"+
                        "0;0\"";
            } else {
                set<mission*> CollectedMissions;
                for (auto mission:(*faction)->missions) {
                    if ((*mission->sourceStation)->name==item->text(0)) {
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
                            mission->targetFaction.toStdString()
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

void data::refreshdata(int depth,json *SavedData, techlevi::Input * input) {
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
                parseData(*input);
            }
        }
        ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->parent());
    }
    ui->treeWidget_3->expandAll();
    if (depth==0) delete SavedData;
}
