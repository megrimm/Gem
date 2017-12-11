////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
//    Copyright (c) 2011 Nicolas Montgermont nicolas_montgermont@yahoo.fr
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
//  pix_noise
//  based on pix_set
//  random function : lagged fibonacci generator
//  2011
//  Nicolas Montgermont
//  mailto:nicolas_montgermont@yahoo.fr
//
/////////////////////////////////////////////////////////



// generate a uniform noise texture

#include "pix_noise.h"
#include "Gem/State.h"

#include <string.h>

CPPEXTERN_NEW_WITH_TWO_ARGS(pix_noise, t_floatarg, A_DEFFLOAT, t_floatarg,
                            A_DEFFLOAT);

/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_noise :: pix_noise(t_floatarg xsize, t_floatarg ysize) :
  m_banged(false), m_automatic(false),
  m_mode(GL_RGBA_GEM),
  m_rand_p(0), m_rand_k(24)
{
  if (xsize < 1) {
    xsize = 256;
  }
  if (ysize < 1) {
    ysize = 256;
  }
  int randInit = 307*1319;
  initRandom(randInit);

  //    m_pixBlock.image = m_imageStruct;
  m_pixBlock.image.xsize = (int)xsize;
  m_pixBlock.image.ysize = (int)ysize;
  m_pixBlock.image.setCsizeByFormat(GL_RGBA_GEM);
  m_pixBlock.image.allocate();

  generateNoise();
}

////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_noise :: ~pix_noise(void)
{
  cleanPixBlock();
}

/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void pix_noise :: render(GemState *state)
{
  if (m_automatic || m_banged) {
    m_banged = false;
    generateNoise();
  }
  state->set(GemState::_PIX,&m_pixBlock);
}

/////////////////////////////////////////////////////////
// startRendering
//
/////////////////////////////////////////////////////////
void pix_noise :: startRendering(void)
{
  m_pixBlock.newimage = true;
}

/////////////////////////////////////////////////////////
// postrender
//
/////////////////////////////////////////////////////////
void pix_noise :: postrender(GemState *state)
{
  m_pixBlock.newimage = false;
  //state->image = NULL;
}

/////////////////////////////////////////////////////////
// initialize random generator
//
/////////////////////////////////////////////////////////
void pix_noise :: initRandom(int seed)
{
  int i;
  m_rand_p = 0;
  m_rand_k = 24;
  for (i=0; i<55; i++) {
    m_rand[i] = seed = seed * 435898247 + 382842987; //random init
  }
}

/////////////////////////////////////////////////////////
// debug internal state of generator
//
/////////////////////////////////////////////////////////
void pix_noise :: debug(void)
{
  post("mrand_p = %i",m_rand_p);
  post("mrand_k = %i",m_rand_k);
  post("mrand[p] = %i",m_rand[m_rand_p]);
  post("mrand[k] = %i",m_rand[m_rand_k]);
  for (int i=0; i<55; i++) {
    post("m_rand[%i] = %i",i,m_rand[i]);
  }

}

/////////////////////////////////////////////////////////
// debug generator
//
/////////////////////////////////////////////////////////
unsigned char pix_noise :: random(void)
{
  // Lagged Fibonacci Generator
  // S[n] = S[n-p]+S[n-k]
  // with p = 55, k = 24

  m_rand[m_rand_p] = m_rand[m_rand_p] + m_rand[m_rand_k];
  if (++m_rand_p>54) {
    m_rand_p = 0;
  }
  if (++m_rand_k>54) {
    m_rand_k = 0;
  }
  return (unsigned char)(m_rand[m_rand_p]);
}

/////////////////////////////////////////////////////////
// generate texture
//
/////////////////////////////////////////////////////////
void pix_noise :: generateNoise(void)
{
  int picturesize = m_pixBlock.image.xsize * m_pixBlock.image.ysize;
  unsigned char *buffer = m_pixBlock.image.data;

  int counter=picturesize;
  switch (m_mode) {
  case GL_RGB:
    while (counter-->0) {
      buffer[chRed]   = random(); // red
      buffer[chGreen] = random(); // green
      buffer[chBlue]  = random(); // blue
      buffer[chAlpha] = 255;      // alpha
      buffer+=4;
    }
    break;
  case GL_LUMINANCE:
    while (counter-->0) {
      buffer[chRed] = buffer[chGreen] = buffer[chBlue] = random();      // rgb
      buffer[chAlpha] = 255;                    // alpha
      buffer+=4;
    }
    break;
  default:
    while (counter-->0) {
      buffer[chRed]   = random(); // red
      buffer[chGreen] = random(); // green
      buffer[chBlue]  = random(); // blue
      buffer[chAlpha] = random(); // alpha
      buffer+=4;
    }
  }
  m_pixBlock.newimage = true;
}
void pix_noise :: seed(int seedval)
{
  initRandom(seedval);
  bang();
}

/////////////////////////////////////////////////////////
// trigger
//
/////////////////////////////////////////////////////////
void pix_noise :: bang(void)
{
  m_banged=true;
}
void pix_noise :: autoMess(bool automatic)
{
  m_automatic=automatic;
}
/////////////////////////////////////////////////////////
// <color>Mess
//
/////////////////////////////////////////////////////////
void pix_noise :: RGBAMess(void)
{
  m_mode = GL_RGBA;
  bang();
}
void pix_noise :: RGBMess(void)
{
  m_mode = GL_RGB;
  bang();
}
void pix_noise :: GREYMess(void)
{
  m_mode = GL_LUMINANCE;
  bang();
}
/////////////////////////////////////////////////////////
// SETMess
//
/////////////////////////////////////////////////////////
void pix_noise :: SETMess(int xsize, int ysize)
{
  if ((xsize < 1) || (ysize < 1)) {
    return;
  }
  m_pixBlock.image.clear();
  m_pixBlock.image.xsize = (int)xsize;
  m_pixBlock.image.ysize = (int)ysize;
  m_pixBlock.image.setCsizeByFormat(GL_RGBA_GEM);
  m_pixBlock.image.reallocate();

  generateNoise();
}

/////////////////////////////////////////////////////////
// cleanPixBlock -- free the pixel buffer memory
//
/////////////////////////////////////////////////////////
void pix_noise :: cleanPixBlock(void)
{
  m_pixBlock.image.clear();
  m_pixBlock.image.data = NULL;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_noise :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG1(classPtr, "auto", autoMess, bool);
  CPPEXTERN_MSG1(classPtr, "seed", seed, int);
  CPPEXTERN_MSG0(classPtr, "bang", bang);

  CPPEXTERN_MSG0(classPtr, "RGBA", RGBAMess);
  CPPEXTERN_MSG0(classPtr, "RGB",  RGBMess);
  CPPEXTERN_MSG0(classPtr, "GREY", GREYMess);
  CPPEXTERN_MSG0(classPtr, "GRAY", GREYMess);

  CPPEXTERN_MSG0(classPtr, "rgba", RGBAMess);
  CPPEXTERN_MSG0(classPtr, "rgb",  RGBMess);
  CPPEXTERN_MSG0(classPtr, "grey", GREYMess);
  CPPEXTERN_MSG0(classPtr, "gray", GREYMess);

  CPPEXTERN_MSG2(classPtr, "set",  SETMess, int, int);

  CPPEXTERN_MSG0(classPtr, "debug",  debug);
}
