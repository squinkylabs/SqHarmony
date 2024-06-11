
# Harmony II demo info

Video is [here](https://www.youtube.com/watch?v=aGEKo4fNvO4)

This is a very simple patch that shows some very basic usage of Harmony II. There is an organ patch playing Eminor, Dmajor, Cmajor, Dmajor, and there is a "lead" patch playing random notes.

In the organ part, Seq3 is just playing E,D,C,D over and over. Then Harmony II is turning the individual notes into triads, picking major or minor as determined by the key signature of E minor.

The lead part is even simpler. Random signals are quantized to the E minor pentatonic scale with Harmony II, then this feeds a subtractive synth path.

Many will recognize that playing random notes in E Minor Pentatonic over this constant chord progression is a staple for bad guitar players.

## More about the modules used

Impromptu CLOCKED is generating the main clock. A high pulse width is used to get a reasonably legato sound.The X1 output drives the lead synth, and the /8 output drives the chords.

### Organ part

Seq3 is used the generate the chord roots. This was done by directly inputting the correct voltages for the notes. There are of course plenty of sequencers where this could be entered as notes rather than numbers.

This then drives Harmony II set to generate a triad (0, +2, and +4). Harmony II is set to E Minor.

The polyphonic output of Harmony II drives Organ 3. This was used just because it's an easy way to make chords, and doesn't required a VCA and envelope generator.

Count Modula Signal Manifold is used to turn the clock into three polyphonic gates, to drive the gate input on Organ 3.

The organ then goes to Surge Rotary Speaker to make it sound better.

### Lead part

The X1 clock drives VCV Random to generate random pitches. This is then run through another instance of Harmony II, which is functioning as a simple quantizer, this time set to E Minor Pentatonic.

This then goes to Surge Modern VCO and Filter. These get a little LFO modulation from two VCV LFOs.

The clock is used to trigger a VCV envelope generator that controls the gain of a VCV VCA.

That is then run into Plateau reverb, and both voices are mixed together.

