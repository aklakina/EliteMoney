#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "api.h"
#include "data.h"
#include "overlay.h"

#include <QMainWindow>
#include <QListWidget>
#include <QObject>
#include <QFileSystemWatcher>

using json=nlohmann::json;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class EventDistributor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:


    void on_treeWidget_3_itemClicked(QTreeWidgetItem *item, int column);

    void on_actionSave_Session_triggered();

    void on_actionLoad_session_triggered();

    void on_actionStart_new_session_triggered();

    void on_horizontalSlider_valueChanged(int value);

    void on_Copy_data_clicked();

    void on_pushButton_clicked();


private:
    Ui::MainWindow *ui;
    API* api;
    techlevi::data* Data;
    EventDistributor *ev;
    int selecteditem=0;
    void firsttreefiller();
    void resetTreeColor();
    void on_listWidget_2_itemClicked(QString file_1);
    Overlay *ol;

public slots:

    void completedData(GlobalFactions const & data, HuntedSystems const & CompleteData, bool deleted=false, mission const * m=nullptr);

    void RefreshTree(GlobalFactions const & GlobalFactions);

    void Refresh_UI(bool switcher,GlobalFactions const & data);

    void addTreeItem(QString name);

    void RebuildTree(AdvancedContainer<ContainerObject> *CompleteData, QTreeWidgetItem* item=nullptr,bool do_not_search=false,int depth=0);

signals:

    void SaveData(QString path);

    void requestUnifiedStatistics(Statistics * stats);

    void LoadData(QString path);

    void requestTheorData(int const & theorKills,TheoreticalResults & _ret);

};



using namespace techlevi;

class EventDistributor : public QObject
{
    Q_OBJECT

private:
    QFileSystemWatcher *notifier;
    techlevi::data *Data;
    MainWindow *mw;
    API *api;

public:
    explicit EventDistributor(QObject *parent = nullptr);
    EventDistributor(MainWindow * MW,API * api,techlevi::data *data);
signals:

private slots:
    void AddFileToFSWatcher(QString path);
};



#endif // MAINWINDOW_H
