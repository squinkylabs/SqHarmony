#pragma once

class VisualizerModule;

class ScoreChord : public app::LightWidget, public Dirty {
public:
    ScoreChord(VisualizerModule *) {
        ++_refCount;
    }
    ~ScoreChord() {
        --_refCount;
    }
    void draw(const DrawArgs &args) override;
    bool isDirty() const override {
        // SQINFO("Score::isDirty will ret %d", scoreIsDirty);
        return _scoreIsDirty;
    }

private:
    bool _scoreIsDirty = false;
    static int _refCount;

    std::pair<float, float> drawMusicNonNotes(const DrawArgs &args) const;
    // void drawChordNumbers(const DrawArgs &args, std::pair<float, float> keysigLayout) const;
    void drawNotes(const DrawArgs &args, std::pair<float, float> keysigLayout) const;
};

int ScoreChord::_refCount = 0;

inline void ScoreChord::draw(const DrawArgs &args) {
    nvgScissor(args.vg, RECT_ARGS(args.clipBox));
    const auto keysigLayout = drawMusicNonNotes(args);
    drawNotes(args, keysigLayout);
    //  drawChordNumbers(args, keysigLayout);
    _scoreIsDirty = false;

    Widget::draw(args);
}

inline std::pair<float, float> ScoreChord::drawMusicNonNotes(const DrawArgs &args) const {
    float keysigWidth = 0;
    float keysigEnd = 0;

    return std::make_pair(keysigWidth, keysigEnd);
}

inline void ScoreChord::drawNotes(const DrawArgs &args, std::pair<float, float> keysigLayout) const {
}