# Smarticles
A particle system that leverages template programming to create a flexible particle system for SFML.
Documentation and further guides can be found in the wiki. Quick-start guides are shown below.

Limitations/TODO:
  Need to know the explicit type of particle being used at all times.

# How to use:

Steps:
1. Create ParticleType.
2. Create ParticleSystem.
3. Add Affectors to ParticleSystem that can modify each particle created by the system every frame.
4. If the emission needs are simple, e.g. creating the same particle on the left border of the screen, you can jerryrig your own function.
5. Else, create an Emitter to methodologically add the particles.
6. Add a ParticleModifier that can both modify the Emitter's state (e.g. move it around in 3D space), and set properties to a particle before it's added to the ParticleSystem.

## How to create new particle types.
Using mixins means that the code won't be beautiful unless you use `using`.
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
Now, PGreen is a struct that holds `position, lifetime, color, distance` in the order `sf::Color, double, sf::Vector2f, sf::Time`. 

## Quick Guide to using ParticleSystem
First, create a default object of Particle PGreen that will be used by the Particle System in the absence of an Emitter attached to it. Let's call it `defaultGreen`. It has sf::Color set to sf::Color::Green.
Now, do 
```cpp
ParticleSystem<PGreen> sys(texture, sf::Color::Green, defaultGreen);
```
The first argument `texture` is the particle's texture that will be rendered. The second argument is the default color the Particles will be rendered to (required even if you give a default color to `defaultGreen`). The third argument is the default particles it will create upon a call to `addParticle(/*no args*/)`.

A note about the second argument: The particle class will use the third argument's color if it has a variable named exactly `color` of type sf::Color defined in it. Otherwise, it will use the second argument's color.

Now that you have a ParticleSystem, you can call any of the `addParticle()` overloads. `mWindow.draw(sys` will draw them to mWindow.

Now, affectors are a big part of particle systems. In this library, affectors are functions (often lambdas) of type `void(std::deque<PGreen> &particleList)`.
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
If you were to do `sys.addAffector(affector);` again, the affector will be run twice per frame, meaning all affectors are retained in the ParticleSystem, and called in the order of their being added.

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
An Emitter inherits from sf::Transformable (which can be changed by giving it your own transformable-ish class as the second template argument, i.e. `Emitter<PGreen, MyTransformable>`).

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
