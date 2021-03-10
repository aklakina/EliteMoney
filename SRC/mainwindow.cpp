#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDir"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>
#include <windows.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //bool firstrun=!QFile::exists("./Ran");
    QDir directory(QDir::currentPath()+"/System/");
    QStringList configs = directory.entryList();
    ui->listWidget_2->addItems(configs);
    delete ui->listWidget_2->item(0);
    delete ui->listWidget_2->item(0);
    ui->missions->setColumnCount(2);
    QStringList a;
    a.push_back("Kills");
    a.push_back("Reward (Mill)");
    ui->missions->setHeaderLabels(a);
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
        /*if (firstrun)
            OnNewEvent(b);*/
        baseabsolute.push_back(b);
    }
    originalContent=basenames;
    notifier->addPaths(baseabsolute);
    connect(notifier,SIGNAL(directoryChanged(QString)),this,SLOT(OnNewFile(QString)));
    connect(notifier,SIGNAL(fileChanged(const QString &)),this,SLOT(OnNewEvent(const QString &)));
    current_station=json::parse("{ \"timestamp\":\"2021-02-14T23:26:06Z\", \"event\":\"Docked\", \"StationName\":\"Sabine Camp\", \"StationType\":\"Outpost\", \"StarSystem\":\"Cuberara\", \"SystemAddress\":2869440554457, \"MarketID\":3225893888, \"StationFaction\":{ \"Name\":\"Albardhas for Equality\", \"FactionState\":\"Election\" }, \"StationGovernment\":\"$government_Democracy;\", \"StationGovernment_Localised\":\"Democracy\", \"StationServices\":[ \"dock\", \"autodock\", \"blackmarket\", \"commodities\", \"contacts\", \"exploration\", \"missions\", \"refuel\", \"repair\", \"tuning\", \"engineer\", \"missionsgenerated\", \"flightcontroller\", \"stationoperations\", \"powerplay\", \"searchrescue\", \"stationMenu\" ], \"StationEconomy\":\"$economy_Extraction;\", \"StationEconomy_Localised\":\"Extraction\", \"StationEconomies\":[ { \"Name\":\"$economy_Extraction;\", \"Name_Localised\":\"Extraction\", \"Proportion\":0.830000 }, { \"Name\":\"$economy_Refinery;\", \"Name_Localised\":\"Refinery\", \"Proportion\":0.170000 } ], \"DistFromStarLS\":5.187345 }");
    ui->kills_left->setText("0");
    ui->kills_made->setText("0");
    /*if (firstrun) {
        QFile file_1("./Ran");
        if (file_1.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file_1);
            stream<<"true";
        }
    }*/
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
    ui->listWidget->clear();
    for (auto i:missions) {
        QListWidgetItem* temp=new QListWidgetItem();
        temp->setText(i.first+"    :    "+QString::number(i.second.second.first)+" for "+QString::number(i.second.second.second)+" Million credits");
        ui->listWidget->addItem(temp);
    }
}

void MainWindow::OnNewEvent(const QString &file) {
    /*if (checkingifdone) {
        therewasanother=true;
        return;
    } else {
        checkingifdone=true;
    }
    for (int i=0;i<100;i++) {
        Sleep(10);
        if (therewasanother) {
            qDebug()<<"restarted counter";
            i=0;
            therewasanother=false;
        }
    }
    checkingifdone=false;*/
    QFile changed(file);
    qDebug()<<"event started file opened";
    if (changed.open(QIODevice::ReadOnly)) {
        QTextStream stream(&changed);
        QString lastEvent="";
        json temp;
        json event;
        while (!stream.atEnd()) {
            lastEvent=stream.readLine();
            try {
                temp=json::parse(lastEvent.toStdString());
                qDebug()<<lastEvent+"   parsed";
            }  catch (const std::exception& e) {
                qDebug()<<lastEvent;
                qDebug()<<e.what();
                return;
            }
            bool found=false;
            for (auto k=events.begin();k!=events.end();k++) {
                if (k.value()==temp) {
                    qDebug()<<"skipping since event already loaded";
                    found=true;
                    break;
                }
            }
            if (!found) {
                event=temp;
                events.push_back(event);
                qDebug()<<"actually doing something";
                try {
                if ((string)event["event"]=="MissionAccepted" && ((string)event["Name"]).find("Massacre")!=string::npos) {
                    MissionTargetFactions.insert({(string)event["TargetFaction"],0});
                    if (MissionTarget=="Nothing_yet") {
                        MissionTarget=(string)event["TargetFaction"];
                    }
                    addMission((string)event["DestinationSystem"],(int)event["KillCount"],(double)event["Reward"]/1000000,(unsigned)event["MissionID"],QString::fromStdString((string)event["Faction"]));
                } else if ((string)event["event"]=="MissionRedirected" && ((string)event["Name"]).find("Massacre")!=string::npos) {
                    missionCompleted((unsigned)event["MissionID"]);
                } else if ((string)event["event"]=="MissionCompleted") {
                    missionCompleted((unsigned)event["MissionID"],true);
                } else if ((string)event["event"]=="Docked") {
                    current_station=event;
                    for (auto i=0;i<ui->systems->count();i++) {
                        if (ui->systems->item(i)->text().toStdString()==(string)current_station["StarSystem"]) {
                            ui->systems->setCurrentItem(ui->systems->item(i));
                            on_systems_itemClicked(ui->systems->currentItem());
                            break;
                        }
                    }

                    for (auto i=0;i<ui->stations->count();i++) {
                        if (ui->stations->item(i)->text().toStdString()==(string)current_station["StationName"]) {
                            ui->stations->setCurrentItem(ui->stations->item(i));
                            on_stations_itemClicked(ui->stations->currentItem());
                            break;
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
                    if (MissionTarget==(string)event["VictimFaction"]) {
                        total_kills_so_far++;
                        //MissionTargetFactions.find((string)event["VictimFaction"])->second+=1;
                        ui->kills_made->setText(QString::number(total_kills_so_far));
                        ui->kills_left->setText(QString::number(max_kills-total_kills_so_far));
                    }
                } else if ((string)event["event"]=="MissionAbandoned") {
                    missionCompleted(QString::fromStdString((string)event["MissionID"]).toUInt(),true);
                }
                } catch (const std::exception& e) {
                    qDebug()<<lastEvent;
                    qDebug()<<e.what();
                }
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
}

void MainWindow::missionCompleted(unsigned ID,bool remove) {
    bool found=false;
    double kill_diff=0;
    double rew_diff=0;
    for (auto i=missions.begin();i!=missions.end();i++) {
        for (auto k=i->second.first.begin();k!=i->second.first.end();k++) {
            if (k->first==ID) {
                if (!remove) {
                    k->second.second.second.second=true;
                    kill_diff=k->second.second.first;
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
                    total_kills_so_far=max_kills-max;
                    max_kills=max;
                    ui->label_9->setText(QString::number(max_kills));
                    ui->kills_made->setText(QString::number(total_kills_so_far));
                    ui->kills_left->setText(QString::number(max_kills-total_kills_so_far));
                    if (max_kills!=0) {
                        ui->label_10->setText(QString::number((double)total_kills/(double)max_kills, 'f', 10));
                        ui->label_11->setText(QString::number(ui->label_8->text().toDouble()*1000000/max_kills, 'f', 10));
                    } else {
                        ui->label_10->setText("0");
                        ui->label_11->setText("0");
                        ui->label_8->setText("0");
                    }
                    ui->systems->clear();
                    ui->stations->clear();
                    ui->factions->clear();
                    ui->missions->clear();
                    ui->treeWidget->clear();
                    displayData(&ID);
                    ui->treeWidget->addTopLevelItem(ui->treeWidget_3->topLevelItem(0)->clone());
                    ui->treeWidget->expandAll();
                    completedData();
                    KillsperFaction();
                    on_factions_itemClicked(ui->factions->currentItem());
                    found=true;
                    break;
                } else {
                    i->second.first.erase(k);
                    ui->systems->clear();
                    ui->stations->clear();
                    ui->factions->clear();
                    ui->missions->clear();
                    ui->treeWidget->clear();
                    ui->treeWidget->addTopLevelItem(ui->treeWidget_3->topLevelItem(0)->clone());
                    ui->treeWidget->expandAll();
                    displayData(&ID,remove);
                    completedData();
                    KillsperFaction();
                    found=true;
                    break;
                }
            }
        }
        if (found) {
            break;
        }
    }
    /*found=false;
    for (auto i=missions.begin();i!=missions.end();i++) {
        for (auto k=i->second.first.begin();k!=i->second.first.end();k++) {
            if (!k->second.second.first) {
                found=true;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        MissionTarget="Nothing_yet";
    }*/
}

void MainWindow::addMission(string dest,int kills, double reward, unsigned ID,QString faction) {
    if (ui->systemName->text()=="System") {
        bool found=false;
        for (int i=0;i<ui->listWidget_2->count();i++) {
            if (ui->listWidget_2->item(i)->text()==QString::fromStdString(dest)) {
                on_listWidget_2_itemClicked(ui->listWidget_2->item(i));
                ui->listWidget_2->setCurrentRow(i);
                found=true;
                break;
            }
        }
        if (!found) {
            on_pushButton_3_clicked();
            ui->treeWidget_3->topLevelItem(0)->setText(0,QString::fromStdString(dest));
            ui->systemName->setText(QString::fromStdString(dest));
        }
    }
    ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->topLevelItem(0));
    CheckCurrentStation(faction);
    ui->systems->clear();
    ui->stations->clear();
    ui->factions->clear();
    ui->missions->clear();
    ui->treeWidget->clear();
    ui->treeWidget->addTopLevelItem(ui->treeWidget_3->topLevelItem(0)->clone());
    ui->treeWidget->expandAll();
    displayData();
    for (auto i=0;i<ui->systems->count();i++) {
        if (ui->systems->item(i)->text().toStdString()==(string)current_station["StarSystem"]) {
            ui->systems->setCurrentRow(i);
            on_systems_itemClicked(ui->systems->item(i));
            break;
        }
    }
    for (auto i=0;i<ui->stations->count();i++) {
        if (ui->stations->item(i)->text().toStdString()==(string)current_station["StationName"]) {
            ui->stations->setCurrentRow(i);
            on_stations_itemClicked(ui->stations->item(i));
            break;
        }
    }
    for (auto i=0;i<ui->factions->count();i++) {
        if (ui->factions->item(i)->text()==faction) {
            ui->factions->setCurrentRow(i);
            on_factions_itemClicked(ui->factions->item(i));
            break;
        }
    }
    kills_needed_for_this=kills;
    reward_for_this=reward;
    on_pushButton_6_clicked();
    auto temp=&(missions.find(faction)->second.first);
    auto temp_2=temp->begin();
    for (auto k=temp->begin();k!=temp->end();k++) {
        if (k->first==QString::fromStdString((string)current_station["StationName"])) {
            temp_2=k;
        }
    }
    temp_2->second.first=ID;
    ui->missions->topLevelItem(ui->missions->topLevelItemCount()-1)->setText(3,QString::number(ID));
    on_pushButton_5_clicked();
}

void MainWindow::CheckCurrentStation(QString &faction,QTreeWidgetItem* item,bool do_not_search,int depth) {
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
            CheckCurrentStation(faction,ui->treeWidget_3->topLevelItem(0)->child(ui->treeWidget_3->topLevelItem(0)->childCount()-1),true,1);
        }
    } else if (do_not_search && depth==1) {
        ui->treeWidget_3->setCurrentItem(item);
        on_pushButton_3_clicked();
        item->child(item->childCount()-1)->setText(0,QString::fromStdString((string)current_station["StationName"]));
        CheckCurrentStation(faction,item->child(item->childCount()-1),true,2);
    } else if (do_not_search && depth==2) {
        ui->treeWidget_3->setCurrentItem(item);
        on_pushButton_3_clicked();
        item->child(item->childCount()-1)->setText(0,faction);
        config[ui->systemName->text().toStdString()][(string)current_station["StarSystem"]][(string)current_station["StationName"]][faction.toStdString()]["0"]="0;0;0";
        on_pushButton_5_clicked();
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
            if (item->child(i)->text(0)==faction) {
                found=true;
                break;
            }
        }
        if (!found) {
            CheckCurrentStation(faction,item,true,2);
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
    ui->systems->clear();
    ui->stations->clear();
    ui->factions->clear();
    ui->missions->clear();
    ui->treeWidget->clear();
    ui->label_8->setText("0");
    ui->label_9->setText("0");
    ui->label_10->setText("0");
    ui->label_11->setText("0");
    current_station.clear();
    missions.clear();
    MissionTarget="Nothing_yet";
}

void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
    if (item->text()==".." || item->text()==".") {
        return;
    }
    GarbageCollector();
    for (int i=0;i<ui->listWidget_2->count();i++) {
        ui->listWidget_2->item(i)->setBackground(QBrush(QColor(255,255,255)));
    }
    item->setBackground(QBrush(QColor(0,255,0)));
    QFile f("./System/"+item->text());
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream stream(&f);
        config= json::parse(stream.readAll().toStdString());
        refreshdata();
        ui->treeWidget->addTopLevelItem(ui->treeWidget_3->topLevelItem(0)->clone());
        ui->treeWidget->expandAll();
        displayData();
        completedData();
        KillsperFaction();
        int max=0;
        for (auto k=missions.begin();k!=missions.end();k++) {
            total_mission_count+=k->second.first.size();
            if (max<k->second.second.first)
                max=k->second.second.first;
            total_kills+=k->second.second.first;
            ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+k->second.second.second));
        }
        max_kills=max;
        MissionTarget=(string)config["Total kills so far"].begin().key();
        total_kills_so_far=QString::fromStdString((string)config["Total kills so far"].begin().value()).toInt();
        ui->label_9->setText(QString::number(max_kills));
        ui->label_10->setText(QString::number(total_kills/max_kills, 'f', 10));
        ui->label_11->setText(QString::number(ui->label_8->text().toDouble()*1000000/max_kills, 'f', 10));
        ui->kills_made->setText(QString::number(total_kills_so_far));
        ui->kills_left->setText(QString::number(max_kills-total_kills_so_far));
    }
}

void MainWindow::completedData() {
    for (auto k=0;k<ui->treeWidget->topLevelItem(0)->childCount();k++) {
        QTreeWidgetItem* systems=ui->treeWidget->topLevelItem(0)->child(k);
        for (auto j=0;j<systems->childCount();j++) {
            QTreeWidgetItem* stations=systems->child(j);
            for (auto n=0;n<stations->childCount();n++) {
                QTreeWidgetItem* factions=stations->child(n);
                auto temp=missions.find(factions->text(0));
                if (temp!=missions.end()) {
                    for (auto k=temp->second.first.begin();k!=temp->second.first.end();k++) {
                        if (k->first==stations->text(0) && k->second.second.second.second) {
                            QTreeWidgetItem* mission=new QTreeWidgetItem();
                            mission->setText(0,QString::number(k->second.second.first)+" kills for "+QString::number(k->second.second.second.first)+" Million Credits");
                            mission->setBackground(0,QBrush(QColor(255,165,0)));
                            factions->addChild(mission);
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::displayData(unsigned *ID,bool remove) {
    ui->systemName->setText(QString::fromStdString((string)config.begin().key()));
    for (auto i=config.at(ui->systemName->text().toStdString()).begin();i!=config.at(ui->systemName->text().toStdString()).end();i++) {
        ui->systems->addItem(QString::fromStdString((string)i.key()));

    }
    ui->systems->setCurrentRow(0);
    ui->systems->currentItem()->setBackground(QBrush(QColor(0,100,255)));
    auto sys=config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString());
    for (auto i=sys.begin();i!=sys.end();i++) {
        ui->stations->addItem(QString::fromStdString((string)i.key()));
    }
    ui->stations->setCurrentRow(0);
    ui->stations->currentItem()->setBackground(QBrush(QColor(0,100,255)));
    auto stat=config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString());
    for (auto i=stat.begin();i!=stat.end();i++) {
        ui->factions->addItem(QString::fromStdString((string)i.key()));
        if (remove) {
            for (auto i=config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString()).begin();i!=config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString()).end();i++) {
                i.value().erase(QString::number(*ID).toStdString());
                /*if (i.value().empty()) {
                    json* temp=&config[ui->systemName->text().toStdString()][ui->systems->currentItem()->text().toStdString()][ui->stations->currentItem()->text().toStdString()];
                    temp->erase(i);
                    if ((*temp).empty()) {
                        json* temp_2=&config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString());
                        temp_2->erase(ui->stations->currentItem()->text().toStdString());
                        if (temp_2->empty()) {
                            json* temp_3=&config.at(ui->systemName->text().toStdString());
                            temp_3->erase(ui->systems->currentItem()->text().toStdString());
                            if (temp_3->empty()) {
                                config.clear();
                            }
                        }
                    }
                }*/
            }
        }
    }
    ui->factions->setCurrentRow(0);
    ui->factions->currentItem()->setBackground(QBrush(QColor(0,100,255)));
    /*if (remove && ID!=nullptr) {
        config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString()).at(ui->factions->currentItem()->text().toStdString()).erase(QString::number(*ID).toStdString());
    }*/
    auto fact=config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString()).at(ui->factions->currentItem()->text().toStdString());

    for (auto i=fact.begin();i!=fact.end();i++) {
        if (QString::fromStdString((string)i.value()).section(";",2,2)=="1") {
        } else if (ID!= nullptr) {
            if (QString::fromStdString((string)i.key())==QString::number(*ID)) {
                i.value()=(QString::fromStdString((string)i.value()).section(";",0,1)+";1").toStdString();
            }
        } else {
            QTreeWidgetItem *temp=new QTreeWidgetItem();
            temp->setText(0,QString::fromStdString((string)i.value()).section(";",0,0));
            temp->setText(1,QString::fromStdString((string)i.value()).section(";",1,1));
            ui->missions->addTopLevelItem(temp);
        }
    }
}

void MainWindow::refreshdata(int depth,json *a) {
    if (a==nullptr)
        a=&config;
    QString key;
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
            }
        }
        if (depth!=3) {
            b=i.value();
            refreshdata(depth+1,&b);
        } else {
            b=i.value();
            vector<pair<QString,pair<unsigned,pair<int,pair<double,bool>>>>> temp;
            unsigned j=0;
            int kill=0;
            double rew=0;
            for (auto k=b.begin();k!=b.end();k++) {
                QString miss=QString::fromStdString((string)k.value());
                bool tempor;
                if (miss.section(";",2,2)=="0") {
                    tempor=false;

                } else {
                    tempor=true;
                }
                if (!current_station.empty()) {
                    station_name=QString::fromStdString((string)current_station["StationName"]);
                }
                temp.push_back({station_name,{QString::fromStdString((string)k.key()).toUInt(),{miss.section(";",0,0).toInt(),{miss.section(";",1,1).toDouble(),tempor}}}});
                if (!tempor) {
                    kill+=temp[j].second.second.first;
                    rew+=temp[j].second.second.second.first;
                }
                j++;
            }
            missions.insert({QString::fromStdString((string)i.key()),{temp,{kill,rew}}});
        }
        ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->parent());
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    //qDebug()<<"megnyomtad:"+ui->treeWidget_3->currentItem()->text(0);
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

void MainWindow::on_pushButton_5_clicked()
{
    QFile file("./System/"+first->text(0)+".json");
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
        //qDebug()<<b;
        stream<<b;
        QDir directory(QDir::currentPath()+"/System/");
        QStringList configs = directory.entryList();
        ui->listWidget_2->clear();
        ui->listWidget_2->addItems(configs);
        delete ui->listWidget_2->item(0);
        delete ui->listWidget_2->item(0);
        for (auto i=0;i!=configs.size();i++) {
            if (configs[i].toStdString()==item->text(0).toStdString()) {
                ui->listWidget_2->setCurrentRow(i);
                ui->listWidget_2->setCurrentItem(ui->listWidget_2->item(i));
                on_listWidget_2_itemClicked(ui->listWidget_2->item(i));
                break;
            }
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
        a+="\""+item->child(i)->text(0).toStdString()+"\":";
        if (item->child(i)->childCount()!=0) {
            configreader(item->child(i),a);
            if (i!=item->childCount()-1)
                a+=",";
        } else {
            a+="{";
            auto result_1=missions.find(item->child(i)->text(0));
            item->text(0);
            if (result_1==missions.end()) {
                a+="\""+QString::number(i).toStdString()+"\":\""+
                        "0"
                        +";"+
                        "0;0\"";
            } else {
                vector<pair<QString,pair<unsigned,pair<int,pair<double,bool>>>>> result;
                for (auto k=result_1->second.first.begin();k!=result_1->second.first.end();k++) {
                    if (k->first==item->text(0)) {
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
                    a+="\""+QString::number(result.at(i).second.first).toStdString()+"\":\""+
                            QString::number(result.at(i).second.second.first).toStdString()
                            +";"+
                            QString::number(result.at(i).second.second.second.first).toStdString()
                            +";"+
                            QString::number(tempor).toStdString()
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
                if (i->first==a->parent()->text(0)) {
                    temp->second.first.erase(i);
                }
            }
        }
    }
}

void MainWindow::on_systems_itemClicked(QListWidgetItem *item)
{
    for (auto i=0;i<ui->systems->count();i++) {
        ui->systems->item(i)->setBackground(QBrush(QColor(255,255,255)));
    }
    item->setBackground(QBrush(QColor(0,100,255)));
    ui->stations->clear();
    ui->factions->clear();
    ui->missions->clear();
    json a=config.at(ui->systemName->text().toStdString()).at(item->text().toStdString());
    for (auto i=a.begin();i!=a.end();i++) {
        ui->stations->addItem(QString::fromStdString((string)i.key()));
    }
}

void MainWindow::on_stations_itemClicked(QListWidgetItem *item)
{
    for (auto i=0;i<ui->stations->count();i++) {
        ui->stations->item(i)->setBackground(QBrush(QColor(255,255,255)));
    }
    item->setBackground(QBrush(QColor(0,100,255)));
    ui->factions->clear();
    ui->missions->clear();
    json a=config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString()).at(item->text().toStdString());
    for (auto i=a.begin();i!=a.end();i++) {
        ui->factions->addItem(QString::fromStdString((string)i.key()));
    }
}

void MainWindow::on_factions_itemClicked(QListWidgetItem *item)
{
    for (auto i=0;i<ui->factions->count();i++) {
        ui->factions->item(i)->setBackground(QBrush(QColor(255,255,255)));
    }
    item->setBackground(QBrush(QColor(0,100,255)));
    ui->factions->setCurrentItem(item);
    ui->missions->clear();
    if (missions.empty()) {
        return;
    }
    auto a=missions.find(ui->factions->currentItem()->text());
    if (a!=missions.end()) {
        for (auto i=a->second.first.begin();i!=a->second.first.end();i++) {
            if (!i->second.second.second.second && i->first==ui->stations->currentItem()->text()) {
                QTreeWidgetItem *temp=new QTreeWidgetItem();
                temp->setText(0,QString::number(i->second.second.first));
                temp->setText(1,QString::number(i->second.second.second.first));
                temp->setText(2,QString::number(i->second.first));
                ui->missions->addTopLevelItem(temp);
            }
        }
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    auto result=missions.find(ui->factions->currentItem()->text());
    //ui->stations->currentItem()->text()
    QTreeWidgetItem *temp=new QTreeWidgetItem();
    temp->setText(0,QString::number(kills_needed_for_this));
    temp->setText(1,QString::number((double)reward_for_this));
    ui->missions->addTopLevelItem(temp);
    if (result==missions.end()) {
        vector<pair<QString,pair<unsigned,pair<int,pair<double,bool>>>>> temp;
        int kill=0;
        double rew=0;
        for (auto i=0;i<ui->missions->topLevelItemCount();i++) {
            temp.push_back({ui->stations->currentItem()->text(),{i,{ui->missions->topLevelItem(i)->text(0).toInt(),{ui->missions->topLevelItem(i)->text(1).toDouble(),false}}}});
            kill+=ui->missions->topLevelItem(i)->text(0).toInt();
            rew+=ui->missions->topLevelItem(i)->text(1).toDouble();
        }
        total_kills+=kill;
        missions.insert({ui->factions->currentItem()->text(),{temp,{kill,rew}}});
        ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+rew));
    } else {
        int kill=kills_needed_for_this;
        double rew=reward_for_this;
        result->second.first.push_back({ui->stations->currentItem()->text(),{result->second.first.size(),{ui->missions->topLevelItem(ui->missions->topLevelItemCount()-1)->text(0).toInt(),{ui->missions->topLevelItem(ui->missions->topLevelItemCount()-1)->text(1).toDouble(),false}}}});
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
    ui->label_9->setText(QString::number(max_kills));
    ui->label_10->setText(QString::number(total_kills/max_kills, 'f', 10));
    ui->label_11->setText(QString::number(ui->label_8->text().toDouble()*1000000/max_kills, 'f', 10));
    ui->kills_made->setText(QString::number(total_kills_so_far));
    ui->kills_left->setText(QString::number(max_kills-total_kills_so_far));
}



/*void MainWindow::on_pushButton_7_clicked()
{
    unsigned j=ui->missions->currentIndex().row();
    int kill_diff=ui->missions->currentItem()->text(0).toInt();
    double rew_diff=ui->missions->currentItem()->text(1).toDouble();
    auto result=missions.find(ui->factions->currentItem()->text());
    for (auto i=result->second.first.begin();i!=result->second.first.end();i++) {
        if (i->first==j) {
            result->second.first.erase(i);
            break;
        }
    }
    result->second.second.first-=kill_diff;
    total_kills-=kill_diff;
    result->second.second.second-=rew_diff;
    ui->label_8->setText(QString::number(ui->label_8->text().toDouble()-rew_diff));
    delete ui->missions->topLevelItem(j);
    unsigned k=0;
    for (auto i=result->second.first.begin();i!=result->second.first.end();i++) {
        i->first=k;
        k++;
    }
    int max=0;
    for (auto k=missions.begin();k!=missions.end();k++) {
        int kills=k->second.second.first;
        if (max<kills)
            max=kills;
    }
    total_mission_count--;
    max_kills=max;
    ui->label_9->setText(QString::number(max_kills));
    if (max_kills!=0) {
        ui->label_10->setText(QString::number((double)total_kills/(double)max_kills, 'f', 10));
        ui->label_11->setText(QString::number(ui->label_8->text().toDouble()*1000000/max_kills, 'f', 10));
    } else {
        ui->label_10->setText("0");
        ui->label_11->setText("0");
    }
}*/

void MainWindow::on_pushButton_clicked()
{
    QFile::remove("./Systems/"+ui->treeWidget_3->topLevelItem(0)->text(0));
    GarbageCollector();
    if (ui->listWidget_2->count()>0) {
    ui->listWidget_2->setCurrentRow(0);
    on_listWidget_2_itemClicked(ui->listWidget_2->currentItem());
    } else {
        QDir directory(QDir::currentPath()+"/System/");
        QStringList configs = directory.entryList();
        ui->listWidget_2->addItems(configs);
        delete ui->listWidget_2->item(0);
        delete ui->listWidget_2->item(0);
    }
}

void MainWindow::on_pushButton_8_clicked()
{
    GarbageCollector();
    int row=0;
    if (ui->listWidget_2->currentRow()==0) {
        row=ui->listWidget_2->count()-1;
    } else {
        row=ui->listWidget_2->currentRow()-1;
    }
    ui->listWidget_2->setCurrentRow(row);
    on_listWidget_2_itemClicked(ui->listWidget_2->currentItem());
}

void MainWindow::on_pushButton_9_clicked()
{
    GarbageCollector();
    int row=0;
    if (ui->listWidget_2->currentRow()==ui->listWidget_2->count()-1) {
        row=0;
    } else {
        row=ui->listWidget_2->currentRow()+1;
    }
    ui->listWidget_2->setCurrentRow(row);
    on_listWidget_2_itemClicked(ui->listWidget_2->currentItem());
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

void MainWindow::on_save_session_clicked()
{
    on_pushButton_5_clicked();
}

void MainWindow::on_Save_Session_clicked()
{
    on_pushButton_5_clicked();
}
