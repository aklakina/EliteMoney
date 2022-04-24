#include "containerobject.h"

#pragma BaseClassDefinitions{
ContainerObject::ContainerObject(QString name1) {
    name=name1;
}

bool ContainerObject::operator <(ContainerObject const &b) const {
    return this->name<b.name;
}
bool ContainerObject::operator >(ContainerObject const &b) const {
    return this->name>b.name;
}
bool ContainerObject::operator ==(ContainerObject const &b) const {
    return this->name==b.name;
}
bool ContainerObject::operator !=(ContainerObject const &b) const {
    return this->name!=b.name;
}
bool ContainerObject::operator <(QString const &b) const {
    return this->name<b;
}
bool ContainerObject::operator >(QString const &b) const {
    return this->name>b;
}
bool ContainerObject::operator ==(QString const &b) const {
    return this->name==b;
}
bool ContainerObject::operator !=(QString const &b) const {
    return this->name!=b;
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
    return container.find(name);
}

template<class T>
pair<typename set<T*>::iterator, bool> AdvancedContainer<T>::add(T*& input)
{
    auto res=container.insert(input);
    if (!res.second) {
        delete input;
        input=(*res.first);
    }
    return res;
}

template<class T>
pair<T*,bool> AdvancedContainer<T>::pop(T* f)
{
    auto res=container.find(f);
    if (res==container.end()) {
        return false;
    }
    container.erase(f);
    return true;
}
template<class T>
T* AdvancedContainer<T>::remove(typename set<T*>::iterator f)
{
    auto res=*f;
    container.erase(f);
    return res;
}
template<class T>
unsigned AdvancedContainer<T>::getSize()
{
    return container.size();
}
template<class T>
typename set<T*>::iterator AdvancedContainer<T>::Get(unsigned i)
{
    typename set<T*>::iterator it=this->container.begin();
    for (unsigned j=0; j<i; j++) {
        it++;
    }
    return it;
}

bool mission::operator <(mission const &b) const {
    return this->MID<b.MID;
}

bool mission::operator >(mission const &b) const {
    return this->MID>b.MID;
}

bool mission::operator ==(mission const &b) const {
    return this->MID==b.MID;
}

bool mission::operator !=(mission const &b) const {
    return this->MID!=b.MID;
}

bool mission::operator ==(unsigned const &ID) const {
    return this->MID==ID;
}

bool mission::operator !=(unsigned const &ID) const {
    return this->MID!=ID;
}

bool mission::operator<(unsigned int const &ID) const {
    return this->MID<ID;
}

bool mission::operator>(unsigned int const &ID) const {
    return this->MID>ID;
}

sysStat::sysStat(pair<set<System*>::iterator,set<station*>::iterator> in) : a(in) {
    
}

bool sysStat::operator <(sysStat const &b) const {
    if ((*this->a.first)<(*b.a.first)) {
        return true;
    } else if ((*this->a.first)==(*b.a.first) && (*this->a.second)<(*b.a.second)){
        return true;
    } else {
        return false;
    }
}

bool sysStat::operator >(sysStat const &b) const {
    if ((*this->a.first)>(*b.a.first)) {
        return true;
    } else if ((*this->a.first)==(*b.a.first) && (*this->a.second)>(*b.a.second)){
        return true;
    } else {
        return false;
    }
}

bool sysStat::operator ==(sysStat const &b) const {
    if ((*this->a.first)==(*b.a.first) && (*this->a.second)==(*b.a.second)) {
        return true;
    } else {
        return false;
    }
}

bool sysStat::operator !=(sysStat const &b) const {
    if ((*this->a.first)!=(*b.a.first) || (*this->a.second)!=(*b.a.second)) {
        return true;
    } else {
        return false;
    }
}

set<System*>::iterator sysStat::getSys() {return a.first;}

set<station*>::iterator sysStat::getStat() {return a.second;}

#pragma }

#pragma StructuredClassDefinitions{

unsigned faction::reCalcStackHeight()
{
    unsigned kills=0;
    for (auto i:missions) {
        kills+=i->overallKillsNeeded;
    }
    totalKillsNeeded=kills;
    return kills;
}

double faction::reCalcTotalReward() {
    double reward=0;
    for (auto mission:missions) {
        reward+=mission->payout;
    }
    totalReward=reward;
    return reward;
}

pair<set<mission*>::iterator, bool> faction::add(mission *&input)
{
    auto res=missions.insert(input);
    if (!res.second) {
        delete input;
        input=(*res.first);
    } else {
        this->totalKillsNeeded+=input->overallKillsNeeded;
        this->totalReward+=input->payout;
        this->totalKillsSoFar+=input->killsSoFar;
        emit MissionAdded(input);
    }
    return res;
}

pair<unsigned,unsigned> faction::countMissions()
{
    pair<unsigned,unsigned> output={0,0};
    for (auto mission:missions) {
        if (mission->Completed) {
            output.second++;
        } else {
            output.first++;
        }
    }
    return output;
}

Statistics huntedSystem::getStats(GlobalFactions const & glob)
{
    Statistics stats;
    for (auto faction:glob) {
        auto res=findFaction(faction->name);
        if (res!=nullptr) {
            stats.totalNumberOfMissions=res->missions.size();
            stats.totalKillsNeeded=res->totalKillsNeeded;
            stats.totalPayout=res->totalReward;
            stats.killsSoFar=res->totalKillsSoFar;
            stats.completedMissions=res->countMissions().second;
        }
    }
    return stats;
}

faction *huntedSystem::findFaction(QString faction)
{
    for (auto system:container) {
        for (auto station:*system) {
            auto res=station->find(faction);
            if (res!=station->end()) return *res;
        }
    }
    return nullptr;
}

GlobalFactions::GlobalFactions(QString name)
{
    this->name=name;
    for (auto faction:container) {
        connect(faction,SIGNAL(MissionAdded(mission*)),this,SLOT(MissionAdded(mission*)));
    }
}

result GlobalFactions::findMission(unsigned ID)
{
    auto tempMission=new mission(ID);
    for (set<faction*>::iterator factions=container.begin();factions!=container.end();factions++) {
        auto res=(*factions)->missions.find(tempMission);
        if (res!=(*factions)->missions.end()) {
            delete tempMission;
            result temp;
            temp.factionPlace=factions;
            temp.missionPlace=res;
            return temp;
        }
    }
    throw "No such mission";
}

unsigned GlobalFactions::reCalcStackHeight()
{
    unsigned max=0;
    for (auto i:container) {
        if (max<i->reCalcStackHeight()) {
            max=i->totalKillsNeeded;
        }
    }
    stackHeight=max;
    return max;
}

unsigned GlobalFactions::reCalcTotalKills()
{
    unsigned kills=0;
    for (auto i:container) {
        kills+=i->reCalcStackHeight();
    }
    totalKills=kills;
    return kills;
}

unsigned GlobalFactions::getNumberOfMissions() {
    unsigned num=0;
    for (auto faction:container) {
        num+=faction->missions.size();
    }
    totalMissionCount=num;
    return num;
}

double GlobalFactions::reCalcTotalPayout() {
    double rew=0;
    for (auto faction:container) {
        rew+=faction->reCalcTotalReward();
    }
    this->payout=rew;
    return rew;
}

#pragma }


