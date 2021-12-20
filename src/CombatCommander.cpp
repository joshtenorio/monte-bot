#include "CombatCommander.h"

CombatCommander::CombatCommander(){
    sm = ScoutManager();
    logger = Logger("CombatCommander");
    std::cout << "before squad" << std::endl;
    //mainArmy = Squad("main", 10);
    std::cout << "after squad" << std::endl;

}

void CombatCommander::OnGameStart(){

    sm.OnGameStart();

    groundMap.initialize();
    airMap.initialize();

    //mainArmy.initialize();
}

void CombatCommander::OnStep(){
    
    // update influence maps
    if(gInterface->observation->GetGameLoop() % 4 == 0){
        groundMap.setGroundMap();
        groundMap.propagate();
        //airMap.setAirMap();
        //airMap.propagate();
    }

    // scout manager
    sm.OnStep();



    // handle killing changelings every so often
    if(gInterface->observation->GetGameLoop() % 24 == 0){
        handleChangelings();
    } // end if gameloop % 24 == 0
    
    if(gInterface->observation->GetGameLoop() > 70 ){
        //mainArmy.onStep();
    }
    

    // if we have a bunker, put marines in it
    // TODO: fix
    sc2::Units bunkers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BUNKER));

} // end OnStep

void CombatCommander::OnUnitCreated(const sc2::Unit* unit_){
    if(unit_ == nullptr) return; // if for whatever reason its nullptr, dont do anything

    // TODO: don't add first 4 marines to attacksquad, they go into bunker
    switch(unit_->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_REAPER:
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
        case sc2::UNIT_TYPEID::TERRAN_MARINE:
        case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
            //mainArmy.addUnit(unit_->tag);
        break;
    }
    
}

void CombatCommander::OnUnitDestroyed(const sc2::Unit* unit_){
    sm.OnUnitDestroyed(unit_);
    //mainArmy.removeUnit(unit_->tag);

}

void CombatCommander::OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_){
    if(unit_->alliance != sc2::Unit::Alliance::Self) return;

    if(unit_->is_building || unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV){
        // 1. get a list of 7 closest workers to pull
        sc2::Units workers;
        workers = API::getClosestNUnits(unit_->pos, 7, 9, sc2::Unit::Alliance::Self, sc2::UNIT_TYPEID::TERRAN_SCV);

        // 2. get a list of nearby idle army
        //sc2::Units armyPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnits(bio));
        sc2::Units armyPool = API::getClosestNUnits(unit_->pos, 50, 36, sc2::Unit::Alliance::Self);
        sc2::Units idleArmy;
        for(auto& a : armyPool)
            if(a->orders.empty() && !a->is_building) idleArmy.emplace_back(a);

        // 3. get closest enemy units to unit_ and find their center
        sc2::Units enemies = API::getClosestNUnits(unit_->pos, 11, 12, sc2::Unit::Alliance::Enemy);
        float x = 0.0, y = 0.0;
        float numEnemies = (float) enemies.size();
        for(auto& e : enemies){
            x += e->pos.x;
            y += e->pos.y;
        }
        sc2::Point2D enemyCenter = sc2::Point2D(x/numEnemies, y/numEnemies);

        // 4. have idle army attack, and have some workers repair and some workers attack
        // pulling workers for attacking/repairing is disabled after worker rush window
        gInterface->actions->UnitCommand(idleArmy, sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);

        if(gInterface->observation->GetGameLoop() > 3000) return;
        if(unit_->is_building)
            for(int n = 0; n < workers.size(); n++){
                if(n % 3 == 0){
                    gInterface->actions->UnitCommand(workers[n], sc2::ABILITY_ID::EFFECT_REPAIR, unit_);
                }
                else
                    gInterface->actions->UnitCommand(workers[n], sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);
            }
        else
            gInterface->actions->UnitCommand(workers, sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);
        
    }
}

void CombatCommander::OnUnitEnterVision(const sc2::Unit* unit_){
    sm.OnUnitEnterVision(unit_);
}

void CombatCommander::handleChangelings(){
    // TODO: move this to somehwere where we only do this once
    std::vector<sc2::UNIT_TYPEID> changelings;
    changelings.emplace_back(sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINE);
    changelings.emplace_back(sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINESHIELD);

    sc2::Units friendlyMarines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_MARINE));
    if(friendlyMarines.empty()) return;

    // TODO: move the "figure out what enemy race is" code into InformationManager initialize
    std::vector<sc2::PlayerInfo> info = gInterface->observation->GetGameInfo().player_info;
    for(auto p : info)
        if(p.race_requested == sc2::Race::Zerg || p.race_actual == sc2::Race::Zerg){ // FIXME: simplify this at some point please
            sc2::Units enemyChangelings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnits(changelings));
            if(enemyChangelings.empty()) return;

            for(auto& c : enemyChangelings){
                const sc2::Unit* closestMarine = friendlyMarines.front();
                for(auto& m : friendlyMarines){
                    if(sc2::DistanceSquared2D(c->pos, m->pos) < sc2::DistanceSquared2D(c->pos, closestMarine->pos))
                        closestMarine = m;
                }
                
                // kill changeling with closest marine
                gInterface->actions->UnitCommand(closestMarine, sc2::ABILITY_ID::ATTACK_ATTACK, c);
            } // end for c : changelings
        } // end if race == zerg
}

bool CombatCommander::isUnitInSquad(sc2::Tag tag){
    /*
    for(auto& s : squads){
        if(s.second.containsUnit(tag)) return true;
    }
    */
    return false;
}