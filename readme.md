## Compiling a static library and linking to example program:
./clr.sh

# Configuration examples
## tracing to opened tcp port
export TRACEMETHOD=0
export TCP_HOST=8.8.8.8
export TCP_PORT=80

## tracing to a file
export TRACEMETHOD=1
export TRACEFILE=output.txt

## tracing to serial port
export TRACEMETHOD=2
export SERIALPORT=/dev/ttyS0

