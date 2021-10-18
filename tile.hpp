#ifndef TILE_HPP

#define TILE_HPP

#include <vector>
#include <iostream>

struct tile
{
    tile(int id,int x,int y,int w,int h,tile*bl=nullptr,tile*lb=nullptr,tile*tr=nullptr,tile*rt=nullptr)
        :_id{id},_x{x},_y{y},_w{w},_h{h},
            _bl{bl},_lb{lb},_tr{tr},_rt{rt}
        {}
    tile() = default;

    // setter
    void setbl(tile*t){_bl = t;}
    void setlb(tile*t){_lb = t;}
    void settr(tile*t){_tr = t;}
    void setrt(tile*t){_rt = t;}

    // getter
    int x()const{return _x;}
    int y()const{return _y;}
    int w()const{return _w;}
    int h()const{return _h;}
    int id()const{return _id;}
    tile* bl()const{return _bl;}
    tile* lb()const{return _lb;}
    tile* tr()const{return _tr;}
    tile* rt()const{return _rt;}

private:
    int _id; // if = -1 : space tile
    int _x,_y;
    int _w,_h; 
    tile *_bl,*_lb,*_tr,*_rt;
};


// show information of tile t , including id,x,y,w,h.    
std::ostream& operator<<(std::ostream&os,const tile &t);

tile* init;

// From "start" to do pointFinding algorithm until find some tile which contain (x,y) point.
// (x,y) Point is allowed to lie on left/top edge of tile, but is not allowed to lie on right/bottom of tile.       
// This property make one point only be contained by one tile.   
tile* pointFinding(int x,int y,tile*hint);

// Find all tiles cross from (x,y) to (x,0) and return.
// start is a hint of pointfinding.
std::vector<tile*> Vsearch(int x,int y,tile*start);

// Split A tile into two part.    
// In Vsplit, if left is true, then return left part.
tile* Vsplit(tile*t,int x,bool left = true); 
tile* Hsplit(tile*t,int y,bool bottom = true);



void InsertBlock(int id,int x,int y,int w,int h);
void mergeTiles(); //merge if have same horizontal span.  use lb pointer.





// Fptr 
inline tile* bl(tile*t){return t->bl();}
inline tile* lb(tile*t){return t->lb();}
inline tile* tr(tile*t){return t->tr();}
inline tile* rt(tile*t){return t->rt();}
inline int getx(tile *t){return t->x();}
inline int gety(tile *t){return t->y();}


inline int getw(tile *t){return t->w();}
inline int geth(tile *t){return t->h();}
inline int getid(tile *t){return t->id();}
inline void setbl(tile*t,tile *other){t->setbl(other);}
inline void setlb(tile*t,tile *other){t->setlb(other);}
inline void settr(tile*t,tile *other){t->settr(other);}
inline void setrt(tile*t,tile *other){t->setrt(other);}

// not less than
inline bool nls(int a,int b){return a >= b;}
// less than
inline bool ls(int a,int b){return a < b;}

using cmp = decltype(nls);
using t_get = decltype(getx);
using t_set = decltype(setbl);
using t_move = decltype(bl);



























#endif
