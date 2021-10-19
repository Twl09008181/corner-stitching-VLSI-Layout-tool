#ifndef TILE_HPP
#define TILE_HPP


#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
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
    void sety(int y){_y = y;}
    void seth(int h){_h = h;}
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

    bool isSpace()const{return _id < 0;}

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


tile* InsertBlock(int id,int x,int y,int w,int h);








// Fptr 
inline tile* bl(tile*t){return t ? t->bl():nullptr;}
inline tile* lb(tile*t){return t ? t->lb():nullptr;}
inline tile* tr(tile*t){return t ? t->tr():nullptr;}
inline tile* rt(tile*t){return t ? t->rt():nullptr;}
inline int getx(tile *t){return t ? t->x():-1;}
inline int gety(tile *t){return t ? t->y():-1;}
inline int getx2(tile *t){return t ? t->x() + t->w():-1;}
inline int gety2(tile *t){return t ? t->y() + t->h():-1;}

inline int getw(tile *t){return t? t->w() : -1;}
inline int geth(tile *t){return t? t->h() : -1;}
inline int getid(tile *t){return t? t->id() : 0;}
inline void setbl(tile*t,tile *other){t->setbl(other);}
inline void setlb(tile*t,tile *other){t->setlb(other);}
inline void settr(tile*t,tile *other){t->settr(other);}
inline void setrt(tile*t,tile *other){t->setrt(other);}



// Set if can set.
void setTr_if(tile* left,tile* right);
void setRt_if(tile* bottom,tile* top);
void setBl_if(tile* left,tile*right);
void setLb_if(tile* bottom,tile* top);






// not less than
inline bool nls(int a,int b){return a >= b;}
// less than
inline bool ls(int a,int b){return a < b;}
// not greater than   
inline bool ngt(int a,int b){return a <= b;}
// greater than 
inline bool gt(int a,int b){return a > b;}

using cmp = decltype(nls);
using t_get = decltype(getx);
using t_set = decltype(setbl);
using t_move = decltype(bl);
using set_if = decltype(setRt_if);

class userlog{
public:
    userlog() = default;
    userlog(int w,int h)
        :_h{h},_w{w},lastuser{ new tile(-1,0,0,w,h)}{}
    void addLog(tile*t){lastuser = t;}
    tile* lastLog()const{return lastuser;}
    int getw(){return _w;}
    int geth(){return _h;}
private:
    int _w,_h;
    tile * lastuser = nullptr;
};



// Neighbor getter 
std::vector<tile*> NeighborTraversal(tile*t,tile*nb,t_get*get,int v,cmp *comp,t_move*next);
inline std::vector<tile*>getLeft(tile*t)  {   return (t&&t->bl()) ? NeighborTraversal(t,t->bl(),gety,gety2(t),ls,rt) : std::vector<tile*>{}; }
inline std::vector<tile*>getRight(tile*t) {   return (t&&t->tr()) ? NeighborTraversal(t,t->tr(),gety2,gety(t),gt,lb) : std::vector<tile*>{}; }
inline std::vector<tile*>getTop(tile*t)   {   return (t&&t->rt()) ? NeighborTraversal(t,t->rt(),getx2,getx(t),gt,bl) : std::vector<tile*>{}; }
inline std::vector<tile*>getBottom(tile*t){   return (t&&t->lb()) ? NeighborTraversal(t,t->lb(),getx,getx2(t),ls,tr) : std::vector<tile*>{}; }
std::vector<tile*>getNeighbor(tile*t);

// Merge
void doMerge(const std::vector<tile*>&tiles,t_move* next,t_move* precheck,t_get* v);
inline void MergeRight(const std::vector<tile*>&tiles){doMerge(tiles,lb,rt,getx);}
inline void MergLeft(const std::vector<tile*>&tiles){doMerge(tiles,rt,lb,getx2);}

// OneSideUpdating
void OneSideUpdate(set_if* s1,set_if*s2,tile* t,const std::vector<tile*>&neighbors,bool tIsLB);
inline void updateRight(tile*t,const std::vector<tile*>&neighbors){OneSideUpdate(setTr_if,setBl_if,t,neighbors,true);}
inline void updateTop(tile*t,const std::vector<tile*>&neighbors){OneSideUpdate(setRt_if,setLb_if,t,neighbors,true);}
inline void updateLeft(tile*t,const std::vector<tile*>&neighbors){OneSideUpdate(setTr_if,setBl_if,t,neighbors,false);}
inline void updateBottom(tile*t,const std::vector<tile*>&neighbors){OneSideUpdate(setRt_if,setLb_if,t,neighbors,false);}















#endif
