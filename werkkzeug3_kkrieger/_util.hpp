// This file is distributed under a BSD license. See LICENSE.txt for details.

#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include "_types.hpp"
#include "_start.hpp"

/****************************************************************************/

extern class sGuiPainter *sPainter;
extern class sPerfMon_ *sPerfMon;

/****************************************************************************/
/***                                                                      ***/
/***   Simple Helpers                                                     ***/
/***                                                                      ***/
/****************************************************************************/

sBitmap *sLoadBitmap(sChar *filename);  // BMP & PIC
sBool sSaveBitmap(sChar *filename,sBitmap *bm);     // BMP & TGA

/****************************************************************************/
/***                                                                      ***/
/***   GuiPainter - draw sprites and print text                           ***/
/***                                                                      ***/
/****************************************************************************/

struct sGuiPainterLetter          // font metrics
{
  sU16 PosX;                      // pixel-position im bitmap
  sU16 PosY;
  sS8 PreKern;                    // pixels to add before cell
  sS8 CellWidth;                  // width of cell
  sS8 PostKern;                   // pixels to add after cell
  sS8 pad;
};

struct sGuiPainterExtPos          // sometimes we want more than just 2d-sprites
{
  sF32 XYUV[4][4];                // 4 xyuv's
  sU32 Col[4];
};

struct sGuiPainterLine
{
  sF32 x0,y0,x1,y1;
  sU32 col;
};

struct sGuiPainterVert
{
  sF32 x,y;
  sF32 u,v;
  sU32 col;
  void Init(sF32 X,sF32 Y,sU32 C=~0,sF32 U=0,sF32 V=0)
  { x=X; y=Y; col=C; u=U; v=V; }
};

/****************************************************************************/
/****************************************************************************/

struct sGuiPainterJob             // one sprite or one line of text
{
  sGuiPainterJob *Next;           // next job fir this material
  sInt Handle;                    // backreference to matieral (redundant!)
  sF32 x,y,z;                     // posiiton of upper left corner
  sU32 Color;                     // color
  sChar *Text;                    // if this is a printjob...
  sGuiPainterExtPos *ExtPos;      // extendet position if needed...
  sInt ClipRect;                  // 0 = no clipping
};

/****************************************************************************/

struct sGuiPainterMaterial        // a material used for painting and printing
{
  sU32 *States;                   // material to draw this sprite (user must free)
  sInt Texture;                   // texture handle for states
  sInt XSize;                     // size of bitmap
  sInt YSize;                     // size of bitmap
  sMaterial *Material;            // the material itself.

  sInt QuadCount;                 // number of quads registered for this frame
  sGuiPainterJob *FirstJob;       // first paintjob this frame, or -1
  sGuiPainterJob **LastJob;       // pointer to next field of last job

  sGuiPainterLetter *Letters;     // if it is a font, here are the metrics
  sInt Height;                    // font height
  sInt Advance;                   // font advance
  sInt BaseLine;                  // font baseline
};

/****************************************************************************/

class sGuiPainter : public sObject
{
/*
  sU32 FlatMat[256];
  sU32 TexMat[256];
  sU32 AlphaMat[256];
*/
  sMaterialEnv MtrlEnv;

  sGuiPainterMaterial *Mat;
  sInt MatAlloc;
  sInt MatCount;

  sGuiPainterJob *Job;
  sInt JobAlloc;
  sInt JobCount;

  sChar *Text;
  sInt TextAlloc;
  sInt TextCount;

  sGuiPainterExtPos *Ext;
  sInt ExtAlloc;
  sInt ExtCount;

  sRect *Rect;
  sInt RectCount;
  sInt RectAlloc;

  sGuiPainterLine *Lines;
  sInt LineCount;
  sInt LineAlloc;

  sF32 PaintZ;
  sU32 PaintCol;
  sF32 PrintZ;
  sU32 PrintCol;
  sInt GeoHandle;
  sInt LineHandle;

  sInt Clipping;
  void FlushRect(sVertexDouble *&vp,sGuiPainterJob *job,sF32 x0,sF32 y0,sF32 x1,sF32 y1,sF32 u0,sF32 v0,sF32 u1,sF32 v1,sU32 col00,sU32 col01,sU32 col10,sU32 col11);
  void CheckOverflow();

public:
  sGuiPainter();
  ~sGuiPainter();
  sU32 GetClassID() { return sCID_GUIPAINTER; }
  void Tag();
  void Init(sInt matcount=64,sInt jobcount=0x4000,sInt textcount=0x10000,sInt extcount=0x4000,sInt rectcount=0x1000,sInt linecount=0x10000);

  sInt LoadFont(sChar *name,sInt height,sInt width=0,sInt style=0);
  sInt LoadBitmap(sBitmap *);
  sInt LoadBitmapAlpha(sBitmap *);
  sInt LoadBitmap(sChar *);
  sInt LoadMaterial(sMaterial *mat,sInt xs=0,sInt ys=0);
  void SetMaterial(sInt handle);
 
  void Paint(sInt handle,sInt x,sInt y,sU32 col=0);
  void Paint(sInt handle,sInt x,sInt y,sInt xs,sInt ys,sU32 col=0);
  void Paint(sInt handle,const sRect &r,sU32 col);
  void Paint(sInt handle,const sRect &pos,const sFRect &uv,sU32 col=0);
  void Paint(sInt handle,const sInt *pos,const sF32 *uv,sU32 col=0);
  void PaintHole(sInt handle,const sRect &out,const sRect &in,sU32 col=0);
  void PaintF(sInt handle,sF32 x,sF32 y,sU32 col=0);
  void PaintF(sInt handle,sF32 x,sF32 y,sF32 xs,sF32 ys,sU32 col=0);
  void PaintF(sInt handle,sF32 x0,sF32 y0,sF32 x1,sF32 y1,sF32 u0,sF32 v0,sF32 u1,sF32 v1,sU32 col=0);
  void Paint(sInt handle,sGuiPainterVert *v,sInt aligned);
  void PaintMode(sU32 col,sF32 z);

  void Line(sInt x0,sInt y0,sInt x1,sInt y1,sU32 col);
  void LineF(sF32 x0,sF32 y0,sF32 x1,sF32 y1,sU32 col);

  void Print(sInt handle,sInt x,sInt y,const sChar *text,sU32 col=0,sInt len=-1);
  sInt PrintC(sInt handle,sRect &,sInt align,const sChar *text,sU32 col=0,sInt len=-1);
  sInt PrintM(sInt handle,sRect &,sInt align,const sChar *text,sU32 col=0,sInt len=-1);   // returns last used ypos (NOT height!)
  void PrintMode(sU32 col,sF32 z);
  sInt GetWidth(sInt handle,const sChar *text,sInt len=-1);
  sInt GetHeight(sInt handle);

  void Flush();
  void SetClipping(sRect &r);
  void EnableClipping(sBool clip);
};

#define sFA_TOP     1             // alignment for PrintC
#define sFA_BOTTOM  2
#define sFA_LEFT    4
#define sFA_RIGHT   8
#define sFA_SPACE   16            // adds a space to left or right on sFA_LEFT / sFA_RIGHT. this usually looks much better...
#define sFA_CLIP    32            // clip if needed

/****************************************************************************/
/***                                                                      ***/
/***   Performance Monitor                                                ***/
/***                                                                      ***/
/****************************************************************************/

struct sPerfMonToken              // declare name and color of a zone
{
  sChar *Name;
  sU32 Color;
  sInt Index;
 // sPerfMonToken(sChar *name,sU32 color);  
};

struct sPerfMonZone               // automatically enter end leave a zone
{
  sPerfMonZone(sPerfMonToken&);
  ~sPerfMonZone();
  sInt Token;
};

struct sPerfMonRecord             // record an enter/leave event
{
  sU16 Type;                      // 0 = end, 1 = enter, 2 = leave
  sU16 Token;                     // token index
  sInt Time;                      // time of event
};

struct sPerfMonZone2              // copy of zone and temporal zone storeage. 
{                                 // don't acces sPerfMonZone directly because of cache!
  sChar *Name;                    // copy of name
  sU32 Color;                     // copy of color
  sInt EnterTime;                 // last enter time when evaluating the recording
  sInt TotalTime;                 // total time this frame, when evaluating the recording
  sInt AverageTime;               // FIR filtered time
};

class sPerfMon_ : public sObject
{
public:
  sPerfMonZone2 Tokens[512];
  sInt TokenCount;
  sPerfMonRecord Rec[2][0x8000];  // record the enter/leave events
  sInt SoundRec[2][0x100];
  sInt Markers[2][16];
  sInt DBuffer;                   // write to Rec[DBuffer][], read from Rec[1-DBuffer][]
  sInt Index;                     // index while writing
  sInt IndexSound;
  sInt IndexOld;                  // old index. to see how near we are to the absolute limit

  sPerfMon_();
  ~sPerfMon_();

  void Flip();
  void Register(sPerfMonToken &tok);
  void Marker(sInt index);
};

// macros for easily using the perfmon are defined in types.hpp
//
// sMAKEZONE(zonename,namestring,color);
// initcode() { sREGTONE(zonename); /* may be called more than once! */ }
// innerloop() { sZONE(zonename); for(many) expensive_calculation(); } }


/****************************************************************************/
/****************************************************************************/

#endif
