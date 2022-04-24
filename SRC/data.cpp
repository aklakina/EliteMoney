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
        globalFactions->totalKillsSoFar-=globalFactions->stackHeight-max;
        emit Refresh(*globalFactions,*CompleteData);
        KillsperFaction();
        /*for (auto h=faction->homes.begin();h!=faction->homes.end();h++) {
            if (mission->sourceStation==(*h)->getStat().name) {
                for (auto z=u->first.begin();z!=u->first.end();z++) {
                    if (z->first==i->first) {
                        z->second-=kill_diff;
                        break;
                    }
                }
                break;
            }
        }*/ //This is only needed to reduce the kills at the faction kills counter
        emit UpdateTree();
        ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
    } else {
        faction->missions.erase(res.missionPlace);
        ui->treeWidget->clear();
        RemoveMission(&ID,remove);
        emit Refresh(*globalFactions,*CompleteData);
        KillsperFaction();
        ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()-1));
    }
}

void data::RemoveMission(unsigned *ID,bool remove) {
    auto res=globalFactions->findMission(*ID);
    if (remove) {
        (*res.factionPlace)->missions.erase(res.missionPlace);
        if ((*res.factionPlace)->missions.empty()) {
            globalFactions->remove((*res.factionPlace));
        }
    } else {
        (*res.missionPlace)->Completed=true;
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
    auto parsed=parseData(input);
    if (ui->label_10->text().toDouble()<3.0) {
        ui->label_20->setText("Pick up some more missions to complete");
    } else {
        ui->label_20->setText("You can go and kill some pirate scum");
    }
    auto temp=&(missions.find(Sfaction)->second.first);
    auto temp_2=temp->begin();
    for (auto k=temp->begin();k!=temp->end();k++) {
        if (k->first.first==QString::fromStdString((string)current_station["StationName"])) {
            temp_2=k;
        }
    }
    temp_2->second.first.first=ID;
    KillsperFaction();
    RefreshTree();
    completedData();
}

Result data::parseData(Input input)
{
#pragma check if TargetSystem exists{
    if (CompleteData==nullptr) {
        CompleteData=new HuntedSystems();
    }
    huntedSystem* Huntedtemp=new huntedSystem(input.TSystem);
    auto HuntedRes=CompleteData->add(Huntedtemp);
#pragma}
    Result parsedData;
    parsedData.TSystem=HuntedRes.first;
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


    /*auto result=missions.find(faction_global);
    QString system_name;
    for (auto z:systemStation) {
        bool found=false;
        for (auto u:z.first) {
            if (u==station) {
                found=true;
                system_name=z.second;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (result==missions.end()) {
        vector<pair<pair<QString,QString>,pair<pair<unsigned,QString>,pair<pair<int,int>,pair<double,bool>>>>> temp;
        int kill=0;
        double rew=0;
        temp.push_back({{station,system_name},{{0,temp_mission_target},{{kills_needed_for_this,0},{reward_for_this,false}}}});
        kill+=kills_needed_for_this;
        rew+=reward_for_this;
        total_kills+=kill;
        missions.insert({faction_global,{temp,{kill,rew}}});
    } else {
        int kill=kills_needed_for_this;
        double rew=reward_for_this;
        result->second.first.push_back({{station,system_name},{{result->second.first.size(),temp_mission_target},{{kills_needed_for_this,0},{reward_for_this,false}}}});
        result->second.second.first+=kill;
        total_kills+=kill;
        result->second.second.second+=rew;
        ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+rew));
    }
    int max=0;
    for (auto k=missions.begin();k!=missions.end();k++) {
        int kills=k->second.second.first;
        if (max<kills)
            max=kills;
    }
    */
}

map<set<huntedSystem*>::iterator,Statistics> data::getStatistics() {
    map<set<huntedSystem*>::iterator,Statistics> output;
    for (auto hunted=CompleteData->begin();hunted!=CompleteData->end();hunted++) {
        output.insert({hunted,(*hunted)->getStats(*globalFactions)});
    }
}

Statistics data::getUnifiedStatistics()
{
    Statistics output;
    auto res=this->getStatistics();
    for (auto hunted:res) {
        output+=hunted.second;
    }
    return output;
}
