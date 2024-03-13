# Squinktronix Harmony II manual

Harmony II is a chord generator with a trick - it can generate the correct chords for they key that you set. For example, in the key of C Major, the triads in the key are C Major, D minor, E minor, F Major, G Major, A minor, and B diminished.

Much common music uses some form of diatonic harmony. For some typical examples, consider the chord progressions for some common songs. For example, Dylan's "All Along The Watchtower". The chords are A minor, G major, F major. Those are the diatonic triads on 8, 7, 6 in A Minor. Or every teen-age car-crash song is G major, E minor, C major and D major. Those are the diatonic triads on 8, 6, 4, 5 in G major.

Many chord generators tend to generate all the chord at different patches, like C Major, D Major, etc... Many chord generators are unable to generate the "correct" chords at different pitches. For an in-depth look at this, see [analysis of chord transposition](./more-on-harmony.md/#analysis-of-quantizers).

Harmony II also has a bunch of non-diatonic scales in it. So it isn't limited to diatonic harmony at all.

For more on scales and modes and such, look at all of [more or harmony](./more-on-harmony.md)

## What it does

At the simplest level, Harmony II will take a monophonic CV input, and output a polyphonic CV containing a chord. All of the controls, CVs, etc. will control exactly what chord comes out.

## Preset

(text coming soon)

## Panel controls, per transposer

**Enable**: there are six transposers in Harmony II. They are only active if enabled. If you want to output a three not chord, for example, enable three of them.

**Transpose Degrees**: This is the main control for each transposer. This sets by how many scale degrees each transposer will transpose. So, to transpose by a thirds, set this to +2 (since a third is up two scale degrees from the root).

**Transpose Octave**: Each transposer will transpose by the number of octaves entered, plus the number of degrees. So, in a diatonic scale, one octave and four degrees will transpose up an  octave and a fifth.

## Global panel controls

**Key signature root**: Where the scale starts. For example, if you want to be in C, set this to C.

**Key signature mode**: What are the notes in the scale.


## CV inputs

(text coming soon)

## CV output

(text coming soon)

## Context menu things
