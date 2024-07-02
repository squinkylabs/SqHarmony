# Squinktronix Visualizer manual

## What it does

Visualizer is a simple module, and really does one thing. It looks at incoming pitch data, and tries to determine what chord that corresponds to.

Visualizer also displays the input in standard music notation, but that is really a secondary function of the module. The notation display and controls may be completely ignored if you wish.

When identifying chords, Visualizer doesn't care what octave a chord is in, or which channels the pitches are in. So, for example, E2, G7, C5 is still a C Major chord. Also, duplicates/doublings are ignored, so C, E, G, C is recognized as C Major, but C, E, G, A is not.

While some of the name of the chords may come more from the classical world, it's useful for lots of tonal music. Some people may call chords by different names, but usually the translation isn't too bad. An example: we call (G C E) "C Major, second inversion" - others might call it "C over G". But most people know it's the same thing.

## Notes for testers

It would definitely be a bug it a chord is mis-characterized. If it say something is a C Major it should be that. Also if an inversion is displayed it would be a definite bug to be fixed if the information is incorrect.

Some things about chord identification and spelling are more ambiguous, and subject to personal style. For example the chord C, E, G, B flat is often called "C7". But it is also called the dominant seventh. But if you find that the naming of some of these chords is correct, but strange to you, that may very well be something that should change.

The "transcription" of the input to standard music notation. Even though it's just "eye candy" it's supposed to be legal. So in any case that's remotely tricky, like clusters of notes, the resulting display should always be legal, and correspond to the pitches coming in. In a lot of cases it won't look nearly as good as you could expect a hand-written score to look. But it's a fixable bug if it's just plain wrong.

It is definitely possible to get notes and accidentals to pile up in certain keys and with certain combinations of input notes. I may be able to improve some of these, others maybe not. But worth noting.

In any case where the recognized chord is bad, or the score is bad, it's best if you can take a screen capture of the issue, and write down what the input pitches are.

## Using Visualizer

## Reference

### Chords recognized by Visualizer

(Examples below in C, but of course they are recognized at all roots)

- C Major : C, E, G
- C minor : C, E flat, G
- C Diminished : C, E flat, G flat
- C Augmented : C, E, G sharp
- C Sus2 : C, D, G
- C Sus4 : C, F, G

- C Seventh : C, E, G, B flat
- C Major Seventh: C, E, G, B
- C minor Seventh: C, E flat, G, B flat
- C mM Seventh: C, E flat, G, B

Ninth chords are recognized with the fifth present, or not:

- C Ninth : C, E, [G], B flat, D
- C MajorNinth : C, E, [G], B, D
- C minor Ninth : C, E flat, [G], B flat, D
- C mM Ninth : C, E flat, [G], B, D

### CV I/O

V/Oct: Polyphonic pitch input that follows the VCV standards. This is the main input.

[Gate]: polyphonic gate input. Does not need to be hooked up. If it's unconnected, than Visualizer will look at all the pitch channels in the input. If [Gate] is patched, then Visualizer will only look at pitch channels that have their gate active. This could be useful if you are looking at the pitches coming from the VCV MIDI CV module, for example.

PES Input, and PES output. Useful when used with other modules that support PES. Any signal coming into the PES input will control Visualizer's key signature. And Visualizers key signature will be sent to the PES output. You can find more on PES [here](./pes.md)

### Controls

Key Root, Diatonic Mode: this control only affect the music notation displayed at the top. They do not affect chord recognition at all. They do set the key signature for the transcription.

## Visualizer block diagram

![Visualizer block diagram](./visualizer-block-diagram.svg)
