#ifndef TILE_HPP

#define TILE_HPP

#include <vector>
#include <iostream>
#include <set>

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
    void setid(int id){_id = id;}
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

    bool isSpace()const{return _id<0;}

private:
    int _id; // if = -1 : space tile
    int _x,_y;
    int _w,_h; 
    tile *_bl,*_lb,*_tr,*_rt;
};


// show information of tile t , including id,x,y,w,h.    
std::ostream& operator<<(std::ostream&os,const tile &t);

// (x,y) Point is allowed to lie on left/top edge of tile, but is not allowed to lie on right/bottom of tile.       
// This property make one point only be contained by one tile.   
tile* pointFinding(int x,int y);


// left - bottom (x1,y1)   right top (x2,y2)
// check if this area has any block by checkin maximal horizontal stripes.
// If has no blocks , return the space tiles in this region    
// otherwise return empty.     
std::vector<tile*> AreaSearch(int x1,int y1,int x2,int y2);


// Split A tile into two part.    
// In Vsplit, if left is true, then return left part.
tile* Vsplit(tile*t,int x,bool left = true); 
tile* Hsplit(tile*t,int y,bool bottom = true);


std::set<tile*> getNeighbor(tile*t);
tile* InsertBlock(int id,int x,int y,int w,int h);
void mergeTiles(); //merge if have same horizontal span.  use lb pointer.





// Fptr 
inline tile* bl(tile*t){return t->bl();}
inline tile* lb(tile*t){return t->lb();}
inline tile* tr(tile*t){return t->tr();}
inline tile* rt(tile*t){return t->rt();}
inline int getx(tile *t){return t->x();}
inline int gety(tile *t){return t->y();}
inline int getx2(tile *t){return t->x() + t->w();}
inline int gety2(tile *t){return t->y() + t->y();}

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

inline bool ngt(int a,int b){return a<=b;}

using cmp = decltype(nls);
using t_get = decltype(getx);
using t_set = decltype(setbl);
using t_move = decltype(bl);


class userlog{
public:
    userlog() = default;
    userlog(int w,int h)
        :lastuser{ new tile(-1,0,0,w,h)}{}
    void addLog(tile*t){lastuser = t;}
    tile* lastLog()const{return lastuser;}
private:
    tile * lastuser = nullptr;
};
























#endif
