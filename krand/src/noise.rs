use crate::image_output::*;
use std::time::{Duration, SystemTime};
use simdnoise::*;
use core_simd::*;

//use std::arch::x86_64::*;

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

// 0..1
fn f322(x: u32, y: u32, seed: u32) -> f32 {
    khash(x + y * 0xA341316C + seed * 0xF73DB187) as f32 / std::u32::MAX as f32
}

fn lerp(a: f32, b: f32, t: f32) -> f32 {
    a * (1.0-t) + b * t
}

fn bilinear(a: f32, b: f32, c: f32, d: f32, t1: f32, t2: f32) -> f32 {
    //let u = |x| x*x*(3.0-2.0*x);
    //let u = |x| x*x*(10.0-3.0*x*(5.0-2.0*x));                 // looks fucked, directional artifacts I wonder why. maybe because derivative is discontinuous in middle
    //let u = |x: f32| (std::f32::consts::FRAC_PI_2*x).sin();   // looks fucked, I expected better from you sin, maybe derivative discontinuous in middle
    //let u = |x| x;
    let u = |x| ((6.0*x - 15.0)*x + 10.0)*x*x*x;
    lerp(lerp(a, b, u(t1)), lerp(c, d, u(t1)), u(t2))
}

fn noise2_interp(x: f32, y: f32, seed: u32) -> f32 {
    let xu = x.floor() as u32;
    let yu = y.floor() as u32;
    bilinear(
        f322(xu, yu, seed),
        f322(xu + 1, yu, seed),
        f322(xu, yu + 1, seed),
        f322(xu + 1, yu + 1, seed),
        x.fract(),
        y.fract(),
    )
}
// check for fract slowness
// also get some derivative action happening now that im a smart boy
// also exponential distribution is apparently a good

fn fbm2(x: f32, y: f32, seed: u32) -> f32 {
    (1.000 * noise2_interp(x, y, seed*0x3523423) +
    0.500 * noise2_interp(x * 2.0, y * 2.0, seed*0xF73DB187) + 
    0.250 * noise2_interp(x * 4.0, y * 4.0, seed*0x159CBAFE) + 
    0.125 * noise2_interp(x * 8.0, y * 8.0, seed*0x83242364)) /
    1.675
}

fn fbm2_more(x: f32, y: f32, seed: u32) -> f32 {
    (1.0000 * noise2_interp(x, y, seed*0x3523423) +
    0.5000 * noise2_interp(x * 2.0, y * 2.0, seed*0xF73DB187) + 
    0.2500 * noise2_interp(x * 4.0, y * 4.0, seed*0x159CBAFE) + 
    0.1250 * noise2_interp(x * 8.0, y * 8.0, seed*0x83242364) +
    0.0675 * noise2_interp(x * 16.0, y * 16.0, seed*0xFC783244)
) /
    1.675
}

fn fgrad2(x: f32, y: f32, seed: u32) -> f32 {
    (1.000 * grad2(x, y, seed*0x3523423) +
    0.500 * grad2(x * 2.0, y * 2.0, seed*0xF73DB187) + 
    0.250 * grad2(x * 4.0, y * 4.0, seed*0x159CBAFE) + 
    0.125 * grad2(x * 8.0, y * 8.0, seed*0x83242364)) /
    1.675
}

fn fgrad2_isotropic(x: f32, y: f32, seed: u32) -> f32 {
    (1.000 * grad2_isotropic(x, y, seed*0x3523423) +
    0.500 * grad2_isotropic(x * 2.0, y * 2.0, seed*0xF73DB187) + 
    0.250 * grad2_isotropic(x * 4.0, y * 4.0, seed*0x159CBAFE) + 
    0.125 * grad2_isotropic(x * 8.0, y * 8.0, seed*0x83242364)) /
    1.675
}

fn fgrad2_isoexp(x: f32, y: f32, seed: u32) -> f32 {
    (1.000 * grad2_isoexp(x, y, seed*0x3523423) +
    0.500 * grad2_isoexp(x * 2.0, y * 2.0, seed*0xF73DB187) + 
    0.250 * grad2_isoexp(x * 4.0, y * 4.0, seed*0x159CBAFE) + 
    0.125 * grad2_isoexp(x * 8.0, y * 8.0, seed*0x83242364)) /
    1.675
}

fn fgrad2_isouni(x: f32, y: f32, seed: u32) -> f32 {
    (1.000 * grad2_isouni(x, y, seed*0x3523423) +
    0.500 * grad2_isouni(x * 2.0, y * 2.0, seed*0xF73DB187) + 
    0.250 * grad2_isouni(x * 4.0, y * 4.0, seed*0x159CBAFE) + 
    0.125 * grad2_isouni(x * 8.0, y * 8.0, seed*0x83242364)) /
    1.675
}

fn grad2(x: f32, y: f32, seed: u32) -> f32 {
    let xu = x.floor() as u32;
    let yu = y.floor() as u32;

    let cf = |corner_x: u32, corner_y: u32| {
        let v = f322(xu + corner_x, yu + corner_y, seed) * std::f32::consts::PI * 2.0;
        let dx = v.cos();
        let dy = v.sin();
        dx * (x.fract() - corner_x as f32) + dy * (y.fract() - corner_y as f32)
    };
    
    let c1 = cf(0,0);
    let c2 = cf(1,0);
    let c3 = cf(0,1);
    let c4 = cf(1,1);

    let result = bilinear(c1, c2, c3, c4, x.fract(), y.fract());
    
        if x == 1.5 && y == 1.5 {
            println!("c1 {} c2 {} c3 {} c4 {}", c1, c2, c3, c4);
            println!("res: {}", result);
        }
    (result + 1.0) / 2.0
}

const ROOT3ON2: f32 = 0.8660254037844386467637231707529361834714026269051903140279034897;

fn grad2_isotropic(x: f32, y: f32, seed: u32) -> f32 {
    let root2 = std::f32::consts::SQRT_2;

    // also why not use a bigger gradient table
    //let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (root2, root2), (-root2, root2), (root2, -root2), (-root2, -root2)];
    let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (root2, root2), (-root2, root2), (root2, -root2), (-root2, -root2),
        (0.5, ROOT3ON2), (0.5, ROOT3ON2), (-0.5, -ROOT3ON2), (-ROOT3ON2, -0.5), (-0.5, ROOT3ON2), (-ROOT3ON2, 0.5), (0.5, -ROOT3ON2), (ROOT3ON2, -0.5)
    ];
    // idk whystefan gustavson does the below and not the above. it kinda does look better lol
    // also why not more gradients?
    //let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (1.0, 1.0), (-1.0, 1.0), (1.0, -1.0), (-1.0, -1.0)];

    let xu = x.floor() as u32;
    let yu = y.floor() as u32;

    let cf = |corner_x: u32, corner_y: u32| {
        let g_idx = khash(xu + corner_x + (yu + corner_y) * 0xA341316C + seed * 0xF73DB187) & 15;
        let (dx, dy) = grads[g_idx as usize];
        dx * (x.fract() - corner_x as f32) + dy * (y.fract() - corner_y as f32)
    };
    
    let c1 = cf(0,0);
    let c2 = cf(1,0);
    let c3 = cf(0,1);
    let c4 = cf(1,1);

    let result = bilinear(c1, c2, c3, c4, x.fract(), y.fract());
    
        if x == 1.5 && y == 1.5 {
            println!("c1 {} c2 {} c3 {} c4 {}", c1, c2, c3, c4);
            println!("res: {}", result);
        }
    (result + 1.0) / 2.0
}

fn grad2_isoexp(x: f32, y: f32, seed: u32) -> f32 {
    let root2 = std::f32::consts::SQRT_2;

    // also why not use a bigger gradient table
    //let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (root2, root2), (-root2, root2), (root2, -root2), (-root2, -root2)];
    let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (root2, root2), (-root2, root2), (root2, -root2), (-root2, -root2),
        (0.5, ROOT3ON2), (0.5, ROOT3ON2), (-0.5, -ROOT3ON2), (-ROOT3ON2, -0.5), (-0.5, ROOT3ON2), (-ROOT3ON2, 0.5), (0.5, -ROOT3ON2), (ROOT3ON2, -0.5)
    ];
    // idk whystefan gustavson does the below and not the above. it kinda does look better lol
    // also why not more gradients?
    //let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (1.0, 1.0), (-1.0, 1.0), (1.0, -1.0), (-1.0, -1.0)];

    let xu = x.floor() as u32;
    let yu = y.floor() as u32;

    let cf = |corner_x: u32, corner_y: u32| {
        let g_idx = khash(xu + corner_x + (yu + corner_y) * 0xA341316C + seed * 0xF73DB187) & 15;
        let magnitude = f322_exp(xu + corner_x, yu + corner_y, seed * 235452347);
        let (dx, dy) = grads[g_idx as usize];
        magnitude * dx * (x.fract() - corner_x as f32) + magnitude * dy * (y.fract() - corner_y as f32)
    };
    
    let c1 = cf(0,0);
    let c2 = cf(1,0);
    let c3 = cf(0,1);
    let c4 = cf(1,1);

    let result = bilinear(c1, c2, c3, c4, x.fract(), y.fract());
    
        if x == 1.5 && y == 1.5 {
            println!("c1 {} c2 {} c3 {} c4 {}", c1, c2, c3, c4);
            println!("res: {}", result);
        }
    (result + 1.0) / 2.0
}


fn grad2_isouni(x: f32, y: f32, seed: u32) -> f32 {
    let root2 = std::f32::consts::SQRT_2;

    // also why not use a bigger gradient table
    //let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (root2, root2), (-root2, root2), (root2, -root2), (-root2, -root2)];
    let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (root2, root2), (-root2, root2), (root2, -root2), (-root2, -root2),
        (0.5, ROOT3ON2), (0.5, ROOT3ON2), (-0.5, -ROOT3ON2), (-ROOT3ON2, -0.5), (-0.5, ROOT3ON2), (-ROOT3ON2, 0.5), (0.5, -ROOT3ON2), (ROOT3ON2, -0.5)
    ];
    // idk whystefan gustavson does the below and not the above. it kinda does look better lol
    // also why not more gradients?
    //let grads = [(-1.0, 0.0), (1.0, 0.0), (0.0, 1.0), (0.0, -1.0), (1.0, 1.0), (-1.0, 1.0), (1.0, -1.0), (-1.0, -1.0)];

    let xu = x.floor() as u32;
    let yu = y.floor() as u32;

    let cf = |corner_x: u32, corner_y: u32| {
        let g_idx = khash(xu + corner_x + (yu + corner_y) * 0xA341316C + seed * 0xF73DB187) & 15;
        let mut m = f322(xu + corner_x, yu + corner_y, seed * 235452347);
        m = 6.0*m*m*m*m*m*m*m*m;
        //m = 2.0*(1.0 - m*m); // idk i need to do some plotting, im just curious how exponential fn has the effect it does
                                    // and why this does nothing. maybe because 
                                    // why doesnt squaring it make it just darker. im not working in -1..1 am i
                                    // o its because its the gradient. im making the gradients really small.
                                    // you want most to be really small but some really big.

                                    // o this kinda looks like it.

                                    // o but theres a lot of big ditch next to big mountain, it kinda looks like shaded but it isnt! hmmm
        let (dx, dy) = grads[g_idx as usize];
        m * dx * (x.fract() - corner_x as f32) + m * dy * (y.fract() - corner_y as f32)
    };
    
    let c1 = cf(0,0);
    let c2 = cf(1,0);
    let c3 = cf(0,1);
    let c4 = cf(1,1);

    let result = bilinear(c1, c2, c3, c4, x.fract(), y.fract());
    
        if x == 1.5 && y == 1.5 {
            println!("c1 {} c2 {} c3 {} c4 {}", c1, c2, c3, c4);
            println!("res: {}", result);
        }
    (result + 1.0) / 2.0
}

pub fn test_noise() {
    let s = 1000;
    let scale = 1.0/250.0;
    let mut v = Vec::with_capacity(s*s as usize);
    for i in 0..s {
        for j in 0..s {
            v.push(fbm2(i as f32 * scale, j as f32 * scale, 69));
        }
    }

    dump_image_f32("fbm2.png", &v);

}

fn do_fn2(f: fn(f32, f32, u32)->f32, s: u32) -> (Vec<f32>, f64) {
    let mut v = Vec::with_capacity((s*s) as usize);

    let t_start = SystemTime::now();

    let seed = 0;

    let scale = 1.0/250.0;


    for i in 0..s {
        for j in 0..s {
            v.push(f(i as f32 * scale, j as f32 * scale, seed));
        }
    }
    
    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = (s*s) as f64 / t_taken;
    
    (v, runs_per_second/1000000.0f64)
}

fn test_2d(name: &str, result: (Vec<f32>, f64)) {
    println!("{} -- M evals/sec: {:?}", name, result.1);
    let mut name = name.to_owned();
    name.push_str(".png");
    dump_image_f32(&*name, &result.0);
}

fn test_simd() {

    let t_start = SystemTime::now();
    let s = 1000;

    // Get a block of 2d fbm noise with default settings, 100 x 100, with values scaled to the range [0,1]
    let noise = NoiseBuilder::fbm_2d(s, s)
        .with_octaves(4)
        .with_lacunarity(0.5)
        .with_freq(0.004)
        .generate_scaled(0.0, 1.0);

    let t_end = SystemTime::now();
    let t_taken = t_end.duration_since(t_start).unwrap().as_secs_f64();
    let runs_per_second = (s*s) as f64 / t_taken;

    println!("SIMD -- M evals/sec: {:?}", runs_per_second/1000000.0f64);
    dump_image_f32("SIMD.png", &noise);
}

// for some reason thanks maths
fn f322_exp(x: u32, y: u32, seed: u32) -> f32 {
    -f322(x, y, seed).ln()
}


pub fn noise_bench() {
    /*
    test_2d("2d_noise2", do_fn2(noise2_interp, 1000));
    test_2d("2d_fbm2", do_fn2(fbm2, 1000));
    test_2d("2d_fbm2_more", do_fn2(fbm2_more, 1000));
    test_2d("2d_grad2", do_fn2(grad2, 1000));
    test_2d("2d_fgrad2", do_fn2(fgrad2, 1000));
    test_2d("2d_grad2_isotropic", do_fn2(grad2_isotropic, 1000));
    test_2d("2d_fgrad2_isotropic", do_fn2(fgrad2_isotropic, 1000));
    test_2d("2d_fgrad2_isoexp", do_fn2(fgrad2_isoexp, 5000));
    */
    test_2d("2d_fgrad2_isouni", do_fn2(fgrad2_isouni, 2000));
    //test_simd();
}

// yeah so it kinda looks like shit lmao
// i wonder if a perlin noise style might be better
// im pretty sus of my functions I feel like they arent working

// todo compare performance
// try exponential dist etc

// gradient noise