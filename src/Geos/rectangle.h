/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    A rectangle

    Copyright (c) 1997-2000 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef _INCLUDE__GEM_GEOS_RECTANGLE_H_
#define _INCLUDE__GEM_GEOS_RECTANGLE_H_

#include "Base/GemShape.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    rectangle

    Creates a rectangle

KEYWORDS
    geo

DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXTERN rectangle : public GemShape
{
  CPPEXTERN_HEADER(rectangle, GemShape);

public:

  //////////
  // Constructor
  rectangle(t_floatarg width, t_floatarg height);

protected:

  //////////
  // Destructor
  virtual ~rectangle(void);

  //////////
  // The height of the object
  void            heightMess(float size);

  //////////
  // Do the rendering
  virtual void    renderShape(GemState *state);

  //////////
  // The height of the object
  GLfloat         m_height;

  //////////
  // The height inlet
  t_inlet         *m_inletH;
};

#endif  // for header file
