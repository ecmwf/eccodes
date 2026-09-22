//! Minimal eccodes-sys consumer: proves the linked libraries resolve at
//! runtime by calling into libeccodes.

fn main() {
    let version = unsafe { eccodes_sys::codes_get_api_version() };
    // Encoded as major*10000 + minor*100 + revision.
    println!(
        "eccodes {}.{}.{}",
        version / 10000,
        version / 100 % 100,
        version % 100
    );
}
