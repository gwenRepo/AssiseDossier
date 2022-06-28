using namespace std;

#include <iostream>

#include <Fonctions.hpp>
#include <VtkUtils.hpp>
#include "../../FiletsUtils/include/FiletsUtils.hpp"
#include "../../VTKUtils/include/VtkUtils.hpp"

#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopTools.hxx>
#include <BRepTools.hxx>
#include <gp_Vec.hxx>

//vtk
#include <vtkAutoInit.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkOCC_Shape.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)


int main(int argc, char *argv[])
{
/***************************
 * Traitement des arguments
 * ************************/
if(argc!= 3){
    cout << "Nombre d'argumnts incorrect" <<endl;
    cout << "---------------------------" <<endl;
    cout << "AssiseDossier CheminFichiers VTKrender" <<endl;
    cout << "---------------------------" <<endl;
    return 1;
}
int VTKrender = stoi(argv[2]);
stringstream sstr; sstr << argv[1] ;

/***************************
 * algo
 * ************************/
Assise assise(410,290,18,5);
Assise assise2(410,290,18,7.5);
Assise assise3(410,290,18,10);
// stockage dans un fichier
stringstream sstr2;
sstr2 << sstr.str() <<"/solideAssise.brep";
string path2file = sstr2.str();
assise3.writeSolid2File(path2file);
stringstream sstr21;
sstr21 << sstr.str() <<"/solideEstampe.brep";
string path2file21 = sstr21.str();
assise3.writeEstampe2File(0.1,path2file21);
stringstream sstr3;
sstr3 << sstr.str() <<"/shapeAssise.brep";
string path2file2 = sstr3.str();
assise3.writeShape2File(path2file2);
/***************************
 * affichage VTK
 * ************************/
if(VTKrender == 1){
    //renderer et rendering window
    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renwin = SetUpRenderingWindow(ren, 1000, 800);
    //interactor
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> istyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renwin);
    iren->SetInteractorStyle(istyle);
    //scene
    double color[3];color[0]=1.0;color[1]=1.0;color[2]=1.0;//blanc
    double color2[3];color2[0]=0.0;color2[1]=1.0;color2[2]=0.0;//vert
    double color3[3];color3[0]=1.0;color3[1]=1.0;color3[2]=0.0;//jaune

    AddIsos2Renderer(ren,assise3.getSurfaceAssise(),30,40,color); //surface en isos
    AddActor2Renderer(ren,assise3.getShapeAssise(),0,color3,1);

    //affichage
    renwin->Render();
    iren->Start();
}
return 0;
}