#[macro_use]
extern crate rouille;

#[path = "./router/cors.rs"]
mod cors;
#[path = "./router/current_window.rs"]
mod current_window;
#[path = "./lib/system_info.rs"]
mod system_info;

use std::sync::Mutex;
use system_info::SystemInfo;

fn main() {
    let system_info = Mutex::new(SystemInfo::new());
    rouille::start_server("localhost:8000", move |request| {
        if request.method() == "OPTIONS" {
            return cors::corsify(request, rouille::Response::empty_204());
        }
        router!(request,
            (GET) (/cpu_temp) => {
                let response = rouille::Response::text(&system_info.lock().unwrap().cpu_temp().to_string());
                cors::corsify(request, response)
            },
            (GET) (/current_window) => {
                let response = cors::corsify(request, current_window::get());
                cors::corsify(request, response)
            },
            _ => rouille::Response::empty_404()
        )
    });
}
