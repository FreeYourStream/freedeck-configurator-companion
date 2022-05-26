#[macro_use]
extern crate rouille;

use std::{process::Command, sync::Mutex};
use sysinfo::{ComponentExt, System, SystemExt};

struct SystemInfo {
    sys: System,
}

impl SystemInfo {
    fn new() -> SystemInfo {
        let sys = System::new_all();
        println!("{:?}", sys.components());
        SystemInfo { sys }
    }
    fn cpu(&mut self) -> f32 {
        let cpu = self
            .sys
            .components_mut()
            .iter_mut()
            .find(|c| c.label() == "Tdie" || c.label() == "Package id 0")
            .unwrap();
        cpu.refresh();
        cpu.temperature()
    }
}

fn main() {
    let system_info = Mutex::new(SystemInfo::new());
    rouille::start_server("localhost:8000", move |request| {
        router!(request,
            (GET) (/cpu_temp) => {
                rouille::Response::text(&system_info.lock().unwrap().cpu().to_string())
            },
            (GET) (/current_window) => {
                if cfg!(target_os = "windows") {
                    rouille::Response::text("not implemented yet")
                } else {
                    let output = Command::new("bash")
                        .arg("-c")

                        .arg("xprop -id $(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2) _NET_WM_NAME 2>/dev/null | awk -F= '{print($2)}'")
                        .output()
                        .expect("failed to execute process");
                    let result = match String::from_utf8(output.stdout) {
                        Ok(v) => v,
                        Err(e) => panic!("Invalid UTF-8 sequence: {}", e),
                    };
                    rouille::Response::text(&result)
                }
            },
            (GET) (/hello) => {
                let output = if cfg!(target_os = "windows") {
                    Command::new("cmd")
                            .args(["/C", "echo hello"])
                            .output()
                            .expect("failed to execute process")
                } else {
                    Command::new("bash")
                            .arg("-c")
                            .arg("xprop -id $(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2) _NET_WM_NAME 2>/dev/null | awk -F= '{print($2)}'")
                            .output()
                            .expect("failed to execute process")
                };
                let result = match String::from_utf8(output.stdout) {
                    Ok(v) => v,
                    Err(e) => panic!("Invalid UTF-8 sequence: {}", e),
                };

                rouille::Response::text(&result)
            },
            _ => rouille::Response::empty_404()
        )
    });
}
