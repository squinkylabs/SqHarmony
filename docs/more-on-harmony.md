# More on Harmony

Some ramblings on things that are useful when trying to use some Squinktronix module.

## Sharps or flats

 C Major has no sharps or flats in it, but the other major keys do, and so their relative modes do, too. Any diatonic key may be notated with all sharps or all flats. The most common thing is to notate them with the minimal number of accidentals.

https://www.britannica.com/art/key-signature

So, the major keys are usually notated like so:

* C major - no sharps or flats.
* G major - one sharp (F#).
* D major - two sharps (F#, C#).
* A major - three sharps (F#, C#, G#).

* F sharp major - six sharps (F#, C#, G#, D#, A#, E#).

* D flat major - five flats (B-, E-, A-, D-, G-)

Of course there are many, many reasons a composer might choose different ways to notate something. Not only minimizing the number of accidentals in the key signature, but also issues of local harmony, what instrument is being written for, personal preference, etc.

This is only a small issue in Harmony II. The user preference for sharps or flats only affects how the key signature on the front panels looks.

### Are B flat and A sharp the same

Usually yes, sometimes no?

## Modes

As people have remarked, it's very difficult to get your music to sound like it is in a particular mode. After all, you can make all seven modes from the notes of C Major. So what happens if you play random white keys on a keyboard? If anything, it will sound like you are in C Major. Possibly A minor. Why? Well, hard to say exactly, but for one reason or another your ear tends to pull everything into a familiar key. So if you want to make music in a certain diatonic mode that isn't Ionian (major) or Aeolian (minor), it can be difficult. It can often sound like the music is really in the realtive major of the mode you are trying to usel

Some common ways to "establish a key/mode" are:

1. use a pedal tone at the root to establish the root.
2. emphasize the characteristic tones in the mode.
3. use a really prominent melody that implies the mode.
4. accompany the mode with chords that don't imply any particular scale.

1 - use a pedal tone is an obvious one. I the simplest implementation, just drone the root prominently in the bass register. If you want to establish the key as E Phrygian, drone a low E and use all the white keys.

3 - In this song, [Samba Pa Ti](https://www.youtube.com/watch?v=timZoOs9ozo), by Carlos Santana, the melody is very distinctive, and if you try to play it, or play along with it, it's quite obvious it's in a Mixolydian mode. Mixolydian happens to be one of the more common modes in pop songs. Most ppl think of it as Major with a flat 7th.

## Analysis of quantizers

Say you wanted to get the "parallel third" effect common in many "classic rock" songs. For this effect we want to generate a new note that is "a third" above the note we provide. Now of course in normal Major and Minor scales some thirds are minor thirds, and some are major thirds. In A minor, the thirds above A is C, a minor third. But the third above C is E, a major third. If you try to harmonize your note by adding a note a fixed minor or major third up, you will not get the same effect.

You might think that most quantizers could be tricked into doing what is desired. Specifically, you might think that you could transpose up by a minor or major third, and then quantize the result to be in the original scale. But we will see that often doesn't work.

Let's assume our input is already quantized to a chromatic scale. And lets say say it's the output of the MIDI CV module. So for notes already in the quantizer's scale it will output the note unaltered. For notes not in the scale, it will round them to the nearest note in the scale. If it's a tie the quantizer will probably always round up or always round down.

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

### Case 4: A minor, transpose up a major third, round ties upward

(tbd)

### Case 5: C major, transpose up a minor third, round ties downward

(tbd)

### Case 6: C major

(tbd)

### Case 7: C major

(tbd)

### Case 8: C major