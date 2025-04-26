#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace PolygonalLibrary
{
bool ImportMesh(PolygonalMesh& mesh)
    {
        if(!ImportCell0Ds(mesh))
            return false;
    
        if(!ImportCell1Ds(mesh))
            return false;
    
        if(!ImportCell2Ds(mesh))
            return false;
    
        return true;
    
    }


bool ImportCell0Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell0Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;

    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell0Ds = listLines.size();

    if (mesh.NumCell0Ds == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DsId.reserve(mesh.NumCell0Ds);
    mesh.Cell0DsCoordinates = Eigen::MatrixXd::Zero(3, mesh.NumCell0Ds);

    for (const string& line : listLines)
    {
        string lineCopy = line;
        replace(lineCopy.begin(), lineCopy.end(), ';', ' ');
        istringstream converter(lineCopy);

        unsigned int id;
        unsigned int marker;

        converter >>  id >>  marker >> mesh.Cell0DsCoordinates(0, id) >> mesh.Cell0DsCoordinates(1, id);

        mesh.Cell0DsId.push_back(id);

        /// Memorizza i marker
        if(marker != 0)
        {
           auto itor = mesh.MarkerCell0Ds.find(marker);
           if(itor == mesh.MarkerCell0Ds.end()){
            mesh.MarkerCell0Ds.insert({marker, {id}});
           } else {
            itor->second.push_back(id);
           }
        }

    }

    return true;
}
// ***************************************************************************
bool ImportCell1Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell1Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell1Ds = listLines.size();

    if (mesh.NumCell1Ds == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    mesh.Cell1DsExtrema = Eigen::MatrixXi(2, mesh.NumCell1Ds);

    for (const string& line : listLines)
    {
        string lineCopy = line;
        replace(lineCopy.begin(), lineCopy.end(), ';', ' ');
        istringstream converter(lineCopy);

        unsigned int id;
        unsigned int marker;

        converter >>  id >> marker >>  mesh.Cell1DsExtrema(0, id) >>  mesh.Cell1DsExtrema(1, id);
        mesh.Cell1DsId.push_back(id);

        /// Memorizza i marker
        if(marker != 0)
        {
            const auto itor = mesh.MarkerCell1Ds.find(marker);
            if(itor == mesh.MarkerCell1Ds.end())
            {
                mesh.MarkerCell1Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell1Ds[marker].push_back(id);
                itor->second.push_back(id);
            }
        }
    }

    return true;
}
// ***************************************************************************
bool ImportCell2Ds(PolygonalMesh& mesh)
{
    ifstream file;
    file.open("./Cell2Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell2Ds = listLines.size();

    if (mesh.NumCell2Ds == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DsId.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsVertices.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsEdges.reserve(mesh.NumCell2Ds);

    for (const string& line : listLines)
    {
        string lineCopy = line;
        replace(lineCopy.begin(), lineCopy.end(), ';', ' ');
        istringstream converter(lineCopy);

        unsigned int id;
        unsigned int marker;
        unsigned int NumVertices;
        unsigned int NumEdges;
        vector<unsigned int> vertices;
        vector<unsigned int> edges;

        converter >> id >> marker >> NumVertices;

        if(NumVertices < 3) {
            cerr << "Not enough vertices for a non degenerate polygon" << endl;
            return false;
        }
        
        vertices.resize(NumVertices);

        for(unsigned int i = 0; i < NumVertices; i++){
            converter >> vertices[i];
        }

        converter >> NumEdges;

        if(NumEdges < 3) {
            cerr << "Not enough edges for a non degenerate polygon" << endl;
            return false;
        }

        edges.resize(NumEdges);

        for(unsigned int i = 0; i < NumEdges; i++)
            converter >> edges[i];

        mesh.Cell2DsId.push_back(id);
        mesh.Cell2DsVertices.push_back(vertices);
        mesh.Cell2DsEdges.push_back(edges);

        /// Memorizza i marker
        if(marker != 0)
        {
            const auto itor = mesh.MarkerCell2Ds.find(marker);
            if(itor == mesh.MarkerCell2Ds.end())
            {
                mesh.MarkerCell2Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell1Ds[marker].push_back(id);
                itor->second.push_back(id);
            }
        }
    }

    return true;
}

bool SegmentCheck(PolygonalMesh& mesh, const double& tol)
{
    for(unsigned int id = 0; id < mesh.NumCell1Ds; id++)
    {
        int v1 = mesh.Cell1DsExtrema(0, id);
        int v2 = mesh.Cell1DsExtrema(1, id);
        double x1 = mesh.Cell0DsCoordinates(0, v1);
        double x2 = mesh.Cell0DsCoordinates(0, v2);
        double y1 = mesh.Cell0DsCoordinates(1, v1);
        double y2 = mesh.Cell0DsCoordinates(1, v2);


        double distance = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));

        if(distance < tol){
            cerr << "One segment has lenght lower than: " << tol << endl;
            return false;
        }
    }

    cout << "All segments have lenght bigger than: " << tol << endl;
    
    return true; 
}

bool AreaCheck(PolygonalMesh& mesh, const double& tol)
{
    /*First I have to calculate the area of the equilateral triangle with side length tol.*/
    const double AreaTol = (tol * tol)*sqrt(3.0) / 4.0;
    bool AllValid = true;
    
    for(unsigned int id = 0; id < mesh.NumCell2Ds; id++)
    {
        const vector<unsigned int>& vertices = mesh.Cell2DsVertices[id];
        const unsigned int NumVertices = vertices.size();
        
        if(NumVertices < 3) {
            cerr << "Polygon " << id << " has less than 3 vertices" << endl;
            return false;
        }
        
        double area = 0.0;

        for(unsigned int i = 0; i < NumVertices; i++) {
            const unsigned int v1 = vertices[i];
            const unsigned int v2 = vertices[(i + 1) % NumVertices]; // the module guarantee that the last vertex is connected back with the first one 
            
            double x1 = mesh.Cell0DsCoordinates(0, v1);
            double x2 = mesh.Cell0DsCoordinates(0, v2);
            double y1 = mesh.Cell0DsCoordinates(1, v1);
            double y2 = mesh.Cell0DsCoordinates(1, v2);

            area += (x1 * y2 - x2 * y1);
            }

            area = 0.5 * abs(area);

            if(area <= AreaTol) {
                cerr << "Polygon " << id << "has area " << area << "that is <= tol of the area " << AreaTol << endl;
                AllValid = false;
            }
        }
    
    if (AllValid){
        cout << "All polygons have a bigger area than: " << AreaTol << endl;
    
        }
    return AllValid;
    

}
}