use std::process::Command;

pub fn get() -> rouille::Response {
    if cfg!(target_os = "windows") {
        rouille::Response::text("not implemented yet")
    } else {
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
}
