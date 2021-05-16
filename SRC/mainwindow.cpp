#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDir"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QClipboard>
#include <QApplication>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_8->setText("0");
    ui->label_9->setText("0");
    ui->label_10->setText("0");
    ui->label_11->setText("0");
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
    current_station=json::parse("{ \"timestamp\":\"2021-02-14T23:26:06Z\", \"event\":\"Docked\", \"StationName\":\"Sabine Camp\", \"StationType\":\"Outpost\", \"StarSystem\":\"Cuberara\", \"SystemAddress\":2869440554457, \"MarketID\":3225893888, \"StationFaction\":{ \"Name\":\"Albardhas for Equality\", \"FactionState\":\"Election\" }, \"StationGovernment\":\"$government_Democracy;\", \"StationGovernment_Localised\":\"Democracy\", \"StationServices\":[ \"dock\", \"autodock\", \"blackmarket\", \"commodities\", \"contacts\", \"exploration\", \"missions\", \"refuel\", \"repair\", \"tuning\", \"engineer\", \"missionsgenerated\", \"flightcontroller\", \"stationoperations\", \"powerplay\", \"searchrescue\", \"stationMenu\" ], \"StationEconomy\":\"$economy_Extraction;\", \"StationEconomy_Localised\":\"Extraction\", \"StationEconomies\":[ { \"Name\":\"$economy_Extraction;\", \"Name_Localised\":\"Extraction\", \"Proportion\":0.830000 }, { \"Name\":\"$economy_Refinery;\", \"Name_Localised\":\"Refinery\", \"Proportion\":0.170000 } ], \"DistFromStarLS\":5.187345 }");
    ui->kills_left->setText("0 (100%)");
    ui->kills_made->setText("0 (0%)");
    QDir saves(QDir::currentPath()+"/System/");
    QDateTime a=saves.entryInfoList().begin()->lastModified();
    QString c=saves.entryInfoList().begin()->canonicalFilePath();
    for (auto i:saves.entryInfoList()) {
        if (a<i.lastModified()) {
            a=i.lastModified();
            c=i.canonicalFilePath();
        }
    }
    on_listWidget_2_itemClicked(c);
}

void MainWindow::OnNewFile(const QString &file) {
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

void MainWindow::KillsperFaction() {
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(missions.size());
    int curr_row=0;
    int curr_col=0;
    vector<int> frags;
    for (auto i:missions) {
        if (ui->tableWidget->columnCount()<(int)i.second.first.size()+2) {
            ui->tableWidget->setColumnCount(i.second.first.size()+2);
        }
        for (unsigned k=0;k<missions.size();k++) {
            for (unsigned j=ui->tableWidget->columnCount();j>0;j--) {
                ui->tableWidget->setItem(k,j,new QTableWidgetItem(""));
            }
        }
    }
    for (auto i:missions) {
        QTableWidgetItem* temp=new QTableWidgetItem(i.first);
        int frags_needed=0;
        curr_col=0;
        for (auto j:i.second.first) {
            if (!j.second.second.second.second) {
                int kills_needed=0;
                if (curr_col>0) {
                    kills_needed=j.second.second.first.first;
                } else {
                    kills_needed=j.second.second.first.first-j.second.second.first.second;
                }
                ui->tableWidget->item(curr_row,curr_col+1)->setText(QString::number(kills_needed));
                frags_needed+=j.second.second.first.first-j.second.second.first.second;
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

void MainWindow::OnNewEvent(const QString &file) {
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
                    HandleEvent(event);
                    on_pushButton_5_clicked("./System/AutoBackup_"+ui->systemName->text()+".json");
                }  catch (const std::exception& e) {
                    qDebug()<<lastEvent;
                    qDebug()<<e.what();
                }
            }
        }
    }
}


void MainWindow::HandleEvent(json event) {
    try {
        if ((string)event["event"]=="MissionAccepted" && ((string)event["Name"]).find("Massacre")!=string::npos) {
            MissionTargetFactions.insert({(string)event["TargetFaction"],0});
            if (MissionTarget=="Nothing_yet") {
                MissionTarget=(string)event["TargetFaction"];
            }
            temp_mission_target=QString::fromStdString((string)event["TargetFaction"]);
            addMission((string)event["DestinationSystem"],(int)event["KillCount"],(double)event["Reward"]/1000,(unsigned)event["MissionID"],QString::fromStdString((string)event["Faction"]));
        } else if ((string)event["event"]=="MissionRedirected" && ((string)event["Name"]).find("Massacre")!=string::npos) {
            missionCompleted((unsigned)event["MissionID"]);
        } else if ((string)event["event"]=="MissionCompleted") {
            missionCompleted((unsigned)event["MissionID"],true);
        } else if ((string)event["event"]=="Docked") {
            current_station=event;
            for (auto i:factionStation) {
                if ((string)current_station["StationName"]==i.second.toStdString()) {
                    for (auto z:i.first) {
                        for (auto j=0;j<ui->tableWidget->rowCount();j++) {
                            if (ui->tableWidget->item(j,0)->text()==z.first) {
                                for (auto z=0;z<ui->tableWidget->columnCount();z++) {
                                    ui->tableWidget->item(j,z)->setBackground(QBrush(QColor(10,90,170)));
                                }
                            }
                        }
                    }
                }
            }
            if (ui->treeWidget->topLevelItemCount()!=0) {
                resetTreeColor();
                ui->treeWidget->collapseAll();
                ui->treeWidget->expandItem(ui->treeWidget->topLevelItem(0));
                QTreeWidgetItem* temp;
                temp=ui->treeWidget->topLevelItem(0);
                for (auto i=0;i<temp->childCount();i++) {
                    if (temp->child(i)->text(0).toStdString()==(string)current_station["StarSystem"]) {
                        temp=temp->child(i);
                        temp->setBackground(0,QBrush(QColor(10,90,170)));
                        ui->treeWidget->expandItem(temp);
                        break;
                    }
                }
                for (auto i=0;i<temp->childCount();i++) {
                    if (temp->child(i)->text(0).toStdString()==(string)current_station["StationName"]) {
                        temp=temp->child(i);
                        temp->setBackground(0,QBrush(QColor(10,90,170)));
                        ui->treeWidget->expandItem(temp);
                        break;
                    }
                }
                for (auto i=0;i<temp->childCount();i++) {
                    ui->treeWidget->expandItem(temp->child(i));
                }
            }
        } else if ((string)event["event"]=="Bounty") {
            int max=0;
            for (auto i=missions.begin();i!=missions.end();i++) {
                for (auto k=i->second.first.begin();k!=i->second.first.end();k++) {
                    if ((string)event["VictimFaction"]==k->second.first.second.toStdString() && !k->second.second.second.second) {
                        k->second.second.first.second+=1;
                        if (max<k->second.second.first.second) {
                            max=k->second.second.first.second;
                        }
                        break;
                    }
                }
            }
            if (max!=0) {
                total_kills_so_far=max;
                int temp_4=100*total_kills_so_far/max_kills;
                ui->kills_made->setText(QString::number(total_kills_so_far)+" ("+QString::number(temp_4)+"%)");
                ui->kills_left->setText(QString::number(max_kills-total_kills_so_far)+" ("+QString::number(100-temp_4)+"%)");
            }
        } else if ((string)event["event"]=="MissionAbandoned") {
            qDebug()<<"You Abadoned a mission";
            missionCompleted((unsigned)event["MissionID"]);
            missionCompleted((unsigned)event["MissionID"],true);
        }
    } catch (const std::exception& e) {
        qDebug()<<QString::fromStdString(event.dump());
        qDebug()<<e.what();
    }
}

void MainWindow::GetMissions() {
    GettingMissions=true;
    QStringList path1=QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QDir base(path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous");
    auto files=base.entryList();
    for (auto i=files.end()-13;i!=files.end();i++) {
        if (i->contains(".log") && i->contains("Journal.")) {
            OnNewEvent(*i);
            if (Current_Missions.size()>0) {
                i-=2;
            }
        } else {
            if (Current_Missions.size()>0) {
                i-=2;
            }
        }
    }
}

void MainWindow::resetTreeColor() {
    QTreeWidgetItem* temp;
    for (auto i=0;i<ui->treeWidget->topLevelItemCount();i++) {
        temp=ui->treeWidget->topLevelItem(i);
        temp->setBackground(0,QBrush(QColor(255,255,255)));
        QTreeWidgetItem* temp_1;
        for (auto i=0;i<temp->childCount();i++) {
            temp_1=temp->child(i);
            temp_1->setBackground(0,QBrush(QColor(255,255,255)));
            QTreeWidgetItem* temp_2;
            for (auto i=0;i<temp_1->childCount();i++) {
                temp_2=temp_1->child(i);
                temp_2->setBackground(0,QBrush(QColor(255,255,255)));
                QTreeWidgetItem* temp_3;
                for (auto i=0;i<temp_2->childCount();i++) {
                    temp_3=temp_2->child(i);
                    temp_3->setBackground(0,QBrush(QColor(255,165,0)));
                }
            }
        }
    }
    for (auto i=0;i<ui->tableWidget->rowCount();i++) {
        for (auto j=0;j<ui->tableWidget->columnCount();j++) {
            ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(255,255,255)));
        }
    }
}

void MainWindow::missionCompleted(unsigned ID,bool remove) {
    bool found=false;
    double kill_diff=0;
    double rew_diff=0;
    for (auto i=missions.begin();i!=missions.end();i++) {
        for (auto k=i->second.first.begin();k!=i->second.first.end();k++) {
            if (k->second.first.first==ID) {
                if (!remove) {
                    k->second.second.second.second=true;
                    kill_diff=k->second.second.first.first;
                    rew_diff=k->second.second.second.first;
                    i->second.second.first-=kill_diff;
                    total_kills-=kill_diff;
                    i->second.second.second-=rew_diff;
                    ui->label_8->setText(QString::number(ui->label_8->text().toDouble()-rew_diff));
                    int max=0;
                    for (auto j=missions.begin();j!=missions.end();j++) {
                        int kills=j->second.second.first;
                        if (max<kills)
                            max=kills;
                    }
                    total_mission_count--;
                    total_kills_so_far-=max_kills-max;
                    if (total_kills_so_far<0) {
                        total_kills_so_far=0;
                    }
                    max_kills=max;
                    ui->label_9->setText(QString::number(max_kills));
                    int temp_4=100*total_kills_so_far/max_kills;
                    ui->kills_made->setText(QString::number(total_kills_so_far)+" ("+QString::number(temp_4)+"%)");
                    ui->kills_left->setText(QString::number(max_kills-total_kills_so_far)+" ("+QString::number(100-temp_4)+"%)");
                    if (max_kills!=0) {
                        ui->label_10->setText(QString::number((double)total_kills/(double)max_kills, 'f', 10));
                        if (ui->label_10->text().toDouble()<2.0f) {
                            ui->label_20->setText("Stop hunting and go back and turn in your missions");
                        } else {
                            ui->label_20->setText("You can continue the fun");
                        }
                        ui->label_11->setText((QString::number((ui->label_8->text().toDouble()*1000/max_kills)/1000000, 'f', 10))+"$ M");
                    } else {
                        ui->label_10->setText("0");
                        ui->label_11->setText("0");
                        ui->label_8->setText("0");
                    }
                    ui->treeWidget->clear();
                    RemoveMission(&ID);
                    completedData();
                    KillsperFaction();
                    for (auto u=factionStation.begin();u!=factionStation.end();u++) {
                        if (k->first.first==u->second) {
                            for (auto z=u->first.begin();z!=u->first.end();z++) {
                                if (z->first==i->first) {
                                    z->second-=kill_diff;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    RefreshTree();
                    ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
                    found=true;
                    break;
                } else {
                    i->second.first.erase(k);
                    ui->treeWidget->clear();
                    RemoveMission(&ID,remove);
                    completedData();
                    KillsperFaction();
                    ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()-1));
                    found=true;
                    break;
                }
            }
        }
        if (found) {
            break;
        }
    }
}

void MainWindow::addMission(string dest,int kills, double reward, unsigned ID,QString faction) {
    if (ui->systemName->text()=="System") {
        on_pushButton_3_clicked();
        ui->treeWidget_3->topLevelItem(0)->setText(0,QString::fromStdString(dest));
        ui->systemName->setText(QString::fromStdString(dest));
    }
    ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->topLevelItem(0));
    CheckCurrentStation(faction,nullptr,false,0,QString::fromStdString(dest));
    ui->treeWidget->clear();
    station=QString::fromStdString((string)current_station["StationName"]);
    faction_global=faction;
    kills_needed_for_this=kills;
    reward_for_this=reward;
    on_pushButton_6_clicked();
    for (auto i=factionStation.begin();i!=factionStation.end();i++) {
        if (i->second==station) {
            for (auto k=i->first.begin();k!=i->first.end();k++) {
                if (k->first==faction) {
                    k->second+=kills;
                    break;
                }
            }
            break;
        }
    }
    if (ui->label_10->text().toDouble()<3.0) {
        ui->label_20->setText("Pick up some more missions to complete");
    } else {
        ui->label_20->setText("You can go and kill some pirate scum");
    }
    auto temp=&(missions.find(faction)->second.first);
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

void MainWindow::CheckCurrentStation(QString &faction,QTreeWidgetItem* item,bool do_not_search,int depth,QString Target_system) {
    if (ui->treeWidget_3->topLevelItemCount()==0) {
        on_pushButton_3_clicked();
        ui->treeWidget_3->topLevelItem(0)->setText(0,Target_system);
    }
    if (item==nullptr) {
        bool found=false;
        for (auto i=0;i<ui->treeWidget_3->topLevelItem(0)->childCount();i++) {
            if (ui->treeWidget_3->topLevelItem(0)->child(i)->text(0).toStdString()==(string)current_station["StarSystem"]) {
                CheckCurrentStation(faction,ui->treeWidget_3->topLevelItem(0)->child(i),false,1);
                found=true;
                break;
            }
        }
        if (!found) {
            on_pushButton_3_clicked();
            ui->treeWidget_3->topLevelItem(0)->child(ui->treeWidget_3->topLevelItem(0)->childCount()-1)->setText(0,QString::fromStdString((string)current_station["StarSystem"]));
            ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->topLevelItem(0)->child(ui->treeWidget_3->topLevelItem(0)->childCount()-1));
            systemStation.push_back({vector<QString>(),QString::fromStdString((string)current_station["StarSystem"])});
            CheckCurrentStation(faction,ui->treeWidget_3->topLevelItem(0)->child(ui->treeWidget_3->topLevelItem(0)->childCount()-1),true,1);
        }
    } else if (do_not_search && depth==1) {
        ui->treeWidget_3->setCurrentItem(item);
        on_pushButton_3_clicked();
        item->child(item->childCount()-1)->setText(0,QString::fromStdString((string)current_station["StationName"]));
        for (auto z=systemStation.begin();z!=systemStation.end();z++) {
            if (z->second==QString::fromStdString((string)current_station["StarSystem"])) {
                z->first.push_back(QString::fromStdString((string)current_station["StationName"]));
            }
        }
        factionStation.push_back({vector<pair<QString,int>>(),QString::fromStdString((string)current_station["StationName"])});
        CheckCurrentStation(faction,item->child(item->childCount()-1),true,2);
    } else if (do_not_search && depth==2) {
        ui->treeWidget_3->setCurrentItem(item);
        on_pushButton_3_clicked();
        item->child(item->childCount()-1)->setText(0,"[0] "+faction);
        for (auto z=factionStation.begin();z!=factionStation.end();z++) {
            if (z->second==QString::fromStdString((string)current_station["StationName"])) {
                z->first.push_back({faction,0});
            }
        }
        config[ui->systemName->text().toStdString()][(string)current_station["StarSystem"]][(string)current_station["StationName"]][faction.toStdString()]["0"]="0;0;0";
    } else if (!do_not_search && depth==1) {
        bool found=false;
        for (auto i=0;i<item->childCount();i++) {
            if (item->child(i)->text(0).toStdString()==(string)current_station["StationName"]) {
                CheckCurrentStation(faction,item->child(i),false,2);
                found=true;
                break;
            }
        }
        if (!found) {
            CheckCurrentStation(faction,item,true,1);
        }
    } else if (!do_not_search && depth==2) {
        bool found=false;
        for (auto i=0;i<item->childCount();i++) {
            QString temp;
            if (item->child(i)->text(0).contains("] ")) {
                temp=item->child(i)->text(0).section("] ",1,1);
            } else {
                temp=item->child(i)->text(0);
            }
            if (temp==faction) {
                found=true;
                break;
            }
        }
        if (!found) {
            CheckCurrentStation(faction,item,true,2);
        }
    }
}

void MainWindow::RefreshTree() {
    auto GetFactionName=[](QTreeWidgetItem* a)->QString {
        if (a->text(0).contains("] ")) {
            return a->text(0).section("] ",1,1);
        } else {
            return a->text(0);
        }
    };
    for (auto i=0;i<ui->treeWidget_3->topLevelItem(0)->childCount();i++) {
        for (auto k=0;k<ui->treeWidget_3->topLevelItem(0)->child(i)->childCount();k++) {
            QString station=ui->treeWidget_3->topLevelItem(0)->child(i)->child(k)->text(0);
            map<QString,int> some_helper;
            for (auto u:factionStation) {
                for (auto z:u.first) {
                    auto done=some_helper.insert({z.first,z.second});
                    if (!done.second) {
                        done.first->second+=z.second;
                    }
                }
            }
            for (auto j=0;j<ui->treeWidget_3->topLevelItem(0)->child(i)->child(k)->childCount();j++) {
                auto faction=ui->treeWidget_3->topLevelItem(0)->child(i)->child(k)->child(j);
                for (auto z:some_helper) {
                    if (z.first==faction->text(0).section("] ",1,1) || (!faction->text(0).contains("]") && faction->text(0)==z.first)) {
                        faction->setText(0,"["+QString::number(z.second)+"] "+GetFactionName(faction));
                        break;
                    }
                }
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GarbageCollector() {
    config.clear();
    total_mission_count=0;
    total_kills=0;
    max_kills=0;
    ratio=0;
    ui->treeWidget_3->clear();
    ui->treeWidget->clear();
    ui->label_8->setText("0");
    ui->label_9->setText("0");
    ui->label_10->setText("0");
    ui->label_11->setText("0");
    current_station.clear();
    missions.clear();
    MissionTarget="Nothing_yet";
}

void MainWindow::on_listWidget_2_itemClicked(QString file_1)
{
    GarbageCollector();
    QFile f(file_1);
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream stream(&f);
        config= json::parse(stream.readAll().toStdString());
        refreshdata();
        completedData();
        KillsperFaction();
        RefreshTree();
        int max=0;
        for (auto k=missions.begin();k!=missions.end();k++) {
            total_mission_count+=k->second.first.size();
            if (max<k->second.second.first)
                max=k->second.second.first;
            total_kills+=k->second.second.first;
            ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+k->second.second.second));
        }
        max_kills=max;
        ui->horizontalSlider->setMaximum(max);
        ui->systemName->setText(QString::fromStdString((string)config.begin().key()));
        total_kills_so_far=QString::fromStdString((string)config["Total kills so far"].begin().value()).toInt();
        ui->label_9->setText(QString::number(max_kills));
        ui->label_10->setText(QString::number(total_kills/max_kills, 'f', 10));
        ui->label_11->setText(QString::number((ui->label_8->text().toDouble()*1000/max_kills/1000000), 'f', 10)+"$ M");
        int temp_4=100*total_kills_so_far/max_kills;
        ui->kills_made->setText(QString::number(total_kills_so_far)+" ("+QString::number(temp_4)+"%)");
        ui->kills_left->setText(QString::number(max_kills-total_kills_so_far)+" ("+QString::number(100-temp_4)+"%)");
    }
}

void MainWindow::completedData() {
    ui->Curr_payout->setText("0");
    for (auto i:systemStation) {
        for (auto z:i.first) {
            QTreeWidgetItem * temp=new QTreeWidgetItem();
            temp->setText(0,i.second+"::"+z);
            for (auto k:missions) {
                for (auto j:k.second.first) {
                    if (j.first.second==i.second && j.first.first==z && j.second.second.second.second) {
                        QTreeWidgetItem * temp_1=new QTreeWidgetItem();
                        temp_1->setText(0,k.first+"::     "+QString::number(j.second.second.second.first)+"$ K");
                        temp->addChild(temp_1);
                        ui->Curr_payout->setText(QString::number(ui->Curr_payout->text().toDouble()+j.second.second.second.first));
                    }
                }
            }
            ui->treeWidget->addTopLevelItem(temp);
        }
    }
    ui->treeWidget->expandAll();
}

void MainWindow::RemoveMission(unsigned *ID,bool remove) {
    for (auto i=missions.begin();i!=missions.end();i++) {
        for (auto k=i->second.first.begin();k!=i->second.first.end();k++) {
            if (k->second.first.first==*ID) {
                if (remove) {
                    i->second.first.erase(k);
                    if (i->second.first.empty()) {
                        missions.erase(i);
                    }
                } else {
                    k->second.second.second.second=true;
                }
            }
        }
    }
}

void MainWindow::refreshdata(int depth,json *a) {
    if (a==nullptr)
        a=&config;
    QString key;
    QString alma;
    json b;
    for (auto i=a->begin();i!=a->end();i++) {
        key=QString::fromStdString((string)i.key());
        if ((string)i.key()=="Total kills so far") {
            continue;
        }
        if (depth==0) {
            on_pushButton_3_clicked();
            ui->treeWidget_3->currentItem()->setText(0,key);
        } else {
            on_pushButton_3_clicked();
            ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->child(ui->treeWidget_3->currentItem()->childCount()-1));
            ui->treeWidget_3->currentItem()->setText(0,key);
            if (depth==2) {
                station_name=key;
                bool found_out=false;
                for (auto z=systemStation.begin();z!=systemStation.end();z++) {
                    if (z->second==system_namee) {
                        found_out=true;
                        bool found=false;
                        for (auto u=z->first.begin();u!=z->first.end();u++) {
                            if (*u==station_name) {
                                found=true;
                                break;
                            }
                        }
                        if (!found) {
                            z->first.push_back(station_name);
                        }
                        break;
                    }
                }
                factionStation.push_back({vector<pair<QString,int>>(),station_name});
                if (!found_out) {
                    vector<QString> helper;
                    helper.push_back(station_name);
                    systemStation.push_back({helper,system_namee});
                }
            } else if (depth==1) {
                alma=key;
                system_namee=alma;
            }
        }
        if (depth!=3) {
            b=i.value();
            refreshdata(depth+1,&b);
        } else {
            b=i.value();
            vector<pair<pair<QString,QString>,pair<pair<unsigned,QString>,pair<pair<int,int>,pair<double,bool>>>>> temp;
            unsigned j=0;
            int kill=0;
            double rew=0;
            for (auto z=factionStation.begin();z!=factionStation.end();z++) {
                if (z->second==station_name) {
                    QString faction="";
                    int frags=0;
                    if (((string)i.key()).find("] ")!=string::npos) {
                        faction=QString::fromStdString((string)i.key()).section("] ",1,1);
                        frags=QString::fromStdString((string)i.key()).section("] ",0,0).section("[",1,1).toInt();
                    } else {
                        faction=QString::fromStdString((string)i.key());
                    }
                    z->first.push_back({faction,frags});
                }
            }
            for (auto k=b.begin();k!=b.end();k++) {
                QString miss=QString::fromStdString((string)k.value());
                bool tempor;
                if (miss.section(";",3,3)=="0") {
                    tempor=false;

                } else {
                    tempor=true;
                    ui->Missions_completed->setText(QString::number(ui->Missions_completed->text().toInt()+1));
                }
                if (!current_station.empty()) {
                    station_name=QString::fromStdString((string)current_station["StationName"]);
                }
                temp.push_back({{station_name,system_namee},{{QString::fromStdString((string)k.key()).toUInt(),miss.section(";",4,4)},{{miss.section(";",0,0).toInt(),miss.section(";",1,1).toInt()},{miss.section(";",2,2).toDouble(),tempor}}}});
                if (!tempor) {
                    kill+=temp[j].second.second.first.first;
                    rew+=temp[j].second.second.second.first;
                    for (auto u=factionStation.begin();u!=factionStation.end();u++) {
                        if (u->second==station_name) {
                            for (auto z=u->first.begin();z!=u->first.end();z++) {
                                if (z->first==QString::fromStdString((string)i.key())) {
                                    z->second+=temp[j].second.second.first.first;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                j++;
            }
            auto helper=missions.insert({QString::fromStdString((string)i.key()),{temp,{kill,rew}}});
            if (!helper.second) {
                auto helper_2=missions.find(QString::fromStdString((string)i.key()));
                helper_2->second.second.first+=kill;
                helper_2->second.second.second+=rew;
                for (auto z:temp) {
                    helper_2->second.first.push_back(z);
                }
            }
        }
        ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->parent());
    }
    ui->treeWidget_3->expandAll();
}

void MainWindow::on_pushButton_3_clicked()
{
    if (ui->treeWidget_3->currentItem()==nullptr) {
        QTreeWidgetItem *temp=new QTreeWidgetItem();
        temp->setText(0,"system1");
        temp->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
        ui->treeWidget_3->addTopLevelItem(temp);
        ui->treeWidget_3->setCurrentItem(temp);
        first=temp;
    } else {
        QTreeWidgetItem *temp=new QTreeWidgetItem();
        temp->setText(0,"subsection1");
        temp->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
        ui->treeWidget_3->currentItem()->addChild(temp);
    }
}

void MainWindow::on_pushButton_5_clicked(QString file_1)
{
    if (!config.empty()) {
        QFile file(file_1);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            QTreeWidgetItem *item=ui->treeWidget_3->topLevelItem(0);
            string a="{ \""+item->text(0).toStdString()+"\":";
            configreader(item,a);
            QString b=QString::fromStdString(a);
            b.remove("\\");
            b.remove(0,0);
            b.remove(b.length(),0);
            b+=",\"Total kills so far\":{\""+QString::fromStdString(MissionTarget)+"\":\""+QString::number(total_kills_so_far)+"\"}}";
            b.remove("\\");
            stream<<b;
        } else {
            QMessageBox::warning(this,tr("Error while trying to save session"),tr("Could not open given file"));
        }
    }
}

void MainWindow::configreader(QTreeWidgetItem* item, string &a) {
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
            configreader(item->child(i),a);
            if (i!=item->childCount()-1)
                a+=",";
        } else {
            a+="{";
            auto result_1=missions.find(item->child(i)->text(0).section("] ",1,1));
            item->text(0);
            if (result_1==missions.end()) {
                a+="\""+QString::number(i).toStdString()+"\":\""+
                        "0"
                        +";"+
                        "0;0\"";
            } else {
                vector<pair<pair<QString,QString>,pair<pair<unsigned,QString>,pair<pair<int,int>,pair<double,bool>>>>> result;
                for (auto k=result_1->second.first.begin();k!=result_1->second.first.end();k++) {
                    if (k->first.first==item->text(0)) {
                        result.push_back(*k);
                    }
                }
                for (unsigned i=0;i<result.size();i++) {
                    int tempor;
                    if (result.at(i).second.second.second.second==false) {
                        tempor=0;
                    } else {
                        tempor=1;
                    }
                    a+="\""+QString::number(result[i].second.first.first).toStdString()+"\":\""+
                            QString::number(result[i].second.second.first.first).toStdString()
                            +";"+
                            QString::number(result.at(i).second.second.first.second).toStdString()
                            +";"+
                            QString::number(result.at(i).second.second.second.first).toStdString()
                            +";"+
                            QString::number(tempor).toStdString()
                            +";"+
                            result.at(i).second.first.second.toStdString()
                            +"\"";
                    if (i!=result.size()-1) {
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

void MainWindow::on_pushButton_4_clicked()
{
    deleteing(ui->treeWidget_3->currentItem());
    ui->treeWidget_3->currentItem()->parent()->removeChild(ui->treeWidget_3->currentItem());


}

void MainWindow::deleteing(QTreeWidgetItem*a) {
    if (a->childCount()>0) {
        for (int i=0;i<a->childCount();i++) {
            deleteing(a->child(i));
        }
    } else {
        auto temp=missions.find(a->text(0));
        if (temp!=missions.end()) {
            for (auto i=temp->second.first.begin();i!=temp->second.first.begin();i++) {
                if (i->first.first==a->parent()->text(0)) {
                    temp->second.first.erase(i);
                }
            }
        }
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    auto result=missions.find(faction_global);
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
        ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+rew));
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
    total_mission_count++;
    max_kills=max;
    ui->horizontalSlider->setMaximum(max);
    ui->label_9->setText(QString::number(max_kills));
    ui->label_10->setText(QString::number(total_kills/max_kills, 'f', 10));
    ui->label_11->setText(QString::number((ui->label_8->text().toDouble()*1000/max_kills)/1000000, 'f', 10)+"$ M");
    int temp_4=100*total_kills_so_far/max_kills;
    ui->kills_made->setText(QString::number(total_kills_so_far)+" ("+QString::number(temp_4)+"%)");
    ui->kills_left->setText(QString::number(max_kills-total_kills_so_far)+" ("+QString::number(100-temp_4)+"%)");
}

void MainWindow::on_treeWidget_3_itemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem* temp;
    for (auto i=0;i<ui->treeWidget_3->topLevelItemCount();i++) {
        temp=ui->treeWidget_3->topLevelItem(i);
        temp->setBackground(0,QBrush(QColor(255,255,255)));
        QTreeWidgetItem* temp_1;
        for (auto i=0;i<temp->childCount();i++) {
            temp_1=temp->child(i);
            temp_1->setBackground(0,QBrush(QColor(255,255,255)));
            QTreeWidgetItem* temp_2;
            for (auto i=0;i<temp_1->childCount();i++) {
                temp_2=temp_1->child(i);
                temp_2->setBackground(0,QBrush(QColor(255,255,255)));
                QTreeWidgetItem* temp_3;
                for (auto i=0;i<temp_2->childCount();i++) {
                    temp_3=temp_2->child(i);
                    temp_3->setBackground(0,QBrush(QColor(255,255,255)));
                }
            }
        }
    }
    item->setBackground(column,QBrush(QColor(0,0,155)));
}

void MainWindow::on_actionSave_Session_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save current session"), "./System",
                                                    tr("Save Files (*.json)"), 0,
                                                    QFileDialog::DontConfirmOverwrite);
    on_pushButton_5_clicked(fileName);
}

void MainWindow::on_actionLoad_session_triggered()
{
    on_actionStart_new_session_triggered();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select file to Load"), "./System", tr("Save Files (*.json)"));
    on_listWidget_2_itemClicked(fileName);
}

void MainWindow::on_actionStart_new_session_triggered()
{
    GarbageCollector();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    ui->kills_left->setText("0 (0%)");
    ui->kills_made->setText("0 (100%)");
    total_kills_so_far=0;
    ui->treeWidget->clear();
    ui->treeWidget_3->clear();
    config.clear();
    missions.clear();
    current_station.clear();
    factionStation.clear();
    systemStation.clear();
    ui->Missions_completed->setText("0");
    ui->Curr_payout->setText("0");
    ui->horizontalSlider->setMaximum(1);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    int theor_completed=0;
    double theor_rew=0;
    int all_missions=0;
    double all_reward=0;
    for (auto i:missions) {
        int temp=value;
        for (auto k:i.second.first) {
            if (0<=temp-k.second.second.first.first) {
                temp=temp-k.second.second.first.first;
                theor_completed++;
                theor_rew+=k.second.second.second.first;
            }
            all_missions++;
            all_reward+=k.second.second.second.first;
        }
    }
    ui->Reward->setText(QString::number(theor_rew)+" ("+QString::number(theor_rew*100/all_reward)+"%)");
    ui->max_theoretycal->setText(QString::number(theor_completed)+" ("+QString::number(theor_completed*100/all_missions)+"%)");
    ui->horizontalSlider->setToolTip(QString::number(value));
    ui->Theor_num_kill->setText(QString::number(value)+" ("+QString::number(value*100/ui->horizontalSlider->maximum())+"%)");
}

void MainWindow::on_Copy_data_clicked()
{
    QString a="====================================\n";
    a+="```Total payout "+QString::number(ui->Curr_payout->text().toDouble()/1000)+"$ Mill for "+ui->Missions_completed->text()+" Missions \n";
    a+=ui->systemName->text()+"\n";
    a+="LFW to turn in```\n";
    a+="====================================";
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(a);
}

void MainWindow::on_pushButton_clicked()
{
    int Total_missions=0;
    for (auto i:missions) {
        for (auto k:i.second.first) {
            if (!k.second.second.second.second) {
                Total_missions++;
            }
        }
    }
    QString a="====================================\n";
    a+="```Total payout "+QString::number((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number((ui->Missions_completed->text().toInt()+Total_missions))+" Missions \n";
    if (ui->Missions_completed->text().toInt()>0)
        a+="Completed: "+QString::number((ui->Curr_payout->text().toDouble())/1000)+"$ Mill for "+ui->Missions_completed->text()+" Mission(s) \n";
    if (total_mission_count>0)
        a+="Active: "+QString::number((ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number(Total_missions)+" Mission(s) \n";
    a+="Stack Height: "+QString::number(max_kills)+" Stack Width: "+QString::number(missions.size())+"\n";
    a+="Current Ratio: "+ui->label_10->text()+" Mission average payout: "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)(ui->Missions_completed->text().toInt()+Total_missions))+"$ Mill \nReward per kill (Bounty not included): "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)max_kills)+"$ Mill\n";
    a+="Mission target system: "+ui->systemName->text()+"\n";
    a+="LFW to Complete/Turn in```\n";
    a+="====================================";
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(a);
}
