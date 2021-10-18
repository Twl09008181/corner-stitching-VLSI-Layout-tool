

#include "tile.hpp"
#include <vector>
#include <iostream>


tile* bl(tile*t){return t->bl();}
tile* lb(tile*t){return t->lb();}
tile* tr(tile*t){return t->tr();}
tile* rt(tile*t){return t->rt();}
int getx(tile *t){return t->x();}
int gety(tile *t){return t->y();}
int getw(tile *t){return t->w();}
int geth(tile *t){return t->h();}
int getid(tile *t){return t->id();}
void setbl(tile*t,tile *other){t->setbl(other);}
void setlb(tile*t,tile *other){t->setlb(other);}
void settr(tile*t,tile *other){t->settr(other);}
void setrt(tile*t,tile *other){t->setrt(other);}

bool nls(int a,int b){return a >= b;}//not less
bool ls(int a,int b){return a < b;}//less than

using cmp = decltype(nls);
using t_get = decltype(getx);
using t_set = decltype(setbl);
using t_move = decltype(bl);

void Move(int p,tile*t,t_move*gt,t_move *ls,int(*v)(tile*)){
    if(t && p < v(t)){
        while(t && p < v(t))
            t = ls(t);
    }else if(t && p > v(t)){
        while(t && p > v(t))
            t = gt(t);
    }
}

void Hmove(int x,tile*t){Move(x,t,tr,bl,getx);}
void Vmove(int y,tile*t){Move(y,t,rt,lb,gety);}

tile* pointFinding(int x,int y,tile*t){  // 要求 p.x < t.x + w && p.y > t.y , 允許p在tile左edge以及上edge , 但不允許在bottom 以及 right 
    if(t==nullptr){std::cerr<<"erro arg in point Finding\n";exit(1);}
    auto found = [](int x,int y,tile*t){return ( (x >= t->_x) && (x < t->_x + t->_w) && (y >= t->_y) && (y < t->_y + t->_h));};
    while(!found(x,y,t)){
        Hmove(x,t);//Horizontal 
        if(t && found(x,y,t))break;
        Vmove(y,t);//Vertical
    }
    if(!t){std::cerr<<"erro in point Finding, can't find target!\n";exit(1);}
    return t;
}

std::vector<tile*> Vsearch(int x,int y,tile*t){//find all tiles cross from (x,y) to (x,0). 
    std::vector<tile*>Tiles;
    while( y > 0 && (t = pointFinding(x,y,t))){   //use t as start to find the tile contian point (x,y)
        Tiles.push_back(t);
        y = t->_y - 1;
    } 
    return Tiles;
}


void InsertBlock(int id,int x,int y,int w,int h){
    std::vector<tile*>tiles = Vsearch(x, y + h, init);

    //now get tiles , we can split this tiles by some way 

    //還需要找出包含top,bottom edge的兩種tile 
    //如果是同一種就是切四刀
    





}

std::ostream& operator<<(std::ostream&os,const tile &t){
    return os<<"tile id:"<<t.id()<<" left-corner : ("<<t.x()<<","<<t.y()<<")  width : "<<t.w()<<" height : "<<t.h();
}
tile* updateNeighbor(tile*t,tile*nb,t_get*get,int v,cmp *comp,t_move*next,t_set* set)   //return last neighbor sufficeint to value.
{
    while(nb && comp(get(nb),v)){
        set(nb,t);
        nb = next(nb);
    }
    return nb;
}
void updateRight(tile *origin,tile* partA,tile * partB){
    tile *last = updateNeighbor(partA,origin->tr(),gety,partA->y(),nls,lb,setbl);
    partB->settr(last);
    updateNeighbor(partB,last,gety,partB->y(),nls,lb,setbl);
}
void updateTop(tile *origin,tile* partA,tile * partB){
    tile *last = updateNeighbor(partA,origin->rt(),getx,partA->x(),nls,bl,setlb);
    partB->setrt(last);
    updateNeighbor(partB,last,getx,partB->x(),nls,bl,setlb);
}
void updateLeft(tile *origin,tile* partA,tile * partB){
    tile *last = updateNeighbor(partA,origin->bl(),gety,partA->y() + partA->h(),ls,rt,settr);
    partB->setbl(last);
    updateNeighbor(partB,last,gety,partB->y() + partB->h(),ls,rt,settr);
}
void updateBottom(tile *origin,tile* partA,tile * partB){
    tile *last = updateNeighbor(partA,origin->lb(),getx,partA->x() + partA->w(),ls,tr,setrt);
    partB->setlb(last);
    updateNeighbor(partB,last,getx,partB->x() + partB->w(),ls,tr,setrt);
}


tile* Hsplit(tile*t,int y,bool bottom){
    tile *top = new tile(t->id(),t->x(),y, t->w(), t->y() + t->h() - y,nullptr,nullptr,t->tr(),t->rt());
    tile *bot = new tile(t->id(),t->x(),t->y(),t->w(), y - t->y(),t->bl(),t->lb());
    top->setlb(bot);
    bot->setrt(top);
    updateRight(t,top,bot);
    updateTop(t,top,top);
    updateLeft(t,bot,top);
    updateBottom(t,bot,bot);
    if(bottom){
        return bot;
    }else{
        return top;
    }
}

tile* Vsplit(tile*t,int x,bool left){
    tile *RGT = new tile(t->id(),  x    ,t->y(), t->w() + t->x() - x, t->y(), nullptr,nullptr,t->tr(),t->rt());
    tile *LFT  = new tile(t->id(),t->x() ,t->y(),   x - t->x()       , t->y(), t->bl(),t->lb());
    RGT->setbl(LFT);
    LFT->settr(RGT);
    updateRight(t,RGT,RGT);
    updateTop(t,RGT,LFT);
    updateLeft(t,LFT,LFT);
    updateBottom(t,LFT,RGT);
    if(left){
        return LFT;
    }else{
        return RGT;
    }
}


int main()
{
    tile t(-1,0,0,200,200); // initial space tile.
    init = &t;

    tile* bot = Hsplit(&t,30);


    // std::cout<<bot->x()<<" "<<bot->y()<<" "<<bot->w()<<" "<<bot->h()<<"\n";
    std::cout<<*bot<<"\n"<<*bot->rt()<<"\n";


    tile* lft = Vsplit(bot,100);
    std::cout<<*lft<<"\n"<<*lft->tr()<<"\n";


}
