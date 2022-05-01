#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "io.h"
#include "data.h"
#include "overlay.h"
#include "overlayeditor.h"

#include <QMainWindow>
#include <QListWidget>
#include <QObject>
#include <QFileSystemWatcher>
#include <QFont>
#include <QFontDatabase>
#include <QCloseEvent>
#include <QKeyEvent>

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

    void on_actionEdit_Overlay_triggered();

private:
    Ui::MainWindow *ui;
    IO* io;
    techlevi::data* Data;
    EventDistributor *ev;
    OverlayEditor* olEdit;
    int selecteditem=0;
    void firsttreefiller();
    void resetTreeColor();
    void on_listWidget_2_itemClicked(QString file_1);
    bool SearchTreeRecursively(QTreeWidgetItem* item,QString name);
    void ExpandChildsToo(QTreeWidgetItem* item);
    void RetractAll(QTreeWidgetItem* item);
    void closeEvent (QCloseEvent *event) override;
    void LoadOverlayData();
    void applyTableStyle();
    Overlay *ol;
    QIcon *wing;
    QFont *Header;
    QFont *General;
    QFont *Numbers;
    QFont *Tree;


public slots:

    //void completedData(GlobalFactions const & data, HuntedSystems const & CompleteData, bool deleted=false, mission const * m=nullptr);

    void RefreshTree(techlevi::GlobalFactions const & GlobalFactions);

    void Refresh_UI(bool switcher);

    void addTreeItem(QString name);

    void RebuildTree(techlevi::AdvancedContainer<techlevi::ContainerObject> *CompleteData, QTreeWidgetItem* item=nullptr,bool do_not_search=false,int depth=0);

    void RefreshTable(techlevi::GlobalFactions const & data);

    void ExpandTree(QString name);

    void buildStationMissionData(vector<techlevi::mission*> const & data);

    void buildCompletedMissionData(vector<techlevi::mission*> const & data);

signals:

    void SaveData(QString path);

    void requestStatistics(map<techlevi::huntedSystem*,techlevi::Statistics> *& stats);

    void requestUnifiedStatistics(techlevi::Statistics * stats);

    void LoadData(QString path);

    void requestTheorData(int const & theorKills,techlevi::TheoreticalResults & _ret);

    void requestSession(techlevi::huntedSystem *& input);

    void SaveUIConfig(json conf);

    void getOverlayData(json * conf);

};



using namespace techlevi;

class EventDistributor : public QObject
{
    Q_OBJECT

private:
    QFileSystemWatcher *notifier;
    techlevi::data *Data;
    MainWindow *mw;
    IO *io;

public:
    explicit EventDistributor(QObject *parent = nullptr);
    EventDistributor(MainWindow * MW,IO * api,techlevi::data *data);
signals:

private slots:
    void AddFileToFSWatcher(QString path);
};



#endif // MAINWINDOW_H
