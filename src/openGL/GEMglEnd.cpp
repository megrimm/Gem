////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// Implementation file
//
// Copyright (c) 2002-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
//      zmoelnig@iem.at
//  For information on usage and redistribution, and for a DISCLAIMER
//  *  OF ALL WARRANTIES, see the file, "GEM.LICENSE.TERMS"
//
//  this file has been generated...
////////////////////////////////////////////////////////

#include "GEMglEnd.h"

CPPEXTERN_NEW ( GEMglEnd );

/////////////////////////////////////////////////////////
//
// GEMglViewport
//
/////////////////////////////////////////////////////////
// Constructor
//
GEMglEnd :: GEMglEnd    () {}
/////////////////////////////////////////////////////////
// Destructor
//
GEMglEnd :: ~GEMglEnd () {}

/////////////////////////////////////////////////////////
// Render
//
void GEMglEnd :: render(GemState *state)
{
  glEnd ();
}
/////////////////////////////////////////////////////////
// static member function
void GEMglEnd :: obj_setupCallback(t_class *classPtr) {}
