////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// mark@danks.org
//
// Implementation file
//
//    Copyright (c) 1997-1999 Mark Danks.
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "CPPExtern.h"

#ifdef _MSC_VER  /* This is only for Microsoft's compiler, not cygwin, e.g. */
# define snprintf _snprintf
# define vsnprintf _vsnprintf
#endif

#include <stdio.h>
#include <stdarg.h>


GEM_EXTERN void *operator new(size_t, void *location, void *) {return(location);}

t_object * CPPExtern::m_holder;
char* CPPExtern::m_holdname;

/////////////////////////////////////////////////////////
//
// CPPExtern
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
CPPExtern :: CPPExtern()
  : x_obj(m_holder),
    m_objectname(NULL),
    m_canvas(NULL),
    m_endpost(true)
{
    m_canvas = canvas_getcurrent();
    m_objectname=gensym(m_holdname);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
CPPExtern :: ~CPPExtern()
{ }
void CPPExtern :: post(const char*fmt,...) const
{
  char buf[MAXPDSTRING];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, MAXPDSTRING-1, fmt, ap);
  va_end(ap);
  if(m_endpost && NULL!=m_objectname && NULL!=m_objectname->s_name && &s_ != m_objectname){
    ::post("[%s]: %s", m_objectname->s_name, buf);
  } else {
    ::post("%s", buf);
  }
  m_endpost=true;
}
void CPPExtern :: startpost(const char*fmt,...) const
{
  char buf[MAXPDSTRING];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, MAXPDSTRING-1, fmt, ap);
  va_end(ap);
  if(m_endpost && NULL!=m_objectname && NULL!=m_objectname->s_name && &s_ != m_objectname){
    ::startpost("[%s]: %s", m_objectname->s_name, buf);
  } else {
    ::startpost("%s", buf);
  }
  m_endpost=false;
}
void CPPExtern :: endpost(void) const
{
  ::endpost();
  m_endpost=true;
}
void CPPExtern :: verbose(const int level, const char*fmt,...) const
{
  char buf[MAXPDSTRING];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, MAXPDSTRING-1, fmt, ap);
  va_end(ap);
  /* only pd>=0.39(?) supports ::verbose() */
#if defined PD_MINOR_VERSION && (PD_MAJOR_VERSION > 1 || PD_MINOR_VERSION > 38)
  if(NULL!=m_objectname && NULL!=m_objectname->s_name && &s_ != m_objectname){
    ::verbose(level, "[%s]: %s", m_objectname->s_name, buf);
  } else {
    ::verbose(level, "%s", buf);
  }
#else
    if(NULL!=m_objectname && NULL!=m_objectname->s_name && &s_ != m_objectname){
    ::post("[%s]: %s", m_objectname->s_name, buf);
  } else {
    ::post("%s", buf);
  }
#endif
}

void CPPExtern :: error(const char*fmt,...) const
{
  char buf[MAXPDSTRING];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, MAXPDSTRING-1, fmt, ap);
  va_end(ap);
  if(NULL!=m_objectname && NULL!=m_objectname->s_name && &s_ != m_objectname){
    char*objname=m_objectname->s_name;
    if(x_obj)
      pd_error(x_obj, "[%s]: %s", objname, buf);
    else if (m_holder)
      pd_error(m_holder, "[%s]: %s", objname, buf);
    else
      ::error("[%s]: %s", objname, buf);
  } else {
    if(x_obj)
      pd_error(x_obj, "%s", buf);
    else if (m_holder)
      pd_error(m_holder, "%s", buf);
    else
      ::error("%s", buf);
  }
}


std::string CPPExtern::findFile(const std::string f, const std::string e) const {
  char buf[MAXPDSTRING], buf2[MAXPDSTRING];
  char*bufptr=0;
  std::string result="";
  int fd=-1;

  t_canvas*canvas=const_cast<t_canvas*>(getCanvas());
  char*filename=const_cast<char*>(f.c_str());
  char*ext=const_cast<char*>(e.c_str());
  
  if ((fd=open_via_path(canvas_getdir(canvas)->s_name, filename, ext, 
                        buf2, &bufptr, MAXPDSTRING, 1))>=0){
    close(fd);
    result=buf2;
    result+="/";
    result+=bufptr;
  } else {
    canvas_makefilename(canvas, filename, buf, MAXPDSTRING);
    result=buf;
  }
  return result;
}

std::string CPPExtern::findFile(const std::string file) const {
  return findFile(file, "");

}
bool CPPExtern :: checkGemVersion(const int major, const int minor) {
  if(!GemVersion::versionCheck(major, minor)) {
    ::error("GEM version mismatch: compiled for %d.%d but we are running %s", 
	    major, minor,
	    GemVersion::versionString());
        return false;
  }
  return true;
}
