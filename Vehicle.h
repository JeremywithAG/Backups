#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <string>
#include "GridArea.h" // Needed for VecGridAreaType

class Vehicle {
protected:
    GridArea::VecGridAreaType mapData;  // 1D vector of GridArea tiles
    

public:
    // Constructors
    Vehicle();
    Vehicle(const GridArea::VecGridAreaType& _entireMap);

    // Set and get map data
    void setMapData(const GridArea::VecGridAreaType& _entireMap);
    GridArea::VecGridAreaType getMapData() const;

    // Pure virtual functions to be overridden by subclasses
    virtual void displayRouteReport() const = 0;
    virtual void printRouteReport(const std::string& outputFilename) const = 0;
    virtual std::vector<GridArea::VecGridAreaType> computeAllIdealRoutes() const = 0;

    virtual ~Vehicle() = default;
};

#endif





