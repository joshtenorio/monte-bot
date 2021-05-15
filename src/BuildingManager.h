#pragma once

#include <sc2api/sc2_unit.h>
#include "api.h"
#include "Manager.h"
#include "BuildingPlacer.h"

class BuildingManager : public Manager {
    public:
    BuildingManager() {};
    void OnStep();

    // used for if a worker assigned to a job or building under construction is killed
    void OnUnitDestroyed(const sc2::Unit* unit_);

    // used for resetting worker job and for setting ownership of expansion, and for updating num of friendly refineries in an expansion
    void OnBuildingConstructionComplete(const Unit* building_);
    bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildSupplyDepot();
    bool TryBuildBarracks();
    bool tryBuildRefinery();

    protected:
    BuildingPlacer bp;

    private:
    // put functions here to help determine what building to build from (for upgrades) or if an scv needs to be called
    // idea: assume that if it is not of a barracks/factory/starport/enggbay/armory/academy/etc then an scv is required
};