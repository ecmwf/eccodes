# Migrating from the unofficial bindings

This guide is for users of the [ScaleWeather `eccodes`](https://github.com/ScaleWeather/eccodes)
crate (0.15 API) moving to the official `eccodes` crate. Migration has two
independent steps; you can stop after the first.

## Step 1: switch the implementation, keep your code

```toml
[dependencies]
eccodes = { package = "eccodes-compat", version = "0.1" }
```

Your source stays as it is. Everything now runs on the official bindings
underneath, and you receive new ecCodes versions through this crate's updates.

## Step 2: move to the official API

Drop the package rename; the official crate's versions track the ecCodes C
library:

```toml
[dependencies]
eccodes = "2.49"
```

The official crate covers everything the 0.15 API does, plus BUFR, indexes,
message writing to arbitrary `io::Write`, and per-element array access.
The mapping is mechanical:

| ScaleWeather 0.15 | Official `eccodes` |
| --- | --- |
| `CodesFile::new_from_file(path, ProductKind::GRIB)` | `GribFile::open(path)?` |
| `CodesFile::new_from_memory(buf, ProductKind::GRIB)` | `GribFile::messages_from(Cursor::new(buf))` |
| `handle.ref_message_iter()` / `arc_message_iter()` | `file.messages()?` (a `std` `Iterator`) |
| `RefMessage` / `ArcMessage` / `BufMessage` | `GribMessage` (owned, `Send`, always editable) |
| `msg.read_key::<T>("name")` | `msg.get::<T>("name")?` |
| `msg.read_key_unchecked::<T>("name")` | `msg.get::<T>("name")?` |
| `msg.read_key_dynamic("name")` | `msg.key("name").value_type()?` + typed `get` |
| `msg.try_clone()` | `msg.try_clone()?` |
| `buf_msg.write_key_unchecked("name", v)` | `msg.set("name", v)?` |
| `msg.write_to_file(path, append)` | `msg.write_to(File::create(path)?)?` |
| `msg.default_keys_iterator()` | `msg.keys()` |
| `msg.new_keys_iterator(&flags, "ns")` | `msg.keys().flags(...).namespace("ns")` |
| `msg.codes_nearest()?.find_nearest(lat, lon)` | `msg.nearest()?.find(LatLon::new(lat, lon))?` |
| `NearestGridpoint { lat, lon, distance, .. }` | `NearestPoint { position, distance_km, .. }` |
| `msg.to_ndarray::<T>()` | reshape `msg.get::<Vec<T>>("values")?` by `Ni`/`Nj` |
| `msg.to_lons_lats_values()` | `msg.data_points()?` (a `Vec<GeoPoint>`) |
| `CodesError` | `Error`, inspected via `err.code()` |

### Iteration

`FallibleIterator` is gone; messages arrive as a standard
`Iterator<Item = Result<GribMessage>>`, so `for` loops and the whole
`Iterator` adapter family apply:

```rust
// before
while let Some(msg) = handle.ref_message_iter().next()? {
    let short_name: String = msg.read_key("shortName")?;
}

// after
for msg in GribFile::open("data.grib")?.messages()? {
    let msg = msg?;
    let short_name: String = msg.get("shortName")?;
}
```

`GribFile` is re-iterable: `messages()` opens a fresh stream each call, so
there is no equivalent of the shared cursor to reason about.

### The message ownership triad

`RefMessage`, `ArcMessage` and `BufMessage` collapse into one type. Every
`GribMessage` is owned, independent of its file, `Send`, and writable; the
lifetime juggling and the `try_clone()`-before-edit step disappear:

```rust
// before: clone to edit, ArcMessage to cross threads
let mut editable = msg.try_clone()?;
editable.write_key_unchecked("level", 850)?;

// after: the message from the iterator is already editable and Send
let mut msg = msg;
msg.set("level", 850)?;
std::thread::spawn(move || msg.get::<f64>("latitudeOfFirstGridPointInDegrees"));
```

To read one message from several threads at once, clone it per thread;
ecCodes handles do not support concurrent access.

### Dynamic key reads

There is no `DynamicKeyType`. Ask the key what it is, then read it typed:

```rust
use eccodes::KeyType;

let key = msg.key("values");
match key.value_type()? {
    KeyType::F64 if key.len()? > 1 => drop(msg.get::<Vec<f64>>("values")?),
    KeyType::F64 => drop(msg.get::<f64>("values")?),
    KeyType::I64 => drop(msg.get::<i64>("values")?),
    KeyType::String => drop(msg.get::<String>("values")?),
    _ => {}
}
```

Missing values become `Option`: `msg.get::<Option<f64>>("level")?` returns
`None` where the 0.15 API made you compare against sentinel constants.

### Errors

`Error` carries a `Code` (the `CODES_*` value), the key or path involved,
and any underlying `io::Error`:

```rust
// before
match err { CodesError::Internal(CodesInternal::CodesNotFound) => ..., _ => ... }

// after
match err.code() { Some(Code::NotFound) => ..., _ => ... }
```

### ndarray

The official crate does not depend on `ndarray`. Either keep using this
compat crate for `to_ndarray()`, or reshape directly; the conversion is the
same three keys the 0.15 implementation read:

```rust
let (ni, nj): (usize, usize) = (msg.get::<i64>("Ni")? as usize, msg.get::<i64>("Nj")? as usize);
let values = ndarray::Array2::from_shape_vec((nj, ni), msg.get::<Vec<f64>>("values")?)?;
```

For collocated coordinates, `msg.data_points()?` yields `GeoPoint`s carrying
position and value together.
