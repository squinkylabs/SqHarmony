# Squinktronix Mutator manual

## What it does

Mutator generates a collection of pitches, all output on a single polyphonic output. Then, every time the mutate in input is clocked, one or more of the pitches are changed.

Although mutator puts out a port full of control voltages, and these could be used for anything, the most straight forward use is one that treats that output as a series of pitches. There are a couple of reasons for this. One is that the outputs are always quantized to the notes in the selected scale. The other is that many of the "style" controls treat is that way, and might not make sense if mutator is used for some other purpose.

There are controls the determine how many channels are output (or how many pitches in the sequence). Controls that determine which outputs will be mutated. Controls the affect the "style" of the mutations.

## What it does in more detail

Mutator always output a single cable carrying a polyphonic CV. The number of channels in the output is set with the _Num Notes_ control, which ranges from 1 to 16. Although this CV can be used for anything, it is meant to actually carry a set of musical pitches; the number of pitches is of course _Num Notes_. Often this polyphonic CV is used to drive a sequencer or a switch. In this way the notes can be clocked out one at a time, to make a true series of notes. The patch in the TL;DR section does this.

Mutator will generate something new each time the _MUT_ CV goes from low to high. If the _MUT_ input is driven by an appropriate signal, the effect can be a repeating pitch sequence that changes every repetition. Again, see the TL;DR patch for an example.

When a new "pitch" is generated, Mutator pick a new pitch that is one or two scale steps above or below the current pitch. It uses the key signature you set to determine these notes. So the new notes are always quantized to the current scale. Note that many different scales are supported, including chromatic. Many of the scales are the standard 8 note diatonic modes, but there are plenty of others, including _chromatic_ that contains all 12 notes of the 12 note even tempered scale.

Each time Mutator changes pitches, it will pick the notes to change based on your settings. _How many notes/channels to change each time_ controls how many will change. Note that if you set this to zero, then all the notes will change each "mutation". The style when changing notes may be any of the following:

- Round robin, adjacent: each time the "next" note will change. If more than one note is changing, it will be the next notes that get selected.
- Round robin, distributed: the note to be changed advances

## TL;DR

## Style controls

## Other controls

## CV Outputs

## CV Inputs

## Block diagram
