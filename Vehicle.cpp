#include "Vehicle.h"

// Default constructor
Vehicle::Vehicle() {}

// Constructor that takes the 1D vector of GridArea objects
Vehicle::Vehicle(const GridArea::VecGridAreaType& _entireMap)
    : mapData(_entireMap) {}

// Setter for map data
void Vehicle::setMapData(const GridArea::VecGridAreaType& _entireMap) {
    mapData = _entireMap;
}

// Getter for map data
GridArea::VecGridAreaType Vehicle::getMapData() const {
    return mapData;
}

