bugs:
 - twisty boys
    maybe a negative gradient thing
    or maybe if the thing comes out at too much of an angle it has no choice but to twist

---

stuff to do for leaves:
 - raymarch?
 - proc texturing? can it be done deferred? prob not

 tree inspo:
 - fig tree
 - dragon tree
 - pandanus palm

 Fatties:
 - baobab tree
 - bottle tree
 - adenium obesum socotranum

--

ideas:
bool trunk, trunk only goes up (or deviates very little), like gum trees and that


------
Performance:

(8 trunk polys, 8 foliage polys)

small trees: (1) order of 5000 polys
med trees: (2) order of 50,000 polys
big trees: (3) order of 500,000 polys

predictably its half ish with 4,4

is there a way to efficiently detect occluded geometry and remove it from the mesh?
probably better to get an implicit method

maybe want to limit branching. right now it gives it a really fat tail

----------

so the trunk thing is quite high value as far as stopping branches, looks good,
perpendicular branches work but can look a bit shit plus breaks rng. maybe fixable with a simple order change.

probably still some good L system stuff worth mining

could do more sophisticated branching too, like perp branch math works. might fix twisty boys to do that + explicit angle

could do some presets