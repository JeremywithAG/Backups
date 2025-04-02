/* File : GridArea.cpp */
#include "GridArea.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>

/* Note : Constructor Delegation only works from C++ standard 11 and beyond! */

// Default Constructor - Delegating to constructor with 2 parameters
GridArea::GridArea() : GridArea(0, 0) { }

// Constructor with 2 parameters - Delegating to constructor with 3 parameters
GridArea::GridArea(int _x, int _y) : GridArea(_x, _y, UNKNOWN) { }

// Constructor with 3 parameters - Delegating to (last) constructor with 5 parameters
GridArea::GridArea(int _x, int _y, TERRAIN_TYPE _t) : GridArea(_x, _y, _t, 0, 0) { }

GridArea::GridArea(int _x, int _y, TERRAIN_TYPE _t, long _shldEnrgReqd, long _movtEnrgReqd)
{
    x = _x;
    y = _y;
    terrain = _t;
    shldEnrgReqd = _shldEnrgReqd;
    movtEnrgReqd = _movtEnrgReqd;
}

// -----------------------------------------
// Accessors / "Get" functions
// -----------------------------------------
int GridArea::getX() const { return x; }

int GridArea::getY() const { return y; }

TERRAIN_TYPE GridArea::getTerrain() const { return terrain; }

long GridArea::getShldEnrgReqd() const { return shldEnrgReqd; }

long GridArea::getMovtEnrgReqd() const { return movtEnrgReqd; }

// -----------------------------------------
// Mutators / "Set" functions
// -----------------------------------------
void GridArea::setX(int _x) { x = _x; }

void GridArea::setY(int _y) { y = _y; }

void GridArea::setTerrain(TERRAIN_TYPE _t) { terrain = _t; }

void GridArea::setShldEnrgReqd(long _newValue) { shldEnrgReqd = _newValue; }

void GridArea::setMovtEnrgReqd(long _newValue) { movtEnrgReqd = _newValue; }

// -----------------------------------------
// Utility / "processing" functions
// -----------------------------------------
bool GridArea::isAdjacent(const GridArea& anotherPt) const
{
    int verticalDist = abs(anotherPt.y - y);
    int horizontalDist = abs(anotherPt.x - x);
    return ((anotherPt.x == x && verticalDist == 1) || (anotherPt.y == y && horizontalDist == 1));
}

TERRAIN_TYPE GridArea::convertCharToTerrainType(char terrainSymbol)
{
    TERRAIN_TYPE result = UNKNOWN;

    switch (terrainSymbol)
    {
        case '#': result = BARRIER; break;
        case 'S': result = START; break;
        case 'E': result = END; break;
        case 'X': result = DANGER; break;
        case 'G': result = GRASS_LAND; break;
        case 'J': result = JUNGLE; break;
        case 'M': result = MOUNTAINOUS; break;
        case 'W': result = WATERY; break;
        case 'H': result = HILLY; break;
        case 's': result = SWAMP; break;
        case 'F': result = FOREST; break;
        default: result = UNKNOWN; break;
    }

    return result;
}

string GridArea::convertTerrainToString() const
{
    string result = "UNKNOWN";

    switch (terrain)
    {
        case BARRIER: result = "BARRIER"; break;
        case START: result = "START"; break;
        case END: result = "END"; break;
        case DANGER: result = "DANGER"; break;
        case GRASS_LAND: result = "GRASS_LAND"; break;
        case JUNGLE: result = "JUNGLE"; break;
        case MOUNTAINOUS: result = "MOUNTAINOUS"; break;
        case WATERY: result = "WATERY"; break;
        case HILLY: result = "HILLY"; break;
        case SWAMP: result = "SWAMP"; break;
        case FOREST: result = "FOREST"; break;
        case UNKNOWN: result = "UNKNOWN"; break;
    }

    return result;
}

void GridArea::displayInfo() const
{
    cout << "Grid Area [" << x << ", " << y << "], Terrain: "
         << setw(13) << left << convertTerrainToString()
         << "Shld Enrg Reqd : " << left << setw(8) << shldEnrgReqd
         << "Movt Enrg Reqd : " << left << setw(8) << movtEnrgReqd
         << endl;
}

