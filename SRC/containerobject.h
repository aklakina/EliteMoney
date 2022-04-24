#ifndef CONTAINEROBJECT_H
#define CONTAINEROBJECT_H

#pragma Includes {
#include <set>
#include <map>
#include <QString>
#include <QObject>

using namespace std;
#pragma }

#pragma PreDefinitions{
class System;
class station;
class faction;
class huntedSystem;
class mission;
template <class T>
struct defaultPointerCmp {
    bool operator ()(T *const &a, T *const &b) const
    {
        return (*a) < (*b);
    }
};
struct Statistics {
    unsigned totalKillsNeeded=0,killsSoFar=0,totalNumberOfMissions=0,completedMissions=0;
    double totalPayout=0;
    Statistics operator +=(Statistics const & x) {
        this->completedMissions+=x.completedMissions;
        this->killsSoFar+=x.killsSoFar;
        this->totalNumberOfMissions+=x.totalNumberOfMissions;
        this->completedMissions+=x.completedMissions;
        this->totalPayout+=x.totalPayout;
    }
};

#pragma }

#pragma BaseClasses{
class ContainerObject : public QObject
{
    Q_OBJECT

protected:
    ContainerObject();

public:
    QString name;
    ContainerObject(QString name1);
    bool operator <(ContainerObject const &b) const;
    bool operator >(ContainerObject const &b) const;
    bool operator ==(ContainerObject const &b) const;
    bool operator !=(ContainerObject const &b) const;
    bool operator ==(QString const &b) const;
    bool operator <(QString const &b) const;
    bool operator >(QString const &b) const;
    bool operator !=(QString const &b) const;
};

template<class T>
class AdvancedContainer : public ContainerObject {
protected:
    set<T*, defaultPointerCmp<T>> container;
public:
    using ContainerObject::ContainerObject;
    typename set<T*>::iterator begin();
    typename set<T*>::iterator end();
    const typename set<T*>::iterator begin() const;
    const typename set<T*>::iterator end() const;
    typename set<T*>::iterator find(QString name) const;
    pair<typename set<T*>::iterator,bool> add(T*& input);
    pair<T*,bool> pop(T* f);
    T* remove(typename set<T*>::iterator f);
    unsigned getSize();
    typename set<T*>::iterator Get(unsigned i);
};

class mission {
public:
    QString targetFaction="";
    set<station*>::iterator sourceStation;
    set<faction*>::iterator sourceFaction;
    set<System*>::iterator sourceSystem;
    set<huntedSystem*>::iterator targetSystem;
    unsigned MID=0;
    int overallKillsNeeded=0, killsSoFar=0;
    double payout=0;
    bool Completed=false;
    mission(unsigned ID) : MID(ID) {}
    mission(
            unsigned ID
            ,set<huntedSystem*>::iterator TSystem
            ,set<System*>::iterator SSystem
            ,set<faction*>::iterator SFaction
            ,set<station*>::iterator SStation
            ,unsigned kills
            ,double reward
            ,bool Redirected=false
            ,unsigned killsSoFarin=0
        )
      :
            sourceStation(SStation)
            ,sourceFaction(SFaction)
            ,sourceSystem(SSystem)
            ,targetSystem(TSystem)
            ,MID(ID)
            ,overallKillsNeeded(kills)
            ,killsSoFar(killsSoFarin)
            ,payout(reward)
            ,Completed(Redirected)
    {}

#pragma operators{
    bool operator <(mission const &b) const;

    bool operator >(mission const &b) const;

    bool operator ==(mission const &b) const;

    bool operator !=(mission const &b) const;

    bool operator ==(unsigned const &ID) const;

    bool operator !=(unsigned const &ID) const;

    bool operator <(unsigned const &ID) const;

    bool operator >(unsigned const &ID) const;
#pragma}
};

class sysStat {
private:
    pair<set<System*>::iterator,set<station*>::iterator> a;
public:
    sysStat(pair<set<System*>::iterator,set<station*>::iterator> in);
    bool operator <(sysStat const &b) const;
    bool operator >(sysStat const &b) const;
    bool operator ==(sysStat const &b) const;
    bool operator !=(sysStat const &b) const;
    set<System*>::iterator getSys();
    set<station*>::iterator getStat();
};

struct result {
    set<mission*>::iterator missionPlace;
    set<faction*>::iterator factionPlace;
};
#pragma }

#pragma StructuredClasses{


class faction : public ContainerObject {
public:
    using ContainerObject::ContainerObject;
    set<sysStat*, defaultPointerCmp<sysStat>> homes;
    set<mission*, defaultPointerCmp<mission>> missions;
    double totalReward=0;
    unsigned totalKillsSoFar=0,totalKillsNeeded=0;
    unsigned reCalcStackHeight();
    double reCalcTotalReward();
    pair<set<mission*>::iterator,bool> add(mission*& input);
    pair<unsigned,unsigned> countMissions();
signals:

    void MissionAdded(mission* m);
};

class station : public AdvancedContainer<faction> {
public:
    using AdvancedContainer::AdvancedContainer;
};

class System : public AdvancedContainer<station> {
public:
    using AdvancedContainer::AdvancedContainer;
};

class GlobalFactions : public AdvancedContainer<faction> {
public:
    //using AdvancedContainer::AdvancedContainer;
    GlobalFactions(QString name);
    unsigned GetSize();
    bool Add(faction *f);
    faction* Get(unsigned i);
    bool remove(faction *f);
    unsigned getSize();
    result findMission(unsigned ID);
    unsigned totalKills=0;
    unsigned reCalcTotalKills();
    unsigned reCalcStackHeight();
    unsigned totalMissionCount=0;
    unsigned stackHeight=0;
    unsigned totalKillsSoFar=0;
    unsigned getNumberOfMissions();
    double payout=0;
    double reCalcTotalPayout();

private slots:

    void MissionAdded(mission* m);
};

class huntedSystem : public AdvancedContainer<System> {
public:
    using AdvancedContainer::AdvancedContainer;
    Statistics getStats(GlobalFactions const & glob);
    faction* findFaction(QString faction);
};

class currentStation : public AdvancedContainer<faction> {
public:
    using AdvancedContainer::AdvancedContainer;
    set<System*>::iterator System;
};

class HuntedSystems : public AdvancedContainer<huntedSystem> {
public:
    using AdvancedContainer::AdvancedContainer;
};
#pragma }



#endif // CONTAINEROBJECT_H
