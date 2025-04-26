#include "Utils.hpp"
#include "PolygonalMesh.hpp"
#include "UCDUtilities.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;

int main()
{
    PolygonalMesh mesh;

    if(!ImportMesh(mesh)){
        cerr << "Error importing mesh data" << endl;
        return 1;
    };

    /* Test marker Cell0Ds, Cell1Ds, Cell2Ds */
    // Create a boolean to check if there aren't any problem during the test
    bool CellTest0 = true; 
    bool CellTest1 = true;
    bool CellTest2 = true;
    
    //Import the files
    ifstream file0("./Cell0Ds.csv");
    ifstream file1("./Cell1Ds.csv");
    ifstream file2("./Cell2Ds.csv");

    string firstline0;
    string firstline1;
    string firstline2;

    // Descard first line of every file
    getline(file0, firstline0);
    getline(file1, firstline1);
    getline(file2, firstline2);
    
    list<string> LIstLines0;
    list<string> LIstLines1;
    list<string> LIstLines2;
    
    string line0;
    string line1;
    string line2;
    // List of every line of the files
    while(getline(file0, line0)) {
        LIstLines0.push_back(line0);
    }
    while(getline(file1, line1)) {
        LIstLines1.push_back(line1);
    }
    while(getline(file2, line2)) {
        LIstLines2.push_back(line2);
    }
    file0.close();
    file1.close();
    file2.close();
    // Check inside the lists if the marker is different from zero and it's different from the last position
    // Cell0Ds
    for(string& line0 : LIstLines0) {
        replace(line0.begin(), line0.end(), ';', ' ');
        istringstream converter(line0);
        unsigned int id;
        unsigned int marker;
        converter >> id >> marker;
        if(marker != 0) {
            auto itor = mesh.MarkerCell0Ds.find(marker);
            if(itor == mesh.MarkerCell0Ds.end()) {
                CellTest0 = false;
                break;
            }
        }
    }
    if(CellTest0 == false) {
        cout << "There's an error during the storage of 0d markers" << endl;
        return 1;
    } else {
        cout << "Correct storage of 0d markers" << endl;
    }
    // Cell1Ds
    for(string& line1 : LIstLines1) {
        replace(line1.begin(), line1.end(), ';', ' ');
        istringstream converter(line1);
        unsigned int id;
        unsigned int marker;
        converter >> id >> marker;
        if(marker != 0) {
            auto itor = mesh.MarkerCell1Ds.find(marker);
            if(itor == mesh.MarkerCell1Ds.end()) {
                CellTest1 = false;
                break;
            }
        }
    }
    if(CellTest1 == false) {
        cout << "There's an error during the storage of 1d markers" << endl;
        return 2;
    } else {
        cout << "Correct storage of 1d markers" << endl;
    } 
    // Cell2Ds
    for(string& line2 : LIstLines2) {
        replace(line2.begin(), line2.end(), ';', ' ');
        istringstream converter(line2);
        unsigned int id;
        unsigned int marker;
        converter >> id >> marker;
        if(marker != 0) {
            auto itor = mesh.MarkerCell2Ds.find(marker);
            if(itor == mesh.MarkerCell2Ds.end()) {
                CellTest2 = false;
                break;
            }
        }
    }
    if(CellTest2 == false) {
        cout << "There's an error during the storage of 2d markers" << endl;
        return 3;
    } else {
        cout << "Correct storage of 2d markers" << endl;
    }

    /* Test area and segment */
    const double tol = 1e-8; //set the tolerance

    SegmentCheck(mesh, tol);
    AreaCheck(mesh, tol);

    // Visual test 
    Gedim::UCDUtilities utilities;
    utilities.ExportPoints("./Cell0Ds.inp",
                           mesh.Cell0DsCoordinates);

    utilities.ExportSegments("./Cell1Ds.inp",
                             mesh.Cell0DsCoordinates,
                             mesh.Cell1DsExtrema);


    return 0;
}
