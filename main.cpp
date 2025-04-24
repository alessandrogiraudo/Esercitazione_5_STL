#include "Utils.hpp"
#include "PolygonalMesh.hpp"
#include "UCDUtilities.hpp"
#include <iostream>

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

    // Test marker



    // Test area and segment
    const double tol = 1e-8; 

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
