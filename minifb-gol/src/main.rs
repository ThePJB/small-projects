use minifb::{Key, Window, WindowOptions};
use std::collections::hash_set::HashSet;
use std::collections::hash_map::HashMap;

#[derive(Copy, Clone, Debug)]
enum Command {
    Up,
    Left,
    Down,
    Right,

    PlayPause,
    Step,
    Faster,
    Slower,
    Reset,

    Load,
    Save,


    ZoomIn(i32, i32),
    ZoomOut(i32, i32),

    Set(i32, i32),
    UnSet(i32, i32),
}

struct Application {
    loaded_state: SimState,
    working_state: SimState,

    play: bool,
    speed: i32,

    scale: i32,
    t_x: i32,
    t_y: i32,

    dirty: bool,
}

impl Application {
    fn new() -> Application {
        Application {
            loaded_state: SimState::new(),
            working_state: SimState::new(),
            play: false,
            speed: 3,
            scale: 8,
            t_x: 0,
            t_y: 0,
            dirty: true,
        }
    }

    fn new_from_state(state: SimState) -> Application {
        let mut app = Application::new();
        app.loaded_state = state.clone();
        app.working_state = state.clone();
        app
    }

    fn apply_command(&mut self, command: Command) {
        println!("{:?}", command);

        self.dirty = true;

        let scroll_speed = 4;

        match command {
            Command::Up => self.t_y -= scroll_speed,
            Command::Down => self.t_y += scroll_speed,
            Command::Left => self.t_x -= scroll_speed,
            Command::Right => self.t_x += scroll_speed,
            Command::PlayPause => self.play = !self.play,
            Command::Step => self.step(),
            Command::Faster => self.speed += 1,
            Command::Slower => self.speed = std::cmp::max(1, self.speed - 1),
            Command::Reset => {
                self.play = false;
                self.working_state = self.loaded_state.clone();
            },
            Command::Load => {}, // nyi
            Command::Save => {}, // nyi
            Command::ZoomIn(x, y) => {
                // proper x and y maintaining can wait lol
                self.scale += 1;
            },
            Command::ZoomOut(x, y) => {
                // proper x and y maintaining can wait lol
                self.scale = std::cmp::max(1, self.scale - 1);
            },
            Command::Set(x, y) => {
                self.loaded_state.set(x,y);
                self.working_state.set(x,y);
            }
            Command::UnSet(x, y) => {
                self.loaded_state.kill(x,y);
                self.working_state.kill(x,y);
            }
        }
    }

    fn step(&mut self) {
        self.dirty = true;

        let copy_hm = self.working_state.clone();
        for ((x, y), cell_data) in copy_hm.cells.iter() {
            let alive = cell_data & alive_mask > 0;
            let count = cell_data & dead_mask;

            if count == 3 {
                if !alive {
                    self.working_state.set(*x, *y);
                }
            } else if count == 2 {
                // do nothing
            } else if alive {
                self.working_state.kill(*x, *y);
            }
        }
    }

    fn draw_to_buffer(&mut self, xres: usize, yres: usize, px_buf: &mut Vec<u32>) {

        if !self.dirty {
            return
        }
        self.dirty = false;

        let colour_alive = 0x00FFFFFF;
        let colour_dead = 0x00000000;

        let cell_w = xres as i32 / self.scale + 1;
        let cell_h = yres as i32 / self.scale + 1;

        for cell_i in 0..cell_w {
            for cell_j in 0..cell_h {
                let (alive, nn) = self.working_state.get(cell_i + self.t_x, cell_j + self.t_y);
                let colour = if alive {
                    colour_alive
                } else {
                    colour_dead
                };

                for buf_i in cell_i*self.scale..(cell_i+1)*self.scale {
                    for buf_j in cell_j*self.scale..(cell_j+1)*self.scale {

                        if buf_i < xres as i32 && buf_j < yres as i32 {
                            px_buf[(buf_i + xres as i32 * buf_j) as usize] = colour;
                        }
                    }
                }
            }
        }
    }
}

#[derive(Clone)]
struct SimState {
    cells: HashMap<(i32, i32), u32>,
}

const alive_mask: u32 = 0x80000000;
const dead_mask: u32 = !alive_mask;

impl SimState {
    fn new() -> SimState {
        SimState {
            cells: HashMap::new()
        }
    }

    fn set(&mut self, x: i32, y: i32) {
        if let Some(mut c) = self.cells.get_mut(&(x, y)) {
            if *c & alive_mask > 0 {
                return;
            }
            
            *c |= alive_mask;
        } else {
            self.cells.insert((x, y), alive_mask);
        }
        let neighbour_indices = vec![(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 1), (1, 0)];
        for (dx, dy) in neighbour_indices {
            if let Some(mut n) = self.cells.get_mut(&(x + dx, y + dy)) {
                *n += 1;
            } else {
                self.cells.insert((x + dx, y + dy), 1);
            }
        }
    }

    fn kill(&mut self, x: i32, y: i32) {
        if let Some(mut c) = self.cells.get_mut(&(x, y)) {
            if *c & alive_mask == 0 {
                return;
            }

            *c &= dead_mask;
        }
        let neighbour_indices = vec![(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 1), (1, 0)];
        for (dx, dy) in neighbour_indices {
            if let Some(mut n) = self.cells.get_mut(&(x + dx, y + dy)) {
                // this is a bit defensive but ceebs
                if *n == 0 {
                    self.cells.remove(&(x + dx, y + dy));
                } else {
                    *n -= 1;
                }
            }
        }
    }

    fn get(&self, x: i32, y: i32) -> (bool, u32) {
        let cell_data = self.cells.get(&(x, y)).unwrap_or(&0);
        let alive = cell_data & alive_mask > 0;
        let count = cell_data & dead_mask;
        (alive, count)
    }
}

const WIDTH: usize = 1600;
const HEIGHT: usize = 1600;

fn random_state() {}

fn main() {
    let mut key_schema: HashMap<Key, Command> = HashMap::new();
    key_schema.insert(Key::W, Command::Up);
    key_schema.insert(Key::A, Command::Left);
    key_schema.insert(Key::D, Command::Right);
    key_schema.insert(Key::S, Command::Down);
    key_schema.insert(Key::Space, Command::PlayPause);
    key_schema.insert(Key::Period, Command::Step);
    key_schema.insert(Key::LeftBracket, Command::Slower);
    key_schema.insert(Key::RightBracket, Command::Faster);
    key_schema.insert(Key::R, Command::Reset);
    key_schema.insert(Key::O, Command::Load);
    key_schema.insert(Key::P, Command::Save);
/*
    let mut glider = SimState::new();
    glider.set(10, 11);
    glider.set(11, 12);
    glider.set(12, 10);
    glider.set(12, 11);
    glider.set(12, 12);
*/
    let mut rpentomino = SimState::new();
    rpentomino.set(1, 0);
    rpentomino.set(2, 0);
    rpentomino.set(0, 1);
    rpentomino.set(1, 1);
    rpentomino.set(1, 2);

    let demo_states = vec!{
        rpentomino,

    }

    let mut app = Application::new_from_state(rpentomino);

    let mut buffer: Vec<u32> = vec![0; WIDTH * HEIGHT];

    let mut window = Window::new(
        "Gosh",
        WIDTH,
        HEIGHT,
        WindowOptions::default(),
    )
    .unwrap_or_else(|e| {
        panic!("{}", e);
    });

    // Limit to max ~60 fps update rate
    window.limit_update_rate(Some(std::time::Duration::from_micros(16600)));

    let mut generation = 0;

    while window.is_open() && !window.is_key_down(Key::Escape) {
        if let Some(keys_pressed) = window.get_keys_pressed(minifb::KeyRepeat::Yes) {
            for key in keys_pressed.iter() {
                if let Some(command) = key_schema.get(key) {
                    app.apply_command(*command);
                }
            }
        }

        if let Some((mouse_x, mouse_y)) = window.get_mouse_pos(minifb::MouseMode::Clamp) {
            let mouse_x_i32 = (mouse_x as f32) as i32 / app.scale + app.t_x;
            let mouse_y_i32 = (mouse_y as f32) as i32 / app.scale + app.t_y;

            if let Some((_, scroll_y)) = window.get_scroll_wheel() {
                if scroll_y > 0.0 {
                    app.apply_command(Command::ZoomIn(mouse_x_i32, mouse_y_i32))
                } else if scroll_y < 0.0 {
                    app.apply_command(Command::ZoomOut(mouse_x_i32, mouse_y_i32))
                }
            }

            if window.get_mouse_down(minifb::MouseButton::Left) {
                app.apply_command(Command::Set(mouse_x_i32, mouse_y_i32))
            }
            if window.get_mouse_down(minifb::MouseButton::Right) {
                app.apply_command(Command::UnSet(mouse_x_i32, mouse_y_i32))
            }
        }


        app.draw_to_buffer(WIDTH, HEIGHT, &mut buffer);
        
        generation += 1;
        let fps_divide = if app.speed > 4 {
                1
            } else {
                30 / app.speed
            };
        let updates_per_frame = if app.speed > 4 {
                app.speed - 4
            } else {
                1
            };

        if app.play && generation % fps_divide == 0 {
            for _ in 0..updates_per_frame {
                app.step();
            }
        }

        window
            .update_with_buffer(&buffer, WIDTH, HEIGHT)
            .unwrap();
    }
}