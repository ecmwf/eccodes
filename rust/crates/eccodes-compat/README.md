# eccodes-compat

The [ScaleWeather `eccodes`](https://github.com/ScaleWeather/eccodes) 0.15 API,
reimplemented as a thin layer over the official [`eccodes`](../eccodes) crate.

For existing users of the unofficial bindings, migration is one `Cargo.toml`
line, with no source changes:

```toml
[dependencies]
eccodes = { package = "eccodes-compat", version = "0.1" }
```

This crate is frozen at the 0.15 surface and maintained for migration only.
New code should depend on the official `eccodes` crate directly;
[MIGRATION.md](MIGRATION.md) maps every 0.15 call to its official equivalent.

## Known behavioural differences

Results are identical everywhere; the remaining differences are in
concurrency and performance only. `RefMessage` reads lock-free and its
`CodesNearest` keeps the C-side geometry cache across calls, as upstream.

- `ArcMessage` and `BufMessage` serialise key reads through a mutex instead
  of allowing concurrent reads of one message handle. ecCodes handles are not
  thread-safe (`ENABLE_ECCODES_THREADS` locks the shared context, not
  per-handle access), so the upstream `Sync` claim was a data race; sharing
  still works, reads on one message are just no longer parallel. Clone per
  thread for parallel reads.
- On `ArcMessage`/`BufMessage`, `CodesNearest` rebuilds the underlying search
  per `find_nearest()` call. (Keeping it alive would hold the lock and
  deadlock `read_key` on the same message.)
- `CodesError::LibcNonZero` is never produced. Upstream only emits it when
  `fdopen`/`fmemopen` fail after a successful open (fd exhaustion or OOM);
  every realistic failure surfaces as `FileHandlingInterrupted` in both.
