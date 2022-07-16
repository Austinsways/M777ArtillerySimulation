//Code manipulated and rewritten by Austin Eldredge and Conner Haar on July 16 2022

#include <cassert>      // for ASSERT
#include "uiInteract.h" // for INTERFACE
#include "uiDraw.h"     // for RANDOM and DRAW*
#include "ground.h"     // for GROUND
#include "position.h"   // for POSITION
#include "velocity.h" // for VELOCITY
#include "Acceleration.h" // for ACCELERATION
#include "bullet.h"      // for BULLET
#include "Collision.cpp"
#include "test.h"       // for TESTS

#include <vector>       //for usng vectors
using namespace std;

/*************************************************************************
 * Game
 * Test structure to capture the LM that will move around the screen
 *************************************************************************/
class Game
{
public:
   Game(Position ptUpperRight) :
      ptUpperRight(ptUpperRight),
      ground(ptUpperRight),
      time(0.0)
   {
      ptHowitzer.setPixelsX(Position(ptUpperRight).getPixelsX() / 2.0);
      ground.reset(ptHowitzer);
      /*for (int i = 0; i < 20; i++)

      {
            
             bullet[i].position = ptHowitzer;
             projectilePath[i].setPixelsX((double)i * 2.0);
             projectilePath[i].setPixelsY(ptUpperRight.getPixelsY() / 1.5);

      }*/

   }

   Ground ground;                 // the ground
   Position  projectilePath[20];  // path of the projectile
   Position  ptHowitzer;          // location of the howitzer
   Position  ptUpperRight;        // size of the screen
   Bullet bullet[20]; // creates a vector of bullet objects
   Angles angle;                  // angle of the howitzer
   Collision collision;          
   double time;                   // amount of time since the last firing
   bool hit = false;              //wether or not the bullet has hit the ground
   bool won = false;              //the players win/lose status
   Position endScreen;
   


   //public functions

   //fire a new (resetted) bullet if the user presses space and the old bullet has hit the ground
   void fireBullet() {
       if (hit || bullet->acceleration.ddx == 0.0) {        //this checks if the bullets not been fired yet with horizontal acceleration, or if its time for a retry
           time = 0.0;
           bullet->angle = angle.angle;                     //set the bullet angle to the direction of the howitzer
           bullet->position = ptHowitzer;                   //originate the bullet from the howitzer
           bullet->velocity.initializeVelocities(bullet->angle.angle);
           won = false;
           hit = false;
       }
   }

   void advanceTime() {
       if (!hit)
          time += 0.5;
   }

   void moveBullet() {
       for (int i = 0; i < 20; i++)          //calculate movement 
       {

           // Progresses this instance of bullet to make it move
           if (!hit) {
               bullet[i].tick();
           }

       }
   }

   //use the collision class to check the status of the bullet
   void detectCollisions() {
       if (bullet[0].position.getMetersY() <= ground.getElevationMeters(bullet[0].position))    //check if the bullets hit the ground
       {
           bullet[0].position.setMetersY(ground.getElevationMeters(bullet[0].position));
           // if crashed
           if (!(collision.isCrashed(ground, bullet[0].position))) {
               won = true;      //tell the game the bullet collided in the correct location and won!
           }
           // else its a hit
           hit = true;          //tell the game the bullet has hit the ground
       }
   }

   
};

/*************************************
 * All the interesting work happens here, when
 * I get called back from OpenGL to draw a frame.
 * When I am finished drawing, then the graphics
 * engine will wait until the proper amount of
 * time has passed and put the drawing on the screen.
 **************************************/
void callBack(const Interface* pUI, void* p)
{
   // the first step is to cast the void pointer into a game object. This
   // is the first step of every single callback function in OpenGL.
   Game* pGame = (Game*)p;

   //
   // accept input
   //

   // move a large amount, but not more than 90 degrees or less than -90
   if (pUI->isRight())
       if (pGame->angle.angle <= 1.57) {    //1.57 radians = 90 degrees approximately
           pGame->angle.angle += 0.05;
       }
   if (pUI->isLeft()) {
       if (pGame->angle.angle >= -1.57)
           pGame->angle.angle -= 0.05;
   }

   // move by a little, but not more than 90 degrees or less than -90
   if (pUI->isUp())
      pGame->angle.angle += (pGame->angle.angle >= 0 ? -0.003 : 0.003);
   if (pUI->isDown()) {
       if (pGame->angle.angle <= 1.57 && pGame->angle.angle >= -1.57)           //check to ensure the howitzer isnt aiming less than 90 degrees
           pGame->angle.angle += (pGame->angle.angle >= 0 ? 0.003 : -0.003);
   }
   // fire the bullet
   if (pUI->isSpace()) {        
       pGame->fireBullet();
   }


   //
   // perform all the game logic
   //
   
   //advance time forward
   pGame->advanceTime(); 
   //move bullet forward
   pGame->moveBullet();
   //check if the bullet has crashed
   pGame->detectCollisions();


   //
   // draw everything
   //

   ogstream gout(Position(10.0, pGame->ptUpperRight.getPixelsY() - 20.0));
   
   // sets position of where we want to display the success on screen
   pGame->endScreen.setPixelsX(300);
   pGame->endScreen.setPixelsY(410);
   
   

   //check if the player has won
   if (pGame->won) {
       gout.drawText(pGame->endScreen, "You hit the target!");
   }

   //check if the player has lost
   else if (pGame->hit) {
       gout.drawText(pGame->endScreen, "You missed");
   }
   
   
   // draw the ground first
   pGame->ground.draw(gout);

   // draw the howitzer
   gout.drawHowitzer(pGame->ptHowitzer, pGame->angle.angle, pGame->time);

   // draw the projectile
   for (int i = 0; i < 20; i++) {
       gout.drawProjectile(pGame->bullet[i].position, 0.5 * (double)i);
       //gout.drawProjectile(pGame->projectilePath[i], 0.5 * (double)i);
   }

   // draw some text on the screen
   gout.setf(ios::fixed | ios::showpoint);
   gout.precision(1);
   gout << "Time since the bullet was fired: "
      << pGame->time << "s\n";
}

double Position::metersFromPixels = 40.0;

/*********************************
 * Initialize the simulation and set it in motion
 *********************************/
#ifdef _WIN32_X
#include <windows.h>
int WINAPI wWinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ PWSTR pCmdLine,
   _In_ int nCmdShow)
#else // !_WIN32
int main(int argc, char** argv)
#endif // !_WIN32
{
   // Test Runner
   // testRunner();
   // Initialize OpenGL
   Position ptUpperRight;
   ptUpperRight.setPixelsX(700.0);
   ptUpperRight.setPixelsY(500.0);
   Position().setZoom(40.0 /* 42 meters equals 1 pixel */);
   Interface ui(0, NULL,
      "Game",   /* name on the window */
      ptUpperRight);

   // Initialize the Game
   Game Game(ptUpperRight);

   // set everything into action
   ui.run(callBack, &Game);


   return 0;
}
