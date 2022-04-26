#include "mainwindow.h"



EventDistributor::EventDistributor(QObject *parent) : QObject(parent)
{
    }

EventDistributor::EventDistributor(MainWindow *MW, API *Iapi, techlevi::data *data) : QObject(nullptr), mw(MW),api(Iapi), Data(data)
{
    QStringList path1=QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    notifier=new QFileSystemWatcher(this);
    notifier->addPath(path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous");
    QDir base(path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous");
    QStringList basenames=base.entryList();
    QStringList baseabsolute;
    for (auto & i:basenames) {
        QString b=path1.first()+"/Saved Games/Frontier Developments/Elite Dangerous/"+i;
        baseabsolute.push_back(b);
    }
    this->api->originalContent=basenames;
    notifier->addPaths(baseabsolute);
    connect(notifier,&QFileSystemWatcher::directoryChanged,this->api,&API::OnNewFile);
    connect(notifier,&QFileSystemWatcher::fileChanged,this->api,&API::OnNewEvent);
    connect(Data,&techlevi::data::Refresh,mw,&MainWindow::completedData);
    connect(Data,&techlevi::data::UpdateTree,mw,&MainWindow::RebuildTree);
    connect(Data,&techlevi::data::RefreshUI,mw,&MainWindow::Refresh_UI);
    connect(Data,&techlevi::data::addTreeItem,mw,&MainWindow::addTreeItem, Qt::DirectConnection);
    connect(api,&API::requestStatistics,Data,&techlevi::data::getUnifiedStatistics, Qt::DirectConnection);
    connect(api,&API::requestJson, Data, &techlevi::data::getJsonFormattedData, Qt::DirectConnection);
    connect(api,&API::provideJson, Data, &techlevi::data::LoadDataFromJson);
    connect(api,&API::addMission,Data,&techlevi::data::addMission);
    connect(api,&API::MissionRedirection,Data,&techlevi::data::MissionRedirection);
    connect(api,&API::missionCompleted,Data,&techlevi::data::missionCompleted);
    connect(api,&API::Docked,Data,&techlevi::data::Docked);
    connect(api,&API::unDocked,Data,&techlevi::data::unDocked);
    connect(api,&API::BountyCollected,Data,&techlevi::data::BountyCollected);

    connect(api,&API::AddFileToFSWatcher,this,&EventDistributor::AddFileToFSWatcher);
    connect(mw,&MainWindow::requestTheorData,Data,&techlevi::data::calculateTheoreticalCompletion,Qt::DirectConnection);
    connect(mw,&MainWindow::requestUnifiedStatistics,Data,&techlevi::data::getUnifiedStatistics,Qt::DirectConnection);
    connect(mw,&MainWindow::LoadData,api,&API::LoadData);
    connect(mw,&MainWindow::SaveData,api,&API::SaveData);

}

void EventDistributor::AddFileToFSWatcher(QString path)
{
    notifier->addPath(path);
}
