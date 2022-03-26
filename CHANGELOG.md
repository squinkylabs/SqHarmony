# Changelog for Squinktronix plugins

## 2.0.1

### Harmony

Added all the diatonic modes, and all 12 roots. Please read manual for caveats about this feature.

Tweaked the panel layout to be slightly less bad.

Poly outputs (1-N). Indicators below the port will tell how many channels are patched.

Added controls for chord generation.

Made score drawing in Harmony use less GPU resources.

Added option for black notes on white paper in score section. Re-centered the score so it has less blank space at the top.

Fixed pitches being generated outside correct bounds.

### Arpeggiator

Added shuffle trigger input.

Added second CV In/Out.

Fixed the hold switch.

Fixed some bugs around deciding when to stop playing channels.

Fixed code bugs reported by staircrusher.

Fixed a typo in the Apreggiator module description.

Fixed bug where setting length back to zero did not bring back full input size.

Use a better shuffling algorithm, and better random numbers.

## 2.0.0

First test version.
