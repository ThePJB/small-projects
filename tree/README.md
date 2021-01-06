bugs:
 - twisty boys

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