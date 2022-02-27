#pragma once

class MidiNote;
class FloatNote;
class ScaleNote;
class Scale;

class NoteConvert {
public:
    static void m2f(FloatNote&, const MidiNote&);
    static void f2m(MidiNote&, const FloatNote&);
    static void s2m(MidiNote&, const Scale&, const ScaleNote&);
    static void m2s(ScaleNote&, const Scale&, const MidiNote&);
    static void f2s(ScaleNote&, const Scale&, const FloatNote&);
    static void s2f(FloatNote&, const Scale&, const ScaleNote&);
};