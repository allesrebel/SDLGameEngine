#include "Exception.h"
#include "SDLAbstractionLayer.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

Surface::Surface() :
      s(NULL) {
}

Surface::Surface(SDL_Surface* sdls, Color ck) {
   SDL_Surface* optImg = optimizeImage(sdls);
   setColorKey(ck, optImg);
   s = optImg;
}

Surface::Surface(string filepath, Color ck) :
      s(loadImage(filepath, ck)) {
}

Surface::Surface(string ttfFile, int fontSize, Color textColor, string msg) :
      s(loadText(ttfFile, fontSize, textColor, msg)) {
}

void Surface::setSDL_Surface(SDL_Surface* surface) {
   s = surface;
}

void Surface::setSDL_Surface(string filepath, Color ck) {
   s = loadImage(filepath, ck);
}

void Surface::setSDL_Surface(string ttfFile, int fontSize,
      Surface::Color textColor, string msg) {
   s = loadText(ttfFile, fontSize, textColor, msg);
}

SDL_Surface * Surface::getSDL_Surface() {
   return s;
}

Surface::~Surface() {
   SDL_FreeSurface(s);
}

Music::Music() :
      m(NULL) {
}

Music::Music(string music) :
      m(loadMusic(music)) {
}

void Music::setMusic(string music) {
   m = loadMusic(music);
}

Mix_Music* Music::getMix_Music() {
   return m;
}

Music::~Music() {
   Mix_FreeMusic(m);
}

Chunk::Chunk() :
      c(NULL) {
}

Chunk::Chunk(string chunk) :
      c(loadChunk(chunk)) {
}

void Chunk::setChunk(string chunk) {
   c = loadChunk(chunk);
}

Mix_Chunk* Chunk::getMix_Chunk() {
   return c;
}

Chunk::~Chunk() {
   Mix_FreeChunk(c);
}

Timer::Timer() {
   startTicks = 0;
   pausedTicks = 0;
   paused = false;
   started = false;
}

void Timer::start() {
   started = true;
   paused = false;
   startTicks = SDL_GetTicks();
}

void Timer::stop() {
   started = false;
   paused = false;
}

void Timer::pause() {
   if (started && !paused) {
      paused = true;
      pausedTicks = SDL_GetTicks() - startTicks;
   }
}

void Timer::unpause() {
   if (paused) {
      paused = false;
      startTicks = SDL_GetTicks() - pausedTicks;
      pausedTicks = 0;
   }
}

int Timer::getTicks() {
   if (started) {
      if (paused)
         return pausedTicks;
      else
         return SDL_GetTicks() - startTicks;
   }

   return 0;
}

bool Timer::isStarted() {
   return started;
}

bool Timer::isPaused() {
   return paused;
}

int Timer::delayFrame(int fps) {
   int delay;

   if (this->getTicks() < 1000 / fps)
      SDL_Delay(delay = 1000 / fps - this->getTicks());

   return delay;
}

Particle::Particle(SDL_Rect posDim, Surface* p1, Surface* p2, Surface* p3,
      Surface* p4, SDL_Surface* screen) :
      p1(p1), p2(p2), p3(p3), p4(p4), screen(screen) {
   //set offsets
   this->posDim.x = posDim.x + randRange(-5, posDim.w);
   this->posDim.y = posDim.y + randRange(-5, posDim.h);

   //init animation frame
   frame = randRange(0, 4);

   //set type
   switch (randRange(0, 2)) {
   case 0:
      type = this->p1;
      break;
   case 1:
      type = this->p2;
      break;
   case 2:
      type = this->p3;
      break;
   default:
      break;
   }
}

void Particle::show(SDL_Rect* camera) {
   if (type != NULL)
      applySurface(posDim.x - camera->x, posDim.y - camera->y, *type, screen);

   if (frame % 2 == 0 && p4 != NULL)
      applySurface(posDim.x - camera->x, posDim.y - camera->y, *p4, screen);

   frame++;
}

bool Particle::isDead() {
   if (frame > PARTICLE_DRAG)
      return true;
   return false;
}

int Figure::determineGravity() {
   if (gravityEnabled && !u) {
      if (posDim.y < lh - posDim.h)
         v.y += gravity + abs(v.y * 0.01);
      else
         v.y = 0;
   }

   return v.y;
}

int Figure::determineJump() {

   if (gravityEnabled && u && !jumpAction) {
      jumpAction = true;
      u = false;
   }
   else if (!u && gravityEnabled && jumpAction && jumpFrame < 5) {
      v.y += abs(v.y / JUMP_SMOOTHENER);
      jumpFrame++;
   }
   else {
      jumpFrame = 0;
      jumpAction = false;
   }

   return v.y;
}

void Figure::checkIfInAir(vector<Figure*>& other) {
   int count = 0;

   inAir = true;
   posDim.y += 2;
   if ((v.y == 0 && posDim.y >= lh - posDim.h)
         || (v.y <= gravity && isCollided(other, count)))
      inAir = false;
   posDim.y -= 2;
   if (posDim.y < lh - posDim.h && v.y <= 1 && v.y >= -1)
      inAir = true;
}

void Figure::initialize(int x, int y, double gravity, double speed,
      double jumpStrength, SDL_Surface* screen, Gravity gravityEnabled,
      bool leader, int numClips, int levelWidth, int levelHeight, Surface* p1,
      Surface* p2, Surface* p3, Surface* p4) {
   posDim.x = x;
   posDim.y = y;
   this->leader = leader;

   jumpFrame = 0;
   this->numClips = numClips;
   cl = new SDL_Rect[numClips];
   cr = new SDL_Rect[numClips];
   status = RIGHT;

   setClips(posDim.w, posDim.h);

   this->gravity = gravity;
   this->speed = speed;
   if (jumpStrength < 1)
      this->jumpStrength = 1;

   this->jumpStrength = jumpStrength;
   jumpAction = false;

   l = r = u = d = false;
   this->screen = screen;

   v.x = v.y = 0;

   switch (gravityEnabled) {
   case GRAVITY_ENABLED:
      this->gravityEnabled = true;
      break;
   case GRAVITY_DISABLED:
      this->gravityEnabled = false;
      this->jumpStrength = 1;
      break;
   default:
      throw GravityException();
      break;
   }

   camera = new SDL_Rect;

   if (levelWidth == -1)
      lw = screen->w;
   else
      lw = levelWidth;

   if (levelHeight == -1)
      lh = screen->h;
   else
      lh = levelHeight;

   camera->x = camera->y = 0;
   camera->w = screen->w;
   camera->h = screen->h;

   particleEffects = false;
   this->p1 = this->p2 = this->p3 = this->p4 = NULL;

   if (p1 != NULL) {
      particleEffects = true;
      this->p1 = p1;
   }
   if (p2 != NULL) {
      particleEffects = true;
      this->p2 = p2;
   }
   if (p3 != NULL) {
      particleEffects = true;
      this->p3 = p3;
   }
   if (p4 != NULL) {
      particleEffects = true;
      this->p4 = p4;
   }

   if (particleEffects) {
      for (int i = 0; i < TOTAL_PARTICLES; i++) {
         particles[i] = new Particle(posDim, this->p1, this->p2, this->p3,
               this->p4, screen);
      }
   }
   else {
      for (int i = 0; i < TOTAL_PARTICLES; i++) {
         particles[i] = NULL;
      }
   }
}

void Figure::setClips(int clipWidth, int clipHeight) {
   for (int i = 0; i < numClips; i++) {
      cr[i].w = clipWidth;
      cr[i].h = clipHeight;
      cr[i].x = clipWidth * i;
      cr[i].y = 0;

      cl[i].w = clipWidth;
      cl[i].h = clipHeight;
      cl[i].x = clipWidth * i;
      cl[i].y = clipHeight;
   }
}

void Figure::xMovement(vector<Figure*>& other) {
   int count = 0;

   posDim.x += v.x;

   if (isCollided(other, count))
      posDim.x -= v.x;
   else if (posDim.x > lw - posDim.w)
      posDim.x = lw - posDim.w;
   else if (posDim.x < 0)
      posDim.x = 0;
}

void Figure::yMovement(vector<Figure*>& other) {
   int count = 0;

   //gravity considerations
   determineGravity();

   //jump action
   determineJump();

   //check if inAir is true
   checkIfInAir(other);

   //collision with boundaries or other Figures
   posDim.y += v.y;
   if (isCollided(other, count)) {
      posDim.y -= v.y;

      if (gravityEnabled)
         v.y = 0;
   }
   else if (posDim.y > lh - posDim.h)
      posDim.y = lh - posDim.h;
   //upper bounds
   //else if (posDim.y < 0)
   //posDim.y = 0;
}

void Figure::setCamera() {
   camera->x = posDim.x + posDim.w / 2 - camera->w / 2;
   camera->y = posDim.y + posDim.h / 2 - camera->h / 2;

   if (camera->x < 0)
      camera->x = 0;
   if (camera->y < 0)
      camera->y = 0;
   if (camera->x > lw - camera->w)
      camera->x = lw - camera->w;
   if (camera->y > lh - camera->h)
      camera->y = lh - camera->h;

   if (DEBUG_CAM) {
      cout << "camera->x: " << camera->x << endl;
      cout << "camera->y: " << camera->y << endl;
   }
}

void Figure::debug() {
   cout << "posDim.x: " << posDim.x << endl;
   cout << "posDim.y: " << posDim.y << endl;
   cout << "posDim.w: " << posDim.w << endl;
   cout << "posDim.h: " << posDim.h << endl;
   cout << "gravity: " << this->gravity << endl;
   cout << "jumpstrength: " << this->jumpStrength << endl;
   cout << "speed: " << this->speed << endl;
   cout << "jumpframe: " << jumpFrame << endl;
   cout << "animationframe: " << animationFrame << endl;
   cout << "numClips: " << this->numClips << endl;
   cout << "status: ";
   switch (status) {
   case LEFT:
      cout << "LEFT" << endl;
      break;
   case RIGHT:
      cout << "RIGHT" << endl;
      break;
   default:
      cout << "ERROR: invalid enum" << endl;
      break;
   }
   cout << "Checking cl and cr for validity. "
         << "If program crashes here, then cl and cr are not initialized properly."
         << endl;
   cout << "cl[1].w: " << cl[1].w << endl;
   cout << "cr[1].w: " << cr[1].w << endl;
   cout << "gravityEnabled: " << gravityEnabled << endl;
   cout << "l: " << l << endl;
   cout << "r: " << r << endl;
   cout << "u: " << u << endl;
   cout << "d: " << d << endl;
   cout << "className: " << className << endl;
   cout << "Checking velocity for validity. "
         << "If program crashes here, then v.x and v.y are not initialized properly."
         << endl;
   cout << "v.x, v.y: " << v.x << ", " << v.y << endl;
   cout << "Checking screen for validity. "
         << "If program crashes here, then screen is not initialized properly."
         << endl;
   cout << "screen->w: " << this->screen->w << endl;
   cout << "Checking Surface* image for validity. "
         << "If program crashes here, then image is not initialized properly. "
         << endl;
   cout << "Surface* image width: " << this->image->getSDL_Surface()->w << endl;
   cout << "Checking SDL_Rect* camera for validity. "
         << "If program crashes here, then camera is not initialized properly. "
         << endl;
   cout << "camera->w: " << this->camera->w << endl;
   cout << "lw, lh: " << lw << ", " << lh << endl;
   cout << "leader: " << leader << endl;
   cout << endl;
}

Figure::Figure() {
}

Figure::Figure(int x, int y, Surface& image, SDL_Surface* screen,
      Gravity gravityEnabled, bool leader, double speed, int gravity,
      double jumpStrength, int numClips, int levelWidth, int levelHeight,
      Surface* p1, Surface* p2, Surface* p3, Surface* p4) :
      image(&image) {

   posDim.w = image.getSDL_Surface()->w / numClips;
   posDim.h = image.getSDL_Surface()->h / 2;

   initialize(x, y, gravity, speed, jumpStrength, screen, gravityEnabled,
         leader, numClips, levelWidth, levelHeight, p1, p2, p3, p4);
}

void Figure::setFigure(int x, int y, Surface& image, SDL_Surface* screen,
      Gravity gravityEnabled, bool leader, double speed, int gravity,
      double jumpStrength, int numClips, int levelWidth, int levelHeight,
      Surface* p1, Surface* p2, Surface* p3, Surface* p4) {

   posDim.w = image.getSDL_Surface()->w / numClips;
   posDim.h = image.getSDL_Surface()->h / 2;
   this->image = &image;

   initialize(x, y, gravity, speed, jumpStrength, screen, gravityEnabled,
         leader, numClips, levelWidth, levelHeight, p1, p2, p3, p4);

   if (DEBUG_PRIVATES && this->className == "RectFigure")
      debug();
}

int Figure::getWidth() {
   return posDim.w;
}

int Figure::getHeight() {
   return posDim.h;
}

int Figure::getX() {
   return posDim.x;
}

int Figure::getY() {
   return posDim.y;
}

bool Figure::checkCollision(Figure* f) {
   RectFigure* r;
   CircFigure* c;

   if (f->getClassName() == "RectFigure") {
      r = static_cast<RectFigure*>(f);
      if (checkCollision(r))
         return true;
   }
   else if (f->getClassName() == "CircFigure") {
      c = static_cast<CircFigure*>(f);
      if (checkCollision(c))
         return true;
   }
   else
      throw FigureException();

   return false;
}

bool Figure::isCollided(vector<Figure*>& other, int& count) {
   if (!other.empty()) {
      for (vector<Figure*>::iterator i = other.begin(), end = other.end();
            i != end; i++) {
         if (checkCollision(*i))
            return true;
      }

      count++;
   }

   return false;
}

void Figure::handleInput(SDL_Event& event) {
   if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_UP:
         if ((!jumpAction && !inAir) || !gravityEnabled)
            v.y -= posDim.h * speed / 100 * jumpStrength;

         u = true;
         if (inAir)
            u = false;

         break;
      case SDLK_DOWN:
         v.y += posDim.h * speed / 100;
         d = true;
         break;
      case SDLK_LEFT:
         v.x -= posDim.w * speed / 100;
         l = true;
         break;
      case SDLK_RIGHT:
         v.x += posDim.w * speed / 100;
         r = true;
         break;
      default:
         break;
      }
   }

   else if (event.type == SDL_KEYUP) {
      switch (event.key.keysym.sym) {
      case SDLK_UP:
         if (!gravityEnabled)
            v.y += posDim.h * speed / 100 * jumpStrength;
         u = false;
         break;
      case SDLK_DOWN:
         v.y -= posDim.h * speed / 100;
         d = false;
         break;
      case SDLK_LEFT:
         v.x += posDim.w * speed / 100;
         l = false;
         break;
      case SDLK_RIGHT:
         v.x -= posDim.w * speed / 100;
         r = false;
         break;
      default:
         break;
      }
   }

   if (DEBUG_HANDLE_INPUT) {
      cout << "v.x: " << v.x << endl;
      cout << "v.y: " << v.y << endl;
   }
}

void Figure::move(vector<Figure*>& other) {
   xMovement(other);
   yMovement(other);

   setCamera();

   if (DEBUG_MOVE) {
      cout << "posDim.x: " << posDim.x << endl;
      cout << "posDim.y: " << posDim.y << endl;
   }
}

void Figure::show(SDL_Rect* otherCamera) {
   if (numClips > 0) {
      if (v.x < 0) {
         status = LEFT;
         animationFrame += AFVALUE;
      }
      else if (v.x > 0) {
         status = RIGHT;
         animationFrame += AFVALUE;
      }
      else
         animationFrame = 0;

      if (animationFrame >= numClips)
         animationFrame = 0;

      if (leader) {
         if (status == LEFT)
            applySurface(posDim.x - camera->x, posDim.y - camera->y, *image,
                  screen, &cl[static_cast<int>(animationFrame)]);
         else if (status == RIGHT)
            applySurface(posDim.x - camera->x, posDim.y - camera->y, *image,
                  screen, &cr[static_cast<int>(animationFrame)]);

         if (particleEffects)
            showParticles(camera);
      }
      else {
         if (status == LEFT)
            applySurface(posDim.x - otherCamera->x, posDim.y - otherCamera->y,
                  *image, screen, &cl[static_cast<int>(animationFrame)]);
         else if (status == RIGHT)
            applySurface(posDim.x - otherCamera->x, posDim.y - otherCamera->y,
                  *image, screen, &cr[static_cast<int>(animationFrame)]);

         if (particleEffects)
            showParticles(otherCamera);
      }
   }
}

void Figure::showParticles(SDL_Rect* camera) {
   for (int i = 0; i < TOTAL_PARTICLES; i++) {
      if (particles[i]->isDead()) {
         delete particles[i];
         particles[i] = new Particle(posDim, p1, p2, p3, p4, screen);
      }

      particles[i]->show(camera);
   }
}

SDL_Rect* Figure::getCameraClip() {
   return camera;
}

string Figure::getClassName() {
   return className;
}

Figure::~Figure() {
   if (camera != NULL)
      delete camera;

   for (int i = 0; i < TOTAL_PARTICLES; i++) {
      if (particles[i] != NULL)
         delete particles[i];
   }
}

RectFigure::RectFigure() {
   camera = NULL;

   for (int i = 0; i < TOTAL_PARTICLES; i++) {
      particles[i] = NULL;
   }

   className = "RectFigure";
}

RectFigure::RectFigure(int x, int y, Surface& image, SDL_Surface* screen,
      Gravity gravityEnabled, bool leader, double speed, int gravity,
      double jumpStrength, int numClips, int levelWidth, int levelHeight,
      Surface* p1, Surface* p2, Surface* p3, Surface* p4) :
      Figure(x, y, image, screen, gravityEnabled, leader, speed, gravity,
            jumpStrength, numClips, levelWidth, levelHeight, p1, p2, p3, p4) {
   className = "RectFigure";

   if (DEBUG_PRIVATES)
      debug();
}

bool RectFigure::checkCollision(RectFigure* r) {
   int left1 = posDim.x;
   int right1 = posDim.x + posDim.w;
   int top1 = posDim.y;
   int bot1 = posDim.y + posDim.h;

   int left2 = r->posDim.x;
   int right2 = r->posDim.x + r->posDim.w;
   int top2 = r->posDim.y;
   int bot2 = r->posDim.y + r->posDim.h;

   if (!(bot1 < top2 || top1 > bot2 || right1 < left2 || left1 > right2))
      return true;
   return false;
}

bool RectFigure::checkCollision(CircFigure* c) {
   int cx, cy;

   if (c->getX() < posDim.x)
      cx = posDim.x;
   else if (c->getX() > posDim.x + posDim.w)
      cx = posDim.x + posDim.w;
   else
      cx = c->getX();

   if (c->getY() < posDim.y)
      cy = posDim.y;
   else if (c->getY() > posDim.y + posDim.h)
      cy = posDim.y + posDim.h;
   else
      cy = c->getY();

   Point closestPoint = { cx, cy };
   Point circle = { c->getX(), c->getY() };
   if (dist(closestPoint, circle) < c->getR())
      return true;

   return false;
}

int CircFigure::determineGravity() {
   if (gravityEnabled && !u) {
      if (posDim.y < lh - r)
         v.y += gravity + abs(v.y * 0.01);
      else
         v.y = 0;
   }

   return v.y;
}

void CircFigure::checkIfInAir(vector<Figure*>& other) {
   int count = 0;

   inAir = true;
   posDim.y += 2;
   if ((v.y == 0 && posDim.y >= lh - r)
         || (v.y <= gravity && isCollided(other, count)))
      inAir = false;
   posDim.y -= 2;
   if (posDim.y < lh - r && v.y <= 1 && v.y >= -1)
      inAir = true;
}

void CircFigure::xMovement(vector<Figure*>& other) {
   int count = 0;

   posDim.x += v.x;

   if (isCollided(other, count))
      posDim.x -= v.x;
   else if (posDim.x > lw - r)
      posDim.x = lw - r;
   else if (posDim.x < r)
      posDim.x = r;
}

void CircFigure::yMovement(vector<Figure*>& other) {
   int count = 0;

   //gravity considerations
   determineGravity();

   //jump action
   determineJump();

   //check if inAir is true
   checkIfInAir(other);

   posDim.y += v.y;
   if (isCollided(other, count)) {
      posDim.y -= v.y;

      if (gravityEnabled)
         v.y = 0;
   }
   else if (posDim.y > lh - r)
      posDim.y = lh - r;
   //upper bounds
   //else if (posDim.y < r)
   //posDim.y = r;
}

CircFigure::CircFigure() {
   camera = NULL;

   for (int i = 0; i < TOTAL_PARTICLES; i++) {
      particles[i] = NULL;
   }

   className = "CircFigure";
}

CircFigure::CircFigure(int x, int y, Surface& image, SDL_Surface* screen,
      Gravity gravityEnabled, bool leader, double speed, int gravity,
      double jumpStrength, int numClips, int levelWidth, int levelHeight,
      Surface* p1, Surface* p2, Surface* p3, Surface* p4) :
      Figure(x, y, image, screen, gravityEnabled, leader, speed, gravity,
            jumpStrength, numClips, levelWidth, levelHeight, p1, p2, p3, p4) {
   className = "CircFigure";

   if (posDim.w > posDim.h)
      r = posDim.w / 2;
   else
      r = posDim.h / 2;

   if (DEBUG_PRIVATES)
      debug();
}

void CircFigure::setFigure(int x, int y, Surface& image, SDL_Surface* screen,
      Gravity gravityEnabled, bool leader, double speed, int gravity,
      double jumpStrength, int numClips, int levelWidth, int levelHeight,
      Surface* p1, Surface* p2, Surface* p3, Surface* p4) {
   Figure::setFigure(x, y, image, screen, gravityEnabled, leader, speed,
         gravity, jumpStrength, numClips, levelWidth, levelHeight, p1, p2, p3,
         p4);

   if (posDim.w > posDim.h)
      r = posDim.w / 2;
   else
      r = posDim.h / 2;

   if (DEBUG_PRIVATES)
      debug();
}

int CircFigure::getR() {
   return r;
}

void CircFigure::show(SDL_Rect* otherCamera) {

   if (numClips > 0) {
      if (v.x < 0) {
         status = LEFT;
         animationFrame += AFVALUE;
      }
      else if (v.x > 0) {
         status = RIGHT;
         animationFrame += AFVALUE;
      }
      else
         animationFrame = 0;

      if (animationFrame >= numClips)
         animationFrame = 0;

      if (leader) {
         if (status == LEFT)
            applySurface(posDim.x - r - camera->x, posDim.y - r - camera->y,
                  *image, screen, &cl[static_cast<int>(animationFrame)]);
         else if (status == RIGHT)
            applySurface(posDim.x - r - camera->x, posDim.y - r - camera->y,
                  *image, screen, &cr[static_cast<int>(animationFrame)]);

         if (particleEffects)
            showParticles(camera);
      }
      else {
         if (status == LEFT)
            applySurface(posDim.x - r - otherCamera->x,
                  posDim.y - r - otherCamera->y, *image, screen,
                  &cl[static_cast<int>(animationFrame)]);
         else if (status == RIGHT)
            applySurface(posDim.x - r - otherCamera->x,
                  posDim.y - r - otherCamera->y, *image, screen,
                  &cr[static_cast<int>(animationFrame)]);

         if (particleEffects)
            showParticles(otherCamera);
      }
   }
}

void CircFigure::showParticles(SDL_Rect* camera) {
   SDL_Rect posDimOffset;
   posDimOffset.x = posDim.x - r;
   posDimOffset.y = posDim.y - r;
   posDimOffset.w = posDim.w;
   posDimOffset.h = posDim.h;

   for (int i = 0; i < TOTAL_PARTICLES; i++) {
      if (particles[i]->isDead()) {
         delete particles[i];
         particles[i] = new Particle(posDimOffset, p1, p2, p3, p4, screen);
      }

      particles[i]->show(camera);
   }
}

bool CircFigure::checkCollision(RectFigure* r) {
   int cx, cy;

   if (posDim.x < r->getX())
      cx = r->getX();
   else if (posDim.x > r->getX() + r->getWidth())
      cx = r->getX() + r->getWidth();
   else
      cx = posDim.x;

   if (posDim.y < r->getY())
      cy = r->getY();
   else if (posDim.y > r->getY() + r->getHeight())
      cy = r->getY() + r->getHeight();
   else
      cy = posDim.y;

   Point p1 = { posDim.x, posDim.y };
   Point p2 = { cx, cy };
   if (dist(p1, p2) < this->r)
      return true;

   return false;
}

bool CircFigure::checkCollision(CircFigure* c) {
   Point thisCenter = { posDim.x, posDim.y };
   Point otherCenter = { c->posDim.x, c->posDim.y };

   if (dist(thisCenter, otherCenter) < this->r + c->r) {
      return true;
   }

   return false;
}

StringInput::StringInput(string ttfFile, int fontSize, Surface::Color fontColor,
      SDL_Surface* screen) :
      str(""), ttfFile(ttfFile), fontSize(fontSize), fontColor(fontColor), screen(
            screen), text(ttfFile, fontSize, fontColor, "") {
   SDL_EnableUNICODE(SDL_ENABLE);
}

void StringInput::handleInput(SDL_Event& event) {
   if (event.type == SDL_KEYDOWN) {
      string temp = str;

      if (str.length() <= 16) {
         if (event.key.keysym.unicode == (Uint16) ' ')
            str += (char) event.key.keysym.unicode;
         else if ((event.key.keysym.unicode >= (Uint16) '0'
               && event.key.keysym.unicode <= (Uint16) '9')
               || (event.key.keysym.unicode >= (Uint16) 'A'
                     && event.key.keysym.unicode <= (Uint16) 'Z')
               || (event.key.keysym.unicode >= (Uint16) 'a'
                     && event.key.keysym.unicode <= (Uint16) 'z'))
            str += (char) event.key.keysym.unicode;
      }

      if (event.key.keysym.sym == SDLK_BACKSPACE && str.length() > 0) {
         str.erase(str.length() - 1);
      }

      if (str != temp)
         text.setSDL_Surface(ttfFile, fontSize, fontColor, str);
   }
}

void StringInput::showCentered() {
   applySurfaceMiddle(text, screen);
}

StringInput::~StringInput() {
   SDL_EnableUNICODE(SDL_DISABLE);
}

double dist(Point p1, Point p2) {
   return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

SDL_Surface* optimizeImage(SDL_Surface* s) {
   SDL_Surface* optImg = SDL_DisplayFormat(s);
   if (optImg == NULL)
      throw ConversionException();

   SDL_FreeSurface(s);
   return optImg;
}

void setColorKey(Surface::Color ck, SDL_Surface* s) {
   Uint32 colorkey;

   SDL_Color c = parseColor(ck);
   colorkey = SDL_MapRGB(s->format, c.r, c.g, c.b);
   SDL_SetColorKey(s, SDL_SRCCOLORKEY, colorkey);
}

SDL_Color setRGBColor(int r, int g, int b) {
   SDL_Color c = { r, g, b };
   return c;
}

SDL_Color parseColor(Surface::Color color) {
   SDL_Color c;
   switch (color) {
   case Surface::NONE:
      c = setRGBColor(0, 0, 0);
      break;
   case Surface::RED:
      c = setRGBColor(255, 0, 0);
      break;
   case Surface::GREEN:
      c = setRGBColor(0, 255, 0);
      break;
   case Surface::BLUE:
      c = setRGBColor(0, 0, 255);
      break;
   case Surface::CYAN:
      c = setRGBColor(0, 255, 255);
      break;
   case Surface::BLACK:
      c = setRGBColor(0, 0, 0);
      break;
   case Surface::WHITE:
      c = setRGBColor(255, 255, 255);
      break;
   default:
      throw InvalidColorException();
      break;
   }

   return c;
}

SDL_Surface* loadImage(string filename, Surface::Color ck) {
   SDL_Surface* loadImg = IMG_Load(filename.c_str());
   if (loadImg == NULL)
      throw LoadImageException();
   else {
      SDL_Surface* optImg = optimizeImage(loadImg);

      if (ck != Surface::NONE)
         setColorKey(ck, optImg);

      return optImg;
   }
}

SDL_Surface* loadText(string pathToTTF, int size, Surface::Color color,
      string msg) {
   TTF_Font* font = TTF_OpenFont(pathToTTF.c_str(), size);
   if (font == NULL)
      throw LoadTextException();

   SDL_Color c = parseColor(color);
   if (msg == "")
      msg = " ";

   SDL_Surface* text = TTF_RenderText_Solid(font, msg.c_str(), c);
   if (text == NULL)
      throw LoadTextException();

   TTF_CloseFont(font);

   return text;
}

Mix_Music* loadMusic(string music) {
   Mix_Music* m = Mix_LoadMUS(music.c_str());
   if (m == NULL)
      throw LoadMusicException();

   return m;
}

Mix_Chunk* loadChunk(string chunk) {
   Mix_Chunk* c = Mix_LoadWAV(chunk.c_str());
   if (c == NULL)
      throw LoadChunkException();

   return c;
}

void applySurface(int x, int y, Surface& source, SDL_Surface* destination,
      SDL_Rect* clip) {
   SDL_Rect offset;

   offset.x = x;
   offset.y = y;
   SDL_BlitSurface(source.getSDL_Surface(), clip, destination, &offset);
}

void applySurfaceMiddle(Surface& source, SDL_Surface* destination,
      SDL_Rect* clip) {
   applySurface((destination->w - source.getSDL_Surface()->w) / 2,
         (destination->h - source.getSDL_Surface()->h) / 2, source, destination,
         clip);
}

void flip(SDL_Surface* screen) {
   if (SDL_Flip(screen) < 0)
      throw FlipException();
}

int getVerticalMiddlePosition(Surface& object, SDL_Surface* screen) {
   return (screen->h - object.getSDL_Surface()->h) / 2;
}

int getHorizontalMiddlePosition(Surface& object, SDL_Surface* screen) {
   return (screen->w - object.getSDL_Surface()->w) / 2;
}

bool isHeldDown(SDL_Event& event) {
   while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP)
         return false;
   }
   return true;
}

void fillScreen(SDL_Surface* screen, Surface::Color color) {
   SDL_Color c = parseColor(color);
   SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, c.r, c.g, c.b));
}

int randRange(int low, int high) {
   return rand() % (high + 1 - low) + low;
}

SDL_Surface* init(int w, int h, string title) {
   SDL_Surface* screen = NULL;

   if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
      throw InitException();

   screen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
   if (screen < 0)
      throw SetVideoModeException();

   if (TTF_Init() < 0)
      throw InitException();

   if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
      throw InitException();

   SDL_WM_SetCaption(title.c_str(), NULL);

   return screen;
}

void cleanUp() {
   Mix_CloseAudio();
   TTF_Quit();
   SDL_Quit();
}
