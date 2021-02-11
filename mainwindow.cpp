﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDir"
#include <QFile>
#include <QTextStream>
#include <QDebug>


using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GarbageCollector() {
    config.clear();
    total_kills=0;
    max_kills=0;
    ratio=0;
    total_mission_count=0;
    ui->treeWidget_3->clear();
    ui->systems->clear();
    ui->stations->clear();
    ui->factions->clear();
    ui->missions->clear();
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
        }
        ui->factions->setCurrentRow(0);
        ui->factions->currentItem()->setBackground(QBrush(QColor(0,100,255)));
        auto fact=config.at(ui->systemName->text().toStdString()).at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString()).at(ui->factions->currentItem()->text().toStdString());
        QTreeWidgetItem *temp_temp;
        for (auto i=fact.begin();i!=fact.end();i++) {
            QTreeWidgetItem *temp=new QTreeWidgetItem();
            if (i==fact.begin()) {
                temp_temp=temp;
            }
            temp->setText(0,QString::fromStdString((string)i.value()).section(";",0,0));
            temp->setText(1,QString::fromStdString((string)i.value()).section(";",1,1));
            ui->missions->addTopLevelItem(temp);
        }
        ui->missions->setCurrentItem(temp_temp);
        ui->missions->topLevelItem(0)->setBackground(0,QBrush(QColor(0,100,255)));
        ui->missions->topLevelItem(0)->setBackground(1,QBrush(QColor(0,100,255)));
        int max=0;
        for (auto k=missions.begin();k!=missions.end();k++) {
            total_mission_count+=k->second.first.size();
            if (max<k->second.second.first)
                max=k->second.second.first;
            total_kills+=k->second.second.first;
            ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+k->second.second.second));
        }
        max_kills=max;
        ui->label_9->setText(QString::number(max_kills));
        ui->label_10->setText(QString::number(total_kills/max_kills, 'f', 10));
        ui->label_11->setText(QString::number(ui->label_8->text().toDouble()*1000000/max_kills, 'f', 10));
    }
}

void MainWindow::refreshdata(int depth,json *a) {
    if (a==nullptr)
        a=&config;
    QString key;
    json b;
    for (auto i=a->begin();i!=a->end();i++) {

        key=QString::fromStdString((string)i.key());
        qDebug()<<key;
        if (depth==0) {
            on_pushButton_3_clicked();
            ui->treeWidget_3->currentItem()->setText(0,key);
        } else {
            on_pushButton_3_clicked();
            ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->child(ui->treeWidget_3->currentItem()->childCount()-1));
            ui->treeWidget_3->currentItem()->setText(0,key);
        }
        if (depth!=3) {
            b=i.value();
            refreshdata(depth+1,&b);
        } else {
            b=i.value();
            vector<pair<unsigned,pair<int,double>>> temp;
            unsigned j=0;
            int kill=0;
            double rew=0;
            for (auto k=b.begin();k!=b.end();k++) {
                QString miss=QString::fromStdString((string)k.value());
                temp.push_back({j,{miss.section(";",0,0).toInt(),miss.section(";",1,1).toDouble()}});
                kill+=temp[j].second.first;
                rew+=temp[j].second.second;
                j++;
            }
            missions.insert({QString::fromStdString((string)i.key()),{temp,{kill,rew}}});
        }
        ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->parent());
    }

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
        b+="}";
        qDebug()<<b;
        stream<<b;
        QDir directory(QDir::currentPath()+"/System/");
        QStringList configs = directory.entryList();
        ui->listWidget_2->addItems(configs);
        for (auto i=0;i!=configs.size();i++) {
            if (configs[i].toStdString()==item->text(0).toStdString()) {
                ui->listWidget_2->setCurrentRow(i);
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
            auto result=missions.find(item->child(i)->text(0));
            if (result==missions.end()) {
                a+="\""+QString::number(i).toStdString()+"\":\""+
                        "0"
                        +";"+
                        "0\"";
            } else {
                for (unsigned i=0;i<result->second.first.size();i++) {
                    a+="\""+QString::number(i).toStdString()+"\":\""+
                            QString::number(result->second.first.at(i).second.first).toStdString()
                            +";"+
                            QString::number(result->second.first.at(i).second.second).toStdString()
                            +"\"";
                    if (i!=result->second.first.size()-1) {
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
        if (missions.find(a->text(0))!=missions.end())
            missions.erase(missions.find(a->text(0)));
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
    ui->missions->clear();
    auto a=missions.find(ui->factions->currentItem()->text());
    for (auto i=a->second.first.begin();i!=a->second.first.end();i++) {
        QTreeWidgetItem *temp=new QTreeWidgetItem();
        temp->setText(0,QString::number(i->second.first));
        temp->setText(1,QString::number(i->second.second));
        ui->missions->addTopLevelItem(temp);
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if (column>-1) {
        for (int i=0;i<ui->missions->topLevelItemCount();i++) {
            ui->missions->topLevelItem(i)->setBackground(0,QBrush(QColor(255,255,255)));
            ui->missions->topLevelItem(i)->setBackground(1,QBrush(QColor(255,255,255)));
        }
        item->setBackground(0,QBrush(QColor(0,100,255)));
        item->setBackground(1,QBrush(QColor(0,100,255)));
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    auto result=missions.find(ui->factions->currentItem()->text());
    QTreeWidgetItem *temp=new QTreeWidgetItem();
    temp->setText(0,QString::number(ui->spinBox->value()));
    temp->setText(1,QString::number((double)ui->doubleSpinBox->value()));
    ui->missions->addTopLevelItem(temp);
    if (result==missions.end()) {
        vector<pair<unsigned,pair<int,double>>> temp;
        int kill=0;
        double rew=0;
        for (auto i=0;i<ui->missions->topLevelItemCount();i++) {
            temp.push_back({i,{ui->missions->topLevelItem(i)->text(0).toInt(),ui->missions->topLevelItem(i)->text(1).toDouble()}});
            kill+=ui->missions->topLevelItem(i)->text(0).toInt();
            rew+=ui->missions->topLevelItem(i)->text(1).toDouble();
        }
        total_kills+=kill;
        missions.insert({ui->factions->currentItem()->text(),{temp,{kill,rew}}});
        ui->label_8->setText(QString::number(ui->label_8->text().toDouble()+rew));
    } else {
        int kill=ui->spinBox->value();
        double rew=ui->doubleSpinBox->value();
        result->second.first.push_back({result->second.first.size(),{ui->missions->topLevelItem(ui->missions->topLevelItemCount()-1)->text(0).toInt(),ui->missions->topLevelItem(ui->missions->topLevelItemCount()-1)->text(1).toDouble()}});
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
}

void MainWindow::on_pushButton_7_clicked()
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
}

void MainWindow::on_pushButton_2_clicked()
{
    GarbageCollector();
    ui->listWidget_2->addItem("new.json");
    ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_clicked()
{
    QFile::remove("./Systems/"+ui->treeWidget_3->topLevelItem(0)->text(0));
    GarbageCollector();
    ui->listWidget_2->setCurrentRow(0);
    on_listWidget_2_itemClicked(ui->listWidget_2->currentItem());
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
