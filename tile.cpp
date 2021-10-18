#include "tile.hpp"
#include <vector>
#include <iostream>


// Horizontal/Vertical move   
tile* Hfind(int x,tile*t){
    if(t && x < t->x()){
        while(t && x < t->x())
            t = t -> bl();
    }else if(t && x >= t->x() + t->w()){   // point can't lie on right edge , so when x = t->x() + t->w() , it must move.
        while(t && x >= t->x() + t->w())
            t = t -> tr();
    }
    return t;
}
tile* Vfind(int y,tile*t){
    if(t && y > t->y() + t->h()){
        while(t && y > t->y() + t->h())
            t = t -> rt();
    }else if(t && y <= t->y()){   // point can't lie on bottom edge , so when y = t->y()  , it must move.
        while(t && y <= t->y())
            t = t -> lb();
    }
    return t;
}

// From "start" to do pointFinding algorithm until find some tile which contain (x,y) point.
// (x,y) Point is allowed to lie on left/top edge of tile, but is not allowed to lie on right/bottom of tile.       
// This property make one point only be contained by one tile.   
//hint   x can equal to t->x()  (left edge )  , y can equal to t->y() + t->h()  (top edge )
// but x can't equal to t->x() + t->w() (right edge) , y can't eqautl to t->y() (bottom edge)
tile* pointFinding(int x,int y,tile*t){  
    if(t==nullptr){std::cerr<<"erro arg in point Finding\n";exit(1);}
    auto found = [](int x,int y,tile*t){return ( (x >= t->x()) && (x < t->x() + t->w()) && (y > t->y()) && (y <= t->y() + t->h()));};
    while(t&&!found(x,y,t)){
        t = Hfind(x,t);//Horizontal 
        if(t && found(x,y,t))break;
        t = Vfind(y,t);//Vertical
    }
    if(!t){std::cerr<<"erro in point Finding, can't find target!\n";exit(1);}
    return t;
}


// Find all tiles cross from (x,y) to (x,0) and return.
std::vector<tile*> Vsearch(int x,int y,tile*t){
    std::vector<tile*>Tiles;
    while( y > 0 && (t = pointFinding(x,y,t))){ 
        Tiles.push_back(t);
        y = t->y() - 1;
    } 
    return Tiles;
}



// show information of tile t , including id,x,y,w,h.    
std::ostream& operator<<(std::ostream&os,const tile &t){
    return os<<"tile id:"<<t.id()<<" left-corner : ("<<t.x()<<","<<t.y()<<")  width : "<<t.w()<<" height : "<<t.h();
}


// Split's helper function    
// updating all neighbors nb's pointer to t if  target condition is happend.    
// while comp(get(nb),v) is true, then set nb's some ptr to tile t.    
// nb use the "set" function to decide which pointer to be set i.e lb,bl,tr,rt....
// nb use the "next" function to find next neighbor.      
// this function return last nb which is not neighbor of tile t.    
tile* updateNeighbor(tile*t,tile*nb,t_get*get,int v,cmp *comp,t_move*next,t_set* set){   //return last neighbor sufficeint to value.
    while(nb && comp(get(nb),v)){
        set(nb,t);
        nb = next(nb);
    }
    return nb;
}

// updateFunc(tile *origin,tile *partA,tile *part B)

// use partA to find the start of neighbor    
// for example, if you want to update rightside, you need use the "tr" pointer, which is belong to right/top part , so part A is right or top 
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
    return bottom ? bot:top;
}

tile* Vsplit(tile*t,int x,bool left){
    tile *RGT = new tile(t->id(),  x    ,t->y(), t->w() + t->x() - x, t->h(), nullptr,nullptr,t->tr(),t->rt());
    tile *LFT  = new tile(t->id(),t->x() ,t->y(),   x - t->x()       , t->h(), t->bl(),t->lb());
    RGT->setbl(LFT);
    LFT->settr(RGT);
    updateRight(t,RGT,RGT);
    updateTop(t,RGT,LFT);
    updateLeft(t,LFT,LFT);
    updateBottom(t,LFT,RGT);
    return left? LFT:RGT;
}



void InsertBlock(int id,int x,int y,int w,int h){
    std::vector<tile*>tiles = Vsearch(x, y + h, init);

    //now get tiles , we can split this tiles by some way 

    //還需要找出包含top,bottom edge的兩種tile 
    //如果是同一種就是切四刀
    



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

    tile *rgt = lft->tr();

    std::cout<<"rt = "<<*rgt->rt()<<"\n";

    // std::cout<<lft->tr()->x()<<"\n";
    // std::cout<<"point finding:"<<*pointFinding(0,30,lft)<<"\n";
    
    std::cout<<"point finding:"<<*pointFinding(100,30,lft)<<"\n";

    std::cout<<"point finding:"<<*pointFinding(100,60,lft)<<"\n";

    std::cout<<"point finding:"<<*pointFinding(0,200,lft)<<"\n";
}
