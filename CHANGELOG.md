# Changelog for Squinktronix plugins

## 2.2.2

Harmony stuff other than score drawing:

* Re-specify the ranges. Now "narrow" is very similar to old "don't care", and "don't care" is now even wider.
* Enhanced rules for no-notes in common.
* Changed defaults to "narrow mode".
* Added rules for leading tone in chords, and for chords whose root is the leading tone.

Harmony score drawing:

* Use standard figured bass chord notation.
* Adjust spacing of score layout to make it more readable.

## 2.2.1

Harmony:

* New default values for Harmony preference (inversions, and centered).
* Fixed memory leak in Harmony module.
* Sped up evaluation of harmony rules.
* Made initial fake score in Harmony Stay up. (should go back?)

New Harmony rules:

* Forbid doubling of leading tones.
* Chord of the leading tone prevented in root position.
* Forbid melodic jumps in the soprano voice of more than five semitones.
* Discourage motion in Bass, Tenor, and Alto. But less restrictive than the Soprano rule.
* Implement the piston rule for V-VI progression.

Arpeggiator:

* Fixed bug where notes stuck when reducing polyphony with mono gates.

Phase Patterns:

* Added "under construction" to the panel. For more on that, look at the manual.
* Uses correct font on the panel.
* Changed the name from Phasepatterns to Phase Patterns.

Miscellaneous:

* Fixed broken links to CHANGELOG.
* Fixed spurious warnings on MacOS and Linux builds.

## 2.2.0

New release for test.

New test plugin: Phase Patterns.

Fixed spelling error in module slug.

## 2.1.0

Finished beta 1 testing

## 2.0.5

Clock+Gate delay for Arpeggiator.

## 2.0.4

Arpeggiator pitch droop bug fixed.

## 2.0.3

Added X-pose and trigger delay for Harmony.

## 2.0.2

### Harmony

Added trigger input for Harmony.

Added repetition avoidance.

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
