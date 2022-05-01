#include "mainwindow.h"



EventDistributor::EventDistributor(QObject *parent) : QObject(parent)
{
    }

EventDistributor::EventDistributor(MainWindow *MW, IO *Iapi, techlevi::data *data) : QObject(nullptr), mw(MW),io(Iapi), Data(data)
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
    this->io->originalContent=basenames;
    notifier->addPaths(baseabsolute);
    connect(notifier,&QFileSystemWatcher::directoryChanged,this->io,&IO::OnNewFile);
    connect(notifier,&QFileSystemWatcher::fileChanged,this->io,&IO::OnNewEvent);
    connect(Data,&techlevi::data::UpdateTree,mw,&MainWindow::RebuildTree);
    connect(Data,&techlevi::data::RefreshUI,mw,&MainWindow::Refresh_UI);
    connect(Data,&techlevi::data::addTreeItem,mw,&MainWindow::addTreeItem, Qt::DirectConnection);
    connect(Data,&techlevi::data::RefreshTable,mw,&MainWindow::RefreshTable);
    connect(Data,&techlevi::data::BuildStationMissionData,mw,&MainWindow::buildStationMissionData);
    connect(Data,&techlevi::data::BuildCompletedMissionData,mw,&MainWindow::buildCompletedMissionData);
    connect(io,&IO::requestStatistics,Data,&techlevi::data::getUnifiedStatistics, Qt::DirectConnection);
    connect(io,&IO::requestJson, Data, &techlevi::data::getJsonFormattedData, Qt::DirectConnection);
    connect(io,&IO::provideJson, Data, &techlevi::data::LoadDataFromJson);
    connect(io,&IO::addMission,Data,&techlevi::data::addMission);
    connect(io,&IO::MissionRedirection,Data,&techlevi::data::MissionRedirection);
    connect(io,&IO::missionCompleted,Data,&techlevi::data::missionCompleted);
    connect(io,&IO::Docked,Data,&techlevi::data::Docked);
    connect(io,&IO::unDocked,Data,&techlevi::data::unDocked);
    connect(io,&IO::BountyCollected,Data,&techlevi::data::BountyCollected);
    connect(io,&IO::AddFileToFSWatcher,this,&EventDistributor::AddFileToFSWatcher);
    connect(io,&IO::JumpedToSystem,mw,&MainWindow::ExpandTree);
    connect(mw,&MainWindow::requestTheorData,Data,&techlevi::data::calculateTheoreticalCompletion,Qt::DirectConnection);
    connect(mw,&MainWindow::requestStatistics,Data,&techlevi::data::getStatistics,Qt::DirectConnection);
    connect(mw,&MainWindow::requestUnifiedStatistics,Data,&techlevi::data::getUnifiedStatistics,Qt::DirectConnection);
    connect(mw,&MainWindow::requestSession,Data,&techlevi::data::GetSession,Qt::DirectConnection);
    connect(mw,&MainWindow::LoadData,io,&IO::LoadData);
    connect(mw,&MainWindow::SaveData,io,&IO::SaveData);
    connect(mw,&MainWindow::SaveUIConfig,io,&IO::SaveOverlayConfig,Qt::DirectConnection);
    connect(mw,&MainWindow::getOverlayData,io,&IO::SendUIConfig,Qt::DirectConnection);
}

void EventDistributor::AddFileToFSWatcher(QString path)
{
    notifier->addPath(path);
}
