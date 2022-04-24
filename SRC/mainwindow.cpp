#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QDebug>
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
    this->api=new API(this);
    connect(api,SIGNAL(Signal_Event(json)),this,SLOT(OnEvent(json)));
    this->Data=new techlevi::data(this);
    this->Data->Init(this->ui,this->api);
    connect(Data,SIGNAL(Refresh(GlobalFactions &, HuntedSystems &)),this,SLOT(completedData(GlobalFactions&,HuntedSystems&)));
    connect(Data,SIGNAL(UpdateTree()),this,SLOT(RefreshTree()));
    //current_station=json::parse("{ \"timestamp\":\"2021-02-14T23:26:06Z\", \"event\":\"Docked\", \"StationName\":\"Sabine Camp\", \"StationType\":\"Outpost\", \"StarSystem\":\"Cuberara\", \"SystemAddress\":2869440554457, \"MarketID\":3225893888, \"StationFaction\":{ \"Name\":\"Albardhas for Equality\", \"FactionState\":\"Election\" }, \"StationGovernment\":\"$government_Democracy;\", \"StationGovernment_Localised\":\"Democracy\", \"StationServices\":[ \"dock\", \"autodock\", \"blackmarket\", \"commodities\", \"contacts\", \"exploration\", \"missions\", \"refuel\", \"repair\", \"tuning\", \"engineer\", \"missionsgenerated\", \"flightcontroller\", \"stationoperations\", \"powerplay\", \"searchrescue\", \"stationMenu\" ], \"StationEconomy\":\"$economy_Extraction;\", \"StationEconomy_Localised\":\"Extraction\", \"StationEconomies\":[ { \"Name\":\"$economy_Extraction;\", \"Name_Localised\":\"Extraction\", \"Proportion\":0.830000 }, { \"Name\":\"$economy_Refinery;\", \"Name_Localised\":\"Refinery\", \"Proportion\":0.170000 } ], \"DistFromStarLS\":5.187345 }");
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





/*

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
    on_pushButton_5_clicked("./System/AutoBackup_"+ui->systemName->text()+".json");

}
*/
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





void MainWindow::CheckCurrentStation(huntedSystem *HuntedSystem, currentStation *currStat, set<faction*>::iterator faction,QTreeWidgetItem* item,bool do_not_search,int depth,QString Target_system) {
    /*if (ui->treeWidget_3->topLevelItemCount()==0) {
        addTreeItem();
        ui->treeWidget_3->topLevelItem(0)->setText(0,Target_system);
    }*/
    if (item==nullptr) {
        bool found=false;
        auto TreeItem=ui->treeWidget_3->topLevelItem(0);
        for (auto i=0;i<TreeItem->childCount();i++) {
            if (TreeItem->child(i)->text(0)==(*currStat->System)->name) {
                CheckCurrentStation(currStat,faction,TreeItem->child(i),false,1);
                found=true;
                break;
            }
        }
        if (!found) {
            addTreeItem();
            TreeItem->child(ui->treeWidget_3->topLevelItem(0)->childCount()-1)->setText(0,(*currStat->System)->name);
            ui->treeWidget_3->setCurrentItem(TreeItem->child(TreeItem->childCount()-1));
            HuntedSystem->add((*currStat->System));

            //systemStation.push_back({vector<QString>(),QString::fromStdString((string)current_station["StarSystem"])});
            CheckCurrentStation(currStat,faction,ui->treeWidget_3->topLevelItem(0)->child(ui->treeWidget_3->topLevelItem(0)->childCount()-1),true,1);
        }
    } else if (do_not_search && depth==1) {
        ui->treeWidget_3->setCurrentItem(item);
        addTreeItem();
        item->child(item->childCount()-1)->setText(0,currStat->name);
        HuntedSystem->find(currStat->System)
        /*for (auto z=systemStation.begin();z!=systemStation.end();z++) {
            if (z->second==QString::fromStdString((string)current_station["StarSystem"])) {
                z->first.push_back(QString::fromStdString((string)current_station["StationName"]));
            }
        }*/
        factionStation.push_back({vector<pair<QString,int>>(),QString::fromStdString((string)current_station["StationName"])});
        CheckCurrentStation(faction,item->child(item->childCount()-1),true,2);
    } else if (do_not_search && depth==2) {
        ui->treeWidget_3->setCurrentItem(item);
        addTreeItem();
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

void MainWindow::completedData(GlobalFactions & data,HuntedSystems const & targetSystem) {
    ui->Curr_payout->setText("0");
    for (auto system:targetSystem) {
        for (auto station:(*system)) {
            QTreeWidgetItem * temp=new QTreeWidgetItem();
            temp->setText(0,system->name+"::"+station->name);
            for (auto factionPlace:data) {
                faction* faction=(factionPlace);
                for (auto mission:factionPlace->missions) {
                    if ((*mission->sourceStation)->name==station->name && mission->Completed) {
                        QTreeWidgetItem * temp_1=new QTreeWidgetItem();
                        temp_1->setText(0,faction->name+"::     "+QString::number(mission->payout)+"$ K");
                        temp->addChild(temp_1);
                        ui->Curr_payout->setText(QString::number(ui->Curr_payout->text().toDouble()+mission->payout));
                    }
                }
            }
        }
    }
    ui->treeWidget->expandAll();
    ui->label_8->setText(QString::number(ui->label_8->text().toDouble()-rew_diff));
    unsigned max=data.reCalcStackHeight();
    ui->label_9->setText(QString::number(data.stackHeight));
    int temp_4=100*data.totalKillsSoFar/data.stackHeight;
    ui->kills_made->setText(QString::number(data.totalKillsSoFar)+" ("+QString::number(temp_4)+"%)");
    ui->kills_left->setText(QString::number(data.stackHeight-data.totalKillsSoFar)+" ("+QString::number(100-temp_4)+"%)");
    if (data.stackHeight!=0) {
        ui->label_10->setText(QString::number((double)data.totalKills/(double)data.stackHeight, 'f', 10));
        if (ui->label_10->text().toDouble()<2.0f) {
            ui->label_20->setText("Stop hunting and go back and turn in your missions");
        } else {
            ui->label_20->setText("You can continue the fun");
        }
        ui->label_11->setText((QString::number((ui->label_8->text().toDouble()*1000/data.stackHeight)/1000000, 'f', 10))+"$ M");
    } else {
        ui->label_10->setText("0");
        ui->label_11->setText("0");
        ui->label_8->setText("0");
    }
    ui->treeWidget->clear();
    faction->missions.erase(res.missionPlace);
}

void MainWindow::addTreeItem()
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

