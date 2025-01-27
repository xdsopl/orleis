# O'RLE is! - The incredibly fast and compact run-length encoding tool

# Encoding
`./orleis e < uncoded.dat > encoded.dat`

# Decoding
`./orleis d < encoded.dat > decoded.dat`

# Compare
`du -h uncoded.dat encoded.dat decoded.dat`
`diff -q -s uncoded.dat decoded.dat`

