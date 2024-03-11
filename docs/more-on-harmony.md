# More on Harmony

## Analysis of quantizers

Say you wanted to get the "parallel third" effect common in many "classic rock" songs. For this effect we want to generate a new note that is "a third" above the note we provide. Now of course in normal Major and Minor scales some thirds are minor thirds, and some are major thirds. In A minor, the thirds above A is C, a minor third. But the third above C is E, a major thirds. If you try to harmonize your note by adding a note a fixed minor or major third up, you will not get the same effect.

You might think that most quantizers could be tricked into doing what is desired. Specifically, you might think that you could transpose up by a minor or major third, and then quantize the result to be in the original scale.

Let's assume our input is already quantized to a chromatic scale - say it's they output of the MIDI CV module. So for notes already in the quantizer's scale it will output the note unaltered. For notes not in the scale, it will round them to the nearest not in the scale. If it's a tie the quantizer will probably always round up or always round down.

So, let's set the quantizer up to A minor, and then try to move everything up a third, using transposition and quantizing.

### Case 1: transpose up a minor third, round ties downward

:negative_squared_cross_mark:

❌

| Input | desired 3rd up | +3 semitones | quantized |
| --- | --- | --- | --- |
| A | C | C | C :heavy_check_mark: |