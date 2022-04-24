#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTreeWidget>
#include "api.h"
#include "data.h"
#include <set>

using json=nlohmann::json;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //vector<pair<vector<pair<QString,int>>,QString>> factionStation;
    Ui::MainWindow *ui;

private slots:

    void addTreeItem();

    void on_pushButton_4_clicked();

    void on_treeWidget_3_itemClicked(QTreeWidgetItem *item, int column);

    void on_actionSave_Session_triggered();

    void on_actionLoad_session_triggered();

    void on_actionStart_new_session_triggered();

    void on_horizontalSlider_valueChanged(int value);

    void on_Copy_data_clicked();

    void on_pushButton_clicked();

//public slots:

    void completedData(GlobalFactions & data, HuntedSystems & targetSystem);

    void RefreshTree();

private:
    API* api;
    techlevi::data* Data;
    int selecteditem=0;
    QTreeWidgetItem *first=nullptr;
    json config;
    void configreader(QTreeWidgetItem* item, string &a);
    //map<QString,pair<vector<pair<pair<QString,QString>,pair<pair<unsigned,QString>,pair<pair<int,int>,pair<double,bool>>>>>,pair<int,double>>> missions;
    void firsttreefiller();
    //double total_kills=0; stack height
    //double max_kills=0; overall kills
    //double ratio=0; kredit/kill
    //unsigned total_mission_count=0;
    void GarbageCollector();
    void deleteing(QTreeWidgetItem*a);
    //map<string,int> MissionTargetFactions;
    //string MissionTarget="Nothing_yet";
    //json current_station;
    void CheckCurrentStation(huntedSystem *HuntedSystem, currentStation *currStat, set<faction*>::iterator faction,QTreeWidgetItem* item=nullptr,bool do_not_search=false,int depth=0,QString Target_system="");
    //bool checkingifdone,therewasanother;
    //int total_kills_so_far=0;
    void resetTreeColor();
    //QString station_name="";
    //int kills_needed_for_this=0;
    //int reward_for_this=0;
    //QString faction_global="";
    //QString station="";
    //vector<pair<vector<QString>,QString>> systemStation;
    //QString system_namee;
    //QString temp_mission_target;
    void on_pushButton_5_clicked(QString file_1);
    void on_listWidget_2_itemClicked(QString file_1);
    //bool should_i_wait=false;

public:

    map<unsigned,bool> Current_Missions;

};
#endif // MAINWINDOW_H
