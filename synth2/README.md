Up next,

time for block drawing, vus, parameter tweaking, connecting


interacting with blocks
-----------------------

States: 
click block
drag block
click background
drag background box
right click background
right click drag backround

adding and deleting blocks? we will see

click on background and hold: start drawing box

click on block and hold: drag block
click on block and release: start drawing connection
   from this state, right click cancels drawing connection

rollover block: highlight (in block draw)
rollover connection: highlight (in connection draw)



--------
how to implement? some more stuff in module_manager struct
 * selected_connection (or -1)
 * selected_module (or -1)
 * interaction_state
and a "method" for mm which handles input:
 * motion, update selected (technically can update state as well from drag to just click and release path, i think)
 * click or unclick, update state

edge case: multi select, maybe its just an array of bools for selection instead



application pattern
-------------------
how we feeling about this? could it be too much indirection?
I think its a pretty good way to separate things, becasue they are different concerns. module manager will always be module managing, but if theres menus and other stuff then thats in application. And if theres clap or dependencies and stuff to init that goes in main.