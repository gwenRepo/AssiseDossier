#ifndef FONC_
#define FONC_

#include <Geom_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <vector>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>


#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>

/* --------------- */
/* --- CLASSES --- */
/* --------------- */
class Assise
{
private:
    double _longeur, _largeur, _epaisseur, _profondeur;
    TopoDS_Face _faceAssise;
    TopoDS_Solid _solidAssise;
    TopoDS_Shape _shapeAssise;

public:
    Assise(double longueur, double largeur, double epaisseur, double profondeur);
    ~Assise();
    void generateFaceAssise();
    void generateSolidAssise();
    TopoDS_Solid generateSolidEstampe(double surepaisseur);
    void generateLamages();
    void writeSolid2File(string chemin);
    void writeShape2File(string chemin);
    void writeEstampe2File(double surepaisseur, string chemin);
    TopoDS_Face getFaceAssise();
    TopoDS_Solid getSolidAssise();
    TopoDS_Shape getShapeAssise();
    Handle(Geom_Surface) getSurfaceAssise();

};




/* ------------- */
/* --- SURFS --- */
/* ------------- */

/* surface de test pour assise */
Handle(Geom_BSplineSurface) generate77surface(double lon, double la, double h, double p);
Handle(Geom_BSplineSurface) generate77test();

/* surface de test pour banc sino et utils */
Handle(Geom_BSplineSurface) generate1313surface();
vector<TopoDS_Vertex> generate713poles();
TopoDS_Edge generateEdge1();
TopoDS_Edge generateEdge2();

/* surfaces de test pour 'coin conique' -> habillage étagères cuisine */
Handle(Geom_BSplineSurface) generate44surface();
Handle(Geom_BSplineSurface) generate77surfaceCoin();

/* ------------- */
/* --- UTILS --- */
/* ------------- */

/* récupérer les poles à partir d'une Bspline  surface (pour affichage, utilisé dans VTKutils) */
vector<TopoDS_Vertex> getPolesFromSurf(Handle(Geom_BSplineSurface));


#endif