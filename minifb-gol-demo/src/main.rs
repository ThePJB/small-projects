use minifb::{Key, Window, WindowOptions};
mod krand;
use krand::*;
use std::collections::HashMap;

const WIDTH: usize = 100;
const HEIGHT: usize = 100;

const ALIVE: u32 = 0x00FFFFFFFF;
const DEAD: u32 = 0;

enum SimulationState {
    Running,
    Countdown(u32),
}
enum DeathReason {
    NotDead,
    Dead,
    Cycle(i32),
}

fn main() {
    let mut options  = WindowOptions::default();
    options.scale = minifb::Scale::X4;

    let mut window = Window::new(
        "Gosh",
        WIDTH,
        HEIGHT,
        options,
    )
    .unwrap_or_else(|e| {
        panic!("{}", e);
    });

    let mut curr_buffer = 0;
    let mut buffers = vec![vec![0; WIDTH * HEIGHT]; 2];

    let mut cycle = 0;

    let mut gen = 0;
    let mut run_hashes = HashMap::new();

    let mut state = SimulationState::Running;
    let mut death_reason = DeathReason::NotDead;

    let mut switch_to_countdown = false;

    while window.is_open() && !window.is_key_down(Key::Escape) {

        let mut hash_acc = 0;
        gen += 1;

        match state {
            SimulationState::Running => {
                match death_reason {
                    DeathReason::NotDead => {
                        // Ok then do death checks
                        for i in 0..WIDTH {
                            for j in 0..HEIGHT {
                                let idx = j*WIDTH + i;
                                let alive = buffers[curr_buffer][idx] == ALIVE;
                                if alive {
                                    hash_acc += khash(idx as u32);
                                }
                            }
                        }

                        if let Some(prev_gen) = run_hashes.get(&hash_acc) {
                            death_reason = DeathReason::Cycle(gen - prev_gen);
                        };
                        
                        run_hashes.insert(hash_acc, gen);
                        
                        if hash_acc == 0 {
                            death_reason = DeathReason::Dead;
                        }

                    },
                    DeathReason::Dead => {
                        println!("dead on gen {}", gen);
                        death_reason = DeathReason::NotDead;
                        state = SimulationState::Countdown(100);
                    },
                    DeathReason::Cycle(length) => {
                        println!("cycle on gen {} of length {}", gen, length);
                        death_reason = DeathReason::NotDead;
                        state = SimulationState::Countdown(100);
                    },
                }
            },
            SimulationState::Countdown(ticks_remaining) if ticks_remaining == 0 => {
                // State reset
                state = SimulationState::Running;
                run_hashes.clear();
                gen = 0;
                cycle += 1;
                switch_to_countdown = false;

                println!("begin cycle {}", cycle);
                for i in 0..WIDTH*HEIGHT {
                    buffers[curr_buffer][i] = if uniform_f32((cycle + 36897439 * i) as u32) > 0.5 {
                        ALIVE
                    } else {
                        DEAD
                    };
                }
            },
            SimulationState::Countdown(ticks_remaining) => {
                state = SimulationState::Countdown(ticks_remaining - 1);
            },
        }

        for i in 0..WIDTH {
            for j in 0..HEIGHT {
                let mut neigh_count = 0;
                for dir in [(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)] {
                    let si = i as i32 + dir.0;
                    let sj = j as i32 + dir.1;
                    if si < 0 || sj < 0 || si >= WIDTH as i32 || sj >= HEIGHT as i32 {
                        continue;
                    }
                    let idx = sj as usize * WIDTH + si as usize;
                    if buffers[curr_buffer][idx] == ALIVE {
                        neigh_count += 1;
                    }
                }
                let idx = j * WIDTH + i;
                if neigh_count < 2 {
                    buffers[1 - curr_buffer][idx] = DEAD;
                } else if neigh_count == 2 {
                    buffers[1 - curr_buffer][idx] = buffers[curr_buffer][idx];
                } else if neigh_count == 3 {
                    buffers[1 - curr_buffer][idx] = ALIVE;
                } else {
                    buffers[1 - curr_buffer][idx] = DEAD;
                }
            }
        }

        window
            .update_with_buffer(&buffers[curr_buffer], WIDTH, HEIGHT)
            .unwrap();

        curr_buffer = 1 - curr_buffer;
    }
}
