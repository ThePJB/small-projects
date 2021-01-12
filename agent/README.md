# Agent sim

so heres the plan

 * first do the world. its a grid of tiles
 * each tile has a float: 0 to 1 of grass grownness
 * and also fertility which affects how fast it grows
 * then add entities: dudes go around harvesting grass

then it could go a zillion differents ways. original plan:
 * dudes put down a city
 * they harvest food and take it there making more dudes
 * dudes of rival cities war
 * assign fertility by fbm, lead to anything cool?
 * dynamic events: rain, everywhere grows more, famine after?
 * city collapse

e.g.
 * 2 dudes meet:
   * if friendly, they found a city
   * if unfriendly, flip a coin and one kills the other

 * dude meets city:
   * if friendly, drop of food, maybe make more dude, health = max again?
   * if unfriendly, fuck the city up, delete it or reduce its health

   could even have the growth of a city and have it be tiles not entities

how to make it be performant, i thought i could with a hashmap
but of course the dudes continually move, maybe i must just make them be tiles

other obvious ones
 * game theory
 * predator prey