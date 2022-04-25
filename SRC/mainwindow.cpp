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
    ui->horizontalSlider->setMaximum(0);
    this->api=new API(this);
    //connect(api,&API::Signal_Event,Data,&techlevi::data::OnEvent);
    this->Data=new techlevi::data(this);
    this->Data->Init(this->ui,this->api);
    connect(Data,&techlevi::data::Refresh,this,&MainWindow::completedData);
    connect(Data,&techlevi::data::UpdateTree,this,&MainWindow::RefreshTree);
    connect(Data,&techlevi::data::RefreshUI,this,&MainWindow::Refresh_UI);
    connect(Data,&techlevi::data::addTreeItem,this,&MainWindow::addTreeItem, Qt::DirectConnection);
    connect(api,&API::requestStatistics,Data,&techlevi::data::getUnifiedStatistics, Qt::DirectConnection);
    connect(api, &API::requestJson, Data, &techlevi::data::getJsonFormattedData, Qt::DirectConnection);
    connect(api, &API::provideJson, Data, &techlevi::data::LoadDataFromJson);
    //current_station=json::parse("{ \"timestamp\":\"2021-02-14T23:26:06Z\", \"event\":\"Docked\", \"StationName\":\"Sabine Camp\", \"StationType\":\"Outpost\", \"StarSystem\":\"Cuberara\", \"SystemAddress\":2869440554457, \"MarketID\":3225893888, \"StationFaction\":{ \"Name\":\"Albardhas for Equality\", \"FactionState\":\"Election\" }, \"StationGovernment\":\"$government_Democracy;\", \"StationGovernment_Localised\":\"Democracy\", \"StationServices\":[ \"dock\", \"autodock\", \"blackmarket\", \"commodities\", \"contacts\", \"exploration\", \"missions\", \"refuel\", \"repair\", \"tuning\", \"engineer\", \"missionsgenerated\", \"flightcontroller\", \"stationoperations\", \"powerplay\", \"searchrescue\", \"stationMenu\" ], \"StationEconomy\":\"$economy_Extraction;\", \"StationEconomy_Localised\":\"Extraction\", \"StationEconomies\":[ { \"Name\":\"$economy_Extraction;\", \"Name_Localised\":\"Extraction\", \"Proportion\":0.830000 }, { \"Name\":\"$economy_Refinery;\", \"Name_Localised\":\"Refinery\", \"Proportion\":0.170000 } ], \"DistFromStarLS\":5.187345 }");
    ui->kills_left->setText("0 (100%)");
    ui->kills_made->setText("0 (0%)");
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
    api->LoadData(c);
    ol=new Overlay(this);
    //on_listWidget_2_itemClicked(c);
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

void MainWindow::RebuildTree(huntedSystem *HuntedSystem, currentStation *currStat, set<faction*>::iterator faction,QTreeWidgetItem* item,bool do_not_search,int depth) {

    if (item==nullptr) {
        bool found=false;
        auto TreeItem=ui->treeWidget_3->topLevelItem(0);
        for (auto i=0;i<TreeItem->childCount();i++) {
            if (TreeItem->child(i)->text(0)==(*currStat->System)->name) {
                RebuildTree(HuntedSystem,currStat,faction,TreeItem->child(i),false,1);
                found=true;
                break;
            }
        }
        if (!found) {
            addTreeItem((*currStat->System)->name);
            RebuildTree(HuntedSystem,currStat,faction,ui->treeWidget_3->topLevelItem(0)->child(ui->treeWidget_3->topLevelItem(0)->childCount()-1),true,1);
        }
    } else if (do_not_search && depth==1) {
        ui->treeWidget_3->setCurrentItem(item);
        addTreeItem(currStat->name);
        RebuildTree(HuntedSystem,currStat,faction,item->child(item->childCount()-1),true,2);
    } else if (do_not_search && depth==2) {
        ui->treeWidget_3->setCurrentItem(item);
        addTreeItem("[0] "+(*faction)->name);
    } else if (!do_not_search && depth==1) {
        bool found=false;
        for (auto i=0;i<item->childCount();i++) {
            if (item->child(i)->text(0)==currStat->name) {
                RebuildTree(HuntedSystem,currStat,faction,item->child(i),false,2);
                found=true;
                break;
            }
        }
        if (!found) {
            RebuildTree(HuntedSystem,currStat,faction,item,true,1);
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
            if (temp==(*faction)->name) {
                found=true;
                break;
            }
        }
        if (!found) {
            RebuildTree(HuntedSystem,currStat,faction,item,true,2);
        }
    }
}

void MainWindow::RefreshTree(GlobalFactions const & GlobalFactions) {
    auto GetFactionName=[](QTreeWidgetItem* a)->QString {
        if (a->text(0).contains("] ")) {
            return a->text(0).section("] ",1,1);
        } else {
            return a->text(0);
        }
    };
    for (auto toplevel=0;toplevel<ui->treeWidget_3->topLevelItemCount();toplevel++) {
        for (auto i=0;i<ui->treeWidget_3->topLevelItem(toplevel)->childCount();i++) {
            for (auto k=0;k<ui->treeWidget_3->topLevelItem(toplevel)->child(i)->childCount();k++) {
                QString stationItem=ui->treeWidget_3->topLevelItem(toplevel)->child(i)->child(k)->text(0);
                for (auto j=0;j<ui->treeWidget_3->topLevelItem(toplevel)->child(i)->child(k)->childCount();j++) {
                    auto factionItem=ui->treeWidget_3->topLevelItem(toplevel)->child(i)->child(k)->child(j);
                    for (auto faction:GlobalFactions) {
                        if (faction->name==GetFactionName(factionItem)) {
                            factionItem->setText(0,"["+QString::number(faction->totalKillsNeeded)+"] "+faction->name);
                            break;
                        }
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

void MainWindow::completedData(GlobalFactions const & data,HuntedSystems const & CompleteData, bool deleted, mission const * m) {
    ui->Curr_payout->setText("0");
    for (auto huntedSystem:CompleteData) {
        for (auto system:*huntedSystem) {
            for (auto station:(*system)) {
                QTreeWidgetItem * temp=new QTreeWidgetItem();
                temp->setText(0,system->name+"::"+station->name);
                for (auto factionPlace:data) {
                    faction* faction=(factionPlace);
                    for (auto mission:(factionPlace->missions)) {
                        if ((mission->sourceStation)->name==station->name && mission->Completed) {
                            QTreeWidgetItem * temp_1=new QTreeWidgetItem();
                            temp_1->setText(0,faction->name+"::     "+QString::number(mission->payout)+"$ K");
                            temp->addChild(temp_1);
                            ui->Curr_payout->setText(QString::number(ui->Curr_payout->text().toDouble()+mission->payout));
                        }
                    }
                }
            }
        }
    }
    Refresh_UI(true,data);
    if (deleted) {
        ui->label_8->setText(QString::number(ui->label_8->text().toDouble()-m->payout));
    }
}

void MainWindow::Refresh_UI(bool switcher,GlobalFactions const & data) {
    auto Stats=new Statistics();
    Data->getUnifiedStatistics(Stats);
    ui->horizontalSlider->setMaximum(data.stackHeight);
    ui->label_8->setText(QString::number(Stats->totalPayout));
    ui->Curr_payout->setText(QString::number(Stats->currentPayout));
    ui->treeWidget->expandAll();
    ui->label_9->setText(QString::number(data.stackHeight));
    int temp_4=100*data.totalKillsSoFar/data.stackHeight;
    ui->kills_made->setText(QString::number(data.totalKillsSoFar)+" ("+QString::number(temp_4)+"%)");
    ui->kills_left->setText(QString::number(data.stackHeight-data.totalKillsSoFar)+" ("+QString::number(100-temp_4)+"%)");
    if (data.stackHeight!=0) {
        double res=(double)data.totalKills/(double)data.stackHeight;
        ui->label_10->setText(QString::number(res, 'f', 10));
        if (switcher) {
            if (ui->label_10->text().toDouble()<2.0f) {
                ui->label_20->setText("Stop hunting and go back and turn in your missions");
            } else {
                ui->label_20->setText("You can continue the fun");
            }
        } else {
            if (ui->label_10->text().toDouble()<3.0f) {
                ui->label_20->setText("Pick up some more missions to complete");
            } else {
                ui->label_20->setText("You can go and kill some pirate scum");
            }
        }
        res=(ui->label_8->text().toDouble()*1000/data.stackHeight)/1000000;
        ui->label_11->setText((QString::number(res, 'f', 10))+"$ M");
    } else {
        ui->label_10->setText("0");
        ui->label_11->setText("0");
        ui->label_8->setText("0");
    }
    ui->treeWidget->clear();
}

void MainWindow::addTreeItem(QString name)
{
    if (ui->treeWidget_3->currentItem()==nullptr) {
        QTreeWidgetItem *temp=new QTreeWidgetItem();
        temp->setText(0,name);
        temp->setFlags(Qt::ItemIsEnabled);
        ui->treeWidget_3->addTopLevelItem(temp);
        ui->treeWidget_3->setCurrentItem(temp);
    } else {
        QTreeWidgetItem *temp=new QTreeWidgetItem();
        temp->setText(0,name);
        temp->setFlags(Qt::ItemIsEnabled);
        ui->treeWidget_3->currentItem()->addChild(temp);
        ui->treeWidget_3->setCurrentItem(temp);
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
    try {
        api->SaveData(fileName);
    }  catch (...) {
        QMessageBox::warning(this,tr("Error while trying to save session"),tr("Could not open given file"));
    }
}

void MainWindow::on_actionLoad_session_triggered()
{
    on_actionStart_new_session_triggered();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select file to Load"), "./System", tr("Save Files (*.json)"));
    api->LoadData(fileName);
}

void MainWindow::on_actionStart_new_session_triggered()
{
    /*
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
    */
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    int theor_completed=0;
    double theor_rew=0;
    int all_missions=0;
    double all_reward=0;
    for (auto faction:Data->getFactions()) {
        int TheorKilled=value;
        for (auto mission:(faction->missions)) {
            if (TheorKilled>=mission->overallKillsNeeded) {
                TheorKilled=TheorKilled-mission->overallKillsNeeded;
                theor_completed++;
                theor_rew+=mission->payout;
            }
            all_missions++;
            all_reward+=mission->payout;
        }
    }
    ui->Reward->setText(QString::number(theor_rew)+" ("+QString::number(theor_rew*100/all_reward)+"%)");
    auto number=((double)theor_completed/(all_missions==0?1:(double)all_missions))*100;
    ui->max_theoretycal->setText(QString::number(theor_completed)+" ("+QString::number(number)+"%)");
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
    QString a="====================================\n";
    auto res=new Statistics();
    Data->getUnifiedStatistics(res);
    int total_mission_count=res->totalNumberOfMissions-res->completedMissions;
    a+="```Total payout "+QString::number((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number((ui->Missions_completed->text().toInt()+total_mission_count))+" Missions \n";
    if (ui->Missions_completed->text().toInt()>0)
        a+="Completed: "+QString::number((ui->Curr_payout->text().toDouble())/1000)+"$ Mill for "+ui->Missions_completed->text()+" Mission(s) \n";
    if (total_mission_count>0)
        a+="Active: "+QString::number((ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number(total_mission_count)+" Mission(s) \n";
    a+="Stack Height: "+QString::number(Data->getStackHeight())+" Stack Width: "+QString::number(Data->getStackWidth())+"\n";
    a+="Current Ratio: "+ui->label_10->text()+" Mission average payout: "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)(ui->Missions_completed->text().toInt()+total_mission_count))+"$ Mill \nReward per kill (Bounty not included): "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)Data->getStackHeight())+"$ Mill\n";
    a+="Mission target system: "+ui->systemName->text()+"\n";
    a+="LFW to Complete/Turn in```\n";
    a+="====================================";
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(a);
}

