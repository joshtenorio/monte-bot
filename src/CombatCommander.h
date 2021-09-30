#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"
#include "ScoutManager.h"
#include "AttackSquad.h"

namespace Monte {
    enum TankState {
        Null = -1,
        Unsieged,
        Sieging,
        Sieged,
        Unsieging
    };

    typedef struct Tank_s_t {
        Tank_s_t(sc2::Tag tag_) { tag = tag_; state = Monte::TankState::Null; };
        sc2::Tag tag;
        Monte::TankState state;
    } Tank;

    enum BioState {
        Null = -1,
        Defending,
        Kiting,
        Attacking
    };

    enum StimState {
        Null = -1,
        Unstimmed, // change state to eligibleStim if we have health and enemies are nearby
        EligibleStim, // give marine order to stim, then change state to Stimmed
        Stimmed // change state to unstimmed once unit is not buffed anymore
    };

    typedef struct Bio_s_t {
        Bio_s_t(sc2::Tag tag_) {tag = tag_; state = Monte::BioState::Null; stimStatus = Monte::StimState::Null; };
        sc2::Tag tag;
        Monte::BioState state;
        Monte::StimState stimStatus;

    } Bio;
} // end namespace Monte

class CombatCommander : public Manager {
    public:
    // constructors
    CombatCommander() { sm = ScoutManager(); logger = Logger("CombatCommander"); mainSquad = AttackSquad(); };
    // TODO: add a constructor with strategy, bc we need CombatConfig from strategy

    void OnGameStart();
    void OnStep();
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);
    void OnUnitEnterVision(const sc2::Unit* unit_);

    void marineOnStep();
    void medivacOnStep();
    void siegeTankOnStep();
    void ravenOnStep(); // if nearby marine count is low, focus on putting down auto turrets, else use anti armor missles

    void handleChangelings();
    

    protected:
    ScoutManager sm;

    std::vector<Monte::Tank> tanks;

    // used for marine control
    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bio; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
};