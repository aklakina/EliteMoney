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
    class data : public QObject
    {
        Q_OBJECT

    public:
        explicit data(QObject *parent = nullptr);
        //explicit data(QObject *parent, Ui::MainWindow *Mui, API *parent_api);
        void Init(Ui::MainWindow *Mui, API *parent_api);

        void RemoveMission(unsigned *ID=nullptr);

        Result parseData(Input input, Result *prev=nullptr, bool SingleInsert=true);

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

        void RefreshUI(bool switcher);

        void RefreshTable(GlobalFactions const & data);


    private:

        Ui::MainWindow *ui;

        API *api;

        GlobalFactions *globalFactions=nullptr;

        HuntedSystems *CompleteData=nullptr;

        currentStation *CurrentStation=nullptr;

        huntedSystem* Session=nullptr;

        void RecursiveDataReader(AdvancedContainer<ContainerObject>* item, json &a,unsigned depth=0);

        void refreshdata(int depth=0, json *SavedData=nullptr, techlevi::Input * input=nullptr,techlevi::Result * Prev=nullptr);

    public slots:

        void getUnifiedStatistics(Statistics* input);

        map<huntedSystem*,Statistics> getStatistics(map<huntedSystem*,Statistics> *& res);

        void getJsonFormattedData(string * input);

        void LoadDataFromJson(json & input);

        void addMission(string dest,int kills, double reward, unsigned ID, bool wing,QString Sfaction, QString Tfaction, QDateTime AcceptanceTime, QDateTime Expiry);

        void MissionRedirection(unsigned ID,QString newDest);

        void missionCompleted(unsigned ID,bool remove=false);

        void BountyCollected(QString Faction,unsigned Reward);

        void calculateTheoreticalCompletion(int theoreticalKills, TheoreticalResults & _ret);

        void Docked(QString System,QString Station);

        void unDocked();

        void JumpedToSystem(QString system);

        void GetSession(huntedSystem *& input);

    private slots:
    };
}

#endif // DATA_H
