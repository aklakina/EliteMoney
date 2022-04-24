#ifndef DATA_H
#define DATA_H

#include <QObject>
#include "containerobject.h"
#include "ui_mainwindow.h"
#include "api.h"
#include <QTreeWidget>

using namespace std;

namespace techlevi {

    struct Result {
        set<huntedSystem*>::iterator TSystem;
        set<System*>::iterator SSystem ;
        set<faction*>::iterator SFaction;
        set<station*>::iterator SStation;
        set<mission*>::iterator Mission;
    };

    struct Input {
        QString TSystem, SSystem, SFaction, SStation, TFaction;
        unsigned kills,MID,killsSoFar=0;
        double reward;
        bool completed=false;
    };

    class data : public QObject
    {
        Q_OBJECT

    public:
        explicit data(QObject *parent = nullptr);
        //explicit data(QObject *parent, Ui::MainWindow *Mui, API *parent_api);
        void Init(Ui::MainWindow *Mui, API *parent_api);

        void KillsperFaction();

        void missionCompleted(unsigned ID,bool remove=false);

        void RemoveMission(unsigned *ID=nullptr);

        void addMission(string dest,int kills, double reward, unsigned ID,QString Sfaction, QString Tfaction);

        map<set<huntedSystem*>::iterator,Statistics> getStatistics();

        Result parseData(Input input);

        const GlobalFactions& getFactions() {return *globalFactions;}

        unsigned getStackHeight() {return globalFactions->stackHeight;}

        unsigned getStackWidth() {return globalFactions->stackWidth();}

        set<faction*>::iterator getFaction(QString name) {return globalFactions->find(name);}

        set<faction*>::iterator FactionsEnd() {return globalFactions->end();}

        set<faction*>::iterator FactionsBegin() {return globalFactions->begin();}

    signals:

        void Refresh(GlobalFactions const &faction, HuntedSystems const & targetSystem, mission const & m, bool deleted=false);

        void UpdateTree(GlobalFactions const & GlobalFactions);

        void addTreeItem();

        void RefreshUI(bool switcher,GlobalFactions const & faction);

    private:

        Ui::MainWindow *ui;

        API *api;

        GlobalFactions *globalFactions;

        HuntedSystems *CompleteData;

        currentStation *CurrentStation;

        void RecursiveDataReader(QTreeWidgetItem* item, string &a);

        void refreshdata(int depth=0, json *SavedData=nullptr, techlevi::Input * input=nullptr);

    public slots:

        void getUnifiedStatistics(Statistics* input);

        void getJsonFormattedData(string * input,QTreeWidgetItem* item=nullptr);

    };
}

#endif // DATA_H
