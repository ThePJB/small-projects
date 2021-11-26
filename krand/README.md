badprimes got rekt by repeated hashing

the shitty ones get rekt by 0, 1, 2, 3 ...



Though remember that in order to sample a function we first need to compute the PDF of that function, then its CDF and finally the invert of the CDF.

so this might be for random gradients and also for how computers do exp distribution etc


hmm round up noise libs and see


simd is only 3x faster!

exp noise hmm...
apply it to the magnitude of the perlin gradients
looks kinda sus lol. maybe cause its super zoomed out, it could be real mad..

could speed it up maybe with lut

maybe domain warp etc (maybe with a cheaper noise function)
like where are me mountain ranges
what if magnitudes themself came from another noise function... or is that essentially where they come from hmm
what if we just do uniform gradient or slowstart gradient

yea oath although whats interesting is that this is just as slow

could use less entropy for gradient lookups with symmetry, or not actually ? might be just the same



I also happen to feel like I could simd mine. its just 8 things at a time right so I could exploit parallelism generating 8 blocks at a time or whatever
but it could also probably be self contained to one block for minimal impact on the program

eg perlin needs 4 hash evaluations, one for each corner of the block, then you need more per subdivision, say 16 (but theres also a lot of overlap)
its pretty hectic really I wonder if theres a way to exploit it harder, including intentionally inlining it
Theres also how many bits of entropy would you like but also the corresponence is important, sometimes you dont need that many. F32 is 23 bits? could get away with 16?

so you could have one hash of u64 make 4 floats from 4 seeds, convenient

would it fuck the cache to jsut do it at the highest granularity first for the hash functions and then index it, probably. It would be interesting to benchmark

could use xoshiro within a chunk or whatever just for hyper optimized heightmap if you wanted to. actually could you. maybe you could for the generate a big table first approach


could test skew per bit pos as well


surprised its not speeding up more what if u just spin on it

// maybe indexing vec slab instead of using vec push is faster?