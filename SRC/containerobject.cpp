#include "containerobject.h"

#pragma BaseClassDefinitions{

Statistics Statistics::operator +=(const Statistics &x) {
    this->completedMissions+=x.completedMissions;
    this->killsSoFar+=x.killsSoFar;
    this->totalNumberOfMissions+=x.totalNumberOfMissions;
    this->completedMissions+=x.completedMissions;
    this->totalPayout+=x.totalPayout;
    return *this;
}

ContainerObject::ContainerObject() {}

ContainerObject::ContainerObject(QString name1) {
    name=name1;
}

bool ContainerObject::operator <(const ContainerObject &b) const {
    return this->name<b.name;
}
bool ContainerObject::operator >(const ContainerObject &b) const {
    return this->name>b.name;
}
bool ContainerObject::operator ==(const ContainerObject &b) const {
    return this->name==b.name;
}
bool ContainerObject::operator !=(const ContainerObject &b) const {
    return this->name!=b.name;
}
bool ContainerObject::operator <(const QString &b) const {
    return this->name<b;
}
bool ContainerObject::operator >(const QString &b) const {
    return this->name>b;
}
bool ContainerObject::operator ==(const QString &b) const {
    return this->name==b;
}
bool ContainerObject::operator !=(const QString &b) const {
    return this->name!=b;
}

mission::mission(unsigned ID) : MID(ID) {}

mission::mission(unsigned ID, TargetedFaction* TFaction,  huntedSystem*  TSystem,  System*  SSystem,  faction*  SFaction,  station*  SStation, unsigned kills, double reward, bool Redirected, unsigned killsSoFarin)
    :
      sourceStation(SStation)
    ,targetFaction(TFaction)
    ,sourceFaction(SFaction)
    ,sourceSystem(SSystem)
    ,targetSystem(TSystem)
    ,MID(ID)
    ,overallKillsNeeded(kills)
    ,killsSoFar(killsSoFarin)
    ,payout(reward)
    ,Completed(Redirected)
{}

bool mission::operator <(const mission &b) const {
    return this->MID<b.MID;
}

bool mission::operator >(const mission &b) const {
    return this->MID>b.MID;
}

bool mission::operator ==(const mission &b) const {
    return this->MID==b.MID;
}

bool mission::operator !=(const mission &b) const {
    return this->MID!=b.MID;
}

bool mission::operator ==(const unsigned &ID) const {
    return this->MID==ID;
}

bool mission::operator !=(const unsigned &ID) const {
    return this->MID!=ID;
}

bool mission::operator<(const unsigned int &ID) const {
    return this->MID<ID;
}

bool mission::operator>(const unsigned int &ID) const {
    return this->MID>ID;
}

sysStat::sysStat(pair<System*,station*> in) : a(in) {
    
}

bool sysStat::operator <(const sysStat &b) const {
    if ((*this->a.first)<(*b.a.first)) {
        return true;
    } else if ((*this->a.first)==(*b.a.first) && (*this->a.second)<(*b.a.second)){
        return true;
    } else {
        return false;
    }
}

bool sysStat::operator >(const sysStat &b) const {
    if ((*this->a.first)>(*b.a.first)) {
        return true;
    } else if ((*this->a.first)==(*b.a.first) && (*this->a.second)>(*b.a.second)){
        return true;
    } else {
        return false;
    }
}

bool sysStat::operator ==(const sysStat &b) const {
    if ((*this->a.first)==(*b.a.first) && (*this->a.second)==(*b.a.second)) {
        return true;
    } else {
        return false;
    }
}

bool sysStat::operator !=(const sysStat &b) const {
    if ((*this->a.first)!=(*b.a.first) || (*this->a.second)!=(*b.a.second)) {
        return true;
    } else {
        return false;
    }
}

System* sysStat::getSys() {return a.first;}

station* sysStat::getStat() {return a.second;}

#pragma }

#pragma StructuredClassDefinitions{

faction::faction(QString name)
{
    this->name=name;
}

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
    pair<set<mission*>::iterator, bool> res;
    try {
        res=missions.insert(input);
    }  catch (exception const & e) {
        qDebug()<<QString::fromStdString(e.what());
    }
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

Statistics huntedSystem::getStats(const GlobalFactions &glob)
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
        connect(faction,&faction::MissionAdded,this,&GlobalFactions::MissionAdded);
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

unsigned GlobalFactions::stackWidth() {return container.size();}

void GlobalFactions::MissionAdded(mission *m)
{
    this->CompletedMissionCount+=m->Completed? 1:0;
    this->totalKills+=m->overallKillsNeeded;
    this->payout+=m->payout;
    this->totalKillsSoFar+=m->killsSoFar;
    this->totalMissionCount++;
    reCalcStackHeight();
}

#pragma }






