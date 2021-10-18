#ifndef TILE_HPP

#define TILE_HPP

#include <vector>
#include <iostream>
struct tile
{
    int _id; // if = -1 : space tile
    int _x,_y;
    int _w,_h; 
    tile *_bl,*_lb,*_tr,*_rt;

    tile(int id,int x,int y,int w,int h,tile*bl=nullptr,tile*lb=nullptr,tile*tr=nullptr,tile*rt=nullptr)
        :_id{id},_x{x},_y{y},_w{w},_h{h},
            _bl{bl},_lb{lb},_tr{tr},_rt{rt}
        {}
    tile() = default;
    tile* bl()const{return _bl;}
    tile* lb()const{return _lb;}
    tile* tr()const{return _tr;}
    tile* rt()const{return _rt;}

    void setbl(tile*t){_bl = t;}
    void setlb(tile*t){_lb = t;}
    void settr(tile*t){_tr = t;}
    void setrt(tile*t){_rt = t;}


    int x()const{return _x;}
    int y()const{return _y;}
    int w()const{return _w;}
    int h()const{return _h;}
    int id()const{return _id;}
};


std::ostream& operator<<(std::ostream&os,const tile &t);

tile* init;
tile* pointFinding(int x,int y,tile*start);
std::vector<tile*> Vsearch(int x,int y,tile*start);//find all tiles cross from (x,y) to (x,0). 

tile* Vsplit(tile*t,int x,bool left = true); 
tile* Hsplit(tile*t,int y,bool bottom = true);

void InsertBlock(int id,int x,int y,int w,int h);

void mergeTiles(); //merge if have same horizontal span.  use lb pointer.

































#endif
