#ifndef DATA_H
#define DATA_H

#include <QObject>
#include "mainwindow.h"
#include "containerobject.h"
#include "ui_mainwindow.h"
#include "api.h"

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

        void RemoveMission(unsigned *ID=nullptr,bool remove=false);

        void addMission(string dest,int kills, double reward, unsigned ID,QString Sfaction, QString Tfaction);

        map<set<huntedSystem*>::iterator,Statistics> getStatistics();

        Statistics getUnifiedStatistics();

        Result parseData(Input input);
    signals:

        void Refresh(GlobalFactions &faction, HuntedSystems & targetSystem);

        void UpdateTree();

        void addTreeItem();

    private:

        Ui::MainWindow *ui;

        API *api;

        GlobalFactions *globalFactions;

        HuntedSystems *CompleteData;

        currentStation *CurrentStation;


    };
}

#endif // DATA_H
