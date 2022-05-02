#ifndef DATA_H
#define DATA_H

#include <QObject>
#include "containerobject.h"
#include "ui_mainwindow.h"
#include <QTreeWidget>

using json=nlohmann::json;
using namespace std;
using namespace techlevi;
namespace techlevi {
    class data : public QObject
    {
        Q_OBJECT

    public:
        explicit data(QObject *parent = nullptr);
        //explicit data(QObject *parent, Ui::MainWindow *Mui, API *parent_api);
        void Init(Ui::MainWindow *Mui);

        void RemoveMission(unsigned *ID=nullptr);

        Result parseData(Input input, Result *prev=nullptr, bool SingleInsert=true);

        unsigned getStackHeight() {return globalFactions->stackHeight;}

        unsigned getStackWidth() {return globalFactions->stackWidth();}

        set<faction*>::iterator getFaction(QString name) {return globalFactions->find(name);}

        set<faction*>::iterator FactionsEnd() {return globalFactions->end();}

        set<faction*>::iterator FactionsBegin() {return globalFactions->begin();}

        unsigned getMissionNumber() {return globalFactions->totalMissionCount;}

    signals:

        void UpdateTree(techlevi::AdvancedContainer<techlevi::ContainerObject> * GlobalFactions, QTreeWidgetItem* item=nullptr,bool do_not_search=false,int depth=0);

        void addTreeItem(QString name);

        void RefreshUI(bool switcher);

        void RefreshTable(techlevi::GlobalFactions const & data);

        void BuildCompletedMissionData(vector<techlevi::mission*> const & data);

        void BuildStationMissionData(vector<techlevi::mission*> const & data);

    private:

        Ui::MainWindow *ui;

        GlobalFactions *globalFactions=nullptr;

        HuntedSystems *CompleteData=nullptr;

        currentStation *CurrentStation=nullptr;

        huntedSystem* Session=nullptr;

        void RecursiveDataReader(AdvancedContainer<ContainerObject>* item, json &a,unsigned depth=0);

        void refreshdata(int depth=0, json *SavedData=nullptr, techlevi::Input * input=nullptr,techlevi::Result * Prev=nullptr);

        vector<mission *> getStationMissionData();

        vector<mission *> getCompletedMissionData();

    public slots:

        void getUnifiedStatistics(techlevi::Statistics* input);

        map<techlevi::huntedSystem*,techlevi::Statistics> getStatistics(map<techlevi::huntedSystem*,techlevi::Statistics> *& res);

        void getJsonFormattedData(std::string * input);

        void LoadDataFromJson(std::string & input);

        void addMission(std::string dest,int kills, double reward, unsigned ID, bool wing,QString Sfaction, QString Tfaction, QDateTime AcceptanceTime, QDateTime Expiry);

        void MissionRedirection(unsigned ID,QString newDest);

        void missionCompleted(unsigned ID,bool remove=false);

        void BountyCollected(QString Faction,unsigned Reward);

        void calculateTheoreticalCompletion(int theoreticalKills, techlevi::TheoreticalResults & _ret);

        void Docked(QString System,QString Station);

        void unDocked();

        void JumpedToSystem(QString system);

        void GetSession(techlevi::huntedSystem *& input);

    private slots:
    };
}

#endif // DATA_H
