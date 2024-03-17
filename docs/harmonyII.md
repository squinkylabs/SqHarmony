# Squinktronix Harmony II manual

Harmony II is a chord generator with a trick - it can generate the correct chords for they key that you set. For example, in the key of C Major, the triads in the key are C Major, D minor, E minor, F Major, G Major, A minor, and B diminished.

Much common music uses some form of diatonic harmony. For some typical examples, consider the chord progressions for some common songs. For example, Dylan's ["All Along The Watchtower"](https://www.youtube.com/watch?v=bT7Hj-ea0VE). The chords are A minor, G major, F major. Those are the diatonic triads on 8, 7, 6 in A Minor. Or [every teen-age car-crash song](https://www.youtube.com/watch?v=bh4se9YMV3A) is G major, E minor, C major and D major. Those are the diatonic triads on 8, 6, 4, 5 in G major.

Many chord generators tend to generate all the chord at different patches, like C Major, D Major, etc... Many chord generators are unable to generate the "correct" chords at different pitches. For an in-depth look at this, see [analysis of chord transposition](./more-on-harmony.md/#analysis-of-quantizers).

Harmony II also has a bunch of non-diatonic scales in it. So it isn't limited to diatonic harmony at all.

For more on scales and modes and such, look at all of [more or harmony](./more-on-harmony.md). If you don't know any of this stuff, and are interested that has some good links in some n00b info.

Aside from generating "in key" triads, like the examples above, you can also use harmony to generate a single parallel interval. This is usually done with a third, or inverted third. Some common rock examples are [The Beatles And Your Bird Can Sing](https://www.youtube.com/watch?v=sOUlbredoUM), the intro to [Bon Jovi's Wanted Dead or Alive](https://www.youtube.com/watch?v=SRvCvsRp5ho), [The Beatles Blackbird](https://www.youtube.com/watch?v=Man4Xw8Xypo), [Van Morrison's Brown Eyed Girl](https://www.youtube.com/watch?v=kqXSBe-qMGo)

If you are interested, the links above take you to YouTube videos featuring the songs in question.

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

**CVI** is the main CV input. It is monophonic and follows the VCV standard for pitch.

**CVO** is the output. It is polyphonic and follows the VCV standard for pitch. 

**Key** will set the Key signature root. It is monophonic, and also follows the VCV pitch standard.

**Mode** will set the Key signature mode. It is monophonic, and puts a different scale every 1/12 volt. So it's similar to the VCV pitch standard, but not really.

**XP** transpose amount. It is polyphonic, does not follow the VCV pitch standard. It is one volt per octave, but the octave is evenly divided by the number of notes in the scale. So a transpose of one degree is 1/8 in a diatonic scale, not the 1/12 for a semitone.

## CV output

(text coming soon)

## Context menu things
