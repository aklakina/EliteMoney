#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "json.hpp"
#include <QTreeWidget>
#include <QFileSystemWatcher>
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

private slots:

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void OnNewEvent(const QString &file);

    void OnNewFile(const QString &file);

    void on_treeWidget_3_itemClicked(QTreeWidgetItem *item, int column);

    void on_actionSave_Session_triggered();

    void on_actionLoad_session_triggered();

    void on_actionStart_new_session_triggered();

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    int selecteditem=0;
    QTreeWidgetItem *first=nullptr;
    json config;
    void configreader(QTreeWidgetItem* item, string &a);
    void refreshdata(int depth=0, json *a=nullptr);
    map<QString,pair<vector<pair<pair<QString,QString>,pair<pair<unsigned,QString>,pair<pair<int,int>,pair<double,bool>>>>>,pair<int,double>>> missions;
    void firsttreefiller();
    double total_kills=0;
    double max_kills=0;
    double ratio=0;
    unsigned total_mission_count=0;
    void GarbageCollector();
    void deleteing(QTreeWidgetItem*a);
    QFileSystemWatcher *notifier;
    void addMission(string dest,int kills, double reward, unsigned ID,QString faction);
    map<string,int> MissionTargetFactions;
    string MissionTarget="Nothing_yet";
    json current_station;
    void CheckCurrentStation(QString &faction,QTreeWidgetItem* item=nullptr,bool do_not_search=false,int depth=0,QString Target_system="");
    void missionCompleted(unsigned ID,bool remove=false);
    void RemoveMission(unsigned *ID=nullptr,bool remove=false);
    void completedData();
    QStringList originalContent;
    json events;
    bool checkingifdone,therewasanother;
    int total_kills_so_far=0;
    void resetTreeColor();
    QString station_name="";
    void KillsperFaction();
    void on_pushButton_6_clicked();
    int kills_needed_for_this=0;
    int reward_for_this=0;
    QString faction_global="";
    QString station="";
    vector<pair<vector<QString>,QString>> systemStation;
    QString system_namee;
    vector<pair<vector<pair<QString,int>>,QString>> factionStation;
    QString temp_mission_target;
    void on_pushButton_5_clicked(QString file_1);
    void on_listWidget_2_itemClicked(QString file_1);
    bool should_i_wait=false;
    void RefreshTree();
};
#endif // MAINWINDOW_H
