#ifndef CONTAINEROBJECT_H
#define CONTAINEROBJECT_H

#pragma Includes {

#include "json.hpp"

#include <set>
#include <map>
#include <QLabel>
#include <QDebug>
#include <QDateTime>
#include <QGridLayout>
#include <QStyle>

using json=nlohmann::json;

using namespace std;
#pragma }
namespace techlevi {
#pragma PreDefinitions{
class System;
class station;
class faction;
class huntedSystem;
class mission;
class TargetedFaction;
class UiProperties {
    unsigned col=0,row=0;
    int px=0;
    int py=0;
    int dy=50;
    int dx=100;
    int ody=0;
    map<pair<unsigned,unsigned>,QLabel*>* labels=nullptr;
public:
    UiProperties(map<pair<unsigned,unsigned>,QLabel*>* _labels);
    void move();

    void setSize(unsigned _col,unsigned _row);

    pair<map<pair<unsigned,unsigned>,QLabel*>::iterator,bool> insert(unsigned _col,unsigned _row,QLabel* label);

    void addToLayout(QGridLayout* layout);
    void setPos(int _px, int _py, int _dx, int _dy, int _ody);
    void setPx(int _px);
    void setPy(int _py);
    void setDx(int _dx);
    void setDy(int _dy);
    void setODy(int _ody);
    int getPx();
    int getPy();
    int getDx();
    int getDy();
    int getODy();
    void setText(unsigned _col,unsigned _row, QString text);
};
struct Input {
    QString TSystem, SSystem, SFaction, SStation, TFaction;
    QDateTime AcceptanceTime,Expiry;
    unsigned kills=0,MID,killsSoFar=0;
    double reward=0;
    bool completed=false,Winged;
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

template <class T>
struct defaultPointerCmp {
    bool operator ()(T * const &a, T * const &b) const {
        return (*a) < (*b);
    }
};

struct Statistics {
    unsigned totalKillsNeeded=0,killsSoFar=0
            ,totalNumberOfMissions=0,completedMissions=0
            ,StackHeight=0,StackWidth=0;
    double totalPayout=0,currentPayout=0;
    Statistics operator +=(const Statistics &x);
};
struct TheoreticalResults {
    unsigned allMissions=0,TheoreticallyCompletedMissions=0;
    double totalPossiblePayout=0,TheoreticallyGainedPayout=0;
    double getPayoutPercentage() {
        return TheoreticallyGainedPayout*100/totalPossiblePayout;
    }
    double getMissionPercentage() {
        return ((double)TheoreticallyCompletedMissions/(allMissions==0?1:(double)allMissions))*100;
    }
};
struct SystemStation {
    set<System*>::iterator systemPlace;
    set<station*>::iterator stationPlace;
};

#pragma }

#pragma BaseClasses{
class ContainerObject : public QObject
{
    Q_OBJECT

protected:

public:
    QString name;
    ContainerObject();
    explicit ContainerObject(QString name1);
    bool operator <(const ContainerObject &b) const;
    bool operator >(const ContainerObject &b) const;
    bool operator ==(const ContainerObject &b) const;
    bool operator !=(const ContainerObject &b) const;
    bool operator ==(const QString &b) const;
    bool operator <(const QString &b) const;
    bool operator >(const QString &b) const;
    bool operator !=(const QString &b) const;
};

template<class T>
class AdvancedContainer : public ContainerObject {
protected:
    set<T*, defaultPointerCmp<T>> container;
public:
    using ContainerObject::ContainerObject;
    ~AdvancedContainer();
    typename set<T*>::iterator begin();
    typename set<T*>::iterator end();
    const typename set<T*>::iterator begin() const;
    const typename set<T*>::iterator end() const;
    typename set<T*>::iterator find(QString name) const;
    pair<typename set<T*>::iterator,bool> add(T *&input);
    pair<T*,bool> pop(T *f);
    T *remove(typename set<T*>::iterator f);
    unsigned getSize();
    unsigned getSize() const;
    typename set<T*>::iterator Get(unsigned i);
};


class mission {
public:
    station* sourceStation;
    faction* sourceFaction;
    System* sourceSystem;
    huntedSystem* targetSystem;
    TargetedFaction* targetFaction;
    unsigned MID=0;
    int overallKillsNeeded=0, killsSoFar=0;
    double payout=0;
    bool Completed=false,Winged;
    QDateTime AcceptanceTime,Expiry;
    mission(unsigned ID);
    mission(
            unsigned ID
            ,TargetedFaction* TFaction
            ,huntedSystem* TSystem
            ,System* SSystem
            ,faction* SFaction
            ,station* SStation
            ,unsigned kills
            ,double reward
            ,QDateTime AT
            ,QDateTime Exp
            ,bool Winged
            ,bool Redirected=false
            ,unsigned killsSoFarin=0
            );

#pragma operators{
    bool operator <(const mission &b) const;

    bool operator >(const mission &b) const;

    bool operator ==(const mission &b) const;

    bool operator !=(const mission &b) const;

    bool operator ==(const unsigned &ID) const;

    bool operator !=(const unsigned &ID) const;

    bool operator <(const unsigned &ID) const;

    bool operator >(const unsigned &ID) const;
#pragma}
};

struct DateComperator {
    bool operator()(mission * const & a, mission * const & b) const {
        return a->AcceptanceTime<b->AcceptanceTime;
    }
};

class sysStat {
private:
    pair<System*,station*> a;
public:
    sysStat(pair<System*,station*> in);
    bool operator <(const sysStat &b) const;
    bool operator >(const sysStat &b) const;
    bool operator ==(const sysStat &b) const;
    bool operator !=(const sysStat &b) const;
    System* getSys();
    station* getStat();
};

struct result {
    set<mission*>::iterator missionPlace;
    set<faction*>::iterator factionPlace;
};
#pragma }

#pragma StructuredClasses{


class faction : public ContainerObject {
    Q_OBJECT
private:
    set<sysStat*, defaultPointerCmp<sysStat>> homes;
    set<mission*, defaultPointerCmp<mission>> missionsbyID;
    multiset<mission*, DateComperator> missionsbyDate;
public:
    faction(QString name);
    double totalReward=0, currentReward=0;
    unsigned totalKillsSoFar=0,totalKillsNeeded=0;
    unsigned reCalcStackHeight();
    double reCalcTotalReward();
    pair<mission*,bool> add(mission *&input);
    pair<unsigned,unsigned> countMissions();
    const set<mission*, defaultPointerCmp<mission>> &getMissionsbyID() const {return missionsbyID;}
    const multiset<mission*, DateComperator> &getMissionsbyDate() const {return missionsbyDate;}
    const set<sysStat*, defaultPointerCmp<sysStat>> &getHomes() const {return homes;}
    void InsertHome(sysStat * input) {homes.insert(input);}
    void removeMission(mission* toRemove);
    bool hasMissions() {return missionsbyID.size()>0?true:false;}
signals:

    void MissionAdded(mission *m);
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
    Q_OBJECT
public:
    GlobalFactions(QString name);
    bool Add(faction *f);
    faction *Get(unsigned i);
    result findMission(unsigned ID);
    unsigned totalKills=0;
    unsigned reCalcTotalKills();
    unsigned reCalcStackHeight();
    unsigned totalMissionCount=0,CompletedMissionCount=0;
    unsigned stackHeight=0;
    unsigned totalKillsSoFar=0;
    unsigned getNumberOfMissions();
    double payout=0,currentPayout=0;
    double reCalcTotalPayout();
    unsigned stackWidth() const;
    void RefreshStatistics();

private slots:

    void MissionAdded(mission *m);
};

class TargetedFaction : public AdvancedContainer<mission> {
public:
    using AdvancedContainer::AdvancedContainer;
    unsigned killsSoFar=0;

};

class huntedSystem : public AdvancedContainer<System> {
public:
    using AdvancedContainer::AdvancedContainer;
    Statistics getStats(GlobalFactions &glob);
    faction *findFaction(QString faction);
    set<TargetedFaction*> TargetedFactions;
};

class currentStation : public ContainerObject {
public:
    using ContainerObject::ContainerObject;
    QString systemName;
};

class HuntedSystems : public AdvancedContainer<huntedSystem> {
public:
    using AdvancedContainer::AdvancedContainer;
};
#pragma }

#pragma Template class definitions {


template<class T>
AdvancedContainer<T>::~AdvancedContainer()
{
    for (auto object:container) {
        container.extract(object);
        delete object;
    }
}

template<class T>
typename set<T*>::iterator AdvancedContainer<T>::begin()
{
    return  container.begin();
}

template<class T>
typename set<T*>::iterator AdvancedContainer<T>::end()
{
    return  container.end();
}

template<class T>
const typename set<T*>::iterator AdvancedContainer<T>::begin() const
{
    return  container.begin();
}

template<class T>
const typename set<T*>::iterator AdvancedContainer<T>::end() const
{
    return  container.end();
}

template<class T>
typename set<T*>::iterator AdvancedContainer<T>::find(QString name) const
{
    auto temp=new T(name);
    auto res=container.find(temp);
    delete temp;
    return res;
}

template<class T>
pair<typename set<T*>::iterator, bool> AdvancedContainer<T>::add(T *&input)
{
    return container.insert(input);
}

template<class T>
pair<T *, bool> AdvancedContainer<T>::pop(T *f)
{
    auto res=container.find(f);
    if (res==container.end()) {
        return {nullptr,false};
    }
    auto output=container.erase(res);
    return {*output,true};
}

template<class T>
T *AdvancedContainer<T>::remove(typename set<T*>::iterator f)
{
    auto res=*f;
    container.erase(f);
    return res;
}

template<class T>
unsigned AdvancedContainer<T>::getSize() {return container.size();}

template<class T>
unsigned AdvancedContainer<T>::getSize() const {return container.size();}

template<class T>
typename set<T*>::iterator AdvancedContainer<T>::Get(unsigned i)
{
    typename set<T*>::iterator it=this->container.begin();
    for (unsigned j=0; j<i; j++) {
        it++;
    }
    return it;
}

#pragma}

}


#endif // CONTAINEROBJECT_H
