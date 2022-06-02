# labyrinth
An extremely simple random-maze-generated game made using C++ with SDL2.0 library.
This is an educational project, the game IS NOT intended to be entertaining.

## Game details
### Gameplay
    You find yourself trapped in a maze, there is only one way out through a secret door.
    You'll encounter, through your journey to freedom, dangerous viruses that will threaten your life.
    The Weak Virus:
        Once encountered, this virus will reduce your hp by one.
    Covid-19 Virus:
        Once encountered, this virus will reduce your hp by 3.
        A 60 seconds timer will be set-up. You should find a disinfectant gel ASAP!
    Fortunatly, there are hepfull stuff on the way. Potions can restore your health. If you encounter one,
    your hp will be boosted by 1.
    Disinfectant gels can save you against Covid-19 viruses. Though these will not increase your hp!
      
### Features
   - User can provide the size of the maze.
   - Each round the maze is __randomly generated__.
   - Each case in the maze, as long as it is NOT a wall, is accessible.
   - User chooses the initial position and direction of the player.
   - User chooses the initial position of the viruses and the health potions.
   - User chooses the position of the exit point.
    
## What this game lacks
  - Sounds
  - Interesting gameplay (this is an educational project)
  - Character customization
  - Fluide animations
  - Dynamic environment (destructable walls)
  - Proper crossplatform installation support (add CMake)

## How to install
    - install SDL2.0 (see official webpage for the how-to).
    - download the source code from this repository or just fetch it.
    - run these commands in root directory:
        make
        ./game
        
## License
  You are free to do whatever you like with the source files!
