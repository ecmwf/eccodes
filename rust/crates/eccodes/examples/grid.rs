use eccodes::{GribMessage, LatLon};

fn main() -> eccodes::Result<()> {
    let message = GribMessage::from_sample("GRIB2")?;

    // [grid]
    let values = message.values()?;
    println!("{} values", values.len());

    for point in message.grid_points()?.take(3) {
        println!(
            "{:.2} {:.2} {}",
            point.position.lat, point.position.lon, point.value
        );
    }

    println!("{} positions", message.grid_positions()?.count());
    // [end grid]

    // [nearest]
    let mut nearest = message.nearest()?;
    for point in nearest.find(LatLon::new(51.1, 10.3))? {
        println!(
            "{:.2} {:.2} at {:.1} km, index {}",
            point.position.lat, point.position.lon, point.distance_km, point.index
        );
    }

    let stations = [LatLon::new(51.1, 10.3), LatLon::new(40.0, 20.0)];
    for point in nearest.find_each(&stations)? {
        println!("{:?} {}", point.position, point.value);
    }
    // [end nearest]
    Ok(())
}
