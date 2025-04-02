#ifndef HIGHLANDER_H
#define HIGHLANDER_H

#include "Vehicle.h"

class HighLander : public Vehicle {
public:
    HighLander();
    HighLander(const GridArea::VecGridAreaType& _entireMap);





    void displayRouteReport() const override;
    void printRouteReport(const std::string& outputFilename) const override;
    std::vector<GridArea::VecGridAreaType> computeAllIdealRoutes() const override;
};

#endif // HIGHLANDER_H

