#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "json.hpp"
#include <QTreeWidget>

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
    void on_listWidget_2_itemClicked(QListWidgetItem *item);

    void on_listWidget_3_itemClicked(QListWidgetItem *item);

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_systems_itemClicked(QListWidgetItem *item);

    void on_stations_itemClicked(QListWidgetItem *item);

    void on_factions_itemClicked(QListWidgetItem *item);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
    int selecteditem=0;
    QTreeWidgetItem *first=nullptr;
    json config;
    void configreader(QTreeWidgetItem* item, string &a);
    void refreshdata(int depth=0, json *a=nullptr);
    void jsonreader();
    pair<QString,map<QString,map<QString,map<QString,map<double,double>>>>> config_loaded;
    void firsttreefiller();
};
#endif // MAINWINDOW_H
