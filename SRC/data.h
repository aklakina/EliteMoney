#ifndef DATA_H
#define DATA_H

#include <QObject>
#include "containerobject.h"
#include "ui_mainwindow.h"
#include "api.h"
#include <QTreeWidget>

using namespace std;
using namespace techlevi;
namespace techlevi {


    struct Input {
        QString TSystem, SSystem, SFaction, SStation, TFaction;
        QDateTime AcceptanceTime,Expiry;
        unsigned kills=0,MID,killsSoFar=0;
        double reward=0;
        bool completed=false;
    };
    struct Result {
        TargetedFaction* TFaction=nullptr;
        huntedSystem* TSystem=nullptr;
        System* SSystem=nullptr;
        faction* SFaction=nullptr;
        station* SStation=nullptr;
        mission* Mission=nullptr;
        void operator=(Result const & input) {
            this->TSystem=input.TSystem;
            this->SStation=input.SStation;
            this->SSystem=input.SSystem;
            this->SFaction=input.SFaction;
            this->Mission=input.Mission;
            this->TFaction=input.TFaction;
        }
    };

    class data : public QObject
    {
        Q_OBJECT

    public:
        explicit data(QObject *parent = nullptr);
        //explicit data(QObject *parent, Ui::MainWindow *Mui, API *parent_api);
        void Init(Ui::MainWindow *Mui, API *parent_api);

        void KillsperFaction();

        void RemoveMission(unsigned *ID=nullptr);

        map<huntedSystem*,Statistics> getStatistics();

        Result parseData(Input input, Result *prev=nullptr);

        unsigned getStackHeight() {return globalFactions->stackHeight;}

        unsigned getStackWidth() {return globalFactions->stackWidth();}

        set<faction*>::iterator getFaction(QString name) {return globalFactions->find(name);}

        set<faction*>::iterator FactionsEnd() {return globalFactions->end();}

        set<faction*>::iterator FactionsBegin() {return globalFactions->begin();}

        unsigned getMissionNumber() {return globalFactions->totalMissionCount;}

    signals:

        void Refresh(GlobalFactions const &faction, HuntedSystems const & targetSystem,  bool deleted=false, mission const * m=nullptr);

        void UpdateTree(AdvancedContainer<ContainerObject> * GlobalFactions, QTreeWidgetItem* item=nullptr,bool do_not_search=false,int depth=0);

        void addTreeItem(QString name);

        void RefreshUI(bool switcher,GlobalFactions const & faction);


    private:

        Ui::MainWindow *ui;

        API *api;

        GlobalFactions *globalFactions=nullptr;

        HuntedSystems *CompleteData=nullptr;

        currentStation *CurrentStation=nullptr;

        void RecursiveDataReader(QTreeWidgetItem* item, string &a);

        void refreshdata(int depth=0, json *SavedData=nullptr, techlevi::Input * input=nullptr,techlevi::Result * Prev=nullptr);

    public slots:

        void getUnifiedStatistics(Statistics* input);

        void getJsonFormattedData(string * input,QTreeWidgetItem* item=nullptr);

        void LoadDataFromJson(json & input);

        void addMission(string dest,int kills, double reward, unsigned ID, bool wing,QString Sfaction, QString Tfaction, QDateTime AcceptanceTime, QDateTime Expiry);

        void MissionRedirection(unsigned ID,QString newDest);

        void missionCompleted(unsigned ID,bool remove=false);

        void BountyCollected(QString Faction,unsigned Reward);

        void calculateTheoreticalCompletion(int theoreticalKills, TheoreticalResults & _ret);

        void Docked(QString System,QString Station);

        void unDocked();
    };
}

#endif // DATA_H
