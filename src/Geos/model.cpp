////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "model.h"
#include "Gem/State.h"
#include "plugins/modelloader.h"

CPPEXTERN_NEW_WITH_ONE_ARG(model, t_symbol *, A_DEFSYM);

/////////////////////////////////////////////////////////
//
// model
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
model :: model(t_symbol *filename) :
  m_loader(gem::plugins::modelloader::getInstance()),
  m_loaded(false),
  m_position(256,3),
  m_texture (256,2),
  m_color   (256,4),
  m_normal  (256,3)
{
  // make sure that there are some characters
  if (filename&&filename->s_name&&*filename->s_name) openMess(filename->s_name);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
model :: ~model(void)
{
  if(m_loaded) {
    m_loader->close();
  }
  if(m_loader) {
    delete m_loader;
    m_loader=NULL;
  }
}

void model :: applyProperties(void)
{
#if 0
  std::vector<std::string>keys=m_properties.keys();
  unsigned int i;
  for(i=0; i<keys.size(); i++) {
    post("key[%d]=%s ... %d", i, keys[i].c_str(), m_properties.type(keys[i]));
  }
#endif

  if(m_loader)
    m_loader->setProperties(m_properties);
}

/////////////////////////////////////////////////////////
// materialMess
//
/////////////////////////////////////////////////////////
void model :: materialMess(int material)
{
  gem::any value=material;
  m_properties.set("usematerials", value);
  applyProperties();
}

/////////////////////////////////////////////////////////
// materialMess
//
/////////////////////////////////////////////////////////
void model :: textureMess(int state)
{
  std::string textype;
  switch(state) {
  case 0:
    textype="linear";
    break;
  case 1:
    textype="spheremap";
    break;
  case 2:
    textype="UV";
    break;
  default:
    break;
  }
  if(textype.empty()) {
    m_properties.erase("textype");
  } else {
    gem::any value=textype;
    m_properties.set("textype", value);
  }
  applyProperties();
}

/////////////////////////////////////////////////////////
// smoothMess
//
/////////////////////////////////////////////////////////
void model :: smoothMess(t_float fsmooth)
{
  m_properties.set("smooth", fsmooth);
  applyProperties();
}

/////////////////////////////////////////////////////////
// rescaleMess
//
/////////////////////////////////////////////////////////
void model :: reverseMess(bool reverse)
{
  gem::any value=(double)reverse;
  m_properties.set("reverse", value);
  applyProperties();
}
/////////////////////////////////////////////////////////
// matrialMess
//
/////////////////////////////////////////////////////////
void model :: rescaleMess(bool state)
{
  gem::any value=(double)state;
  m_properties.set("rescale", value);
  applyProperties();
}

/////////////////////////////////////////////////////////
// matrialMess
//
/////////////////////////////////////////////////////////
void model :: groupMess(int state)
{
  gem::any value=state;
  m_properties.set("group", value);
  applyProperties();
}


/////////////////////////////////////////////////////////
// openMess
//
/////////////////////////////////////////////////////////
void model :: openMess(const std::string&filename)
{
  if(!m_loader) {
    error("no model loader backends found");
    return;
  }
  m_loader->close();
  m_loaded=false;

  char buf[MAXPDSTRING];
  canvas_makefilename(const_cast<t_canvas*>(getCanvas()), const_cast<char*>(filename.c_str()), buf, MAXPDSTRING);
  if(!m_loader->open(buf, m_properties)) {
      error("unable to read model '%s'", buf);
      return;
  }

  m_loaded=true;
  copyArray("vertices", m_position);
  copyArray("texcoords", m_texture);
  copyArray("normals", m_normal);
  copyArray("colors", m_color);
  setModified();
}

/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void model :: render(GemState *state)
{
  if(!m_loaded)return;

  if ( !m_position.vbo || !m_texture.vbo || !m_color.vbo || !m_normal.vbo || m_size_change_flag ) {
    createVBO();
    m_size_change_flag = false;
  }
  std::vector<unsigned int> sizeList;

  m_loader->render();
  if(m_position.render()) {
    glVertexPointer(m_position.stride, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    sizeList.push_back(m_position.size);
  }
  if(m_texture.render()) {
    glTexCoordPointer(m_texture.stride, GL_FLOAT, 0, 0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    sizeList.push_back(m_texture.size);
  }
  if(m_color.render()) {
    glColorPointer(m_color.stride, GL_FLOAT, 0, 0);
    glEnableClientState(GL_COLOR_ARRAY);
    sizeList.push_back(m_color.size);
  }
  if(m_normal.render()) {
    glNormalPointer(GL_FLOAT, 0, 0);
    glEnableClientState(GL_NORMAL_ARRAY);
    sizeList.push_back(m_normal.size);
  }

  unsigned int npoints = *std::min_element(sizeList.begin(),sizeList.end());

  glDrawArrays(GL_TRIANGLES, 0, npoints);

  if ( m_position.enabled ) {
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  if ( m_color.enabled    ) {
    glDisableClientState(GL_COLOR_ARRAY);
  }
  if ( m_texture.enabled  ) {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  if ( m_normal.enabled   ) {
    glDisableClientState(GL_NORMAL_ARRAY);
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void model :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG1(classPtr, "open", openMess, std::string);
  CPPEXTERN_MSG1(classPtr, "rescale", rescaleMess, bool);
  CPPEXTERN_MSG1(classPtr, "smooth", smoothMess, float);
  CPPEXTERN_MSG1(classPtr, "revert", reverseMess, bool);
  CPPEXTERN_MSG1(classPtr, "material", materialMess, int);
  CPPEXTERN_MSG1(classPtr, "texture", textureMess, int);
  CPPEXTERN_MSG1(classPtr, "group", groupMess, int);
}

void model :: createVBO(void)
{
  m_position.create();
  m_texture .create();
  m_color   .create();
  m_normal  .create();
}

void model :: copyArray(std::string vectorName, VertexBuffer&vb)
{
  unsigned int size(0), i(0), npts(0);

  std::vector<float> tab = m_loader->getVector(vectorName);
  size=tab.size();
  if ( size == 0 ) return;

  npts = size / vb.stride;
  printf("array size : %d, npoints : %d\n",size,npts);

  if(npts!=vb.size) {
    vb.resize(npts);
    m_size_change_flag=true;
  }

  for ( i = 0 ; i < size ; i++ )	{
    vb.array[i] = tab[i];
  }
  vb.dirty=true;
  vb.enabled=true;
}
