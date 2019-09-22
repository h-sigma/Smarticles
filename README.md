# Smarticles
A particle system that leverages template programming to create a flexible particle system for SFML.
Documentation and further guides can be found in the wiki. Quick-start guides are shown below.

To use for your own project, include the headers `Particle.hpp, ParticleSystem.hpp, and Emitter.hpp` into it. No build required.

Limitations/TODO:

  Need to know the explicit type of particle being used at all times.

# How to use:

Steps:
1. Create ParticleType.
2. Create ParticleSystem.
3. Add Affectors to ParticleSystem that can modify each particle created by the system every frame.
4. If the emission needs are simple, e.g. creating the same particle on the left border of the screen, you can jerryrig your own function. Also, if you don't need a constant emission rate, jerryrig your own function that calls `addParticle()`.
5. Else, create an Emitter to methodologically add the particles.
6. Add a ParticleModifier that can both modify the Emitter's state (e.g. move it around in 3D space), and set properties to a particle before it's added to the ParticleSystem.

## How to create new particle types.
Let's assume we need a particle that can hold four variables : `position, lifetime, color, distance`.
The first two, position and lifetime, are the only constituents of `BaseParticle` provided to you. 
```cpp
struct BaseParticle{  //details omitted
  sf::Vector2f position;
  sf::Time lifetime;
};
```
We now create another struct in a namespace that would hold the other values:
```cpp
namespace my{
  struct ColorNRadius{
    sf::Color color;
    double radius;
  };
}
```
Now, pass this struct as a template type to the BaseParticle, and we have our new particle type.
```cpp
using PGreen = BaseParticle<my::ColorNRadius>;
```
Result: PGreen is a struct that holds `position, lifetime, color, distance` in the order `sf::Color, double, sf::Vector2f, sf::Time`. 

## Quick Guide to using ParticleSystem
First, create a default object of ParticleType PGreen that will be used by the Particle System in the absence of an Emitter attached to it. Let's call it `defaultGreen`. It has sf::Color set to sf::Color::Green, hence the name.
Now, do 
```cpp
ParticleSystem<PGreen> sys(texture, sf::Color::Green, defaultGreen);
```
The first argument `texture` is the particle's texture that will be rendered. The second argument is the default color the Particles will be rendered to (required even if you give a default color to `defaultGreen`). The third argument is the default particle it will create upon a call to `addParticle(/*no args*/)`.

A note about the second argument: The particle class will use the third argument's member color if it has a variable exactly `sf::Color color`  in it. Otherwise, it will use the second argument's color. (If we do ParticleSystem<BaseParticle>, the particles created will use the explicit color from the constructor, because they don't hold a color variable themselves.)

Now that you have a ParticleSystem, you can call any of the `addParticle()` overloads. `mWindow.draw(sys` will draw them to mWindow.

Affectors are an important part of particle systems. In this library, affectors are functions (often lambdas) of type `void(std::deque<ParticleType> &particleList)`.
Example:
```cpp
auto affector = [](std::deque<PGreen> &particleList){
  for(auto& particle : particleList)
  { 
    particle.position.x++;
  }
};
sys.addAffector(affector);
```
Result: After particles are created in whatever state, they move to the right one pixel per frame.
If you were to do `sys.addAffector(affector);` again, the affector will be run twice per frame, meaning that all affectors are retained in the ParticleSystem, and called in the order of their being added.

Quick Note: You can use affectors to calculate some data based on the particles before passing it to another affector, thus allowing you to write more modular affectors.
```cpp
SomeData daataa;
auto affector1 = [&daataa](std::deque<ParticleType> &particleList) { /* modify daataa */ };
auto affector2 = [&daataa](std::deque<ParticleType> &particleList) { /* use daataa */ };
sys.addAffector(affector1); // order is very important, as the daataa modifier affector1 needs to run before the user affector2
sys.addAffector(affector2); 
```

## Quick guide to using Emitter
Pass the particle being created to Emitter and give it the default particle to generate. 
```cpp
Emitter<PGreen> myEmitter(defaultGreen);
```
This can be changed by a call to `Emitter::setDefaultParticle(ParticleType);`
Now, you need to attach a particle system to this emitter. 
```cpp
  myEmitter.setParticleSystem(&sys);
```
The emission rate is the number of particles generated per second. Manipulate through `Emitter::getEmissionRate()` and `Emitter::setEmissionRate()`.

Now, you can attach modifiers to Emitters as well. These are functions, often lambdas, of type `void(ParticleType&, Emitter<ParticleType>*)`.
Notice the two arguments. 

`ParticleType&` gives you control of the particle about to be added to the ParticleSystem. Want a red particle to appear every 10 green particles?
```cpp
auto modifier = [](PGreen& particle, Emitter<PGreen>* emitter) {
  static int i = 0;
  if(i++ == 10)
  {
    particle.color = sf::Color::Red;
    i = 0;
  }
  else
  {
    particle.color = sf::Color::Green;
  }
};
myEmitter.addParticleModifier(modifier);
```
Much like ParticleSystems and affectors, multiple modifiers may be added to an Emitter.

As for the second argument:-

An Emitter inherits from sf::Transformable (which can be changed by giving it your own transformable-ish class as the second template argument, i.e. `Emitter<PGreen, MyTransformable>`). This is the main reason behind the `Emitter<PGreen>*` parameter in the Emitter modifiers: you can modify the emitter on a per-emitted-particle basis if required. Remember, the modifier is run for each particle emitted.

Following is an example of a modifier that moves the emitter up and down the screen.
```cpp
auto moverupperdowner = [&mWindow](PGreen& particle, Emitter<PGreen)* emitter){
  //captures the SFML window by reference, yay lambdas!
  //update position
  static int sign = +1; //-ve is up, +ve is down in SFML
  const STEP = 4; //move by 4 pixels per particle
  auto pos = emitter->getPosition();
  pos.y += STEP * sign;
  emitter->setPosition();
  //update sign
  auto screen_height = mWindow.getDefaultView().getSize().y;
  if(pos.y > screen_height || pos.y < 0)
    sign *= -1;
};
sys.addModifier(moverupperdowner);
```

Result: the emitter moves up and down the screen at STEP pixels per particle, turning once it goes past the border. Combined with the constant emission rate, you get a uniform spread of particles.
