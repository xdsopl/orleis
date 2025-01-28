# O'RLE is! - The incredibly fast and compact run-length encoding tool

# About
`orleis` analyzes byte sequences and applies [run-length encoding](https://en.wikipedia.org/wiki/Run-length_encoding) (RLE) adaptively

# Encoding
```
./orleis e < uncoded.dat > encoded.dat
```

# Decoding
```
./orleis d < encoded.dat > decoded.dat
```

# Compare
```
ls -l uncoded.dat encoded.dat decoded.dat
diff -q -s uncoded.dat decoded.dat
```

