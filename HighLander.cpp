#include "HighLander.h"
#include <iostream>
#include <vector>
#include <queue>
#include <utility>      // for std::pair
#include <climits>      // for INT_MAX
#include <algorithm>    // for std::reverse and std::find
#include <iomanip>

using namespace std;

// Default constructor
HighLander::HighLander() {}

// Constructor that takes a 1D vector of GridArea tiles
HighLander::HighLander(const GridArea::VecGridAreaType& _entireMap)
    : Vehicle(_entireMap) {}

// Just outputs a message for now
void HighLander::displayRouteReport() const {
    cout << "[HighLander] Displaying Route Report..." << endl;
}

// Placeholder for file-saving logic
void HighLander::printRouteReport(const std::string& outputFilename) const {
    cout << "[HighLander] Saving report to " << outputFilename << endl;
}

// Computes the best route and prints it
std::vector<GridArea::VecGridAreaType> HighLander::computeAllIdealRoutes() const {
    cout << "[HighLander] Computing ideal route using BFS with preferred terrain logic...\n";

    // 1. Convert 1D map to 2D char map
    int rows = Vehicle::mapRows;
    int cols = Vehicle::mapCols;
    vector<vector<char>> map(rows, vector<char>(cols, ' '));
    for (const auto& tile : entireMap) {
        map[tile.row][tile.col] = tile.terrainSymbol;
    }

    // Define HighLander preferences
    MYVehicleType currentVehicle;
    currentVehicle.name = "HighLander";
    currentVehicle.discountRate = 0.5f;
    currentVehicle.preferredTerrains = {'H', 'M'};

    int terrainCount = sizeof(terrainEnergyArray) / sizeof(terrainEnergyArray[0]);

    vector<pair<int, int>> starts, ends;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (map[r][c] == 'S') starts.push_back({r, c});
            if (map[r][c] == 'E') ends.push_back({r, c});
        }
    }

    if (starts.empty() || ends.empty()) {
        cout << "Error: No start or end points found!\n";
        return {};
    }

    unordered_map<char, int> energyMap;
    for (int i = 0; i < terrainCount; ++i) {
        char terrainChar = terrainEnergyArray[i].terrainType;
        int movement = terrainEnergyArray[i].movementEnergy;
        if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrainChar) != currentVehicle.preferredTerrains.end()) {
            movement = static_cast<int>(movement * currentVehicle.discountRate);
        }
        energyMap[terrainChar] = movement;
    }

    int routeIndex = 1;
    for (const auto& start : starts) {
        for (const auto& end : ends) {
            cout << "\nRoute " << routeIndex++
                 << " - From [" << start.first << ", " << start.second << "] to ["
                 << end.first << ", " << end.second << "]\n";

            vector<pair<int, int>> fullPath;

            bool hasPref = false;
            for (const auto& row : map) {
                for (char cell : row) {
                    if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), cell) != currentVehicle.preferredTerrains.end()) {
                        hasPref = true;
                        break;
                    }
                }
                if (hasPref) break;
            }

            auto bfsPath = [&](pair<int, int> src, pair<int, int> dst) -> vector<pair<int, int>> {
                vector<vector<bool>> visited(rows, vector<bool>(cols, false));
                vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols, {-1, -1}));
                queue<pair<int, int>> q;
                q.push(src);
                visited[src.first][src.second] = true;

                while (!q.empty()) {
                    auto [r, c] = q.front(); q.pop();
                    if (r == dst.first && c == dst.second) {
                        vector<pair<int, int>> path;
                        for (pair<int, int> at = dst; at != make_pair(-1, -1); at = parent[at.first][at.second])
                            path.push_back(at);
                        reverse(path.begin(), path.end());
                        return path;
                    }
                    for (int i = 0; i < 4; ++i) {
                        int nr = r + dRow[i], nc = c + dCol[i];
                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !visited[nr][nc] && map[nr][nc] != '#') {
                            visited[nr][nc] = true;
                            parent[nr][nc] = {r, c};
                            q.push({nr, nc});
                        }
                    }
                }
                return {};
            };

            if (hasPref) {
                pair<int, int> meanXY = {(start.first + end.first) / 2, (start.second + end.second) / 2};
                pair<int, int> nearestPref = {-1, -1};
                int minDist = INT_MAX;

                for (int r = 0; r < rows; ++r) {
                    for (int c = 0; c < cols; ++c) {
                        char terrain = map[r][c];
                        if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrain) != currentVehicle.preferredTerrains.end()) {
                            int dist = abs(r - meanXY.first) + abs(c - meanXY.second);
                            if (dist < minDist) {
                                minDist = dist;
                                nearestPref = {r, c};
                            }
                        }
                    }
                }

                if (nearestPref.first == -1) {
                    cout << "Preferred terrain not reachable.\n";
                    continue;
                }

                auto path1 = bfsPath(start, nearestPref);
                auto path2 = bfsPath(nearestPref, end);

                if (path1.empty() || path2.empty()) {
                    cout << "No valid path found from S → PREF or PREF → E.\n";
                    continue;
                }

                path1.pop_back();
                path1.insert(path1.end(), path2.begin(), path2.end());
                fullPath = path1;

            } else {
                fullPath = bfsPath(start, end);
                if (fullPath.empty()) {
                    cout << "No valid path found.\n";
                    continue;
                }
            }

            vector<vector<char>> visualMap(rows, vector<char>(cols, ' '));
            for (int r = 0; r < rows; ++r)
                for (int c = 0; c < cols; ++c)
                    if (map[r][c] == '#') visualMap[r][c] = '#';

            int preferredCount = 0, totalEnergy = 0, discountedTotal = 0;

            cout << "\nPath Sequence:\n";
            for (size_t i = 0; i < fullPath.size(); ++i) {
                auto [r, c] = fullPath[i];
                cout << "[" << r << ", " << c << "]";
                if (i != fullPath.size() - 1) cout << ", ";
                char terrain = map[r][c];
                if (terrain != 'S' && terrain != 'E' &&
                    find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrain) != currentVehicle.preferredTerrains.end()) {
                    preferredCount++;
                }
            }

            cout << "\n\nTot. No. of PREFERRED TERRAIN explored = " << preferredCount << endl;
            cout << "Steps taken [excluding 'S' and 'E']: " << fullPath.size() - 2 << endl;

            cout << "\nTot. Movt Enrg Reqd (Generic Veh) = ";
            for (size_t i = 1; i < fullPath.size(); ++i) {
                int r = fullPath[i].first, c = fullPath[i].second;
                char terrain = map[r][c];
                int moveEnergy = 0;
                for (int j = 0; j < terrainCount; ++j) {
                    if (terrainEnergyArray[j].terrainType == terrain) {
                        moveEnergy = terrainEnergyArray[j].movementEnergy;
                        break;
                    }
                }
                cout << terrain << "(" << moveEnergy << ")";
                if (i != fullPath.size() - 1) cout << " + ";
                totalEnergy += moveEnergy;
            }
            cout << " = \033[1;31m" << totalEnergy << "\033[0m\n";

            cout << "\nTot. Movt Enrg Reqd (" << currentVehicle.name << ") = ";
            cout << totalEnergy << " - [";

            for (size_t i = 1; i < fullPath.size() - 1; ++i) {
                int r = fullPath[i].first, c = fullPath[i].second;
                char terrain = map[r][c];
                int moveEnergy = 0;
                for (int j = 0; j < terrainCount; ++j) {
                    if (terrainEnergyArray[j].terrainType == terrain) {
                        moveEnergy = terrainEnergyArray[j].movementEnergy;
                        break;
                    }
                }
                if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrain) != currentVehicle.preferredTerrains.end()) {
                    discountedTotal += static_cast<int>(moveEnergy * currentVehicle.discountRate);
                    cout << moveEnergy << " ";
                } else {
                    discountedTotal += moveEnergy;
                }
            }

            cout << ")] = \033[1;31m" << discountedTotal << "\033[0m\n";

            for (auto [r, c] : fullPath) {
                char ch = map[r][c];
                if (ch == ' ') ch = '_';
                visualMap[r][c] = ch;
            }

            visualMap[start.first][start.second] = 'S';
            visualMap[end.first][end.second] = 'E';

            cout << "\nPath Visualization:\n   ";
            for (int c = 0; c < cols; ++c) cout << setw(2) << c << " ";
            cout << "\n";
            for (int r = 0; r < rows; ++r) {
                cout << setw(2) << r << " ";
                for (int c = 0; c < cols; ++c) {
                    cout << setw(2) << visualMap[r][c] << " ";
                }
                cout << "\n";
            }
        }
    }

    return {};
}




