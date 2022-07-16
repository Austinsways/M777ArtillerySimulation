#include "ground.h"
using namespace::std;

class Collision {
public:
    bool isCrashed(Ground ground, Position pt) {
        // if bullet is in range of target then its not a crash, its a hit
        Position target;
        target = ground.getTarget();
        if (target.getMetersX() - 400 <= pt.getMetersX() && pt.getMetersX() <= target.getMetersX()+400)
        {
            return false;
            }
        // if bullet is not in range of targer then its true its a crash
        else{ return true;}
    }
    
    
};
