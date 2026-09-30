# eccodes-compat

The [ScaleWeather `eccodes`](https://github.com/ScaleWeather/eccodes) 0.15 API,
reimplemented as a thin layer over the official [`eccodes`](../eccodes) crate.

For existing users of the unofficial bindings, migration is one `Cargo.toml` line —
no source changes:

```toml
[dependencies]
eccodes = { package = "eccodes-compat", version = "0.1" }
```

This crate is frozen at the 0.15 surface and maintained for migration only.
New code should depend on the official `eccodes` crate directly.

## Known behavioural differences

Results are identical everywhere; the remaining differences are in
concurrency and performance only.

- `ArcMessage` serialises key reads through a mutex instead of allowing
  concurrent reads of one message handle. ecCodes handles are not thread-safe
  (`ENABLE_ECCODES_THREADS` locks the shared context, not per-handle access),
  so the upstream `Sync` claim was a data race; sharing still works, reads on
  one message are just no longer parallel. Clone per thread for parallel reads.
- `CodesNearest` rebuilds the underlying search per `find_nearest()` call, so
  repeated queries on one message lose the C-side geometry cache. (Keeping it
  alive would hold the lock and deadlock `read_key` on the same message.)
- `CodesError::LibcNonZero` is never produced; the file constructors go
  through `std::fs`, so those failures surface as `FileHandlingInterrupted`.
