# Code Division Multiple Access (CDMA)

This project is a hands-on implementation of Code Division Multiple Access (CDMA), built in C++ and tested with the Google Test framework. CDMA is the technique used in 2G and 3G cellular networks to allow multiple users to share the same radio frequency, increasing the number of users each cell in a network could serve.

It began as an implementation of CDMA using a Hadamard matrix, whose rows serve as spreading codes, and a check that an input message (or multiple simultaneous messages) could be decoded accurately from a signal that combines other messages.

It grew in a few steps:

1. **Spreading and despreading**: These are organized into a `CDMA` class, which defines the methods devices and towers use to send and receive messages.
2. **Messages became frames**: Rather than sending individual bytes, I sent messages in fixed-size frames. Each frame has a small header followed by bytes of the message. This allows messages to arrive out of order and to be arbitrarily long. Messages are split into frames in each `Device` and recombined when they are received.
3. **Devices, towers, and a network**: Each `Device` takes on the abstract role of any device that could connect to a cellular network, such as a cell phone, laptop, or tablet. Devices connect to a `Tower`, and all towers belong to a single `Network`, which assigns spreading codes to devices and transfers frames between towers. This simplified model of a cellular network made it quicker to build while still giving a reasonable demonstration of CDMA without getting too deep into the weeds.
4. **A tick-based loop**: Taking inspiration from Minecraft, I run the simulation as a 'tick-based' system. Each tick, devices are prompted to push to and drain from their queues, while towers combine the signals they receive and forward the decoded frames to their destinations, passing them between towers via the `Network`.
5. **A Google Test suite**: Tests ensure that messages arrive correctly, including when several devices send messages at the same time.

In the end, I had a simplified cellular network simulation implementing one of the brilliant pieces of math that allowed more people to stay connected using the same resources as previous generations.

## Getting Started

### Requirements

- CMake 3.14 or newer
- A C++17 compiler: GCC or Clang (MinGW on Windows). MSVC is not supported, because `Network.cpp` uses the GCC/Clang builtin `__builtin_ctzll`.
- An internet connection the first time you build (see below for information on Google Test)

### Build and run

The quickest way on Mac, Linux, or Windows with MinGW:

```bash
./run.sh
```

This creates the `build/` folder, configures and compiles the project, then runs `./CDMA`. To do the same by hand:

```bash
cmake -S . -B build
cmake --build build
./build/CDMA
```

Running `CDMA` with no arguments runs every test, then a demo simulation. You can also choose one:

| Command                                               | What it does                                           |
| ----------------------------------------------------- | ------------------------------------------------------ |
| `./build/CDMA --tests`                                | Run only the tests                                     |
| `./build/CDMA --demo`                                 | Run only the demo simulation                           |
| `./build/CDMA --tests --gtest_filter='NetworkTest.*'` | Run a subset of the tests (any `--gtest_*` flag works) |
| `./build/CDMA --help`                                 | Show usage                                             |

If any test fails, the program exits with a non-zero code and skips the demo.

The same tests are also built as a standalone executable for `ctest`:

```bash
cd build && ctest --output-on-failure
```

### Where Google Test comes from

You don't need to install Google Test. `CMakeLists.txt` downloads it from GitHub (using CMake's `FetchContent`) the first time you configure the project, and unpacks it into `build/_deps/`. That's why the first build needs internet access; later builds reuse the downloaded copy. Deleting the `build/` folder removes it, and it will be downloaded again on the next build.

### Trying your own simulation

`runDemo()` in [src/main.cpp](src/main.cpp) is a step-by-step example you can copy and modify. The core of it is only a few lines:

```cpp
Network network;                      // declared first, so it outlives the devices
Tower &tower = network.addTower();

Device alice(1), bob(2);
alice.connect(tower);                 // the network assigns each device a spreading code
bob.connect(tower);

alice.sendMessage(2, "Hello!");       // or sendMessage({2, 3}, "...") for several recipients
network.runLoop();                    // run until every message has been sent and received

// bob.receivedMessages() now holds the message
```

### Project layout

- `src/cdma/`: the frame format, the Walsh matrix, and spreading/despreading
- `src/network/`: `Network`, `Tower`, `Device`, and message splitting/reassembly
- `src/main.cpp`: runs the tests and the demo
- `tests/`: the Google Test cases

### Simulation Simplifications & Limitations

- Everything runs in synchronized ticks, so Walsh codes stay orthogonal in _both_ directions. There is no noise, multipath or power control, and no soft capacity limit. I have genuinely no idea if this will still work if it's noisy.
- The whole network shares one pool of Walsh codes, so the device limit applies across all towers together. Real towers reuse codes and tell each other apart by PN offset. The limitation for this simulation is that we can only hold a total of 63 devices (because one of the Walsh codes is reserved by default).
- Tower-to-tower communication goes through the `Network` class, standing in for the wired backbone that connects towers in real networks.
- Each device sends at most one frame per tick, and each tower sends at most one frame to a given device per tick.
- Messages between two devices are limited to strings for this simulation, but in reality can take on any shape (such as HTTP requests, phone calls, and more).
- Currently, moving a device from one tower to another is not supported. A device connects to a single tower and will stay with it.

### Future Change Opportunities

- If I were to introduce noise into the simulation, I would likely include an implementation of Reed-Solomon or Hamming codes to help with error handling and recovery.
- I believe that UDP also holds techniques that we could utilize to request lost packets (frames) again, or to recover them via XOR data.
- I was originally interested in creating more realistic tower-to-tower handoffs of messages, which I could implement for a more realistic or robust simulation. I felt that it was unnecessary for these initial versions.
- I was interested in implementing device hand-offs to simulate devices traveling between cells. My original idea was to implement a grid-style system and use a k-means algorithm to connect to the closest tower.
- I was interested in implementing AES or end-to-end encryption. I was also interested in simulating the Tor network with layered encryption, but I may save that for a different project.

### Sources

- Charan Langton, [CDMA Tutorial](https://complextoreal.com/wp-content/uploads/2013/01/CDMA.pdf): worked examples of spreading, combining and despreading.
- Wikipedia: [Code-division multiple access](https://en.wikipedia.org/wiki/Code-division_multiple_access), [Spread spectrum](https://en.wikipedia.org/wiki/Spread_spectrum)
- Wikipedia: [Hadamard matrix](https://en.wikipedia.org/wiki/Hadamard_matrix) (includes Sylvester's construction) and [Walsh matrix](https://en.wikipedia.org/wiki/Walsh_matrix)
- MathWorks, [Discrete Walsh-Hadamard Transform](https://www.mathworks.com/help/signal/ug/discrete-walsh-hadamard-transform.html)
- [Google Test](https://github.com/google/googletest)
