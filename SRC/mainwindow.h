#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTreeWidget>
#include "api.h"
#include "data.h"
#include <set>
#include <QObject>

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

    void addTreeItem(QString name);

    void on_treeWidget_3_itemClicked(QTreeWidgetItem *item, int column);

    void on_actionSave_Session_triggered();

    void on_actionLoad_session_triggered();

    void on_actionStart_new_session_triggered();

    void on_horizontalSlider_valueChanged(int value);

    void on_Copy_data_clicked();

    void on_pushButton_clicked();

    void completedData(GlobalFactions const & data, HuntedSystems const & CompleteData, bool deleted=false, mission const * m=nullptr);

    void RefreshTree(GlobalFactions const & GlobalFactions);

    void Refresh_UI(bool switcher,GlobalFactions const & data);


private:
    Ui::MainWindow *ui;
    API* api;
    techlevi::data* Data;
    int selecteditem=0;
    json config;
    void firsttreefiller();
    void RebuildTree(huntedSystem *HuntedSystem, currentStation *currStat, set<faction*>::iterator faction,QTreeWidgetItem* item=nullptr,bool do_not_search=false,int depth=0);
    void resetTreeColor();
    void on_listWidget_2_itemClicked(QString file_1);

public:

    map<unsigned,bool> Current_Missions;

};
#endif // MAINWINDOW_H
