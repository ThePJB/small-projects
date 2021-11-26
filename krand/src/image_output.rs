use std::path::Path;
use std::fs::File;
use std::io::BufWriter;

pub fn dump_image(path_str: &str, run_data: &[u32]) {
    let path = Path::new(path_str);
    let file = File::create(path).unwrap();
    let ref mut buf_writer = BufWriter::new(file);

    let w = (run_data.len() as f32).sqrt() as usize;
    let h = (run_data.len() as f32).sqrt() as usize;

    let mut data = vec![0u8; (4*w*h)];

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

    for i in 0..w*h {
        let div_factor = std::u32::MAX / 256;
        let intensity = (run_data[i as usize] / div_factor) as u8;
        data[4*i as usize + 0] = intensity;
        data[4*i as usize + 1] = intensity;
        data[4*i as usize + 2] = intensity;
        data[4*i as usize + 3] = 255;
    }
    writer.write_image_data(&data).unwrap(); // Save
}

pub fn dump_image_f32(path_str: &str, run_data: &[f32]) {
    let path = Path::new(path_str);
    let file = File::create(path).unwrap();
    let ref mut buf_writer = BufWriter::new(file);

    let w = (run_data.len() as f32).sqrt() as usize;
    let h = (run_data.len() as f32).sqrt() as usize;

    let mut data = vec![0u8; (4*w*h)];

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

    for i in 0..w*h {
        let intensity = (run_data[i as usize] * 255.0) as u8;
        data[4*i as usize + 0] = intensity;
        data[4*i as usize + 1] = intensity;
        data[4*i as usize + 2] = intensity;
        data[4*i as usize + 3] = 255;
    }
    writer.write_image_data(&data).unwrap(); // Save
}