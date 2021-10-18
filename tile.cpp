#include "tile.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
userlog *Log;

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


// (x,y) Point is allowed to lie on left/top edge of tile, but is not allowed to lie on right/bottom of tile.       
// This property make one point only be contained by one tile.   
//hint   x can equal to t->x()  (left edge )  , y can equal to t->y() + t->h()  (top edge )
// but x can't equal to t->x() + t->w() (right edge) , y can't eqautl to t->y() (bottom edge)
tile* pointFinding(int x,int y){  
    tile *t = Log->lastLog();
    if(t==nullptr){std::cerr<<"erro arg in point Finding\n";exit(1);}
    auto found = [](int x,int y,tile*t){return ( (x >= t->x()) && (x < t->x() + t->w()) && (y > t->y()) && (y <= t->y() + t->h()));};
    while(t&&!found(x,y,t)){
        t = Hfind(x,t);//Horizontal 
        if(t && found(x,y,t))break;
        t = Vfind(y,t);//Vertical
    }
    if(!t){std::cerr<<"erro in point Finding, can't find target ("<<x<<","<<y<<")\n";exit(1);}
    Log->addLog(t);
    return t;
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
    if(partA!=partB){
        partB->settr(last);
        updateNeighbor(partB,last,gety,partB->y(),nls,lb,setbl);
    }
}
void updateTop(tile *origin,tile* partA,tile * partB){
    tile *last = updateNeighbor(partA,origin->rt(),getx,partA->x(),nls,bl,setlb);
    if(partA!=partB){
        partB->setrt(last);
        updateNeighbor(partB,last,getx,partB->x(),nls,bl,setlb);
    }
}
void updateLeft(tile *origin,tile* partA,tile * partB){
    tile *last = updateNeighbor(partA,origin->bl(),gety2,partA->y() + partA->h(),ngt,rt,settr);
    if(partA!=partB){
        partB->setbl(last);
        updateNeighbor(partB,last,gety,partB->y() + partB->h(),ngt,rt,settr);
    }
}
void updateBottom(tile *origin,tile* partA,tile * partB){
    tile *last = updateNeighbor(partA,partA->lb(),getx2,partA->x() + partA->w(),ngt,tr,setrt);
    if(partA!=partB){
        partB->setlb(last);  
        updateNeighbor(partB,last,getx2,partB->x() + partB->w(),ngt,tr,setrt);
    }
}


tile* Hsplit(tile*t,int y,bool bottom){
    tile *top = new tile(t->id(),t->x(),y, t->w(), t->y() + t->h() - y,nullptr,nullptr,t->tr(),t->rt());
    tile *bot = new tile(t->id(),t->x(),t->y(),t->w(), y - t->y(),t->bl(),t->lb());

    // std::cout<<"top->rt ="<<top->rt()<<"\n";
    top->setlb(bot);
    bot->setrt(top);
    updateRight(t,top,bot);
    updateTop(t,top,top);
    updateLeft(t,bot,top);
    updateBottom(t,bot,bot);
    Log->addLog(bot);
    // std::cout<<"top->rt ="<<top->rt()<<"\n";
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
    
    Log->addLog(LFT);
    return left? LFT:RGT;
}





// left - bottom (x1,y1)   right top (x2,y2)
// check if this area has any block by checkin maximal horizontal stripes.
// If has no blocks , return the space tiles in this region    
// otherwise return empty.     
std::vector<tile*> AreaSearch(int x1,int y1,int x2,int y2){
    std::vector<tile*>Tiles;
    tile*t1,*t2;
    bool block = false;
    while(!block && y2 > y1){  // can't equal
        t1 = pointFinding(x1,y2);
        t2 = pointFinding(x2,y2);
        if(t1 != t2){   // if t1 and t2 are not the same  and t2->x() is smaller than x2 , it must has block tiles in this region!(t2 or t1)      
            if(t2->x() < x2)
                block = true;
        }

        if(!t1->isSpace())
            block = true;
        Tiles.push_back(t1);
        y2 = t1->y();//move down if no block.
    }
    if(block)return {};
    return Tiles;  //if Tiles = empty : this area has block tile.
}



void showptr(tile*t)
{
    std::cout<<"lb:"<<t->lb()<<" bl:"<<t->bl()<<" rt:"<<t->rt()<<" tr:"<<t->tr()<<"\n";
}


tile* insert1(tile*t ,int x,int y,int w,int h)
{
    std::set<tile*>rec;
    if(gety2(t) > y + h){
        rec.insert(t);
        t = Hsplit(t,y+h);
    }
    if(gety(t) < y){
        rec.insert(t);
        t = Hsplit(t,y,false);
    }
    if(getx(t) < x){
        rec.insert(t);
        t = Vsplit(t,x,false);
    }
    if(getx2(t) > x + w){
        rec.insert(t);
        t = Vsplit(t,x+w);
    }
    for(auto r:rec)delete r;
    return t;
}



//還要寫一個merge function
void merge(std::vector<tile*>tiles);
void mergeCheck(tile*t); //merge if have same horizontal span.  use lb pointer.

tile* InsertBlock(int id,int x,int y,int w,int h){
    auto SpaceTiles = AreaSearch(x,y,x+w,y+h);
    if(SpaceTiles.empty()){return nullptr;} // blocks exist.

    tile* blockTile = nullptr;
    if(SpaceTiles.size()==1)
    {
        tile* space = SpaceTiles.at(0);
        if(space->x()==x&&space->y()==y&&space->w()==w&&space->h()==h)//exactly same size.
            blockTile = space;
        else{//much bigger than this block
            blockTile = insert1(space,x,y,w,h);
        }
    }
    else{


    }

    Log->addLog(blockTile);
    blockTile->setid(id);
    return blockTile;
}



std::vector<tile*> NeighborTraversal(tile*t,tile*nb,t_get*get,int v,cmp *comp,t_move*next){   //return last neighbor sufficeint to value.
    std::vector<tile*>nbs;
    while(nb && comp(get(nb),v)){
        nbs.push_back(nb);
        nb = next(nb);
    }
    return nbs;
}


std::set<tile*> getNeighbor(tile*t){
    std::set<tile*>nbs;
    if(t->bl())nbs.insert(t->bl());
    if(t->lb())nbs.insert(t->lb());
    if(t->tr())nbs.insert(t->tr());
    if(t->rt())nbs.insert(t->rt());
    auto left = NeighborTraversal(t,t->bl(),gety2,t->y()+t->h(),ngt,::rt);
    auto bottom = NeighborTraversal(t,t->lb(),getx2,t->x()+t->w(),ngt,::tr);
    auto right = NeighborTraversal(t,t->tr(),gety,t->y(),nls,::lb);
    auto top = NeighborTraversal(t,t->rt(),getx,t->x(),nls,::bl);
    for(auto l:left)nbs.insert(l);
    for(auto r:right)nbs.insert(r);
    for(auto t:top)nbs.insert(t);
    for(auto b:bottom)nbs.insert(b);
    return nbs;
}



int main()
{
    Log = new userlog(500,500);
    


    tile* b1 = InsertBlock(1,50,40,250,60);
    tile* b2 = InsertBlock(2,55,250,50,150);

    std::cout<<*b1<<"\n";
    std::cout<<*b2<<"\n";
   

    auto nbs = getNeighbor(b1);

    for(auto nb:nbs)
    {
        std::cout<<*nb<<"\n";
    }


    auto nbs2 = getNeighbor(b2);

    for(auto nb:nbs2)
    {
        std::cout<<*nb<<"\n";
    }

   
}
