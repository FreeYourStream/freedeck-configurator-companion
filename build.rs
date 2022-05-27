use std::env;
use std::path::Path;
use std::path::PathBuf;

// Example custom build script.
fn main() {
    // Tell Cargo that if the given file changes, to rerun this build script.
    let assets = ["icon.png", "favicon.ico"];
    for asset in assets.iter() {
        println!("cargo:rerun-if-changed=src/{}", asset);
    }
    println!("cargo:rerun-if-changed=src/icon.png");
    println!("cargo:rerun-if-changed=src/favicon.ico");

    let output_path = get_output_path();
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let input_path = Path::new(&manifest_dir);

    for asset in assets.iter() {
        std::fs::copy(
            input_path
                .join("src/".to_string() + asset)
                .to_str()
                .unwrap(),
            output_path.join(asset),
        )
        .unwrap();
    }
}
fn get_output_path() -> PathBuf {
    //<root or manifest path>/target/<profile>/
    let manifest_dir_string = env::var("CARGO_MANIFEST_DIR").unwrap();
    let build_type = env::var("PROFILE").unwrap();
    let path = Path::new(&manifest_dir_string)
        .join("target")
        .join(build_type);
    return PathBuf::from(path);
}
