using namespace std;
#include <iostream>
#include "../include/Fonctions.hpp"
#include <math.h>

#include "../../gwenRepo/FilesUtils/include/FileUtils.hpp"
#include "../../gwenRepo/AssyUtils/include/AssyUtils.hpp"



#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepFill.hxx>
#include <TopoDS_Shell.hxx>

#include <Geom_Curve.hxx>

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>


/* -------------- */
/* --- ASSISE --- */
/* -------------- */
Assise::Assise(double longueur, double largeur, double epaisseur, double profondeur)
{
    _longeur = longueur; _largeur = largeur; _epaisseur = epaisseur; _profondeur = profondeur;
    generateFaceAssise();
    generateSolidAssise();
    generateLamages();
}

Assise::~Assise()
{
}

void Assise::generateFaceAssise(){
    Handle(Geom_BSplineSurface) s = generate77surface(_longeur,_largeur,_epaisseur, _profondeur);
    _faceAssise = BRepBuilderAPI_MakeFace(s,0.000001);
}

void Assise::generateSolidAssise(){
    //recuparation des 4 points d'extremite de la face superieure. Elle doit exister ...
    Handle(Geom_Surface) s;
    s = BRep_Tool::Surface(_faceAssise);
    gp_Pnt pUminVmin, pUminVmax, pUmaxVmin, pUmaxVmax;
    Standard_Real Umin,Umax,Vmin,Vmax;
    s->Translate(gp_Vec(0,0,-_epaisseur));
    s->Bounds(Umin,Umax,Vmin,Vmax);
    s->D0(Umin,Vmin,pUminVmin);s->D0(Umin,Vmax,pUminVmax);s->D0(Umax,Vmin,pUmaxVmin);s->D0(Umax,Vmax,pUmaxVmax);
    s->Translate(gp_Vec(0,0,_epaisseur));

     Handle(Geom_Curve) c1;c1 = s->VIso(Vmin);//c1->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(c1,Umin,Umax);
     Handle(Geom_Curve) c3;c3 = s->VIso(Vmax);//c3->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(c3,Umin,Umax);       
    Handle(Geom_Curve) c2;c2 = s->UIso(Umax);//c2->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(c2,Vmin,Vmax);   
    Handle(Geom_Curve) c4;c4 = s->UIso(Umin);//c4->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(c4,Vmin,Vmax);
    TopoDS_Wire w1 = BRepBuilderAPI_MakeWire(e1,e2,e3,e4);

    TopoDS_Wire w = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(pUminVmin,pUmaxVmin),
        BRepBuilderAPI_MakeEdge(pUmaxVmin,pUmaxVmax),
        BRepBuilderAPI_MakeEdge(pUmaxVmax,pUminVmax),
        BRepBuilderAPI_MakeEdge(pUminVmax,pUminVmin));
    TopoDS_Face faceInferieure = BRepBuilderAPI_MakeFace(w);

    TopoDS_Shell sh = BRepFill::Shell(w,w1);
    BRepBuilderAPI_Sewing sew;
    sew.Add(_faceAssise);
    sew.Add(sh);
    sew.Add(faceInferieure);
    sew.Perform();
    if(sew.NbFreeEdges()==0){
    _solidAssise = BRepBuilderAPI_MakeSolid(TopoDS::Shell(sew.SewedShape()));}
    else{cout << "nb free edge != 0" << endl;}

}
TopoDS_Solid Assise::generateSolidEstampe(double surepaisseur){
    //recuparation des 4 points d'extremite de la face inferieure. Elle doit exister ...
    //mise à la hauteur souhaitée par translation de la surface avant recupertion
    Handle(Geom_Surface) s;
    s = BRep_Tool::Surface(_faceAssise);
    gp_Pnt pUminVmin, pUminVmax, pUmaxVmin, pUmaxVmax;
    Standard_Real Umin,Umax,Vmin,Vmax;
    s->Translate(gp_Vec(0,0,surepaisseur));
    s->Bounds(Umin,Umax,Vmin,Vmax);
    s->D0(Umin,Vmin,pUminVmin);s->D0(Umin,Vmax,pUminVmax);s->D0(Umax,Vmin,pUmaxVmin);s->D0(Umax,Vmax,pUmaxVmax);
    s->Translate(gp_Vec(0,0,-surepaisseur));
    //recuparation des edges de la face assise
     Handle(Geom_Curve) c1;c1 = s->VIso(Vmin);//c1->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(c1,Umin,Umax);
     Handle(Geom_Curve) c3;c3 = s->VIso(Vmax);//c3->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(c3,Umin,Umax);       
    Handle(Geom_Curve) c2;c2 = s->UIso(Umax);//c2->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(c2,Vmin,Vmax);   
    Handle(Geom_Curve) c4;c4 = s->UIso(Umin);//c4->Translate(gp_Vec(0,0,-_epaisseur));
    TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(c4,Vmin,Vmax);
    TopoDS_Wire w1 = BRepBuilderAPI_MakeWire(e1,e2,e3,e4);
    //consruction du wire de la face supereiure
    TopoDS_Wire w = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(pUminVmin,pUmaxVmin),
        BRepBuilderAPI_MakeEdge(pUmaxVmin,pUmaxVmax),
        BRepBuilderAPI_MakeEdge(pUmaxVmax,pUminVmax),
        BRepBuilderAPI_MakeEdge(pUminVmax,pUminVmin));
    TopoDS_Face faceSuperieure = BRepBuilderAPI_MakeFace(w);
    //construction du shell
    TopoDS_Shell sh = BRepFill::Shell(w,w1);
    BRepBuilderAPI_Sewing sew;
    sew.Add(_faceAssise);
    sew.Add(sh);
    sew.Add(faceSuperieure);
    sew.Perform();
    if(sew.NbFreeEdges()!=0){cout << "nb free edge != 0" << endl;}
    TopoDS_Solid sol = BRepBuilderAPI_MakeSolid(TopoDS::Shell(sew.SewedShape()));
    return sol;}


void Assise::generateLamages(){
    vector<gp_Ax2> axes; vector<double> rayons; vector<double> profondeurs;
    gp_Ax2 a0(gp_Pnt(0,0,0),gp_Dir(0,0,-1));
    
    axes.push_back(a0);//,a1,a2,a3,a4,a5,a6,a7,a8,a9;
    rayons.push_back(7.5);
    profondeurs.push_back(50);
    _shapeAssise = removeCylinders(_solidAssise, axes, rayons, profondeurs);
}


void Assise::writeSolid2File(string chemin){
    writeBRepFile(_solidAssise,chemin);
}

void Assise::writeEstampe2File(double surepaisseur, string chemin){
    TopoDS_Solid e = generateSolidEstampe(surepaisseur);
    writeBRepFile(e,chemin);}

void Assise::writeShape2File(string chemin){
    writeBRepFile(_shapeAssise,chemin);
}

TopoDS_Face Assise::getFaceAssise(){
return _faceAssise;}

TopoDS_Solid Assise::getSolidAssise(){
    return _solidAssise;}

TopoDS_Shape Assise::getShapeAssise(){
    return _shapeAssise;}

Handle(Geom_Surface) Assise::getSurfaceAssise(){
    Handle(Geom_Surface) s;
    s = BRep_Tool::Surface(_faceAssise);
    return s;}



/* --------------- */
/* --- DOSSIER --- */
/* --------------- */

/* surface de test pour assise */
Handle(Geom_BSplineSurface) generate77surface(double lon, double la, double h, double p){
    //parametres de forme généraux
   /* double h = _ep; //hauteur
    double p = 30; //profondeur maximale (centre)
    double lon = 500; //longueur
    double la = 400; //largeur*/

    //Bsplines de degré 3 pour assurer la tangence à l'extérieur et au centre
    //2 morceaux par coté, 7 poles, 3 noeuds (un partagé)
    // +: noeuds  o: autres poles
    // l7 +----o----o----+----o----o----+
    //    |    |    |    |    |    |    |
    // l6 o----o----o----o----o----o----o
    //    |    |    |    |    |    |    |
    // l5 o----o----o----o----o----o----o
    //    |    |    |    |    |    |    |
    // l4 +----o----o----+----o----o----+
    //    |    |    |    |    |    |    |
    // l3 o----o----o----o----o----o----o
    //    |    |    |    |    |    |    |
    // l2 o----o----o----o----o----o----o
    //    |    |    |    |    |    |    |
    // l1 +----o----o----+----o----o----+
    //   c1   c2   c3   c4   c5   c6   c7

    int nbPtsLon = 7;
    int nbPtsLa = 7;
    double interLon = lon/nbPtsLon;
    double interLa = la/nbPtsLa;
    double distPoleTanBordLon = interLon/2;
    double distPoleTanCentreLon = interLon* 2;
    double distPoleTanBordLa = interLa;
    double distPoleTanCentreLa = interLa * 2;

    TColgp_Array2OfPnt poles(1,nbPtsLa,1,nbPtsLon);
    TColStd_Array2OfReal weights(1,nbPtsLa,1,nbPtsLon);
    TColStd_Array1OfInteger uMults(1,3);
    TColStd_Array1OfInteger vMults(1,3);
    TColStd_Array1OfReal uKnots(1,3);
    TColStd_Array1OfReal vKnots(1,3);
    int uDeg = 3;
    int vDeg = 3;
    uMults.SetValue(1,4);uMults.SetValue(2,3);uMults.SetValue(3,4);
    vMults.SetValue(1,4);vMults.SetValue(2,3);vMults.SetValue(3,4);
    uKnots.SetValue(1,0);uKnots.SetValue(2,1);uKnots.SetValue(3,2);
    vKnots.SetValue(1,0);vKnots.SetValue(2,1);vKnots.SetValue(3,2);
    //2 lignes a chaque extremites -> droites à h
    //l1
    poles.SetValue(1,1,gp_Pnt(-lon/2, -la/2, h)); weights.SetValue(1,1,1.0);
    poles.SetValue(1,2,gp_Pnt(-lon/2 + distPoleTanBordLon, -la/2, h)); weights.SetValue(1,2,1.0);
    poles.SetValue(1,3,gp_Pnt(0 - distPoleTanCentreLon,-la/2, h-2*p/3)); weights.SetValue(1,3,1.0);
    poles.SetValue(1,4,gp_Pnt(0 , -la/2, h-2*p/3)); weights.SetValue(1,4,1.0);//point partage ligne
    poles.SetValue(1,5,gp_Pnt(0 + distPoleTanCentreLon,-la/2, h-2*p/3)); weights.SetValue(1,5,1.0);
    poles.SetValue(1,6,gp_Pnt(lon/2 - distPoleTanBordLon, -la/2, h)); weights.SetValue(1,6,1.0);
    poles.SetValue(1,7,gp_Pnt(lon/2, -la/2, h)); weights.SetValue(1,7,1.0);
    //l7
    poles.SetValue(7,1,gp_Pnt(-lon/2, la/2, h)); weights.SetValue(7,1,1.0);
    poles.SetValue(7,2,gp_Pnt(-lon/2 + distPoleTanBordLon, la/2, h)); weights.SetValue(7,2,1.0);
    poles.SetValue(7,3,gp_Pnt(0 - distPoleTanCentreLon,la/2, h)); weights.SetValue(7,3,1.0);
    poles.SetValue(7,4,gp_Pnt(0 , la/2, h)); weights.SetValue(7,4,1.0); //point partage ligne
    poles.SetValue(7,5,gp_Pnt(0 + distPoleTanCentreLon,la/2, h)); weights.SetValue(7,5,1.0);
    poles.SetValue(7,6,gp_Pnt(lon/2 - distPoleTanBordLon, la/2, h)); weights.SetValue(7,6,1.0);
    poles.SetValue(7,7,gp_Pnt(lon/2, la/2, h));  weights.SetValue(7,7,1.0);
    //l2
    poles.SetValue(2,1,gp_Pnt(-lon/2, -la/2 + distPoleTanBordLa , h));  weights.SetValue(2,1,1.0);
    poles.SetValue(2,2,gp_Pnt(-lon/2 + distPoleTanBordLon, -la/2 + distPoleTanBordLa , h)); weights.SetValue(2,2,1.0);
    poles.SetValue(2,3,gp_Pnt(0 - distPoleTanCentreLon,-la/2 + distPoleTanBordLa , h-2*p/3)); weights.SetValue(2,3,1.0);
    poles.SetValue(2,4,gp_Pnt(0 , -la/2 + distPoleTanBordLa , h-2*p/3)); weights.SetValue(2,4,1.0);//point partage ligne
    poles.SetValue(2,5,gp_Pnt(0 + distPoleTanCentreLon,-la/2 + distPoleTanBordLa , h-2*p/3)); weights.SetValue(2,5,1.0);
    poles.SetValue(2,6,gp_Pnt(lon/2 - distPoleTanBordLon, -la/2 + distPoleTanBordLa , h)); weights.SetValue(2,6,1.0);
    poles.SetValue(2,7,gp_Pnt(lon/2, -la/2 + distPoleTanBordLa , h)); weights.SetValue(2,7,1.0);
    //l6
    poles.SetValue(6,1,gp_Pnt(-lon/2, la/2 - distPoleTanBordLa , h)); weights.SetValue(6,1,1.0);
    poles.SetValue(6,2,gp_Pnt(-lon/2 + distPoleTanBordLon, la/2 - distPoleTanBordLa , h)); weights.SetValue(6,2,1.0);
    poles.SetValue(6,3,gp_Pnt(0 - distPoleTanCentreLon,la/2 - distPoleTanBordLa , h)); weights.SetValue(6,3,1.0);
    poles.SetValue(6,4,gp_Pnt(0 , la/2 - distPoleTanBordLa , h)); weights.SetValue(6,4,1.0);//point partage ligne
    poles.SetValue(6,5,gp_Pnt(0 + distPoleTanCentreLon,la/2 - distPoleTanBordLa , h)); weights.SetValue(6,5,1.0);
    poles.SetValue(6,6,gp_Pnt(lon/2 - distPoleTanBordLon, la/2 - distPoleTanBordLa , h)); weights.SetValue(6,6,1.0);
    poles.SetValue(6,7,gp_Pnt(lon/2, la/2 - distPoleTanBordLa , h));weights.SetValue(6,7,1.0);
    //c1
    poles.SetValue(3,1,gp_Pnt(-lon/2,0 - distPoleTanCentreLa, h)); weights.SetValue(3,1,1.0);
    poles.SetValue(4,1,gp_Pnt(-lon/2 , 0, h)); weights.SetValue(4,1,1.0);
    poles.SetValue(5,1,gp_Pnt(-lon/2,0 + distPoleTanCentreLa, h)); weights.SetValue(5,1,1.0);
    //c7
    poles.SetValue(3,7,gp_Pnt(lon/2,0 - distPoleTanCentreLa, h)); weights.SetValue(3,7,1.0);
    poles.SetValue(4,7,gp_Pnt(lon/2 , 0, h)); weights.SetValue(4,7,1.0);
    poles.SetValue(5,7,gp_Pnt(lon/2,0 + distPoleTanCentreLa, h)); weights.SetValue(5,7,1.0);
    //c2
    poles.SetValue(3,2,gp_Pnt(-lon/2 + distPoleTanBordLon,0 - distPoleTanCentreLa, h)); weights.SetValue(3,2,1.0);
    poles.SetValue(4,2,gp_Pnt(-lon/2 + distPoleTanBordLon, 0, h)); weights.SetValue(4,2,1.0);
    poles.SetValue(5,2,gp_Pnt(-lon/2 + distPoleTanBordLon,0 + distPoleTanCentreLa, h));  weights.SetValue(5,2,1.0);
    //c6
    poles.SetValue(3,6,gp_Pnt(lon/2 - distPoleTanBordLon,0 - distPoleTanCentreLa, h));  weights.SetValue(3,6,1.0);
    poles.SetValue(4,6,gp_Pnt(lon/2 - distPoleTanBordLon, 0, h));  weights.SetValue(4,6,1.0);
    poles.SetValue(5,6,gp_Pnt(lon/2 - distPoleTanBordLon,0 + distPoleTanCentreLa, h));  weights.SetValue(5,6,1.0);
    //l3
    poles.SetValue(3,3,gp_Pnt(0 - distPoleTanCentreLon,0 - distPoleTanCentreLa , h-p));  weights.SetValue(3,3,1.0);
    poles.SetValue(3,4,gp_Pnt(0 , 0 - distPoleTanCentreLa , h-p));  weights.SetValue(3,4,1.0);//point partage ligne
    poles.SetValue(3,5,gp_Pnt(0 + distPoleTanCentreLon,0 - distPoleTanCentreLa, h-p));  weights.SetValue(3,5,1.0);
    //l5
    poles.SetValue(5,3,gp_Pnt(0 - distPoleTanCentreLon,0 + distPoleTanCentreLa , h-p));  weights.SetValue(5,3,1.0);
    poles.SetValue(5,4,gp_Pnt(0 , 0 + distPoleTanCentreLa , h-p));  weights.SetValue(5,4,1.0);//point partage ligne
    poles.SetValue(5,5,gp_Pnt(0 + distPoleTanCentreLon,0 + distPoleTanCentreLa, h-p));  weights.SetValue(5,5,1.0);
    //l4
    poles.SetValue(4,3,gp_Pnt(0 - distPoleTanCentreLon,0  , h-p));  weights.SetValue(4,3,1.0);
    poles.SetValue(4,4,gp_Pnt(0 , 0 , h-p));  weights.SetValue(4,4,1.0);//point partage ligne
    poles.SetValue(4,5,gp_Pnt(0 + distPoleTanCentreLon,0 , h-p));  weights.SetValue(4,5,1.0);

    Handle(Geom_BSplineSurface) surf = new Geom_BSplineSurface(poles,weights,uKnots,vKnots,uMults,vMults,uDeg,vDeg,Standard_False,Standard_False); 
    surf->Translate(gp_Vec(0,0,-h));
    return surf;
}
Handle(Geom_BSplineSurface) generate77test(){
    //parametres de forme généraux
    double h = 400; //hauteur
    double p = 30; //profondeur maximale (centre)
    double lon = 500; //longueur
    double la = 400; //largeur

    //Bsplines de degré 3 pour assurer la tangence à l'extérieur et au centre
    //2 morceaux par coté, 7 poles, 3 noeuds (un partagé)
    // +: noeuds  o: autres poles
    // l7 +--o--o--+--o--o--+
    //    |  |  |  |  |  |  |
    // l6 o--o--o--o--o--o--o
    //    |  |  |  |  |  |  |
    // l5 o--o--o--o--o--o--o
    //    |  |  |  |  |  |  |
    // l4 +--o--o--+--o--o--+
    //    |  |  |  |  |  |  |
    // l3 o--o--o--o--o--o--o
    //    |  |  |  |  |  |  |
    // l2 o--o--o--o--o--o--o
    //    |  |  |  |  |  |  |
    // l1 +--o--o--+--o--o--+
    //   c1 c2 c3 c4 c5 c6 c7

    int nbPtsLon = 7;
    int nbPtsLa = 7;
    double interLon = lon/nbPtsLon;
    double interLa = la/nbPtsLa;
    double distPoleTanBordLon = interLon;
    double distPoleTanCentreLon = interLon * 2;
    double distPoleTanBordLa = interLa;
    double distPoleTanCentreLa = interLa * 2;

    TColgp_Array2OfPnt poles(1,nbPtsLa,1,nbPtsLon);
    TColStd_Array2OfReal weights(1,nbPtsLa,1,nbPtsLon);
    TColStd_Array1OfInteger uMults(1,3);
    TColStd_Array1OfInteger vMults(1,3);
    TColStd_Array1OfReal uKnots(1,3);
    TColStd_Array1OfReal vKnots(1,3);
    int uDeg = 3;
    int vDeg = 3;
    uMults.SetValue(1,4);uMults.SetValue(2,3);uMults.SetValue(3,4);
    vMults.SetValue(1,4);vMults.SetValue(2,3);vMults.SetValue(3,4);
    uKnots.SetValue(1,0);uKnots.SetValue(2,1);uKnots.SetValue(3,2);
    vKnots.SetValue(1,0);vKnots.SetValue(2,1);vKnots.SetValue(3,2);
    //2 lignes a chaque extremites -> droites à h
    //l1
    poles.SetValue(1,1,gp_Pnt(-lon/2, -la/2, h+50)); weights.SetValue(1,1,1.0);
    poles.SetValue(1,2,gp_Pnt(-lon/2 + distPoleTanBordLon, -la/2, h+50)); weights.SetValue(1,2,1.0);
    poles.SetValue(1,3,gp_Pnt(0 - distPoleTanCentreLon,-la/2, h-2*p/3)); weights.SetValue(1,3,1.0);
    poles.SetValue(1,4,gp_Pnt(0 , -la/2, h-2*p/3)); weights.SetValue(1,4,1.0);//point partage ligne
    poles.SetValue(1,5,gp_Pnt(0 + distPoleTanCentreLon,-la/2, h-2*p/3)); weights.SetValue(1,5,1.0);
    poles.SetValue(1,6,gp_Pnt(lon/2 - distPoleTanBordLon, -la/2, h)); weights.SetValue(1,6,1.0);
    poles.SetValue(1,7,gp_Pnt(lon/2, -la/2, h)); weights.SetValue(1,7,1.0);
    //l7
    poles.SetValue(7,1,gp_Pnt(-lon/2, la/2, h)); weights.SetValue(7,1,1.0);
    poles.SetValue(7,2,gp_Pnt(-lon/2 + distPoleTanBordLon, la/2, h)); weights.SetValue(7,2,1.0);
    poles.SetValue(7,3,gp_Pnt(0 - distPoleTanCentreLon,la/2, h)); weights.SetValue(7,3,1.0);
    poles.SetValue(7,4,gp_Pnt(0 , la/2, h)); weights.SetValue(7,4,1.0); //point partage ligne
    poles.SetValue(7,5,gp_Pnt(0 + distPoleTanCentreLon,la/2, h)); weights.SetValue(7,5,1.0);
    poles.SetValue(7,6,gp_Pnt(lon/2 - distPoleTanBordLon, la/2, h)); weights.SetValue(7,6,1.0);
    poles.SetValue(7,7,gp_Pnt(lon/2, la/2, h));  weights.SetValue(7,7,1.0);
    //l2
    double phi = 20.0;
    poles.SetValue(2,1,gp_Pnt(phi -lon/2, -la/2 + distPoleTanBordLa , h));  weights.SetValue(2,1,1.0);
    poles.SetValue(2,2,gp_Pnt(phi -lon/2 + distPoleTanBordLon, -la/2 + distPoleTanBordLa , h)); weights.SetValue(2,2,1.0);
    poles.SetValue(2,3,gp_Pnt(phi + 0 - distPoleTanCentreLon,-la/2 + distPoleTanBordLa , h-2*p/3)); weights.SetValue(2,3,1.0);
    poles.SetValue(2,4,gp_Pnt(phi + 0 , -la/2 + distPoleTanBordLa , h-2*p/3)); weights.SetValue(2,4,1.0);//point partage ligne
    poles.SetValue(2,5,gp_Pnt(phi + 0 + distPoleTanCentreLon,-la/2 + distPoleTanBordLa , h-2*p/3)); weights.SetValue(2,5,1.0);
    poles.SetValue(2,6,gp_Pnt(phi + lon/2 - distPoleTanBordLon, -la/2 + distPoleTanBordLa , h)); weights.SetValue(2,6,1.0);
    poles.SetValue(2,7,gp_Pnt(phi + lon/2, -la/2 + distPoleTanBordLa , h)); weights.SetValue(2,7,1.0);
    //l6
    poles.SetValue(6,1,gp_Pnt(-lon/2, la/2 - distPoleTanBordLa , h)); weights.SetValue(6,1,1.0);
    poles.SetValue(6,2,gp_Pnt(-lon/2 + distPoleTanBordLon, la/2 - distPoleTanBordLa , h)); weights.SetValue(6,2,1.0);
    poles.SetValue(6,3,gp_Pnt(0 - distPoleTanCentreLon,la/2 - distPoleTanBordLa , h)); weights.SetValue(6,3,1.0);
    poles.SetValue(6,4,gp_Pnt(0 , la/2 - distPoleTanBordLa , h)); weights.SetValue(6,4,1.0);//point partage ligne
    poles.SetValue(6,5,gp_Pnt(0 + distPoleTanCentreLon,la/2 - distPoleTanBordLa , h)); weights.SetValue(6,5,1.0);
    poles.SetValue(6,6,gp_Pnt(lon/2 - distPoleTanBordLon, la/2 - distPoleTanBordLa , h)); weights.SetValue(6,6,1.0);
    poles.SetValue(6,7,gp_Pnt(lon/2, la/2 - distPoleTanBordLa , h));weights.SetValue(6,7,1.0);
    //c1
    poles.SetValue(3,1,gp_Pnt(-lon/2,0 - distPoleTanCentreLa, h)); weights.SetValue(3,1,1.0);
    poles.SetValue(4,1,gp_Pnt(-lon/2 , 0, h)); weights.SetValue(4,1,1.0);
    poles.SetValue(5,1,gp_Pnt(-lon/2,0 + distPoleTanCentreLa, h)); weights.SetValue(5,1,1.0);
    //c7
    poles.SetValue(3,7,gp_Pnt(lon/2,0 - distPoleTanCentreLa, h)); weights.SetValue(3,7,1.0);
    poles.SetValue(4,7,gp_Pnt(lon/2 , 0, h)); weights.SetValue(4,7,1.0);
    poles.SetValue(5,7,gp_Pnt(lon/2,0 + distPoleTanCentreLa, h)); weights.SetValue(5,7,1.0);
    //c2
    poles.SetValue(3,2,gp_Pnt(-lon/2 + distPoleTanBordLon,0 - distPoleTanCentreLa, h)); weights.SetValue(3,2,1.0);
    poles.SetValue(4,2,gp_Pnt(-lon/2 + distPoleTanBordLon, 0, h)); weights.SetValue(4,2,1.0);
    poles.SetValue(5,2,gp_Pnt(-lon/2 + distPoleTanBordLon,0 + distPoleTanCentreLa, h));  weights.SetValue(5,2,1.0);
    //c6
    poles.SetValue(3,6,gp_Pnt(lon/2 - distPoleTanBordLon,0 - distPoleTanCentreLa, h));  weights.SetValue(3,6,1.0);
    poles.SetValue(4,6,gp_Pnt(lon/2 - distPoleTanBordLon, 0, h));  weights.SetValue(4,6,1.0);
    poles.SetValue(5,6,gp_Pnt(lon/2 - distPoleTanBordLon,0 + distPoleTanCentreLa, h));  weights.SetValue(5,6,1.0);
    //l3
    poles.SetValue(3,3,gp_Pnt(0 - distPoleTanCentreLon,0 - distPoleTanCentreLa , h-p));  weights.SetValue(3,3,1.0);
    poles.SetValue(3,4,gp_Pnt(0 , 0 - distPoleTanCentreLa , h-p));  weights.SetValue(3,4,1.0);//point partage ligne
    poles.SetValue(3,5,gp_Pnt(0 + distPoleTanCentreLon,0 - distPoleTanCentreLa, h-p));  weights.SetValue(3,5,1.0);
    //l5
    poles.SetValue(5,3,gp_Pnt(0 - distPoleTanCentreLon,0 + distPoleTanCentreLa , h-p));  weights.SetValue(5,3,1.0);
    poles.SetValue(5,4,gp_Pnt(0 , 0 + distPoleTanCentreLa , h-p));  weights.SetValue(5,4,1.0);//point partage ligne
    poles.SetValue(5,5,gp_Pnt(0 + distPoleTanCentreLon,0 + distPoleTanCentreLa, h-p));  weights.SetValue(5,5,1.0);
    //l4
    poles.SetValue(4,3,gp_Pnt(0 - distPoleTanCentreLon,0  , h-p));  weights.SetValue(4,3,1.0);
    poles.SetValue(4,4,gp_Pnt(0 , 0 , h-p));  weights.SetValue(4,4,1.0);//point partage ligne
    poles.SetValue(4,5,gp_Pnt(0 + distPoleTanCentreLon,0 , h-p));  weights.SetValue(4,5,1.0);

    Handle(Geom_BSplineSurface) surf = new Geom_BSplineSurface(poles,weights,uKnots,vKnots,uMults,vMults,uDeg,vDeg,Standard_False,Standard_False); 

    return surf;
    /*
    Geom_BSplineSurface (const TColgp_Array2OfPnt &Poles,
        const TColStd_Array2OfReal &Weights,
        const TColStd_Array1OfReal &UKnots,
        const TColStd_Array1OfReal &VKnots,
        const TColStd_Array1OfInteger &UMults,
        const TColStd_Array1OfInteger &VMults,
        const Standard_Integer UDegree,
        const Standard_Integer VDegree,
        const Standard_Boolean UPeriodic=Standard_False,
        const Standard_Boolean VPeriodic=Standard_False)*/
}


/* ------------- */
/* --- UTILS --- */
/* ------------- */
vector<TopoDS_Vertex> getPolesFromSurf(Handle(Geom_BSplineSurface) s){
    vector<TopoDS_Vertex> v;
    TColgp_Array2OfPnt p = s->Poles();
    for(int i = 1; i<=p.NbRows();i++){
        for(int j = 1; j <=p.NbColumns();j++){
            v.push_back(BRepBuilderAPI_MakeVertex(p(i,j)));
        }
    }
    return v; 
}