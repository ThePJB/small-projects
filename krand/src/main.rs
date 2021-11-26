#![feature(portable_simd)]
#![feature(vec_into_raw_parts)]
#![feature(slice_as_chunks)]

mod noise;
mod image_output;


use std::time::{Duration, SystemTime};
use rand::*;
use image_output::*;
use noise::*;

use core_simd::*;
use crossbeam::thread;


pub struct Xoshiro256PlusPlus {
    s: [u64; 4],
}

impl Xoshiro256PlusPlus {
    fn next_u32(&mut self) -> u32 {
        // The lowest bits have some linear dependencies, so we use the
        // upper bits instead.
        (self.next_u64() >> 32) as u32
    }

    fn next_u64(&mut self) -> u64 {
        let result_plusplus = self.s[0]
            .wrapping_add(self.s[3])
            .rotate_left(23)
            .wrapping_add(self.s[0]);

        let t = self.s[1] << 17;

        self.s[2] ^= self.s[0];
        self.s[3] ^= self.s[1];
        self.s[1] ^= self.s[2];
        self.s[0] ^= self.s[3];

        self.s[2] ^= t;

        self.s[3] = self.s[3].rotate_left(45);

        result_plusplus
    }
}

// return percent of 1s
fn skew(data: &[u32]) -> f64 {
    let mut n_1s = 0;
    let n_digits = data.len() * 32;
    for x in data {
        n_1s += x.count_ones();
    }
    n_1s as f64 / n_digits as f64
}


fn khash(seed: u32) -> u32 {
    let n1 = 0xB5297A4D;
    let n2 = 0x68E31DA4;
    let n3 = 0x1B56C4E9;

    let mut mangled = seed;
    mangled = mangled.wrapping_mul(n1);
    mangled ^= mangled.rotate_right(13);
    mangled = mangled.wrapping_add(n2);
    mangled ^= mangled.rotate_left(7);
    mangled = mangled.wrapping_mul(n3);
    mangled ^= mangled.rotate_right(9);
    return mangled;
}

fn khash2(x: u32, y: u32) -> u32 {
    khash(x + y*0xA341316C)
}

fn khash2_2(x: u32, y: u32) -> u32 {
    let n1 = 0xB5297A4D;
    let n2 = 0x68E31DA4;
    let n3 = 0x1B56C4E9;

    let mut mangled = x;
    mangled = mangled.wrapping_mul(n1);
    mangled ^= mangled.rotate_right(13);
    mangled = mangled.wrapping_add(y);
    mangled = mangled.wrapping_add(n2);
    mangled ^= mangled.rotate_left(7);
    mangled = mangled.wrapping_mul(n3);
    mangled ^= mangled.rotate_right(9);
    return mangled;
}


fn squirrel(seed: u32) -> u32 {
    let n1 = 0xB5297A4D;
    let n2 = 0x68E31DA4;
    let n3 = 0x1B56C4E9;
    let mut mangled = seed;
    mangled *= n1;
    mangled ^= mangled >> 8;
    mangled += n2;
    mangled ^= mangled << 8;
    mangled *= n3;
    mangled ^= mangled >> 8;
    return mangled;
}

fn tea(n: u32, mut v: [u32; 2]) -> [u32; 2] {
    // feistel coder or something

    let delta = 0x9e3779b9; // is apparently golden ratio or something, is it prime?
    let k0 = 0xA341316C;
    let k1 = 0xC8013EA4;
    let k2 = 0xAD90777D;
    let k3 = 0x7E95761E;


    let mut sum = 0;

    for _ in 0..n {
        sum += delta;
        v[0] += ((v[1] << 4) + k0) ^ (v[1] + sum) ^ ((v[1] >> 5) + k1);
        v[1] += ((v[0] << 4) + k2) ^ (v[0] + sum) ^ ((v[0] >> 5) + k3);
    }

    return v;
}

#[inline]
fn squirrel_lazy(seed: u32) -> u32 {
    let n1 = 0xB5297A4D;
    let n2 = 0x68E31DA4;
    let mut mangled = seed;
    mangled *= n1;
    mangled ^= mangled >> 8;
    mangled += n2;
    mangled ^= mangled << 8;
    return mangled;
}

#[inline]
fn extreme_lazy(seed: u32) -> u32 {
    (seed ^ 0x68E31DA4) * 0xB5297A4D 
}

#[inline]
fn squirrel_bad_primes(seed: u32) -> u32 {
    let n1 = 0x673547AF;
    let n2 = 0x908FD7AB;
    let n3 = 0xF789234A;
    let mut mangled = seed;
    mangled *= n1;
    mangled ^= mangled >> 8;
    mangled += n2;
    mangled ^= mangled << 8;
    mangled *= n3;
    mangled ^= mangled >> 8;
    return mangled;
}

fn test(name: &str, result: (Vec<u32>, f64)) {
    println!("{} -- Skew: {:?} MHash/sec: {:?}", name, skew(&result.0), result.1);
    let mut name = name.to_owned();
    name.push_str(".png");
    dump_image(&*name, &result.0);
}

// times it
fn do_squirrel(runs: u32) -> (Vec<u32>, f64) {
    let mut v = Vec::with_capacity(runs as usize);
    let mut seed = 73456756; // or time now or whatever

    let t_start = SystemTime::now();

    for _ in 0..runs {
        let hash = squirrel(seed);
        v.push(hash);
        seed = hash;

    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;
    
    (v, runs_per_second/1000000.0f64)
}

fn do_tea(n: u32, runs: u32) -> (Vec<u32>, f64) {
    let mut v = Vec::with_capacity(runs as usize);
    let mut s1 = 0xF73DB187u32;
    let mut s2 = 0x159CBAFEu32;

    let t_start = SystemTime::now();

    for _ in 0..runs {
        let hash = tea(n, [s1, s2]);
        v.push(hash[0]);
        s1 = hash[0];
        s2 = hash[1];
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;
    
    (v, runs_per_second/1000000.0f64)
}

// times it
fn do_rand_bad(runs: u32) -> (Vec<u32>, f64) {
    let mut v = Vec::with_capacity(runs as usize);

    let t_start = SystemTime::now();

    for _ in 0..runs {
        v.push(rand::thread_rng().next_u32());

    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;
    
    (v, runs_per_second/1000000.0f64)
}

// times it
fn do_rand_good(runs: u32) -> (Vec<u32>, f64) {
    let mut v = Vec::with_capacity(runs as usize);

    let t_start = SystemTime::now();
    let mut rng = rand::thread_rng();

    for _ in 0..runs {
        v.push(rng.next_u32());

    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;
    
    (v, runs_per_second/1000000.0f64)
}

// times it
fn do_xo(runs: u32) -> (Vec<u32>, f64) {
    let mut v = Vec::with_capacity(runs as usize);

    let t_start = SystemTime::now();
    let mut xopp = Xoshiro256PlusPlus { s: [0xFA987CDEF7932463, 0x1FD8567DAB97C834, 0xD6491AC4859FDEBB, 0x5691209CFDEBF845]};

    for _ in 0..runs {
        v.push(xopp.next_u32());
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;
    
    (v, runs_per_second/1000000.0f64)
}

// times it
fn do_fn(f: fn(u32)->u32, runs: u32) -> (Vec<u32>, f64) {
    let mut v = Vec::with_capacity(runs as usize);

    let t_start = SystemTime::now();

    for i in 0..runs {
        //v.push(f(i*0xFC8120D5));
        v.push(f(i));
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;

    println!("t taken: {}", t_taken);

    
    (v, runs_per_second/1000000.0f64)
}
/*
// times it
fn do_fn_multi(f: fn(u32)->u32, runs: u32, threads: u32) -> (Vec<u32>, f64) {

    //let mut v = Vec::with_capacity(runs as usize);
    let mut v = vec![0; runs as usize];
    //let (chunks, rem) = v.as_chunks_mut();
    let runs_per_thread = threads / runs;
    let t_start = SystemTime::now();

    unsafe {

        let mut chunks = v.chunks_exact_mut(runs_per_thread as usize);



        let mut handles = Vec::new();

        for thread in 0..threads {
            handles.push(thread::spawn(move || {
                let this_thread_idx = thread;
                let start_idx = runs_per_thread * thread;
                let chunk = chunks.nth(thread as usize).unwrap();
                for i in start_idx..start_idx+runs_per_thread {
                    chunk[i as usize] = f(i);
                }
            }));
        }

        for handle in handles {
            handle.join().unwrap();
        }
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;

    println!("t taken: {}", t_taken);

    
    (v, runs_per_second/1000000.0f64)
}
*/

fn do_fn_multi(f: fn(u32)->u32, runs: u32, threads: u32) -> (Vec<u32>, f64) {

    let mut v = vec![0; runs as usize];
    let runs_per_thread = (runs / threads) as usize;
    let t_start = SystemTime::now();
    
    
    let mut chunks = v.chunks_exact_mut(runs_per_thread as usize);
    
    thread::scope(|scope| {
        for (thread_idx, chunk) in chunks.enumerate() {
            scope.spawn(move |_| {
                let start_idx = runs_per_thread * thread_idx;
                for i in 0..runs_per_thread {
                    chunk[i] = f((start_idx + i) as u32);
                }
            });
        }
    }).unwrap();
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;

    println!("t taken: {}", t_taken);

    v.truncate(10000);
    (v, runs_per_second/1000000.0f64)
}


fn do_fn2(f: fn(u32, u32)->u32, s: u32) -> (Vec<u32>, f64) {
    let mut v = Vec::with_capacity((s*s) as usize);

    let t_start = SystemTime::now();

    for i in 0..s {
        for j in 0..s {
            v.push(f(i,j));
        }
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = (s*s) as f64 / t_taken;
    
    (v, runs_per_second/1000000.0f64)
}

fn khash8(seed: u32x8) -> u32x8 {
    let n1 = u32x8::splat(0xB5297A4D);
    let n2 = u32x8::splat(0x68E31DA4);
    let n3 = u32x8::splat(0x1B56C4E9);

    let mut mangled = seed;

    mangled *= n1;
    mangled ^= mangled >> 8;
    mangled += n2;
    mangled ^= mangled << 8;
    mangled *= n3;
    mangled ^= mangled >> 8;

    mangled
}

fn khash4(seed: u32x4) -> u32x4 {
    let n1 = u32x4::splat(0xB5297A4D);
    let n2 = u32x4::splat(0x68E31DA4);
    let n3 = u32x4::splat(0x1B56C4E9);

    let mut mangled = seed;

    mangled *= n1;
    mangled ^= mangled >> 8;
    mangled += n2;
    mangled ^= mangled << 8;
    mangled *= n3;
    mangled ^= mangled >> 8;

    mangled
}

// times it
fn do_k8(runs: u32) -> (Vec<u32>, f64) {


/*


    fn convert_using_into_raw_parts(v: Vec<T>) -> Vec<U> {
 }
    }

*/


    let mut v = Vec::with_capacity(runs as usize);
    let mut seed = u32x8::from_array([0, 1, 2, 3, 4, 5, 6, 7]); // or time now or whatever

    let t_start = SystemTime::now();

    for _ in 0..runs/8 {
        let hash8 = khash8(seed);
        //v.extend(hash8.as_array());
        v.push(hash8);
        //seed = hash8;
        seed += 8;
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;

    println!("t taken: {}", t_taken);

    let (ptr, len, cap) = v.into_raw_parts();

    let mut u = Vec::new();
    unsafe {
        u = Vec::from_raw_parts(ptr as *mut u32, len*8, cap*8)
    }
    
    (u, runs_per_second/1000000.0f64)
}

// times it
fn do_k4(runs: u32) -> (Vec<u32>, f64) {


/*


    fn convert_using_into_raw_parts(v: Vec<T>) -> Vec<U> {
 }
    }

*/


    let mut v = Vec::with_capacity(runs as usize);
    let mut seed = u32x4::from_array([0, 1, 2, 3]); // or time now or whatever

    let t_start = SystemTime::now();

    for _ in 0..runs/4 {
        let hash4 = khash4(seed);
        //v.extend(hash8.as_array());
        v.push(hash4);
        //seed = hash8;
        seed += 4;
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;

    println!("t taken: {}", t_taken);

    let (ptr, len, cap) = v.into_raw_parts();

    let mut u = Vec::new();
    unsafe {
        u = Vec::from_raw_parts(ptr as *mut u32, len*4, cap*4)
    }
    
    (u, runs_per_second/1000000.0f64)
}

fn do_k8_spin(runs: u32) -> u32x8 {
    let mut seed = u32x8::from_array([0, 1, 2, 3, 4, 5, 6, 7]); // or time now or whatever

    let t_start = SystemTime::now();

    for _ in 0..runs/8 {
        seed = khash8(seed);
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;

    println!("t taken: {}", t_taken);

    seed
}

fn do_k1_spin(runs: u32) -> u32 {
    let mut seed = 0;

    let t_start = SystemTime::now();

    for _ in 0..runs {
        seed = khash(seed);
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = runs as f64 / t_taken;

    println!("t taken: {}", t_taken);

    seed
}



fn main() {
    /*
    let nruns = 20000000;
    test("Squirrel fn", do_fn(squirrel, nruns));
    test("Squirrel fn bad primes", do_fn(squirrel_bad_primes, nruns));
    test("Squirrel fn lazy", do_fn(squirrel_lazy, nruns));
    test("extreme lazy", do_fn(extreme_lazy, nruns));
    */
    
    //let n2runs = 10000;
    //test("khash2", do_fn2(khash2, n2runs));
    //test("khash22", do_fn2(khash2_2, n2runs));
    
    // output png as well
    // is 128 even faster?
    
    // to be honest hash properties are so desirable, i guess xorshift isnt
    // is xorshift lcg?
    
    // power spectrum would be good
    
    //test_noise();

    let nruns = (1024*1024*64*32);
    test("khash1_mt1", do_fn_multi(khash, nruns, 1));
    test("khash1_mt2", do_fn_multi(khash, nruns, 2));
    test("khash1_mt4", do_fn_multi(khash, nruns, 4));
    test("khash1_mt8", do_fn_multi(khash, nruns, 8));
    test("khash1_mt12", do_fn_multi(khash, nruns, 12));
    
    
    /*
    test("khash1", do_fn(khash, nruns));
    test("khash4", do_k4(nruns));
    test("khash8", do_k8(nruns));

    println!("k1 spin: {}", do_k1_spin(1024*1024*64));
    println!("k8 spin: {:?}", do_k8_spin(1024*1024*64));

    noise_bench();
    */
}

