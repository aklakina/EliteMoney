#include "mainwindow.h"
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
    ui->missions->setColumnCount(2);
    QStringList a;
    a.push_back("Kills");
    a.push_back("Reward (Mill)");
    ui->missions->setHeaderLabels(a);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
    if (item->text()==".." || item->text()==".") {
        return;
    }
    config.clear();
    ui->treeWidget_3->clear();
    ui->systems->clear();
    ui->stations->clear();
    ui->missions->clear();
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
            temp->setText(0,QString::fromStdString((string)i.key()));
            temp->setText(1,QString::number((int)i.value()));
            ui->missions->addTopLevelItem(temp);
        }
        ui->missions->setCurrentItem(temp_temp);
        ui->missions->topLevelItem(0)->setBackground(0,QBrush(QColor(0,100,255)));
        ui->missions->topLevelItem(0)->setBackground(1,QBrush(QColor(0,100,255)));
    }
}

void MainWindow::jsonreader() {

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
            ui->treeWidget_3->setCurrentItem(ui->treeWidget_3->currentItem()->parent());
        } else {

        }
    }

}

void MainWindow::on_listWidget_3_itemClicked(QListWidgetItem *item)
{

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
        json j=a;
        stream<<b;
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
        if (i!=item->childCount()-1 || item->child(i)->childCount()!=0) {
            configreader(item->child(i),a);
            if (i!=item->childCount()-1)
                a+=",";
        } else {
            a+="{\"0\":0}";
        }
    }
    a+="}";
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->treeWidget_3->currentItem()->parent()->removeChild(ui->treeWidget_3->currentItem());
}

void MainWindow::on_systems_itemClicked(QListWidgetItem *item)
{
    if (item!=ui->systems->currentItem()) {
        for (auto i=0;i<ui->systems->count();i++) {
            ui->systems->item(i)->setBackground(QBrush(QColor(255,255,255)));
        }
        item->setBackground(QBrush(QColor(0,0,255)));
        ui->stations->clear();
        ui->factions->clear();
        ui->missions->clear();
        json a=config.at(ui->systemName->text().toStdString()).at(item->text().toStdString());
        for (auto i=a.begin();i!=a.end();i++) {
            ui->stations->addItem(QString::fromStdString((string)i.key()));
        }
    }
}

void MainWindow::on_stations_itemClicked(QListWidgetItem *item)
{
    if (item!=ui->stations->currentItem()) {
        for (auto i=0;i<ui->stations->count();i++) {
            ui->stations->item(i)->setBackground(QBrush(QColor(255,255,255)));
        }
        item->setBackground(QBrush(QColor(0,0,255)));
        ui->factions->clear();
        ui->missions->clear();
        json a=config.at(ui->systems->currentItem()->text().toStdString()).at(item->text().toStdString());
        for (auto i=a.begin();i!=a.end();i++) {
            ui->factions->addItem(QString::fromStdString((string)i.key()));
        }
    }
}

void MainWindow::on_factions_itemClicked(QListWidgetItem *item)
{
    if (item!=ui->factions->currentItem()) {
        for (auto i=0;i<ui->factions->count();i++) {
            ui->factions->item(i)->setBackground(QBrush(QColor(255,255,255)));
        }
        item->setBackground(QBrush(QColor(0,0,255)));
        ui->missions->clear();
        json a=config.at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString()).at(item->text().toStdString());
        for (auto i=a.begin();i!=a.end();i++) {
            QTreeWidgetItem *temp=new QTreeWidgetItem();
            temp->setText(0,QString::fromStdString((string)i.key()));
            temp->setText(1,QString::fromStdString((string)i.value()));
            ui->missions->addTopLevelItem(temp);
        }
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    /*if (item!=ui->missions->currentItem()) {
        for (int i=0;i<ui->missions->topLevelItemCount();i++) {
            ui->missions->topLevelItem(i)->setBackground(0,QBrush(QColor(255,255,255)));
            ui->missions->topLevelItem(i)->setBackground(1,QBrush(QColor(255,255,255)));
        }
        item->setBackground(0,QBrush(QColor(0,0,255)));
        item->setBackground(1,QBrush(QColor(0,0,255)));
        json a=config.at(ui->systems->currentItem()->text().toStdString()).at(ui->stations->currentItem()->text().toStdString()).at(item->text(0).toStdString());
        for (auto i=a.begin();i!=a.end();i++) {
            QTreeWidgetItem *temp=new QTreeWidgetItem();
            temp->setText(0,QString::number(ui->spinBox->value()));
            temp->setText(1,QString::number(ui->doubleSpinBox->value()));
            ui->missions->addTopLevelItem(temp);
        }
    }*/
}
