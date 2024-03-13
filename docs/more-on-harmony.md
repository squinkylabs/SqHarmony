# More on Harmony

## Sharps or flats

 C Major has no sharps or flats in it, but the other major keys do, and so their relative modes do, too. Any diatonic key may be notated with all sharps or all flats. By far the most common thing is to notate them with the minimal number of accidentals.

https://www.britannica.com/art/key-signature

So, the major keys are usually notated like so:

* C major - no sharps or flats.
* G major - one sharp (F#).
* D major - two sharps (F#, C#).
* A major - three sharps (F#, C#, G#).

* F sharp major - six sharps (F#, C#, G#, D#, A#, E#).

* D flat major - five flats (B-, E-, A-, D-, G-)

## Analysis of quantizers

Say you wanted to get the "parallel third" effect common in many "classic rock" songs. For this effect we want to generate a new note that is "a third" above the note we provide. Now of course in normal Major and Minor scales some thirds are minor thirds, and some are major thirds. In A minor, the thirds above A is C, a minor third. But the third above C is E, a major thirds. If you try to harmonize your note by adding a note a fixed minor or major third up, you will not get the same effect.

You might think that most quantizers could be tricked into doing what is desired. Specifically, you might think that you could transpose up by a minor or major third, and then quantize the result to be in the original scale.

Let's assume our input is already quantized to a chromatic scale - say it's they output of the MIDI CV module. So for notes already in the quantizer's scale it will output the note unaltered. For notes not in the scale, it will round them to the nearest not in the scale. If it's a tie the quantizer will probably always round up or always round down.

So, let's set the quantizer up to A minor, and then try to move everything up a third, using transposition and quantizing.

### Case 1: A minor, transpose up a minor third, round ties downward

| Input | desired 3rd up | +3 semitones | quantized |
| --- | --- | --- | --- |
| A: 1 | C (minor) | C | C :heavy_check_mark: |
| B: 2 | D (minor) | D | D :heavy_check_mark: |
| C: 3 | E (major) | E flat | D ❌ |
| D: 4 | F (minor) | F | F :heavy_check_mark: |
| E: 5 | G (minor) | G | G :heavy_check_mark: |
| F: 6 | A (major) | A flat | G ❌ |
| G: 7 | B (major) | B flat | A ❌ |

### Case 2: A minor, transpose up a major third, round ties downward

| Input | desired 3rd up | +4 semitones | quantized |
| --- | --- | --- | --- |
| A: 1 | C (minor) | C sharp | D |
| B: 2 | D (minor) | D sharp | E |
| C: 3 | E (major) | E | D sharp ❌ |
| D: 4 | F (minor) | F sharp | F :heavy_check_mark: |
| E: 5 | G (minor) | G sharp| G :heavy_check_mark: |
| F: 6 | A (major) | A  | A :heavy_check_mark: |
| G: 7 | B (major) | B  | B :heavy_check_mark: |

### Case 3: A minor, transpose up a minor third, round ties upward

| Input | desired 3rd up | +3 semitones | quantized |
| --- | --- | --- | --- |
| A: 1 | C (minor) | C | C :heavy_check_mark: |
| B: 2 | D (minor) | D | D :heavy_check_mark: |
| C: 3 | E (major) | E flat | E :heavy_check_mark: |
| D: 4 | F (minor) | F | F :heavy_check_mark: |
| E: 5 | G (minor) | G | G :heavy_check_mark: |
| F: 6 | A (major) | A flat | A :heavy_check_mark: |
| G: 7 | B (major) | B flat | B :heavy_check_mark: |