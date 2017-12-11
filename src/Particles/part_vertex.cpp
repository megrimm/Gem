////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "part_vertex.h"


#include "papi/papi.h"

CPPEXTERN_NEW_WITH_THREE_ARGS(part_vertex, t_floatarg, A_DEFFLOAT,
                              t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT);

/////////////////////////////////////////////////////////
//
// part_vertex
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
part_vertex :: part_vertex(t_floatarg x, t_floatarg y, t_floatarg z)
  : m_x(x), m_y(y), m_z(z)
{
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("list"), gensym(""));
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
part_vertex :: ~part_vertex()
{ }

/////////////////////////////////////////////////////////
// renderParticles
//
/////////////////////////////////////////////////////////
void part_vertex :: renderParticles(GemState *state)
{
  if (m_tickTime > 0.f) {
    pVertex(m_x, m_y, m_z);
  }
}

void part_vertex::posMess(t_float x, t_float y, t_float z)
{
  m_x=x;
  m_y=y;
  m_z=z;
}

/////////////////////////////////////////////////////////
// static member functions
//
/////////////////////////////////////////////////////////
void part_vertex :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG3(classPtr, "", posMess, float, float, float);
}
