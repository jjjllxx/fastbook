# fastbook
A low-latency C++ matching engine and market data feed

To run the code:
1. Compile (in project folder)
``` bash
mkdir build
cd build
cmake -DTEST=on -DBENCH=on .. # Turn on TEST or BENCH is optional
make -j
```

2. Start Core Program (in `build` folder)
``` bash
./fb
```

3. Run Unit Test (in `build` folder)
``` bash
./fb_test
```

3. Run Benchmark (in `build` folder)
``` bash
./fb_bench
```