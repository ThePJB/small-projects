use std::path::Path;
use std::fs::File;
use std::io::BufWriter;

use palette::{Hsv, Hsl, Rgb, IntoColor};

use num::complex::{Complex, Complex64};
use num::traits::Pow;

fn main() {
    const ssaa: i32 = ;

    let path = Path::new(r"out.png");
    let file = File::create(path).unwrap();
    let ref mut buf_writer = BufWriter::new(file);

    const w: i32 = 1024;
    const h: i32 = 1024;

    let mut encoder = png::Encoder::new(buf_writer, w as u32, h as u32);
    encoder.set_color(png::ColorType::Rgba);
    encoder.set_depth(png::BitDepth::Eight);
    encoder.set_trns(vec!(0xFFu8, 0xFFu8, 0xFFu8, 0xFFu8));
    encoder.set_source_gamma(png::ScaledFloat::from_scaled(45455)); // 1.0 / 2.2, scaled by 100000
    encoder.set_source_gamma(png::ScaledFloat::new(1.0 / 2.2));     // 1.0 / 2.2, unscaled, but rounded
    let source_chromaticities = png::SourceChromaticities::new(     // Using unscaled instantiation here
        (0.31270, 0.32900),
        (0.64000, 0.33000),
        (0.30000, 0.60000),
        (0.15000, 0.06000)
    );
    encoder.set_source_chromaticities(source_chromaticities);
    let mut writer = encoder.write_header().unwrap();

    let mut data = [0; (4*w*h) as usize];

    let domain = 4.0;

    // todo find l function that gives discontinuities
    // compose l1 for grid lines toooooo

    let f = |x: Complex64| ((x*x - 1.0) * (x - Complex::new(2.0, 1.0)*(x - Complex::new(2.0, 1.0))/((x*x + 2.0*x + Complex::new(0.0, 2.0))*(x*x + Complex::new(1f64, -1f64)))));
    let l1 = |x: f64| 2.0 / std::f64::consts::PI * x.atan();
    let l2 = |x: f64| {
        let a = 0.1;
        x.pow(a) / (x.pow(a) + 1.0)
    };
    let l3 = |x: f64| (x.log2()) % 1.0;
    let l4 = |x: f64| x % 1.0;
    
    let l = l3;


    for i in 0..w {
        for j in 0..h {
            let mut H = 0.0;
            let mut S = 0.0;
            let mut L = 0.0;

            // ssaa = 1: ded center so for i is 0 to 1
            // ssaa = 2: 0 to 

            let px_x = 2.0 * domain / w as f64;
            let px_y = 2.0 * domain / h as f64;

            let x = px_x * (i - w/2) as f64;
            let y = px_y * (j - h/2) as f64;

            for ssaa_i in 0..ssaa {
                for ssaa_j in 0..ssaa {
                    let sp_x = px_x / ssaa as f64;
                    let sp_y = px_y / ssaa as f64;

                    let xp = x + ssaa_i as f64 * sp_x - px_x / 2.0; 
                    let yp = y + ssaa_j as f64 * sp_y - px_y / 2.0;

                    let fxy = f(Complex::new(xp, yp));
                    H += fxy.arg() / std::f64::consts::PI * 360.0;
                    L += l(fxy.norm());
                    S += 1.0;
                }
            }

            H /= (ssaa*ssaa) as f64;
            S /= (ssaa*ssaa) as f64;
            L /= (ssaa*ssaa) as f64;
            
            let hsv = Hsl::new(H.into(),S,L);
            let rgb = hsv.into_rgb();
            //let rgb = Rgb::new(1.0, 1.0, 0.0);
            let px: usize = (i + w*j) as usize;
            data[4*px + 0] = (255 as f64 * rgb.red) as u8;
            data[4*px + 1] = (255 as f64 * rgb.green) as u8;
            data[4*px + 2] = (255 as f64 * rgb.blue) as u8;
            data[4*px + 3] = 255;
        }
    }

    writer.write_image_data(&data).unwrap(); // Save

    println!("Hello, world!");
}
