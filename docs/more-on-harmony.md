# More on Harmony

Some ramblings on things that are useful when trying to use some Squinktronix modules. Mostly about diatonic scales. This is a work in progress, expect some updates.

## more info

There is a ton of information on line and in books on this stuff. Even the wikipedia articles are much better and more detailed than what you are reading now.

Here are some Wikipedia articles:

* [Theoretical key](https://en.wikipedia.org/wiki/Theoretical_key)
* [Diatonic and Chromatic](https://en.wikipedia.org/wiki/Diatonic_and_chromatic)
* [Major and Minor](https://en.wikipedia.org/wiki/Major_and_minor)
* [Mode](https://en.wikipedia.org/wiki/Mode_(music))
* [Enharmonic equivalence](https://en.wikipedia.org/wiki/Enharmonic_equivalence)

## Sharps or flats

 C Major has no sharps or flats in it, but the other major keys do, and so their relative modes do, too. Any diatonic key may be notated with all sharps or all flats. The most common thing is to notate them with the minimal number of accidentals.

https://www.britannica.com/art/key-signature

So, the major keys are usually notated like so:

* C major - no sharps or flats.
* G major - one sharp (F#).
* D major - two sharps (F#, C#).
* A major - three sharps (F#, C#, G#).
* E major - four sharps, (C#, D#, F#, G#)
* B major - five sharps (A#, C#, D#, F#, G#)
* F sharp major X - six sharps (F#, C#, G#, D#, A#, E#).
* F major - one flat (B-)
* B flat major = two flats (B-, E-)
* E flat major - three flats
* A flat major - four flats
* D flat major - five flats (B-, E-, A-, D-, G-)

Of course there are many, many reasons a composer might choose different ways to notate something. Not only minimizing the number of accidentals in the key signature, but also issues of local harmony, what instrument is being written for, personal preference, etc.

This is only a small issue in Harmony, and Harmony II. The user preference for sharps or flats only affects how the key signature on the front panels looks.

### Are B flat and A sharp the same

Usually yes, sometimes no? This is a very complex topic, so the usual caveats apply.

So, yes in a twelve tone equal tempered tuning, B flat and A sharp are _enharmonic equivalents_. They are the same. Also, on a piano keyboard there is only one black key in between A and B. On the other hand, many instruments can play notes in between the pitches. Violin is a classic example. Sometimes advanced players of these instruments will play these notes differently, to make them more in tune with the local context. But for most of us these notes are the same.

How about for an entire key? Well, that is another story! Using conventional note spellings, these scales are:

Notes of B flat major: B-, C, D, E-, F, G, A.
Notes of A sharp major: A#, B#, C##, D#, E#, F##, G#

There are a few reasons that most people would not call this key "A sharp" :

* B# and E# are difficult notes for most people to think about, as they are the same as C and F.
* There are two double sharps in that key. C## and F## are much more commonly called D an G.
* Standard music notion rarely has key signatures with double sharps.
* There are only two flats in B flat major, yet there are (at least) seven sharps in A sharp.

Bottom line is that for the pitches themselves it will depend on context whether you call a note "n sharp" vs. "n+1 flat". For the keys themselves there will usually be a preference for one or the other. In the case of B-/A#, if you tell a band "this piece is in B flat", they will find it much easier than if you say "this piece is in A sharp". If fact, most musicians would most likely translate "A sharp" to B flat in their heads.

## Modes

As people have remarked, it's very difficult to get your music to sound like it is in a particular mode. After all, you can make all seven modes from the notes of C Major. So what happens if you play random white keys on a keyboard? If anything, it will sound like you are in C Major. Possibly A minor. Why? Well, hard to say exactly, but for one reason or another your ear tends to pull everything into a familiar key. So if you want to make music in a certain diatonic mode that isn't Ionian (major) or Aeolian (minor), it can be difficult. It can often sound like the music is really in the realtive major of the mode you are trying to usel

Some common ways to "establish a key/mode" are:

1. use a pedal tone at the root to establish the root.
2. emphasize the characteristic tones in the mode.
3. use a really prominent melody that implies the mode.
4. accompany the mode with chords that don't imply any particular scale.

1 - use a pedal tone is an obvious one. I the simplest implementation, just drone the root prominently in the bass register. If you want to establish the key as E Phrygian, drone a low E and use all the white keys.

3 - In this song, [Samba Pa Ti](https://www.youtube.com/watch?v=timZoOs9ozo), by Carlos Santana, the melody is very distinctive, and if you try to play it, or play along with it, it's quite obvious it's in a Mixolydian mode. Mixolydian happens to be one of the more common modes in pop songs. Most ppl think of it as Major with a flat 7th.

4 - In jazz music, one well known technique to establish a mode is for the piano so smash out parallel fourths. This keeps the piano chords from suggesting a particular "key", and lets the modal notes played by other instruments establish their own scales. This kind of harmony is often called "modal harmony", "quartal harmony", or "non functional harmony". It is a deep topic!

## Some of the scales in Harmony II

The seven diatonic scales are discussed in many places. I don't have much to add to that.

We know that the Ionian (major) and Aeolian (natural minor) are very common. Some other modes are used less commonly, but are still used a lot.

Mixolydian mode is often thought of as "major, with a flat 7th". So it's pretty close to major, but that altered 7th can add a little interest without sounding too weird. The lowered 7th also, make the 7th less like a leading tone - it doesn't want to resolve up the the 8 (1) as much as the major 7th

Dorian is Natural minor with a raised 6th, up to the major 6. So you can use it like a regular natural minor, but again with an altered note.

Minor pentatonic. This is a natural minor, where the second and the sixth are not often played. this scale is very commonly used in blues music, and rock music. Many instrumental solos are constructed from the minor pentatonic. It's something to most pop musicians are very familiar with.

Whole tone. This scale is pretty strange sounding to most people. It is made up entirely of whole steps, with no half steps at all. It is not used a lot in pop music, although King Crimson is (in) famous for using it a lot. Like their song [Red](https://www.youtube.com/watch?v=X_pDwv3tpug).

## Analysis of quantizers

In particular looking at why you can't always get this to work with a "normal" quantizer.

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