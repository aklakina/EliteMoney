#ifndef CONTAINEROBJECT_H
#define CONTAINEROBJECT_H

#pragma Includes {
#include <set>
#include <map>
#include <QString>
#include <QObject>
#include <QDebug>


using namespace std;
#pragma }

#pragma PreDefinitions{
class System;
class station;
class faction;
class huntedSystem;
class mission;
class TargetedFaction;
template <class T>
struct defaultPointerCmp {
    bool operator ()(T * const &a, T * const &b) const {
        return (*a) < (*b);
    }
};
struct Statistics {
    unsigned totalKillsNeeded=0,killsSoFar=0,totalNumberOfMissions=0,completedMissions=0;
    double totalPayout=0,currentPayout=0;
    Statistics operator +=(const Statistics &x);
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
    typename set<T*>::iterator begin();
    typename set<T*>::iterator end();
    const typename set<T*>::iterator begin() const;
    const typename set<T*>::iterator end() const;
    typename set<T*>::iterator find(QString name) const;
    pair<typename set<T*>::iterator,bool> add(T *&input);
    pair<T*,bool> pop(T *f);
    T *remove(typename set<T*>::iterator f);
    unsigned getSize();;
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
    bool Completed=false;
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
public:
    faction(QString name);
    set<sysStat*, defaultPointerCmp<sysStat>> homes;
    set<mission*, defaultPointerCmp<mission>> missions;
    double totalReward=0, currentReward=0;
    unsigned totalKillsSoFar=0,totalKillsNeeded=0;
    unsigned reCalcStackHeight();
    double reCalcTotalReward();
    pair<set<mission*>::iterator,bool> add(mission *&input);
    pair<unsigned,unsigned> countMissions();
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
    //using AdvancedContainer::AdvancedContainer;
    GlobalFactions(QString name);
    bool Add(faction *f);
    faction *Get(unsigned i);
    //bool remove(faction *f);
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
    unsigned stackWidth();


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
    Statistics getStats(const GlobalFactions &glob);
    faction *findFaction(QString faction);
    set<TargetedFaction*> TargetedFactions;
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

#pragma Template class definitions {


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
typename set<T*>::iterator AdvancedContainer<T>::Get(unsigned i)
{
    typename set<T*>::iterator it=this->container.begin();
    for (unsigned j=0; j<i; j++) {
        it++;
    }
    return it;
}

#pragma}


#endif // CONTAINEROBJECT_H
