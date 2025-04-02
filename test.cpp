#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <limits>
#include "SatComRelay.h"
#include "menuOptions.h"
#include <iomanip>
#include <queue>
#include <unordered_map>
#include <climits>
#include <set>
#include <algorithm>
//new
#include "Vehicle.h"
#include "HighLander.h"
#include "GridArea.h"  // For VecGridAreaType


using namespace std;

#define MAX_FILENAME_LENGTH 256

// Terrain type
char terrain_Space = ' '; 
char terrain_f = 'f';      
char terrain_j = 'j';      
char terrain_h = 'h';      
char terrain_M = 'M';      
char terrain_wave = '~';   
char terrain_w = 'w';      
char terrain_X = 'X';      

// Global variables
int xPosition = 1;
int yPosition = 1;
char movePrompt;
vector<vector<char>> mMap(10, vector<char>(10, ' ')); //2D vector used for autoMapping

struct terrainEnergy{ //global energy
long movementEnergy;
int shieldEnergy;
char terrainType;
};

//declare default terrain types
terrainEnergy terrainEnergyArray[10] = {
    {0, 0, ' '},
    {0, 0, 'f'},
    {0, 0, 'j'},
    {0, 0, 'h'},
    {0, 0, 'M'},
    {0, 0, '~'},
    {0, 0, 'w'},
    {0, 0, 'X'},
    {0, 0, 'S'},
    {0, 0, 'E'}
};
struct MYVehicleType {
    string name;            // Name of the vehicle
    vector<char> preferredTerrains;  // Terrain type it gets discount on
    float discountRate;     // e.g. 0.8 means 20% off movement energy
};
MYVehicleType MYvehicleTypes[4] = {
    {"Generic Veh", {}, 1.0f},     // No discount
    {"ShieldHero", {'X'}, 0.5f},        
    {"HighLander",{'h','M'}, 0.5f},    
    {"Dragonfly",{'w','~'}, 0.5f},    
};
//set up shortest distance
vector<vector<char>> fileMap; // 2D vector to store the map
string txtFile;
const int dRow[] = {-1, 1, 0, 0};
const int dCol[] = {0, 0, -1, 1};


// Global vehicle data
VehData vehicle;
SatComRelay vdt;


// Function declarations
void getFilename(char *srcFilename);
void printMmap();
void vectorResize();
void autoMapping();
void TransferAutoMap();
void printWelcomeMsg();
int printMainMenuOptions();
void EnergyCheck(char Terrain, int TerrainE);
void ReadAutoMap(const string &filename, vector<vector<char>> &fileMap);
void endPointFind();
void startPointFind();
int findShortestEnergyPath(vector<vector<char>>& map, const MYVehicleType& currentVehicle);
int findShortestPath(vector<vector<char>>& map, const MYVehicleType& currentVehicle);
GridArea::VecGridAreaType convertCharMapToGridAreaVector(const vector<vector<char>> &charMap);


//global obj
menuDatas options ;
struct Cell {
    int row, col, energyCost;
    bool operator>(const Cell& other) const {
        return energyCost > other.energyCost;  // Min-heap priority
    }
};



int main() {
    
    printWelcomeMsg();
    int userEnteredNumber;
    do
    {
    userEnteredNumber = printMainMenuOptions();
        cout << endl;
        switch(userEnteredNumber){
        	case 1:{
        	options.ConfigureAutopilotMapping();
        	bool fileNeedsDecryption = options.isEncrypted;
			bool randomizeStartPosition = false;
			char srcFileName[MAX_FILENAME_LENGTH];

			strncpy(srcFileName, options.inputScenarioFile.c_str(), MAX_FILENAME_LENGTH - 1);
			srcFileName[MAX_FILENAME_LENGTH - 1] = '\0';  // Ensure null termination

			cout << "\nFilename: " << srcFileName
				 << ", IsFileEncrypted: " << fileNeedsDecryption
				 << ", RandomizeStartPosition: " << randomizeStartPosition
				 << "\n";
			
			int missionType = 9999;
			// Assign to global `vehicle`
			vehicle = vdt.initializeVehicle(srcFileName, fileNeedsDecryption, randomizeStartPosition, missionType);
			vdt.allocateEnergyToShield(500000);
			break;
        	}
        	case 2:
        	char scanResultTwo;

            cout << "scanning" << endl;

            cout << "vehicle energy" <<vehicle.getCurrentEnergy() << endl;

            cout << "shield energy" << vehicle.getCurrentShieldEnergy() << endl;

            scanResultTwo = vdt.scanNorth(vehicle);

            cout << "scan result " << scanResultTwo << endl;

            cout << "vehicle energy" <<vehicle.getCurrentEnergy() << endl;

            cout << "shield energy" << vehicle.getCurrentShieldEnergy() << endl << endl;



            cout << "moving" << endl;

            cout << "vehicle energy" <<vehicle.getCurrentEnergy() << endl;

            cout << "shield energy" << vehicle.getCurrentShieldEnergy();

            vehicle = vdt.moveRightEast();

            cout << "vehicle energy" << vehicle.getCurrentEnergy() << endl;

            cout << "shield energy" << vehicle.getCurrentShieldEnergy() << endl << endl;



            cout << "scanning" << endl;

            cout << "vehicle energy" <<vehicle.getCurrentEnergy() << endl;

            cout << "shield energy" << vehicle.getCurrentShieldEnergy() << endl;

            scanResultTwo = vdt.scanNorth(vehicle);

            cout << "scan result " << scanResultTwo << endl;

            cout << "vehicle energy" <<vehicle.getCurrentEnergy() << endl;

            cout << "shield energy" << vehicle.getCurrentShieldEnergy() << endl << endl;
        	break;
        	
        	
        	case 3:
        	options.StartAutopilotMapping();
        	autoMapping();
        	
			TransferAutoMap();
        	break;
        	
        	
        	case 4:
        	{
        	cout << "What is the file you want to read : ";
        	cin >> txtFile;
        	ReadAutoMap(txtFile, fileMap);
        	cout << endl << "reading map file" << endl << "Minimized total energy expenditure" << endl << endl;
			GridArea::VecGridAreaType gridAreaMap = convertCharMapToGridAreaVector(fileMap);
			Vehicle* v = new HighLander(gridAreaMap);
			v->computeAllIdealRoutes();
			v->displayRouteReport();
			delete v;
			findShortestEnergyPath(fileMap, MYvehicleTypes[1]);


        	cout << endl << "reading map file" << endl << "Finding shortest route" << endl << endl;
        	findShortestPath(fileMap, MYvehicleTypes[1]);//manual set ShieldHero
    		}
			break;	
        	
        	case 5:
        	
        	break;
        	
        	default:
        	cout << endl << "Invalid Choice => " << userEnteredNumber << ", please try again!" << endl;
        	}
    }while (userEnteredNumber != 5);

    cout << "\nThank you for using this program, have a nice day!" << endl;

    return 0;
}


void getFilename(char *srcFilename) {
    ifstream file;

    while (true) {
        cout << "\nEnter the Scenario Number (or type 'exit' to quit): ";
        cin.getline(srcFilename, MAX_FILENAME_LENGTH);

        if (strcmp(srcFilename, "exit") == 0) {
            cout << "Exiting...\n";
            exit(0);
        }

        file.open(srcFilename, ios::binary);
        if (!file) {
            cerr << "Error opening file: " << srcFilename << "\n";
        } else {
            file.read(reinterpret_cast<char *>(&vehicle), sizeof(VehData));
            if (file.gcount() == sizeof(VehData)) {
                cout << "File read successfully.\n";
                file.close();
                return;
            } else {
                cerr << "Error reading data from file. Ensure the file is valid.\n";
                file.close();
            }
        }
    }
}

//function to print map

void printMmap() {
    cout << "\nCurrent Movement Map:\n";

    // Print column numbers with better spacing
    cout << "   "; // Space for row index
    for (size_t col = 0; col < mMap[0].size(); col++) {
        cout << setw(2) << col << " "; // Ensure proper spacing
    }
    cout << endl;

    // Print row numbers and map contents
    for (size_t row = 0; row < mMap.size(); row++) {
        cout << setw(2) << row << " "; // Print row number with spacing
        for (size_t col = 0; col < mMap[row].size(); col++) {
            cout << setw(2) << mMap[row][col] << " "; // Ensures even spacing
        }
        cout << endl;
    }
}

//function to resize vector
void vectorResize() {
    // Ensure mMap is not empty to prevent runtime errors
    if (mMap.empty()) return;

    int oldWidth = mMap[0].size();
    int oldHeight = mMap.size();

    // Expand right if near the right boundary
    if (xPosition >= oldWidth - 1) {
        for (auto& row : mMap) row.push_back(' ');
    }

    // Expand left if near the left boundary
    if (xPosition <= 0) { // Use 0 instead of 1 to avoid unnecessary shifts
        for (auto& row : mMap) row.insert(row.begin(), ' ');
        xPosition++;  // Adjust x-position after left shift
    }

    // Expand down if near the bottom boundary
    if (yPosition >= oldHeight - 1) {
        mMap.push_back(vector<char>(mMap[0].size(), ' '));
    }

    // Expand up if near the top boundary
    if (yPosition <= 0) { // Use 0 instead of 1 to avoid unnecessary shifts
        mMap.insert(mMap.begin(), vector<char>(mMap[0].size(), ' '));
        yPosition++;  // Adjust y-position after top shift
    }
}

void autoMapping(){




		

char scanResult;
bool godown = false;
bool switchDir = false;
char scanResultN,scanResultE, scanResultS, scanResultW;
long BeforeMoveE, AfterMoveE, BeforeMoveSE, AfterMoveSE, TerrainE, TerrainSE;

int i = 0;
	while(i == 0){
	// Move up until hit top barrier
	scanResult = vdt.scanNorth(vehicle);
		if (scanResult != '#'){
			vdt.moveUpNorth();
			vectorResize();
		} 
		else {
		//Move left until hit left barrier
			scanResult = vdt.scanWest(vehicle);
			if (scanResult != '#'){
				vdt.moveLeftWest();
				vectorResize();
			} 
			else{
				while(i == 0){
					//Scan Surroundings
					//Scan N
					scanResultN = vdt.scanNorth(vehicle);
					mMap[yPosition-1][xPosition] = scanResultN;
					//Scan NE
					scanResult = vdt.scanNorthEast(vehicle);
					mMap[yPosition-1][xPosition+1] = scanResult;
					//Scan E
					scanResultE = vdt.scanEast(vehicle);
					mMap[yPosition][xPosition+1] = scanResultE;
					//Scan SE
					scanResult = vdt.scanSouthEast(vehicle);
					mMap[yPosition+1][xPosition+1] = scanResult;
					//Scan S
					scanResultS = vdt.scanSouth(vehicle);
					mMap[yPosition+1][xPosition] = scanResultS;
					//Scan SW
					scanResult = vdt.scanSouthWest(vehicle);
					mMap[yPosition+1][xPosition-1] = scanResult;
					//Scan W
					scanResultW = vdt.scanWest(vehicle);
					mMap[yPosition][xPosition-1] = scanResultW;
					//Scan NW
					scanResult = vdt.scanNorthWest(vehicle);
					mMap[yPosition-1][xPosition-1] = scanResult;
					
					//printMmap(); //(for debugging)
					//Store Shield Energy & Energy BEFORE Moving
					BeforeMoveE = vehicle.getCurrentEnergy();
					BeforeMoveSE = vehicle.getCurrentShieldEnergy();
										
					//End AutoMapping at Bottom Right Corner
					if (scanResultE == '#' &&  scanResultS == '#') {
                        cout << "Mapping complete: No further movement possible!" << endl;
                        i = 1;
                        printMmap();
                        cout << left << setw(20)<<"Terrain Symbol"
                        		 << setw(25)<<"Movement Enrgy Reqd"
                        		 << setw(25) <<"Shield Energy reqd"<<endl;
                        for (int i = 0; i<10; i++){
                            if(terrainEnergyArray[i].movementEnergy == 0 && terrainEnergyArray[i].shieldEnergy == 0){
                            cout << setw(20) << terrainEnergyArray[i].terrainType
                        		 << setw(25) << "N.A"
								 << setw(25) << "N.A" << endl;
                        	}else{                   	
                        	cout << setw(20) << terrainEnergyArray[i].terrainType
                        		 << setw(25) << terrainEnergyArray[i].movementEnergy
								 << setw(25) << terrainEnergyArray[i].shieldEnergy << endl;
								 }
                        }
                    }
                    //AutoPath to the Right
					if(switchDir == false){
						//If Right Not Barrier Move Right
						if (scanResultE != '#'){
							vehicle = vdt.moveRightEast();
							//Store Shield Energy & Energy AFTER Moving
							AfterMoveE = vehicle.getCurrentEnergy();
							AfterMoveSE = vehicle.getCurrentShieldEnergy();

							//Store Shield Energy & Energy of TERRAIN
							TerrainE = BeforeMoveE - AfterMoveE;
							TerrainSE = BeforeMoveSE - AfterMoveSE;
							
							
							for (int i = 0; i<9; i++){
								if(terrainEnergyArray[i].terrainType == scanResult){
									terrainEnergyArray[i].shieldEnergy = TerrainSE;
									terrainEnergyArray[i].movementEnergy = TerrainE;
								}
							}
							
							
							
							xPosition += 1;
							vectorResize();
						} 
						else{// Right IS barrier godown and Switch Dir(Left)
							godown = true;
							switchDir = true;
						}
					}
					//Change Dir == True (Moving Left Now) 
					else{
						//Godown by 2
						if(godown == true){
							//If South Not Barrier Continue
							if (scanResultS != '#'){
								vdt.moveDownSouth();
								yPosition += 1;
								vectorResize();
								scanResultS = vdt.scanSouth(vehicle);
								if (scanResultS != '#'){
								vdt.moveDownSouth();
								yPosition += 1;
								vectorResize();
								}
								godown = false;
							}
						}
						//
						if(switchDir == true){
							scanResult = vdt.scanWest(vehicle);
							if (scanResult != '#'){
								vdt.moveLeftWest();
								xPosition -= 1;
								vectorResize();
							}
							else{
								godown = true;
								switchDir = false;
							}
		
						}
					}
		
		
		
				}
			}
		}
	}
}

//.txt for autoMap
void TransferAutoMap(){
    ofstream autoMapfile;
    autoMapfile.open(options.outputReportFile);
    if (!autoMapfile) {
        cerr << "Error opening file for writing." << endl;
        return;
    }
    // Write each row of mMap to the file.
    for (const auto &row : mMap) {
        for (const auto &ch : row) {
            autoMapfile << ch;
        }
        autoMapfile << "\n"; // Newline at the end of each row.
    }
    autoMapfile.close();
    cout << "file created" << endl;
}

//read map from .txt file
void ReadAutoMap(const string &filename, vector<vector<char>> &fileMap) {
    ifstream autoMapfile(filename);
    if (!autoMapfile) {
        cerr << "Error opening file for reading." << endl;
        return;
    }
    
    fileMap.clear(); // Ensure the vector is empty before reading new data
    string line;
    size_t maxCols = 0; 
    
    while (getline(autoMapfile, line)) {
        vector<char> row(line.begin(), line.end()); // Convert string to vector<char>
        fileMap.push_back(row);
        maxCols = max(maxCols, row.size());
    }
    
    autoMapfile.close();
    
    if (fileMap.empty()) {
        cout << "Warning: The file is empty or contains only blank lines." << endl;
        return;
    }
    
    size_t rows = fileMap.size();

    // Print column numbers with proper alignment
    cout << "   "; // Space for row numbers
    for (size_t col = 0; col < maxCols; ++col) {
        cout << setw(2) << col << " ";
    }
    cout << endl;

    // Print rows with row numbers
    for (size_t row = 0; row < rows; ++row) {
        cout << setw(2) << row << " " ; // Row number
        for (size_t col = 0; col < maxCols; ++col) {
            if (col < fileMap[row].size()) {
                cout << setw(2) << right << fileMap[row][col] << " ";
            } else {
                cout << setw(2) << ' ' << " "; // Fill empty spaces if rows are uneven
            }
        }
        cout << endl;
    }
    cout << endl;
    
}

	
// Function to find shortest energy path using Dijkstra (considers preferred terrains if available)
int findShortestEnergyPath(vector<vector<char>>& map, const MYVehicleType& currentVehicle) {
    int rows = map.size();
    int cols = map[0].size();

    // Step 1: Locate all start ('S') and end ('E') positions on the map
    vector<pair<int, int>> starts, ends;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (map[r][c] == 'S') starts.push_back({r, c});
            if (map[r][c] == 'E') ends.push_back({r, c});
        }
    }

    // Step 2: Error handling for missing start or end points
    if (starts.empty() || ends.empty()) {
        cout << "Error: No start or end points found!\n";
        return -1;
    }

    // Step 3: Create energy map (considering discounts for preferred terrains)
    unordered_map<char, int> energyMap;
    int terrainCount = sizeof(terrainEnergyArray) / sizeof(terrainEnergyArray[0]);
    for (int i = 0; i < terrainCount; ++i) {
        char terrainChar = terrainEnergyArray[i].terrainType;
        int movement = terrainEnergyArray[i].movementEnergy;

        // Apply discount if terrain is preferred by the vehicle
        if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrainChar) != currentVehicle.preferredTerrains.end()) {
            movement = static_cast<int>(movement * currentVehicle.discountRate);
        }

        energyMap[terrainChar] = movement;
    }

    int routeIndex = 1;

    // Step 4: Iterate over all start-end point pairs
    for (const auto& start : starts) {
        for (const auto& end : ends) {
            cout << "\nRoute " << routeIndex++
                 << " - From [" << start.first << ", " << start.second << "] to ["
                 << end.first << ", " << end.second << "]\n";

            // Step 5: Check if any preferred terrain exists on the map
            bool hasPreferred = false;
            for (const auto& row : map) {
                for (char cell : row) {
                    if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), cell) != currentVehicle.preferredTerrains.end()) {
                        hasPreferred = true;
                        break;
                    }
                }
                if (hasPreferred) break;
            }

            // Step 6: Dijkstra function to calculate energy-efficient path
            auto dijkstra = [&](pair<int, int> src, pair<int, int> dst) {
                vector<vector<int>> cost(rows, vector<int>(cols, INT_MAX));
                vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols, {-1, -1}));
                priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<>> pq;

                pq.push({0, src});
                cost[src.first][src.second] = 0;

                while (!pq.empty()) {
                    auto [currEnergy, curr] = pq.top(); pq.pop();
                    int r = curr.first, c = curr.second;

                    // If destination is reached, reconstruct path
                    if (curr == dst) {
                        vector<pair<int, int>> path;
                        for (pair<int, int> at = dst; at != make_pair(-1, -1); at = parent[at.first][at.second])
                            path.push_back(at);
                        reverse(path.begin(), path.end());
                        return path;
                    }

                    // Explore neighbors
                    for (int i = 0; i < 4; ++i) {
                        int nr = r + dRow[i], nc = c + dCol[i];
                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                            char terrain = map[nr][nc];
                            if (energyMap.count(terrain)) {
                                int moveEnergy = energyMap[terrain];
                                int newCost = currEnergy + moveEnergy;
                                if (newCost < cost[nr][nc]) {
                                    cost[nr][nc] = newCost;
                                    parent[nr][nc] = {r, c};
                                    pq.push({newCost, {nr, nc}});
                                }
                            }
                        }
                    }
                }

                return vector<pair<int, int>>(); // No path found
            };

            vector<pair<int, int>> finalPath;

            // Step 7A: Use S → preferred → E if preferred terrain exists
            if (hasPreferred) {
                // Compute midpoint of route
                pair<int, int> meanXY = {(start.first + end.first) / 2, (start.second + end.second) / 2};
                pair<int, int> bestPref = {-1, -1};
                int minDist = INT_MAX;

                // Find nearest preferred terrain to midpoint
                for (int r = 0; r < rows; ++r) {
                    for (int c = 0; c < cols; ++c) {
                        if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), map[r][c]) != currentVehicle.preferredTerrains.end()) {
                            int dist = abs(r - meanXY.first) + abs(c - meanXY.second);
                            if (dist < minDist) {
                                minDist = dist;
                                bestPref = {r, c};
                            }
                        }
                    }
                }

                // If no preferred terrain is reachable
                if (bestPref.first == -1) {
                    cout << "Preferred terrain unreachable.\n";
                    continue;
                }

                // Use Dijkstra from S → preferred, then preferred → E
                auto path1 = dijkstra(start, bestPref);
                auto path2 = dijkstra(bestPref, end);
                if (path1.empty() || path2.empty()) {
                    cout << "Path not reachable through preferred terrain.\n";
                    continue;
                }

                // Merge paths (remove duplicate at junction)
                path1.pop_back();
                path1.insert(path1.end(), path2.begin(), path2.end());
                finalPath = path1;

            } else {
                // Step 7B: No preferred terrain → direct S → E
                finalPath = dijkstra(start, end);
                if (finalPath.empty()) {
                    cout << "No valid direct path found.\n";
                    continue;
                }
            }

            // Step 8: Prepare for energy tracking and visual output
            int preferredCount = 0, totalEnergy = 0, discountedTotal = 0;
            vector<vector<char>> visualMap(rows, vector<char>(cols, ' '));
            for (int r = 0; r < rows; ++r)
                for (int c = 0; c < cols; ++c)
                    if (map[r][c] == '#') visualMap[r][c] = '#';

            // Print path sequence and count preferred terrains
            cout << "\nPath Sequence:\n";
            for (size_t i = 0; i < finalPath.size(); ++i) {
                auto [r, c] = finalPath[i];
                cout << "[" << r << ", " << c << "]";
                if (i != finalPath.size() - 1) cout << ", ";

                char terrain = map[r][c];
                if (terrain == 'S' || terrain == 'E') continue;

                if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrain) != currentVehicle.preferredTerrains.end()) {
                    preferredCount++;
                }
            }
			 // Step 7: Output metrics
            cout << "\n\nTot. No. of PREFERRED TERRAIN explored = " << preferredCount << endl;
            
            // Count steps and preferred terrains
            cout << "\nSteps taken [excluding 'S' and 'E']: " << finalPath.size() - 2 << endl;

            cout << "\nTot. Movt Enrg Reqd (Generic Veh) = ";
            for (size_t i = 1; i < finalPath.size(); ++i) {
                int r = finalPath[i].first, c = finalPath[i].second;
                char terrain = map[r][c];

                // Find energy value for terrain
                int moveEnergy = 0;
                for (int j = 0; j < terrainCount; ++j) {
                    if (terrainEnergyArray[j].terrainType == terrain) {
                        moveEnergy = terrainEnergyArray[j].movementEnergy;
                        break;
                    }
                }

                cout << terrain << "(" << moveEnergy << ")";
                if (i != finalPath.size() - 1) cout << " + ";
                totalEnergy += moveEnergy;
            }
            cout << " = \033[1;31m" << totalEnergy << "\033[0m\n";

            // Step 9: Compute discounted energy cost for this vehicle
            cout << "\nTot. Movt Enrg Reqd (" << currentVehicle.name << ") = ";
            cout << totalEnergy << " - [";

            for (size_t i = 1; i < finalPath.size() - 1; ++i) {
                int r = finalPath[i].first, c = finalPath[i].second;
                char terrain = map[r][c];

                int moveEnergy = 0;
                for (int j = 0; j < terrainCount; ++j) {
                    if (terrainEnergyArray[j].terrainType == terrain) {
                        moveEnergy = terrainEnergyArray[j].movementEnergy;
                        break;
                    }
                }

                // Apply discount if terrain is preferred
                if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrain) != currentVehicle.preferredTerrains.end()) {
                    
                    discountedTotal += static_cast<int>(moveEnergy * currentVehicle.discountRate);
                } else {
                    discountedTotal += moveEnergy;
                }
            }

            // Print energy saved and preferred terrain count
            cout << preferredCount << " * " << currentVehicle.discountRate << " (";
            for (char t : currentVehicle.preferredTerrains) cout << t << " ";
            cout << ")] = \033[1;31m" << discountedTotal << "\033[0m\n";

            

            // Step 10: Mark path on visual map
            for (auto [r, c] : finalPath) {
                char ch = map[r][c];
                if (ch == ' ') ch = '_';
                visualMap[r][c] = ch;
            }

            visualMap[start.first][start.second] = 'S';
            visualMap[end.first][end.second] = 'E';

            // Step 11: Print final visual map
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

    return 0;
}


vector<pair<int, int>> bfsPath(
    const vector<vector<char>>& map,
    const pair<int, int>& start,
    const pair<int, int>& goal,
    const unordered_map<char, int>& energyMap
) {
    int rows = map.size();
    int cols = map[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols, {-1, -1}));

    queue<pair<int, int>> q;
    q.push(start);
    visited[start.first][start.second] = true;

    // Direction arrays
    int dRow[4] = {-1, 1, 0, 0};
    int dCol[4] = {0, 0, -1, 1};

    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();

        if (make_pair(r, c) == goal) {
            // Reconstruct path
            vector<pair<int, int>> path;
            for (pair<int, int> at = goal; at != make_pair(-1, -1); at = parent[at.first][at.second])
                path.push_back(at);
            reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 4; i++) {
            int nr = r + dRow[i], nc = c + dCol[i];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                char terrain = map[nr][nc];
                if (!visited[nr][nc] && terrain != '#' && energyMap.count(terrain)) {
                    visited[nr][nc] = true;
                    parent[nr][nc] = {r, c};
                    q.push({nr, nc});
                }
            }
        }
    }

    return {}; // No path found
}
bool hasPreferredTerrain(const vector<vector<char>>& map, const vector<char>& preferredTerrains) {
    for (const auto& row : map) {
        for (char cell : row) {
            if (find(preferredTerrains.begin(), preferredTerrains.end(), cell) != preferredTerrains.end()) {
                return true;
            }
        }
    }
    return false;
}

// Function to find and print the shortest path (step-based) from 'S' to 'E' using BFS
int findShortestPath(vector<vector<char>>& map, const MYVehicleType& currentVehicle) {
    int rows = map.size();
    int cols = map[0].size();

    // Step 1: Find all starting ('S') and ending ('E') positions in the map
    vector<pair<int, int>> starts, ends;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (map[r][c] == 'S') starts.push_back({r, c});
            if (map[r][c] == 'E') ends.push_back({r, c});
        }
    }

    // Safety check: Make sure there is at least one 'S' and one 'E'
    if (starts.empty() || ends.empty()) {
        cout << "Error: No start or end points found!\n";
        return -1;
    }

    // Step 2: Create a map of terrain energy costs (with discount for preferred terrains)
    unordered_map<char, int> energyMap;
    int terrainCount = sizeof(terrainEnergyArray) / sizeof(terrainEnergyArray[0]);
    for (int i = 0; i < terrainCount; ++i) {
        char terrainChar = terrainEnergyArray[i].terrainType;
        int movement = terrainEnergyArray[i].movementEnergy;

        // Apply discount for preferred terrain types
        if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrainChar) != currentVehicle.preferredTerrains.end()) {
            movement = static_cast<int>(movement * currentVehicle.discountRate);
        }

        energyMap[terrainChar] = movement;
    }

    int routeIndex = 1;

    // Step 3: Loop over every start-end pair
    for (const auto& start : starts) {
        for (const auto& end : ends) {
            cout << "\nRoute " << routeIndex++
                 << " - From [" << start.first << ", " << start.second << "] to ["
                 << end.first << ", " << end.second << "]\n";

            vector<pair<int, int>> fullPath;

            // Step 4: Check if there is any preferred terrain on the map
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

            // Step 5A: If preferred terrain exists, find nearest one to mid-point between S and E
            if (hasPref) {
                pair<int, int> meanXY = {
                    (start.first + end.first) / 2,
                    (start.second + end.second) / 2
                };

                pair<int, int> nearestPref = {-1, -1};
                int minDist = INT_MAX;

                // Find closest preferred terrain to meanXY
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

                // If no reachable preferred terrain, skip this route
                if (nearestPref.first == -1) {
                    cout << "Preferred terrain not reachable.\n";
                    continue;
                }

                // Run BFS from S → PREF, then PREF → E
                auto path1 = bfsPath(map, start, nearestPref, energyMap);
                auto path2 = bfsPath(map, nearestPref, end, energyMap);

                // Skip if no valid paths found
                if (path1.empty() || path2.empty()) {
                    cout << "No valid path found from S → PREF or PREF → E.\n";
                    continue;
                }

                // Combine the paths (remove duplicate junction node)
                path1.pop_back();
                path1.insert(path1.end(), path2.begin(), path2.end());
                fullPath = path1;
            } else {
                // Step 5B: No preferred terrain — use direct S → E BFS
                fullPath = bfsPath(map, start, end, energyMap);
                if (fullPath.empty()) {
                    cout << "No valid path found.\n";
                    continue;
                }
            }

            // Step 6: Path processing and visualization
            vector<vector<char>> visualMap(rows, vector<char>(cols, ' '));
            for (int r = 0; r < rows; ++r)
                for (int c = 0; c < cols; ++c)
                    if (map[r][c] == '#') visualMap[r][c] = '#'; // Copy walls

            int preferredCount = 0, totalEnergy = 0, discountedTotal = 0;

            // Print path sequence and count preferred terrains
            cout << "\nPath Sequence:\n";
            for (size_t i = 0; i < fullPath.size(); ++i) {
                auto [r, c] = fullPath[i];
                cout << "[" << r << ", " << c << "]";
                if (i != fullPath.size() - 1) cout << ", ";

                char terrain = map[r][c];
                if (terrain == 'S' || terrain == 'E') continue;

                if (find(currentVehicle.preferredTerrains.begin(), currentVehicle.preferredTerrains.end(), terrain) != currentVehicle.preferredTerrains.end()) {
                    preferredCount++;
                }
            }

            // Step 7: Output metrics
            cout << "\n\nTot. No. of PREFERRED TERRAIN explored = \n" << preferredCount << endl;
            cout << "Steps taken [excluding 'S' and 'E']: " << fullPath.size() - 2 << endl;

            // Step 8: Print energy usage for generic vehicle (no discounts)
            cout << "\nTot. Movt Enrg Reqd (Generic Veh) = ";
            for (size_t i = 1; i < fullPath.size(); ++i) {
                auto [r, c] = fullPath[i];
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

            // Step 9: Print energy usage for current vehicle with discounts
            cout << "\nTot. Movt Enrg Reqd (" << currentVehicle.name << ") = ";
            cout << totalEnergy << " - [";

            for (size_t i = 1; i < fullPath.size() - 1; ++i) {
                auto [r, c] = fullPath[i];
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
                } else {
                    discountedTotal += moveEnergy;
                }
            }

            // Print discounted energy summary
            cout << preferredCount << " * " << currentVehicle.discountRate << " (";
            for (char t : currentVehicle.preferredTerrains) cout << t << " ";
            cout << ")] = \033[1;31m" << discountedTotal << "\033[0m\n";

            // Step 10: Mark the full path on the visual map
            for (auto [r, c] : fullPath) {
                char ch = map[r][c];
                if (ch == ' ') ch = '_';
                visualMap[r][c] = ch;
            }

            // Mark start and end
            visualMap[start.first][start.second] = 'S';
            visualMap[end.first][end.second] = 'E';

            // Step 11: Print the visual map with coordinates
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

    return 0;
}




//print initial msg
void printWelcomeMsg(){
cout << "\nTeam Number      : <Enter Team number>\n";
        cout << "Team Leader Name   : <Geremy Fruity>   \n";
        cout << "--------------------------------------------\n";
        cout << "Welcome to Team <Enter Team number here> Group Project\n";
        
}
int printMainMenuOptions(){
int userEnteredNumber;
		cout << "1)    Configure Autopilot Mapping Exploration settings\n";
        cout << "2)    Configure Terrain Exploration Simulator settings\n";
        cout << "3)    Start Autopilot Mapping!\n";
        cout << "4)    Start Simulation!\n";
        cout << "5)    End\n";
        cout << "\nPlease enter your choice: ";
    cout << "Please enter your choice (1 - 5): ";
    cin >> userEnteredNumber;

    return (userEnteredNumber);
}
GridArea::VecGridAreaType convertCharMapToGridAreaVector(const vector<vector<char>>& charMap) {
    GridArea::VecGridAreaType gridAreaMap;

    for (int y = 0; y < charMap.size(); ++y) {
        for (int x = 0; x < charMap[y].size(); ++x) {
            char symbol = charMap[y][x];
            TERRAIN_TYPE terrain = GridArea::convertCharToTerrainType(symbol);

            long move = 0, shield = 0;

            // Use terrainEnergyArray to look up energies
            for (int i = 0; i < 10; ++i) {
                if (terrainEnergyArray[i].terrainType == symbol) {
                    move = terrainEnergyArray[i].movementEnergy;
                    shield = terrainEnergyArray[i].shieldEnergy;
                    break;
                }
            }

            GridArea grid(x, y, terrain, shield, move);
            gridAreaMap.push_back(grid);
        }
    }

    return gridAreaMap;
}

