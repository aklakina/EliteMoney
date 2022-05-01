#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QClipboard>

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
    ui->kills_left->setText("0 (100%)");
    ui->kills_made->setText("0 (0%)");
    ui->treeWidget_3->setCurrentItem(nullptr);


    Data=new techlevi::data();
    Data->Init(this->ui);
    io=new techlevi::IO();
    ev=new EventDistributor(this,io,Data);
    wing=new QIcon(":/icons/Resources/Logos/Wing.svg");

    QPixmap pixmap = wing->pixmap(QSize(20,20));
    QImage tmp = pixmap.toImage();
    QColor color("#0A8BD6");
    for(int y = 0; y < tmp.height(); y++) {
        for(int x= 0; x < tmp.width(); x++) {
            color.setAlpha(tmp.pixelColor(x,y).alpha());
            tmp.setPixelColor(x,y,color);
        }
    }
    pixmap = QPixmap::fromImage(tmp);
    delete wing;
    wing=new QIcon(pixmap);

    int id;
    QString family;
    id = QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/EUROCAPS.TTF");
    family = QFontDatabase::applicationFontFamilies(id).at(0);
    Header = new QFont(family);

    id = QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/Sintony-Regular.ttf");
    family = QFontDatabase::applicationFontFamilies(id).at(0);
    General = new QFont(family);

    id = QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/Gugi-Regular.ttf");
    family = QFontDatabase::applicationFontFamilies(id).at(0);
    Tree = new QFont(family);

    ol=new Overlay(this);
    LoadOverlayData();

    ui->systemName->setFont(*Header);
    ui->tableWidget->setFont(*General);
    this->setStyleSheet(this->styleSheet()+"\n font: 10pt "+General->family()+";");
    QString MainWindowStyle="color: rgb(255, 113, 0);"
            "background-color: rgba(43, 34, 34, 100);"
            "selection-background-color: rgba(10, 139, 214, 120);"
            "QPushButton::Hovered {"
                "border-color: rgb(10, 139, 214);"
            "};";
    setStyleSheet(MainWindowStyle);
    ui->treeWidget_3->setStyleSheet(ui->treeWidget_3->styleSheet()+"\n font: 10pt "+Tree->family()+";");
    ui->systemName->setStyleSheet(ui->systemName->styleSheet()+"\n font: 10pt "+Tree->family()+";");
    ol->setStyleSheet("background-color: transparent; font: 9pt "+General->family()+";");
    setWindowIcon(QIcon(":/icons/Resources/Logos/elite-dangerous-vektor.svg"));
    io->init();
}

void MainWindow::RefreshTable(GlobalFactions const & data) {
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(data.getSize()+1);
    int curr_row=0;
    int curr_col=0;
    vector<int> frags;
    for (auto factions:data) {
        int size=0;
        for (auto mission:factions->getMissionsbyDate()) {
            if (!mission->Completed) size++;
        }
        if (ui->tableWidget->columnCount()<size+2) {
            ui->tableWidget->setColumnCount(size+2);
        }
    }
    for (unsigned k=0;k<data.getSize()+1;k++) {
        for (unsigned j=ui->tableWidget->columnCount();j>0;j--) {
            ui->tableWidget->setItem(k,j,new QTableWidgetItem(""));
        }
    }
    for (auto col=0;col<ui->tableWidget->columnCount();col++) {
        if (col==0) {
            ui->tableWidget->setItem(col,0,new QTableWidgetItem("Factions"));
        } else if (col<ui->tableWidget->columnCount()-1) {
            ui->tableWidget->item(0,col)->setText("Mission "+QString::number(col));
        } else if (col==ui->tableWidget->columnCount()-1) {
            ui->tableWidget->item(0,col)->setText("Stack height");
        }
    }
    curr_row++;
    for (auto faction:data) {
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
                if (Mission->Winged) {
                    ui->tableWidget->item(curr_row,curr_col+1)->setIcon(*wing);
                }
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
    curr_row=1;
    for (auto i:frags) {
        ui->tableWidget->item(curr_row,ui->tableWidget->columnCount()-1)->setText(QString::number(i));
        curr_row++;
    }
    applyTableStyle();
    ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::ExpandTree(QString name)
{
    ui->treeWidget_3->collapseAll();
    for (auto topItemIndex=0;topItemIndex<ui->treeWidget_3->topLevelItemCount();topItemIndex++) {
        if (ui->treeWidget_3->topLevelItem(topItemIndex)->text(0)==name) {
            ExpandChildsToo(ui->treeWidget_3->topLevelItem(topItemIndex));
            ui->treeWidget_3->expandItem(ui->treeWidget_3->topLevelItem(topItemIndex));
            break;
        } else {
            if (SearchTreeRecursively(ui->treeWidget_3->topLevelItem(topItemIndex),name)) {
                ExpandChildsToo(ui->treeWidget_3->topLevelItem(topItemIndex));
                ui->treeWidget_3->expandItem(ui->treeWidget_3->topLevelItem(topItemIndex));
            }
        }
    }
}

void MainWindow::buildStationMissionData(const vector<mission *> &data)
{
    ui->Curr_payout->setText("0");
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(data.size()+1);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("Faction"));
    ui->tableWidget->setItem(0,1,new QTableWidgetItem("Kills"));
    ui->tableWidget->setItem(0,2,new QTableWidgetItem("Payout"));
    unsigned curr_row=1;
    for (auto mission:data) {
        ui->tableWidget->setItem(curr_row,0,new QTableWidgetItem(mission->sourceFaction->name));
        ui->tableWidget->setItem(curr_row,1,new QTableWidgetItem(QString::number(mission->overallKillsNeeded)));
        ui->tableWidget->setItem(curr_row,2,new QTableWidgetItem(QString::number(mission->payout)));
        ui->Curr_payout->setText(QString::number(ui->Curr_payout->text().toDouble()+mission->payout));
        curr_row++;
    }
    applyTableStyle();
    ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::buildCompletedMissionData(const vector<mission *> &data)
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(5);
    map<pair<System*,station*>,pair<pair<unsigned,unsigned>,double>> DistinctStationData;

    unsigned curr_row=1;
    for (auto mission:data) {
        auto res=DistinctStationData.insert({{mission->sourceSystem,mission->sourceStation},{{0,0},0}});
        res.first->second.first.first++;
        res.first->second.first.second+= mission->Winged ? 1 : 0;
        res.first->second.second+=mission->payout;
    }
    ui->tableWidget->setRowCount(DistinctStationData.size()+1);
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("System"));
    ui->tableWidget->setItem(0,1,new QTableWidgetItem("Station"));
    ui->tableWidget->setItem(0,2,new QTableWidgetItem("Number of missions"));
    ui->tableWidget->setItem(0,3,new QTableWidgetItem("Shareable missions"));
    ui->tableWidget->setItem(0,4,new QTableWidgetItem("Total Payout"));
    for (auto station:DistinctStationData) {
        ui->tableWidget->setItem(curr_row,0,new QTableWidgetItem(station.first.first->name));
        ui->tableWidget->setItem(curr_row,1,new QTableWidgetItem(station.first.second->name));
        ui->tableWidget->setItem(curr_row,2,new QTableWidgetItem(QString::number(station.second.first.first)));
        ui->tableWidget->setItem(curr_row,3,new QTableWidgetItem(QString::number(station.second.first.second)));
        ui->tableWidget->item(curr_row,3)->setIcon(*wing);
        ui->tableWidget->setItem(curr_row,4,new QTableWidgetItem(QString::number(station.second.second)));
    }
    applyTableStyle();
    ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::resetTreeColor() {
    for (auto i=0;i<ui->tableWidget->rowCount();i++) {
        for (auto j=0;j<ui->tableWidget->columnCount();j++) {
            ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(255,255,255)));
        }
    }
}

bool MainWindow::SearchTreeRecursively(QTreeWidgetItem *item, QString name)
{
    for (auto index=0;index<item->childCount();index++) {
        if (item->child(index)->text(0)==name) return true;
    }
    return false;
}

void MainWindow::ExpandChildsToo(QTreeWidgetItem *item)
{
    for (auto index=0;index<item->childCount();index++) {
        item->child(index)->setExpanded(true);
        ExpandChildsToo(item->child(index));
    }
}

void MainWindow::RetractAll(QTreeWidgetItem *item)
{
    for (auto index=0;index<item->childCount();index++) {
        ExpandChildsToo(item->child(index));
        item->child(index)->setExpanded(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit SaveData(QDir::currentPath()+"/System/AutoBackup_"+ui->systemName->text().replace(' ','_')+".json");
    emit SaveUIConfig(ol->GetUserConfig());
    event->accept();
}

void MainWindow::LoadOverlayData()
{
    json olConf;
    emit getOverlayData(&olConf);
    json CombatStatData=olConf["Combat Statistic config"];
    ol->CombatStatProperties->setPos(CombatStatData["Px"],CombatStatData["Py"],CombatStatData["Dx"],CombatStatData["Dy"],CombatStatData["ODy"]);
}

void MainWindow::applyTableStyle()
{
    for (auto col=0;col<ui->tableWidget->columnCount();col++) {
        ui->tableWidget->item(0,col)->setFont(*Header);
        for (auto row=1;row<ui->tableWidget->rowCount();row++) {
            ui->tableWidget->item(row,col)->setFont(*General);
        }
    }
}

void MainWindow::RebuildTree(AdvancedContainer<ContainerObject> *CompleteData, QTreeWidgetItem* item,bool do_not_search,int depth) {
    if (depth>3) return;
    auto GetFactionName=[](QTreeWidgetItem* a)->QString {
        if (a->text(0).contains("] ")) {
            return a->text(0).section("] ",1,1);
        } else {
            return a->text(0);
        }
    };
    auto goDeeper=[=](ContainerObject* ThingToSearch,QTreeWidgetItem* parent,Ui::MainWindow *ui) -> pair<QTreeWidgetItem*,bool> {
        auto GetFactionName=[](QTreeWidgetItem* a)->QString {
            if (a->text(0).contains("] ")) {
                return a->text(0).section("] ",1,1);
            } else {
                return a->text(0);
            }
        };
        bool found=false;
        QTreeWidgetItem* _ret;
        if (parent==nullptr) {
            for (auto topItemIndex=0;topItemIndex<ui->treeWidget_3->topLevelItemCount();topItemIndex++ ) {
                if (ThingToSearch->name==GetFactionName(ui->treeWidget_3->topLevelItem(topItemIndex))) {
                    found=true;
                    _ret=ui->treeWidget_3->topLevelItem(topItemIndex);
                    break;
                }
            }
            if (!found) {
                QTreeWidgetItem * temp=new QTreeWidgetItem();
                temp->setText(0,ThingToSearch->name);
                ui->treeWidget_3->addTopLevelItem(temp);
                _ret=temp;
            }
        } else {
            for (auto childItemIndex=0;childItemIndex<parent->childCount();childItemIndex++ ) {
                if (ThingToSearch->name==GetFactionName(parent->child(childItemIndex))) {
                    found=true;
                    _ret=parent->child(childItemIndex);
                    break;
                }
            }
            if (!found) {
                QTreeWidgetItem * temp=new QTreeWidgetItem();
                temp->setText(0,ThingToSearch->name);
                parent->addChild(temp);
                _ret=temp;
            }
        }
        return {_ret,found};
    };
    if (!do_not_search && depth<3) {
        for (auto ChildContainer:*CompleteData) {
            //QTreeWidgetItem *temp=item;
            auto Exists=goDeeper(ChildContainer,item,this->ui);
            RebuildTree((AdvancedContainer<ContainerObject>*)ChildContainer,Exists.first,!Exists.second,depth+1);
        }
    } else if (!do_not_search && depth==3) {
        for (faction * Faction: (*(AdvancedContainer<faction>*)CompleteData)) {
            //QTreeWidgetItem * temp=item;
            auto Exists=goDeeper(Faction,item,this->ui);
            unsigned frags=Faction->reCalcStackHeight();
            Exists.first->setText(0,"["+QString::number(frags)+"] "+GetFactionName(Exists.first));
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
    delete ui;

}

void MainWindow::Refresh_UI(bool switcher) {
    auto Stats=new Statistics();
    emit requestUnifiedStatistics(Stats);
    huntedSystem* Session=nullptr;
    emit requestSession(Session);
    ui->horizontalSlider->setMaximum(Stats->StackHeight);
    ui->label_8->setText(QString::number(Stats->totalPayout));
    ui->Curr_payout->setText(QString::number(Stats->currentPayout));
    ui->label_9->setText(QString::number(Stats->StackHeight));
    if (Stats->StackHeight!=0) {
        int temp_4=100*Stats->killsSoFar/Stats->StackHeight;
        ui->kills_made->setText(QString::number(Stats->killsSoFar)+" ("+QString::number(temp_4)+"%)");
        ui->kills_left->setText(QString::number(Stats->StackHeight-Stats->killsSoFar)+" ("+QString::number(100-temp_4)+"%)");
        double res=(double)Stats->totalKillsNeeded/(double)Stats->StackHeight;
        ui->label_10->setText(QString::number(res, 'f', 3));
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
        res=(ui->label_8->text().toDouble()*1000/Stats->StackHeight)/1000000;
        ui->label_11->setText((QString::number(res, 'f', 3))+"$ M");
    } else {
        ui->label_10->setText("0");
        ui->label_11->setText("0");
        ui->label_8->setText("0");
    }
    ui->systemName->setText(Session==nullptr?"SYSTEM":Session->name);
    ol->CombatStatProperties->setText(0,0,QString::number(Stats->StackHeight-Stats->killsSoFar));
    ol->CombatStatProperties->setText(1,0,QString::number(Stats->currentPayout));
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
    ui->max_theoretycal->setText(QString::number(Stats.TheoreticallyCompletedMissions)+" ("+QString::number(number)+"%)");
    ui->horizontalSlider->setToolTip(QString::number(value));
    ui->Theor_num_kill->setText(QString::number(value)+" ("+QString::number(value*100/ui->horizontalSlider->maximum())+"%)");
}

void MainWindow::on_Copy_data_clicked()
{
    QString a="====================================\n";
    map<huntedSystem*,Statistics> *res1=nullptr;
    emit requestStatistics(res1);
    huntedSystem* Session=nullptr;
    emit requestSession(Session);
    auto res=res1->at(Session);
    a+="```Total payout "+QString::number(res.currentPayout/1000)+"$ Mill for "+QString::number(res.completedMissions)+" Missions \n";
    a+=Session->name+"\n";
    a+="LFW to turn in```\n";
    a+="====================================";
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(a);
}

void MainWindow::on_pushButton_clicked()
{
    QString a="====================================\n";
    map<huntedSystem*,Statistics> *res1=nullptr;
    emit requestStatistics(res1);
    huntedSystem* Session=nullptr;
    emit requestSession(Session);
    auto res=res1->at(Session);
    int total_mission_count=res.totalNumberOfMissions-res.completedMissions;
    a+="```Total payout "+QString::number((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number((ui->Missions_completed->text().toInt()+total_mission_count))+" Missions \n";
    if (ui->Missions_completed->text().toInt()>0)
        a+="Completed: "+QString::number((ui->Curr_payout->text().toDouble())/1000)+"$ Mill for "+ui->Missions_completed->text()+" Mission(s) \n";
    if (total_mission_count>0)
        a+="Active: "+QString::number((ui->label_8->text().toDouble())/1000)+"$ Mill for "+QString::number(total_mission_count)+" Mission(s) \n";
    a+="Stack Height: "+QString::number(res.StackHeight)+" Stack Width: "+QString::number(res.StackWidth)+"\n";
    a+="Current Ratio: "+ui->label_10->text()+" Mission average payout: "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)(ui->Missions_completed->text().toInt()+total_mission_count))+"$ Mill \nReward per kill (Bounty not included): "+QString::number(((ui->Curr_payout->text().toDouble()+ui->label_8->text().toDouble())/1000)/(double)res.StackHeight)+"$ Mill\n";
    a+="Mission target system: "+Session->name+"\n";
    a+="LFW to Complete/Turn in```\n";
    a+="====================================";
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(a);
}

void MainWindow::on_actionEdit_Overlay_triggered()
{
    olEdit=new OverlayEditor(ol);
    olEdit->show();
}

