#[macro_use]
extern crate rouille;
#[path = "./router/cors.rs"]
mod cors;
#[path = "./router/current_window.rs"]
mod current_window;
#[path = "./lib/system_info.rs"]
mod system_info;

#[cfg(window)]
use std::sync::Mutex;

use std::sync::Mutex;
use system_info::SystemInfo;
use tray_item::TrayItem;

fn main() {
    let icon_name: String = if cfg!(target_os = "linux") {
        gtk::init().unwrap();
        let exe_path = std::env::current_exe().unwrap();

        let icon_path = exe_path.parent().unwrap().join("icon.png");
        String::from(icon_path.to_str().unwrap())
    } else {
        String::from("fd-icon-tray")
    };
    #[cfg(windows)]
    let (tx, rx) = mpsc::channel();

    let system_info = Mutex::new(SystemInfo::new());

    let server = rouille::Server::new("localhost:8000", move |request| {
        if request.method() == "OPTIONS" {
            return cors::corsify(request, rouille::Response::empty_204());
        }
        router!(request,
            (GET) (/cpu_temp) => {
                let response = rouille::Response::text(&system_info.lock().unwrap().cpu_temp().to_string());
                cors::corsify(request, response)
            },
            (GET) (/current_window) => {
                let response = current_window::get();
                cors::corsify(request, response)
            },
            _ => rouille::Response::empty_404()
        )
    }).unwrap();

    let (_handle, sender) = server.stoppable();

    let mut tray = TrayItem::new("Tray", &icon_name).unwrap();

    tray.add_menu_item("Quit", move || {
        sender.send(()).unwrap();
        println!("Quitting...");
        #[cfg(target_os = "linux")]
        gtk::main_quit();
        #[cfg(windows)]
        tx.send(()).unwrap();
    })
    .unwrap();
    println!("Idle");

    #[cfg(target_os = "linux")]
    gtk::main();
    #[cfg(windows)]
    {
        loop {
            match rx.recv() {
                Ok(Message::Quit) => break,
                _ => {}
            }
        }
    }
    println!("Gone");
}
