/* File : GridArea.h */
#ifndef GRIDAREA_H
#define GRIDAREA_H

#include <string>
#include<vector>

enum TERRAIN_TYPE { BARRIER, START, END, DANGER, GRASS_LAND, JUNGLE, MOUNTAINOUS, WATERY, HILLY, SWAMP, FOREST, UNKNOWN };

using namespace std;

class GridArea
{
private:
    int x;
    int y;
    TERRAIN_TYPE terrain;

    /*
    Below data members are OPTIONAL, if your program design already caters for efficient storage &
    retrieval of shield and movement energy required for each TerrainType in "other places", then it is not
    compulsory to store / associate them with any particular GridArea object!
    */
    long shldEnrgReqd;  // shield protection required to enter into this GridArea's terrain type ...
    long movtEnrgReqd;  // movement energy required to move to this GridArea's terrain type ...

public:
    // Constructors
    GridArea();
    GridArea(int _x, int _y);
    GridArea(int _x, int _y, TERRAIN_TYPE _t);
    GridArea(int _x, int _y, TERRAIN_TYPE _t, long _shldEnrgReqd, long _movtEnrgReqd);

    // Accessors
    int getX() const;
    int getY() const;
    TERRAIN_TYPE getTerrain() const;

    long getShldEnrgReqd() const;
    long getMovtEnrgReqd() const;

    // Mutators
    void setX(int _x);
    void setY(int _y);
    void setTerrain(TERRAIN_TYPE _t);

    void setShldEnrgReqd(long _newValue);
    void setMovtEnrgReqd(long _newValue);

    // Utility functions
    bool isAdjacent(const GridArea& anotherPt) const;
    string convertTerrainToString() const;
    void displayInfo() const;

    static TERRAIN_TYPE convertCharToTerrainType(char terrainSymbol);
    typedef vector<GridArea> VecGridAreaType;


};

#endif // GRIDAREA_H

