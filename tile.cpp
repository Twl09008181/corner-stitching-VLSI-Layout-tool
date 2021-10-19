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
tile* updateOneSide(tile*t,tile*nb,t_get*get,int v,cmp *comp,t_move*next,t_set* set){   //return last neighbor sufficeint to value.
    while(nb && comp(get(nb),v)){
        set(nb,t);
        nb = next(nb);
    }
    return nb;
}


// updateFunc(tile *partA,tile *part B)
// use partA to find the start of neighbor    
// for example, if you want to update rightside, you need use the "tr" pointer, which is belong to right/top part , so part A is right or top 
void updateRight(tile* partA,tile * partB){
    tile *last = updateOneSide(partA,partA->tr(),gety,partA->y(),nls,lb,setbl);
    if(partA!=partB){
        partB->settr(last);
        updateOneSide(partB,last,gety,partB->y(),nls,lb,setbl);
    }
}
void updateTop(tile* partA,tile * partB){
    tile *last = updateOneSide(partA,partA->rt(),getx,partA->x(),nls,bl,setlb);
    if(partA!=partB){
        partB->setrt(last);
        updateOneSide(partB,last,getx,partB->x(),nls,bl,setlb);
    }
}
void updateLeft(tile* partA,tile * partB){
    tile *last = updateOneSide(partA,partA->bl(),gety2,partA->y() + partA->h(),ngt,rt,settr);
    if(partA!=partB){
        partB->setbl(last);
        updateOneSide(partB,last,gety2,partB->y() + partB->h(),ngt,rt,settr);
    }
}
void updateBottom(tile* partA,tile * partB){
    tile *last = updateOneSide(partA,partA->lb(),getx2,partA->x() + partA->w(),ngt,tr,setrt);
    if(partA!=partB){
        partB->setlb(last);  
        updateOneSide(partB,last,getx2,partB->x() + partB->w(),ngt,tr,setrt);
    }
}


tile* Hsplit(tile*t,int y,bool bottom){
    tile *top = new tile(t->id(),t->x(),y, t->w(), t->y() + t->h() - y,nullptr,nullptr,t->tr(),t->rt());
    tile *bot = new tile(t->id(),t->x(),t->y(),t->w(), y - t->y(),t->bl(),t->lb());
    top->setlb(bot);
    bot->setrt(top);
    updateRight(top,bot);
    updateTop(top,top);
    updateLeft(bot,top);
    updateBottom(bot,bot);
    Log->addLog(bot);
    return bottom ? bot:top;
}

tile* Vsplit(tile*t,int x,bool left){
    tile *RGT = new tile(t->id(),  x    ,t->y(), t->w() + t->x() - x, t->h(), nullptr,nullptr,t->tr(),t->rt());
    tile *LFT  = new tile(t->id(),t->x() ,t->y(),   x - t->x()       , t->h(), t->bl(),t->lb());
    RGT->setbl(LFT);
    LFT->settr(RGT);
    updateRight(RGT,RGT);
    updateTop(RGT,LFT);
    updateLeft(LFT,LFT);
    updateBottom(LFT,RGT);
    
    Log->addLog(LFT);
    return left? LFT:RGT;
}





// left - bottom (x1,y1)   right top (x2,y2)
// check if this area has any block by checkin maximal horizontal stripes.
// If has no blocks , return the space tiles in this region    
// otherwise return empty.     
std::vector<tile*> AreaSearch(int x1,int y1,int x2,int y2){
    std::vector<tile*>Tiles;
    
    bool block = false;
    while(!block && y2 > y1){  // can't equal
        tile*t1 = nullptr,*t2 = nullptr;
        
        t1 = pointFinding(x1,y2);
        if(y2!=Log->getw())
            t2 = pointFinding(x2,y2);
        if(t2 && t1 != t2){   // if t1 and t2 are not the same  and t2->x() is smaller than x2 , it must has block tiles in this region!(t2 or t1)      
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




// cut "t" to the shape w * h
tile* tailor(tile*t ,int x,int y,int w,int h)
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




bool checkspan(std::vector<tile*>tiles){
    for(int i = 1;i < tiles.size();i++)
        if(tiles.at(i-1)->x()!=tiles.at(i)->x()||tiles.at(i-1)->w()!=tiles.at(i)->w())
            return false;
    return true;
}

bool continuous(std::vector<tile*>tiles){

    if(!checkspan(tiles))return false;

    for(int i = 1;i < tiles.size();i++)
        if(gety(tiles.at(i-1)) != gety2(tiles.at(i)))
            return false;
    return true;
}

// merge continuous tiles from high to low sorted. 
tile* merge(std::vector<tile*>tiles)
{
    if(continuous(tiles)){
        tile*upper = tiles.at(0);
        tile*lower = tiles.at(tiles.size()-1);
        
        // ptr setting for updating
        upper ->seth(gety2(upper) - lower->y());
        upper ->sety(lower->y());
        
        upper ->setlb(lower->lb());
        upper ->setbl(lower->bl());
        // update
        updateRight(upper,upper);
        updateLeft(upper,upper);
        updateTop(upper,upper);
        updateRight(upper,upper);
    }else{
        std::cout<<"tiles are not continuous, can't merge\n";
    }
    for(int i = 1;i<tiles.size();i++)delete tiles.at(i);
    return tiles.at(0);
}


tile* InsertBlock(int id,int x,int y,int w,int h){
    auto SpaceTiles = AreaSearch(x,y,x+w,y+h);
    if(SpaceTiles.empty()){return nullptr;} // blocks exist.

    std::vector<tile*>blocks;
    for(int i = 0;i < SpaceTiles.size() ; i++)
        blocks.push_back(tailor(SpaceTiles.at(i),x,y,w,h));
    
    
    tile* blockTile = merge(blocks);
    //check left space 
    

    //check right space
    Log->addLog(blockTile);
    blockTile->setid(id);
    return blockTile;
}



std::vector<tile*> NeighborTraversal(tile*t,tile*nb,t_get*get,int v,cmp *comp,t_move*next){   //return last neighbor sufficeint to value.
    std::vector<tile*>nbs;
    nbs.push_back(nb);
    nb = next(nb);
    while(nb && comp(get(nb),v)){
        nbs.push_back(nb);
        nb = next(nb);
    }
    return nbs;
}





std::vector<tile*>getLeft(tile*t){
    return (t&&t->bl()) ? NeighborTraversal(t,t->bl(),gety2,gety2(t),ngt,rt) : std::vector<tile*>{}; 
}
std::vector<tile*>getRight(tile*t){
    return (t&&t->tr()) ? NeighborTraversal(t,t->tr(),gety,gety(t),nls,lb)   : std::vector<tile*>{}; 
}
std::vector<tile*>getTop(tile*t){
    return (t&&t->rt()) ? NeighborTraversal(t,t->rt(),getx,getx(t),nls,bl)   : std::vector<tile*>{}; 
}
std::vector<tile*>getBottom(tile*t){
    return (t&&t->lb())? NeighborTraversal(t,t->lb(),getx2,getx2(t),ngt,tr)  : std::vector<tile*>{}; 
}

std::vector<tile*>getNeighbor(tile*t)
{
    auto left = getLeft(t);
    auto bottom = getBottom(t);
    auto right = getRight(t);
    auto top = getTop(t);
    std::vector<tile*>nbs;
    nbs.resize(left.size() + bottom.size() + right.size() + top.size());
    auto it = std::copy(left.begin(),left.end(),nbs.begin());
    it = std::copy(bottom.begin(),bottom.end(),it);
    it = std::copy(right.begin(),right.end(),it);
    std::copy(top.begin(),top.end(),it);
    return nbs;
}



int main()
{
    Log = new userlog(500,500);
    
    tile* b1 = InsertBlock(1,50,40,250,60);
    tile* b2 = InsertBlock(2,55,250,50,150);
    std::cout<<*b1<<"\n";
    std::cout<<*b2<<"\n";


    auto nbs1 = getNeighbor(b1);
    auto nbs2 = getNeighbor(b2);

    for(auto nb:nbs1)
    {
        std::cout<<*nb<<"\n";
    }
        for(auto nb:nbs2)
    {
        std::cout<<*nb<<"\n";
    }

    // tile* space = b1->tr();
    // std::cout<<"space = "<<*space<<"\n";
    // tile *t1 = Hsplit(space,200);
    // tile *t2 = Hsplit(t1,80,false);

    // std::cout<<"show split spaces\n";
    // std::cout<<*t2->rt()<<"\n";
    // std::cout<<*t2<<"\n";
    // std::cout<<*t2->lb()<<"\n";

    // std::cout<<"check ptr:\n";
    // std::cout<<*b1->tr()<<"\n";
    // std::cout<<*b2->bl()<<"\n";


    // std::vector<tile*> continuousSpace;

    // continuousSpace.push_back(t2->rt());
    // continuousSpace.push_back(t2);
    // continuousSpace.push_back(t2->lb());

    // tile* mergeTile  = merge(continuousSpace);

    // std::cout<<"after merge\n";
    // std::cout<<*mergeTile<<"\n";


    // std::cout<<"check  ptr:\n";
    // std::cout<<*b1->tr()<<"\n";
    // std::cout<<*b2->bl()<<"\n";
   
}
