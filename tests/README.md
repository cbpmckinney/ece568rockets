# Rocket Tests

This directory contains Arduino code for:

- **RF (Radio Frequency)**: Signal transmission (ROCKET) and reception (GROUND STATION).
- **GPS**: Location data acquisition and parsing.

## Notes:
- Because strings are involved, malloc is used in the test functions. For our final product, we will not be using malloc since it is bad process.
- For the GPS, refer to the setup() function for common process using the quick fetch func.