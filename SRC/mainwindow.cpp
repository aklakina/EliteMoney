#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QTextStream>
#include <QDebug>
//#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QClipboard>
//#include <QApplication>

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
    api=new techlevi::API();
    Data=new techlevi::data();
    Data->Init(this->ui,api);
    ev=new EventDistributor(this,api,Data);
    ui->kills_left->setText("0 (100%)");
    ui->kills_made->setText("0 (0%)");
    ui->treeWidget_3->setCurrentItem(nullptr);
    ol=new Overlay(this);
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

void MainWindow::RebuildTree(AdvancedContainer<ContainerObject> *CompleteData, QTreeWidgetItem* item,bool do_not_search,int depth) {
    if (depth>3) return;
    auto goDeeper=[&](ContainerObject* ThingToSearch,QTreeWidgetItem* parent,Ui::MainWindow *ui) -> bool {
        auto GetFactionName=[](QTreeWidgetItem* a)->QString {
            if (a->text(0).contains("] ")) {
                return a->text(0).section("] ",1,1);
            } else {
                return a->text(0);
            }
        };
        bool found=false;
        if (parent==nullptr) {
            for (auto topItemIndex=0;topItemIndex<ui->treeWidget_3->topLevelItemCount();topItemIndex++ ) {
                if (ThingToSearch->name==GetFactionName(ui->treeWidget_3->topLevelItem(topItemIndex))) {
                    found=true;
                    parent=ui->treeWidget_3->topLevelItem(topItemIndex);
                    break;
                }
            }
            if (!found) {
                QTreeWidgetItem * temp=new QTreeWidgetItem();
                temp->setText(0,ThingToSearch->name);
                ui->treeWidget_3->addTopLevelItem(temp);

            }
        } else {
            for (auto childItemIndex=0;childItemIndex<parent->childCount();childItemIndex++ ) {
                if (ThingToSearch->name==GetFactionName(parent->child(childItemIndex))) {
                    found=true;
                    parent=parent->child(childItemIndex);
                    break;
                }
            }
            if (!found) {
                QTreeWidgetItem * temp=new QTreeWidgetItem();
                temp->setText(0,ThingToSearch->name);
                parent->addChild(temp);
            }
        }
        return found;
    };
    if (!do_not_search && depth<3) {
        for (auto ChildContainer:*CompleteData) {
            QTreeWidgetItem *temp=item;
            bool Exists=goDeeper(ChildContainer,temp,this->ui);
            RebuildTree((AdvancedContainer<ContainerObject>*)ChildContainer,temp,!Exists,depth+1);
        }
    } else if (depth==3 && !do_not_search) {
        for (faction * Faction: (*(AdvancedContainer<faction>*)CompleteData)) {
            QTreeWidgetItem * temp=item;
            goDeeper(Faction,temp,this->ui);
            unsigned frags=Faction->reCalcStackHeight();
            temp->setText(0,"["+QString::number(frags)+"] "+temp->text(0));
        }
    } else if (do_not_search && depth<4) {
        for (auto childItem:*CompleteData) {
            QTreeWidgetItem * temp=new QTreeWidgetItem();
            temp->setText(0,childItem->name);
            item->addChild(temp);
            if (depth==3) {
                auto fact=(faction*)childItem;
                unsigned frags=fact->reCalcStackHeight();
                temp->setText(0,"["+QString::number(frags)+"] "+temp->text(0));
            }
            RebuildTree((AdvancedContainer<ContainerObject>*)childItem,item->child(item->childCount()-1),true,depth+1);
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

    emit SaveData("./System/AutoBackup_"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss")+".json");
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
                    for (auto mission:(factionPlace->getMissionsbyID())) {
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
    emit requestUnifiedStatistics(Stats);
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
        emit SaveData(fileName);
    }  catch (...) {
        QMessageBox::warning(this,tr("Error while trying to save session"),tr("Could not open given file"));
    }
}

void MainWindow::on_actionLoad_session_triggered()
{
    on_actionStart_new_session_triggered();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select file to Load"), "./System", tr("Save Files (*.json)"));
    emit LoadData(fileName);
}

void MainWindow::on_actionStart_new_session_triggered()
{
    //TODO
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    TheoreticalResults Stats;
    emit requestTheorData(value,Stats);
    ui->Reward->setText(QString::number(Stats.TheoreticallyGainedPayout)+" ("+QString::number(Stats.getPayoutPercentage())+"%)");
    auto number=Stats.getMissionPercentage();
    ui->max_theoretycal->setText(QString::number(Stats.allMissions)+" ("+QString::number(number)+"%)");
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
    emit requestUnifiedStatistics(res);
    int total_mission_count=res->totalNumberOfMissions-res->completedMissions;
    a+="```Total payout "+QString::number((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number((ui->Missions_completed->text().toInt()+total_mission_count))+" Missions \n";
    if (ui->Missions_completed->text().toInt()>0)
        a+="Completed: "+QString::number((ui->Curr_payout->text().toDouble())/1000)+"$ Mill for "+ui->Missions_completed->text()+" Mission(s) \n";
    if (total_mission_count>0)
        a+="Active: "+QString::number((ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number(total_mission_count)+" Mission(s) \n";
    a+="Stack Height: "+QString::number(res->StackHeight)+" Stack Width: "+QString::number(res->StackWidth)+"\n";
    a+="Current Ratio: "+ui->label_10->text()+" Mission average payout: "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)(ui->Missions_completed->text().toInt()+total_mission_count))+"$ Mill \nReward per kill (Bounty not included): "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)res->StackHeight)+"$ Mill\n";
    a+="Mission target system: "+ui->systemName->text()+"\n";
    a+="LFW to Complete/Turn in```\n";
    a+="====================================";
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(a);
}

