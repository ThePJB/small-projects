ok enough having the shits with windowing libraries

1. make a decent GoL in this

probably just want a renderer on top of this that can draw rectangles right
and maybe abstract the stupid pixels? or do we want pixel perfection

see if this could be a candidate for snowkoban remastered

So
 * Infinite simulation - just fuck it and hashset of points
 * scroll pos / zoom around cursor, can solve the maths for that pretty easy
 * speed qe, space to step i suppose
    * maybe have loaded state and playback state
    * fzf load states would be good. may as well be able to save as well

input schema as usual

---------------------

ok need to optimize

chunk it?
 * applicable to voxelgames ooo
 * but theres the annoying af edge case where its on a boundary

read more than write
 * store neighbour count as well at each cell
 * does this make sense to do the oversample each chunk thing? 

multithread??

---------------

todo:
* fix scroll in / scroll out
* fzf load save - might be hard because gui so might not
    whats simpler, q e next prev and button to save that appends

lessons learned:
* commands for inputs so good

next thing:
maybe work on rustlandkings
pure rust openGL would be so good
glutin + maybe glium if its not too fucked
but it sounds cooked, dont want munted compile times