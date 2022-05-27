#[cfg(windows)]
pub fn get() -> rouille::Response {
    use std::{ffi::OsString, os::windows::prelude::OsStringExt};
    use winapi::um::winuser::{GetForegroundWindow, GetWindowTextW};

    unsafe {
        let window = GetForegroundWindow();
        let mut text: [u16; 512] = [0; 512];
        let _result = GetWindowTextW(window, text.as_mut_ptr(), text.len() as i32);
        return rouille::Response::text(OsString::from_wide(&text).to_str().unwrap());
    }
}

#[cfg(not(windows))]
pub fn get() -> rouille::Response {
    use std::process::Command;
    let mut command = Command::new("sh");
    command
        .arg("-c")
        .arg(include_str!("../linux_active_window.sh"));

    let output = command.output().expect("failed to execute process");
    let result = String::from_utf8(output.stdout).unwrap();
    let success = result.trim().len() > 0;

    if success {
        rouille::Response::text(&result)
    } else {
        rouille::Response::text("failed to get active window, xprop installed?")
            .with_status_code(500)
    }
}
