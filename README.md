# Code Division Multiple Access (CDMA)

## README TODO

- [x] Defining CDMA
- [ ] Setting parameters for testing; define success for the project
- [ ] Track benchmarks and progress
- [ ] DOD

## PROJECT TODO

- [ ] Set up testing harness (including test 'users' and their messages. Whatever the 'message' it will need to be transmitted into raw binary before it's processed)
- [ ] Assign PN (psuedo-random noise) to each 'user'
- [ ] Utilizing each PN, 'spread' each message
- [ ] Combine all spread messages
- [ ] Implement encryption? AES of some kind, elliptical is probably the best one (used in modern phones now)

## Steps

- Initially, each 'user' (which would be a single device, like a phone) is assigned a unique identifier code
- This code is utilized to 'spread' the users message across a spectrum
- Multiple messages are combined together and transmitted together
- When the message reaches the intended target, the user 'decodes' the message to obtain the message intended for them from the other messages included

## Encoding

When transmitting a message through a cellular network, the CDMA encodes each bit of the message.

- 1 -> +1
- 0 -> -1

0 is reserved to mean 'no transmission' by the network.

## NETWORK SIMULATION

This project simulates a network of devices where each device can send and receive messages.

This simulation is run off a 'tick' system, where each tick performs a discrete number of steps before advancing.

## Phases of the Project

### Phase 1: CDMA

TODO I need to write up documentation on what it took to transcribe information to the format that CDMA uses. Include how you realized that the 0 is used to signify that no information is being sent.

### Phase 2: Device to Device

I wanted to start off with the most trivial case I could think of that fit the parameters of the project. This smallest trivial case was to define two devices and a single tower to send messages through. The goal of this phase was to create each device, have the tower assign them a spreading code, and to send a message (a single ASCII character) from one device to another.
