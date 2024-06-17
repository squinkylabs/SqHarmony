#pragma once

class VisualizerModule;

class ScoreChord : public app::LightWidget, public Dirty {
public:
    ScoreChord(VisualizerModule *) {
        
    }
     bool isDirty() const override {
        // SQINFO("Score::isDirty will ret %d", scoreIsDirty);
        return _scoreIsDirty;
    }
private:
    bool _scoreIsDirty = false;

};