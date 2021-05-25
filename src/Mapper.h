#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <string>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_unit_filters.h>
#include <sc2api/sc2_map_info.h>

// max distance for a neighboring mineral patch
#define PATCH_NEIGHBOR_DISTANCE 25.0f  // 5^2 = 25

// offsets used when searching for base location in Expansion
#define SEARCH_MIN_OFFSET   -10
#define SEARCH_MAX_OFFSET   10
#define DISTANCE_ERR_MARGIN 10.0f

// used for defining ownership of an Expansion
#define OWNER_NEUTRAL   0
#define OWNER_SELF      1
#define OWNER_ENEMY     2

// map names (needs to be updated every season, at least until we properly calculate ramps)
// only have ever dream and submarine for now, just waiting on AIE maps to be released since those will probably be used for next season
#define EVERDREAM 0
#define SUBMARINE 1

typedef struct Ramp_s_t {
    Ramp_s_t() {}
    bool isMainRamp = false;
    std::vector<sc2::Point2D> supplyDepotPoints;
    sc2::Point2D barracksPos;
    sc2::Point2D barracksWithAddonPos; // probably the more useful one

} Ramp;


typedef struct Expansion_s_t {
    Expansion_s_t(): isStartingLocation(false), initialized(false) {}
    std::vector<sc2::Point3D> mineralLine;
    sc2::Units gasGeysers;
    sc2::Point2D baseLocation;
    sc2::Point3D mineralMidpoint; // used to find base location

    bool isStartingLocation = false;
    bool initialized = false;
    float distanceToStart;
    int numFriendlyRefineries = 0;
    char ownership = OWNER_NEUTRAL;
    Ramp ramp;

    // used for std::sort
    bool operator < (const Expansion_s_t& e) const {
        return (distanceToStart < e.distanceToStart);
    }

    bool operator == (const Expansion_s_t& e) const {
        if(baseLocation.x == e.baseLocation.x && baseLocation.y == e.baseLocation.y)
            return true;
        else
            return false;
    }


} Expansion;



class Mapper {
    public:
    Mapper() {};
    void initialize();
    Expansion* getClosestExpansion(sc2::Point3D point);

    // TODO: change this so it returns an Expansion*
    Expansion getStartingExpansion();

    // returns a pointer to the nth closest expansion to the starting location
    Expansion* getNthExpansion(int n);

    // returns a pointer to the newest owned expansion
    Expansion* getCurrentExpansion();

    // returns a pointer to the next expansion to build at
    Expansion* getNextExpansion();

    int numOfExpansions();


    protected:
    // get expansions functions are from mullemech
    // https://github.com/ludlyl/MulleMech/blob/master/src/core/Map.cpp
    // TODO: find ways to improve performance
    void calculateExpansions();

    // sort by closest expansion to the parameter to farthest from the parameter
    void sortExpansions(sc2::Point2D point);

    static std::vector<Expansion> expansions;
    static std::vector<Ramp> ramps;

    Expansion startingExpansion; // TODO: could this be a pointer instead? i.e. yes it can
};