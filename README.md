# shoddy compression

The name doesn't lie, it's pretty bad.

Primary goal was to have a very small decompressor that still has an OK ratio.
It also happened that the use case I developed this for needed an ULEB128 decoder anyways, so only the decompression function would count to code size.

Mostly uses `size_t` to get better code size in WASM.

## License

MIT
