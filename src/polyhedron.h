#ifndef _POLYHEDRON3_H_
#define _POLYHEDRON3_H_

struct VertexData;
struct EdgeData;
struct PolygonData;

struct HalfEdgeData
{
    HalfEdgeData* next;
    HalfEdgeData* previous;
    HalfEdgeData* pair;

    VertexData* origin;
    PolygonData* left;
    EdgeData* edge;
};

struct VertexData
{
  Vertex *v;
    HalfEdgeData* half;
};

struct EdgeData
{
    HalfEdgeData* half;
};

struct Polyhedron
{
    HalfEdgeData* half;
};

struct Vertex
{
  double x, y, z;
};


class Polyhedron_3
{

  VertexData vertices[];
  Polyhedron faces[];
  HalfEdgeData half_edge[];


  void addFace(Vertex v1, Vertex v2, Vertex v3)
  {
    
    HalfEdgeData hd;
    VertexData vd1, vd2, vd3;
    EdgeData ed;
    Polyhedron p;

    vd1.v = &v1;
    vd2.v = &v2;
    vd3.v = &v3;

    
    hd.next;
    hd.previous;
    hd.pair;

    hd.origin = vd1;
    hd.left;
    hd.edge;
    
    

  }
  
}


#endif
  
