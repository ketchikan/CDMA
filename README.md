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
