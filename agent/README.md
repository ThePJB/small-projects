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
 * hawks and doves
 * https://en.wikipedia.org/wiki/Evolutionary_game_theory
 * Biham–Middleton–Levine traffic model
  * and variants, see wikipedia
 * Nagel–Schreckenberg model?

is a func(specific ptr) able to be used as a func(void ptr)?

todo: 
 * reset button
 * reseed button
 * parameter struct that is able to be modified at run time with a menu
   * maybe a function that returns for each potential index the string and also handles inc / dec


text working, next job is menu. so maintain the cursor and list of items (growable array)

menu bugs:
 * rule starts at 73 but its actually 96 after u modify
 * doesnt do anything
    * i think im somewhere taking address incorrectly and with void ptr u arent protected from that lol
 * count from 1 for keyboard numbers to select ca
 

reset grid without resetting ur settings?
and only modify settings if menu show