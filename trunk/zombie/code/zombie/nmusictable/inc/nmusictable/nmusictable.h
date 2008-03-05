#ifndef N_MUSIC_TABLE_H
#define N_MUSIC_TABLE_H
//------------------------------------------------------------------------------
/**
    @file nmusictable.h
    @class nMusicTable

    @author Juan Jose Luna Espinosa

    @brief Encapsulation of music mixing information

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/ntimeserver.h"
#include "audio3/nsound3.h"
#include "util/nmaptabletypes.h"

class nstream;

//------------------------------------------------------------------------------
/// Fixed path of music
extern const char* musicPath;

/// Fixed path of music table
extern const char* musicTablePath;

/// NOH path of music table
extern const char* musicTableNOHPath;

//------------------------------------------------------------------------------
class nMusicTable: public nRoot
{
    public:

    #define MIN_EXIT_POINT_SEP 0.1f

    struct MoodInfo
    {
        // Mood name
        nString name;
        // Mood samples
        nArray<int> musicSamples;
        // Mood style
        nString style;
        // Mood style intensity
        int intensity;
    };

    struct StyleInfo
    {
        nString name;
        nArray<nString> moods;
    };

    struct MusicSampleInfo
    {
        // Sample file name without path
        nString name;
        // Sample
        nRef<nSound3> sample;
        // Interval of fixed-spaced exit points in seconds
        float exitPointInterval;
        // User exit points in seconds
        nArray<float>* exitPoints;
    };

    struct TransitionSampleInfo
    {
        // Sample file name without path
        nString name;
        // Sample
        nRef<nSound3> sample;
    };

    struct TransitionInfo
    {
        TransitionInfo()
        {
        }

        ~TransitionInfo()
        {
        }

        // Sample indexes
        int fromSample;
        int toSample;
        int transitionSample;  // if -1, transition is a single crossfade

        float fadeOutDuration; // seconds
        float fadeInDuration; // seconds
    };

    enum PlaybackState
    {
        Stopped,    // No music is playing
        Playing,    // Playing only music sample
        FadingOut,  // Fading out a music sample and mixing with a transition part
        PlayingTransition, // Playing only transition
        FadingIn,   // Fading out the music part and fading in a new music sample.
    };

    // Constructor
    nMusicTable();

    // Destructor
    virtual ~nMusicTable();

    /// Add a music sample
    void AddMusicSample(const char *);

    /// Add a transition sample to use between music samples
    void AddTransitionSample(const char *);

    /// Add a transition from a sample to another one
    void AddTransition(const char *, const char *, const char *, float, float);

    /// Set time interval for regular exit points in a music sample
    void SetMusicSampleExitPointsInterval(const char *, float);

    /// Add a manual exit point in a music sample
    void AddMusicSampleExitPoint(const char *, float);

    /// Play a music part, or queue a part for playing after the current one ends
    bool PlayMusicPartByIndex(int, bool);

    /// Stop music immediatly
    void StopMusic();

    /// Play a music part independently
    void PlayMusicStinger(const char *, float);

    /// Play a music part independently
    void PlayMusicStingerByIndex(int, float);

    /// Add a mood
    void AddMood(const char*);

    /// Add a mood's music sample
    void AddMoodMusicSample(const char*, const char *);

    /// Set current mood
    void SetCurrentMood(const char*, bool);

    /// Add a style
    void AddStyle(const char*);

    /// Add a mood to a style
    void AddMoodToStyle(const char*, const char *, int);

    /// Set current style and intensity
    void SetCurrentStyle(const char *, int, bool);

	/// Set current style (by index) and intensity
    void SetCurrentStyleByIndex(int, int, bool);

    /// Set current style intensity
    void SetCurrentStyleIntensity(int, bool);

    /// Network command to play music part
    void SendPlayMusicPart(const char *, nObject *) const;

    /// Network command to stop music
    void SendStopMusic(nObject *) const;

    /// Network command to play a music part
    void SendPlayMusicStinger(const char *, float, nObject *) const;

    /// Network command to set some mood
    void SendMoodToPlay(const char*, nObject *, bool) const;

    /// Network command to set current style and intensity
    void SendSetCurrentStyle(const char *, int, nObject *, bool);

    /// Load all samples
    bool LoadAllSamples(void);

    /// Update frame
    void Update( float time );

    /// Persist info
    bool SaveCmds (nPersistServer* ps);

    /// Called when starting a transition
    void OnTransitionStart();

    /// Called when finishing a transition
    void OnTransitionEnd();

	/// Receive network command to play music part
    void ReceivePlayMusicPart( nstream* data );

    /// Receive network command to stop music
    void ReceiveStopMusic( nstream* data );

	/// Receive network command to play music part
    void ReceivePlayMusicStinger( nstream* data );
	
	/// Receive network command to set the mood
    void ReceiveMoodToPlay( nstream* data );

	/// Receive network command to set a music style and intensity
    void ReceiveSetCurrentStyle( nstream* data );

    /// Get a sample index by name
    int GetMusicSampleIndex(const char * fileName) const;

    /// Get a transition sample index by name
    int GetTransitionSampleIndex(const char * fileName) const;

    /// Get number of music samples
    int GetNumberOfMusicSamples () const;

    /// Get music sample info by index
    const MusicSampleInfo& GetMusicSampleInfo( int index ) const;

    /// Get the name of a music sample by index
    const nString& GetMusicSampleName( int index ) const;

    /// Get number of moods
    int GetNumberOfMoods () const;

    /// Get mood info by index
    const MoodInfo& GetMoodInfo( int index ) const;

    /// Get the name of a mood by index
    const nString& GetMoodName( int index ) const;

    /// Get number of styles
    int GetNumberOfStyles () const;

	/// Get a style by index
	const char * GetStyleName( int styleIndex );

protected:

private:

    /// Reset all data
    void Reset();

    /// Get first transition associated with start and destination sample
    int GetTransition( int fromSample, int toSample );

    /// Get first exit point next to the given time
    float GetNextExitPoint( int sample, float time );

    /// Get a random manual exit point
    float GetRandomExitPoint( int sample );

    /// Get a mood by name
    int FindMood(const char* name) const;

    /// Get a mood's random music sample
    int GetMoodRandomSample(int moodIndex);

    /// Set current mood by index
    void SetCurrentMoodByIndex( int moodIndex, bool loop );

	/// Get a style index by name
	int FindStyle( const char * styleName );

    // Music info
    nArray<MusicSampleInfo> musicSamples;
    nArray<TransitionSampleInfo> transitionSamples;
    nArray<TransitionInfo> transitions;

    // Samples loaded flag
    bool samplesLoaded;

    // Music playback info
    // play state
    PlaybackState playbackState;
    //sample being played
    int currentSample;
    // incoming transition (-1 = none)
    int nextTransition;
    // transition sample to play (-1 = none)
    int nextTransitionSample;
    // sample to play after transition (-1 = none)
    int nextSample;
    // queued sample to play after next transition (-1 = none)
    int queuedSample;
    // play next music sample at random entry point flag
    bool playNextSampleAtRandomEntryPoint;
    // Position of last music sample played
    float lastMusicSamplePlayPosition;
    // Time when starts the next transition
    float transitionTime;
    // Time when finishes current state (sample, transition sample or fade)
    float finishTime;
    // Time of beginning of current music sample
    float trackBeginT;
    // Elapsed time of current sample or transition sample
    float trackElapsedT;
    // Fade out and fade in begin time
    float fadeOutBegin;
    float fadeInBegin;
    // Duration (inverse) of fade in and out
    float invFadeOutDuration;
    float invFadeInDuration;
    // Total transition duration
    float transitionDuration;
    // Sample that has terminated and is fading out to prevent clicks
    int stopFadingOutSample;

    // Mood info
    nArray<MoodInfo> moodInfo;

    // Style info 
    nMapTableTypes<StyleInfo>::NString styleInfo;

    /// Current mood
    int currentMood;

    /// Next mood to be set
    int nextMood;

    /// Last music part played
    int lastPlayedMusicSample;

    /// If a part is the last one, can be looped or not...
    bool looped;
};
//------------------------------------------------------------------------------
#endif

