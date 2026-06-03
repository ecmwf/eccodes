# ecCodes Rust API

A modern, idiomatic Rust interface to the ecCodes C library for reading and writing
GRIB and BUFR messages.

## Project structure

```
rust/
├── Cargo.toml                  # Workspace definition
├── eccodes-sys/                # Raw FFI bindings (-sys crate)
│   ├── Cargo.toml
│   ├── build.rs                # Finds and links libeccodes
│   └── src/lib.rs              # C function declarations + constants
├── eccodes/                    # High-level Rust API
│   ├── Cargo.toml
│   ├── src/
│   │   ├── lib.rs              # Crate root, re-exports, free functions
│   │   ├── error.rs            # EcCodesError, Result<T>
│   │   ├── types.rs            # ProductKind, KeyType
│   │   ├── message.rs          # Message, MessageIterator, GetKey/SetKey traits
│   │   ├── keys_iterator.rs    # KeysIterator (Iterator<Item=String>)
│   │   ├── geo_iterator.rs     # [advanced] GeoIterator, GeoPoint
│   │   ├── nearest.rs          # [advanced] Nearest, NearestPoint
│   │   ├── index.rs            # [advanced] Index, IndexKey trait
│   │   ├── multi_handle.rs     # [advanced] MultiHandle
│   │   ├── context.rs          # [advanced] Context
│   │   └── util.rs             # [advanced] Julian date conversion, etc.
│   ├── examples/               # 27 Rust examples (ports of C examples)
│   │   ├── grib_get_keys.rs    # Read keys from GRIB
│   │   ├── grib_set_keys.rs    # Set keys in GRIB
│   │   ├── grib_print_data.rs  # Print all data values
│   │   ├── grib_clone.rs       # Clone GRIB messages
│   │   ├── grib_copy_message.rs # Copy message via raw bytes
│   │   ├── grib_keys_iterator.rs # Iterate over keys
│   │   ├── grib_iterator.rs    # Geo-iterator on lat/lon/values
│   │   ├── grib_get_data.rs    # Get lat/lon/values
│   │   ├── grib_nearest.rs     # Nearest neighbour search
│   │   ├── grib_index.rs       # Index-based access
│   │   ├── grib_multi.rs       # Multi-field GRIB2 decoding
│   │   ├── grib_multi_write.rs # Multi-field GRIB2 encoding
│   │   ├── grib_list.rs        # Array key access
│   │   ├── grib_precision.rs   # Decimal precision control
│   │   ├── grib_set_data.rs    # Set data values
│   │   ├── grib_set_bitmap.rs  # Set bitmap
│   │   ├── grib_set_missing.rs # Set missing values
│   │   ├── grib_set_pv.rs      # Set vertical coordinates
│   │   ├── new_sample.rs       # Create from sample
│   │   ├── get_product_kind.rs # Detect product type
│   │   ├── bufr_get_keys.rs    # Read BUFR keys
│   │   ├── bufr_set_keys.rs    # Set BUFR keys
│   │   ├── bufr_read_header.rs # Read BUFR header
│   │   ├── bufr_expanded.rs    # Read expanded BUFR data
│   │   ├── bufr_missing.rs     # Handle BUFR missing values
│   │   ├── bufr_clone.rs       # Clone BUFR messages
│   │   └── bufr_keys_iterator.rs # Iterate BUFR keys
│   └── tests/
│       ├── basic_tests.rs      # 43 tests for the basic API
│       └── advanced_tests.rs   # 22 tests for the advanced API
└── rust_api.md                 # This document
```

## Building

```bash
# Set the path to your ecCodes build
export ECCODES_LIB_DIR=/path/to/eccodes/lib
export LD_LIBRARY_PATH=$ECCODES_LIB_DIR:$LD_LIBRARY_PATH

# Build
cd rust
cargo build

# Build with advanced features
cargo build --features advanced

# Run tests (single-threaded, ecCodes is not thread-safe)
cargo test --features advanced -- --test-threads=1

# Run the example
cargo run --example read_grib -- /path/to/file.grib2
```

## Feature flags

| Feature    | Description |
|------------|-------------|
| (default)  | Basic API: Message, MessageIterator, get/set, keys, write, samples, clone |
| `advanced` | Adds: Index, GeoIterator, Nearest, MultiHandle, Context, util, element access, force-set, raw bytes |

```toml
# Cargo.toml of a downstream crate
[dependencies]
eccodes = { path = "../eccodes/rust/eccodes" }                     # basic only
eccodes = { path = "../eccodes/rust/eccodes", features = ["advanced"] }  # all features
```

## Design principles

### 1. Generic get/set via traits

The C API has ~20 typed functions (`codes_get_long`, `codes_get_double`, `codes_get_string`,
`codes_get_long_array`, `codes_get_double_array`, etc.). In Rust, these collapse into **two
generic methods** via the `GetKey`/`SetKey` traits:

```rust
// One method, many types:
let step: i64        = msg.get("step")?;
let lat: f64         = msg.get("latitudeOfFirstGridPointInDegrees")?;
let name: String     = msg.get("shortName")?;
let values: Vec<f64> = msg.get("values")?;

msg.set("centre", &98_i64)?;
msg.set("shortName", "2t")?;
msg.set("values", &data[..])?;
```

### 2. RAII — automatic resource management

All C resources are wrapped in Rust types with `Drop`:

| C resource | Rust type | Drop calls |
|---|---|---|
| `codes_handle*` | `Message` | `codes_handle_delete` |
| `codes_keys_iterator*` | `KeysIterator` | `codes_keys_iterator_delete` |
| `codes_iterator*` | `GeoIterator` | `codes_grib_iterator_delete` |
| `codes_nearest*` | `Nearest` | `codes_grib_nearest_delete` |
| `codes_index*` | `Index` | `codes_index_delete` |
| `codes_multi_handle*` | `MultiHandle` | `codes_grib_multi_handle_delete` |

No manual `delete`/`close` calls needed.

### 3. Iterator trait

```rust
// C: while ((h = codes_handle_new_from_file(...)) != NULL) { ... }
// Rust:
for message in MessageIterator::from_grib_file("data.grib2")? {
    let message = message?;
    // ...
}
```

### 4. Result-based error handling

All fallible operations return `Result<T, EcCodesError>`. No out-parameter error codes.

### 5. Feature-gated advanced API

Advanced modules are compiled only with `features = ["advanced"]`.

---

## Basic API usage

```rust
use eccodes::{MessageIterator, Message, Result};

fn main() -> Result<()> {
    // Read messages from a GRIB file
    for message in MessageIterator::from_grib_file("data.grib2")? {
        let message = message?;

        let short_name: String = message.get("shortName")?;
        let level: i64         = message.get("level")?;
        let values: Vec<f64>   = message.get("values")?;

        println!("{short_name} level={level} nvalues={}", values.len());

        // Iterate over keys
        for key_name in message.keys() {
            println!("  {key_name}");
        }
    }

    // Create from sample, modify, write
    let mut msg = Message::from_grib_sample("GRIB2")?;
    msg.set("shortName", "2t")?;
    msg.set("centre", &98_i64)?;
    msg.set("values", &vec![273.15f64; 115680][..])?;
    msg.write_to_file("output.grib2")?;

    // Library info
    println!("ecCodes version: {}", eccodes::api_version());
    println!("Messages in file: {}", eccodes::count_messages("data.grib2")?);

    Ok(())
}
```

## Advanced API usage

```rust
use eccodes::{MessageIterator, Message, Result};
use eccodes::index::Index;         // requires features = ["advanced"]
use eccodes::nearest::Nearest;     // requires features = ["advanced"]

fn main() -> Result<()> {
    // Index-based access
    let mut index = Index::from_file("data.grib2", &["shortName", "level"])?;
    let names: Vec<String> = index.get("shortName")?;
    index.select("shortName", &names[0])?;

    while let Some(msg) = index.next_message()? {
        // Nearest neighbour
        let mut nearest = msg.nearest()?;
        let points = nearest.find(51.5, -0.1)?;
        println!("4 nearest points: {points:?}");

        // Geo iterator
        for point in msg.geo_iterator()? {
            println!("lat={} lon={} val={}", point.lat, point.lon, point.value);
        }

        // Raw bytes roundtrip
        let bytes = msg.to_bytes()?;
        let msg2 = Message::from_bytes_copy(&bytes)?;

        // Element access
        let val: f64 = msg.get_element("values", 42)?;
        let vals: Vec<f64> = msg.get_elements("values", &[0, 1, 2])?;
    }

    // Julian date utilities
    let jd = eccodes::util::datetime_to_julian(2024, 6, 15, 12, 0, 0)?;
    let (y, m, d, h, mi, s) = eccodes::util::julian_to_datetime(jd)?;

    Ok(())
}
```

---

## C API → Rust mapping

### Summary: function count reduction

| C API area | # C functions | Rust mechanism | Rust API surface |
|---|---|---|---|
| Get (scalar+array) | 14 | `GetKey` trait + generics | 1 method: `msg.get::<T>()` |
| Set (scalar+array) | 10 | `SetKey` trait + generics | 1 method: `msg.set()` |
| Handle lifecycle | 15 | constructors + `Drop` | constructors, no delete |
| Keys iteration | 11 | `Iterator` trait | `msg.keys()` |
| Geo iteration | 7 | `Iterator` trait | `msg.geo_iterator()` |
| Index | 13 | `IndexKey` trait | `index.get::<T>()`, `index.select()` |
| Error codes | ~60 defines | `EcCodesError` enum | `Result<T>` |
| **Total** | **~150** | | **~55 user-facing items** |

### Detailed function mapping

#### Handle — create/open (basic)

| C function | Rust equivalent |
|---|---|
| `codes_handle_new_from_file` | `MessageIterator::from_file(path, product)` |
| `codes_grib_handle_new_from_file` | `MessageIterator::from_grib_file(path)` |
| `codes_bufr_handle_new_from_file` | `MessageIterator::from_bufr_file(path)` |
| `codes_handle_new_from_samples` | `Message::from_sample(name)` |
| `codes_grib_handle_new_from_samples` | `Message::from_grib_sample(name)` |
| `codes_bufr_handle_new_from_samples` | `Message::from_bufr_sample(name)` |
| `codes_handle_clone` | `msg.clone()` (impl `Clone`) |
| `codes_handle_delete` | `impl Drop for Message` (automatic) |

#### Handle — create (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_handle_new_from_message` | `Message::from_bytes(data)` |
| `codes_handle_new_from_message_copy` | `Message::from_bytes_copy(data)` |
| `codes_handle_clone_headers_only` | `msg.clone_headers_only()` |
| `codes_handle_new_from_index` | `index.next_message()` |

#### Get values (basic) — all via `msg.get::<T>(key)`

| C function | Rust type `T` |
|---|---|
| `codes_get_long` | `i64` |
| `codes_get_double` | `f64` |
| `codes_get_float` | `f32` |
| `codes_get_string` | `String` |
| `codes_get_bytes` | `Vec<u8>` |
| `codes_get_long_array` | `Vec<i64>` |
| `codes_get_double_array` | `Vec<f64>` |
| `codes_get_float_array` | `Vec<f32>` |
| `codes_get_string_array` | `Vec<String>` |

#### Set values (basic) — all via `msg.set(key, value)`

| C function | Rust value type |
|---|---|
| `codes_set_long` | `&i64` |
| `codes_set_double` | `&f64` |
| `codes_set_string` | `&str` / `&String` |
| `codes_set_bytes` | `&[u8]` |
| `codes_set_long_array` | `&[i64]` |
| `codes_set_double_array` | `&[f64]` |
| `codes_set_float_array` | `&[f32]` |
| `codes_set_string_array` | `&[String]` |
| `codes_set_missing` | `msg.set_missing(key)` |

#### Metadata (basic)

| C function | Rust equivalent |
|---|---|
| `codes_get_size` | `msg.get_size(key)` |
| `codes_get_length` | `msg.get_length(key)` |
| `codes_is_missing` | `msg.is_missing(key)` → `Result<bool>` |
| `codes_is_defined` | `msg.is_defined(key)` → `bool` |
| `codes_get_product_kind` | `msg.product_kind()` |

#### Write (basic)

| C function | Rust equivalent |
|---|---|
| `codes_write_message` | `msg.write_to_file(path)` / `msg.append_to_file(path)` |

#### Keys iterator (basic)

| C function | Rust equivalent |
|---|---|
| `codes_keys_iterator_new` | `msg.keys()` / `msg.keys_filtered(flags)` / `msg.keys_in_namespace(ns)` |
| `codes_keys_iterator_next` + `_get_name` | `Iterator::next()` → `Option<String>` |
| `codes_keys_iterator_delete` | `impl Drop` (automatic) |

#### Counting (basic)

| C function | Rust equivalent |
|---|---|
| `codes_count_in_filename` | `eccodes::count_messages(path)` |

#### Version/info (basic)

| C function | Rust equivalent |
|---|---|
| `codes_get_api_version` | `eccodes::api_version()` |
| `codes_get_git_sha1` | `eccodes::git_sha1()` |
| `codes_get_package_name` | `eccodes::package_name()` |
| `codes_get_build_date` | `eccodes::build_date()` |
| `codes_get_git_branch` | `eccodes::git_branch()` |

#### Geo iterator (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_grib_iterator_new` | `msg.geo_iterator()` |
| `codes_grib_iterator_next` | `Iterator::next()` → `Option<GeoPoint>` |
| `codes_grib_iterator_reset` | `iter.reset()` |
| `codes_grib_iterator_delete` | `impl Drop` (automatic) |
| `codes_grib_get_data` | `msg.get_data()` → `Vec<GeoPoint>` |

#### Nearest (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_grib_nearest_new` | `msg.nearest()` |
| `codes_grib_nearest_find` | `nearest.find(lat, lon)` → `[NearestPoint; 4]` |
| `codes_grib_nearest_delete` | `impl Drop` (automatic) |

#### Index (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_index_new` | `Index::new(&["key1", "key2"])` |
| `codes_index_new_from_file` | `Index::from_file(path, &["key1"])` |
| `codes_index_add_file` | `index.add_file(path)` |
| `codes_index_delete` | `impl Drop` (automatic) |
| `codes_index_get_size` | `index.get_size(key)` |
| `codes_index_get_long/double/string` | `index.get::<T>(key)` (generic) |
| `codes_index_select_long/double/string` | `index.select(key, &value)` (generic) |
| `codes_handle_new_from_index` | `index.next_message()` → `Option<Message>` |

#### Multi-handle (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_grib_multi_handle_new` | `MultiHandle::new()` |
| `codes_grib_multi_handle_append` | `mh.append(&mut msg, section)` |
| `codes_grib_multi_handle_write` | `mh.write(path)` |
| `codes_grib_multi_handle_delete` | `impl Drop` (automatic) |

#### Element access (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_get_double_element` / `codes_get_float_element` | `msg.get_element::<T>(key, idx)` |
| `codes_get_double_elements` / `codes_get_float_elements` | `msg.get_elements::<T>(key, &[idx])` |

#### Force-set (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_set_force_double_array` | `msg.set_force(key, &[f64])` |
| `codes_set_force_float_array` | `msg.set_force(key, &[f32])` |

#### Raw message access (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_get_message` | `msg.to_bytes()` → `Vec<u8>` |
| `codes_get_message_offset` | `msg.message_offset()` |
| `codes_get_message_size` | `msg.message_size()` |

#### Key introspection (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_get_native_type` | `msg.native_type(key)` → `KeyType` |
| `codes_key_is_computed` | `msg.key_is_computed(key)` → `bool` |
| `codes_get_offset` | `msg.get_offset(key)` |

#### Utility (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_datetime_to_julian` | `util::datetime_to_julian(y,m,d,h,mi,s)` |
| `codes_julian_to_datetime` | `util::julian_to_datetime(jd)` |
| `codes_date_to_julian` | `util::date_to_julian(date)` |
| `codes_julian_to_date` | `util::julian_to_date(jdate)` |

#### Context (advanced)

| C function | Rust equivalent |
|---|---|
| `codes_context_get_default` | `Context::get_default()` |
| `codes_context_set_definitions_path` | `ctx.set_definitions_path(path)` |
| `codes_context_set_samples_path` | `ctx.set_samples_path(path)` |
| `codes_context_set_debug` | `ctx.set_debug(mode)` |
| `codes_grib_multi_support_on/off` | `ctx.multi_support_on()` / `ctx.multi_support_off()` |

---

## Thread safety

ecCodes is **not thread-safe**. When running tests, use `--test-threads=1`:

```bash
cargo test --features advanced -- --test-threads=1
```
