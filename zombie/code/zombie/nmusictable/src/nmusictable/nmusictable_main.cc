#include "precompiled/pchnmusictable.h"
//------------------------------------------------------------------------------
//  nmusictable_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "nmusictable/nmusictable.h"
#include "stdlib.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "audio3/naudioserver3.h"
#include "audio3/nsound3.h"
#include "kernel/nlogclass.h"
#include "entity/nentityobjectserver.h"

// includes required for network support
#include "nnetworkmanager/nnetworkmanager.h"
#include "util/nstream.h"
#include "rnsgameplay/nrnsentitymanager.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nMusicTable, "nroot");

//------------------------------------------------------------------------------
const char* musicPath = "wc:/export/music/";
const char* musicTablePath = "wc:/export/music/musictable.n2";
const char* musicTableNOHPath = "/usr/musictable";

NCREATELOGLEVEL ( musicLog, "Music", true, 0 ) 

//------------------------------------------------------------------------------
/**
*/
nMusicTable::nMusicTable():
    playbackState( Stopped ),
    currentMood(-1),
    nextMood(-1),
    lastPlayedMusicSample(-1),
    playNextSampleAtRandomEntryPoint(false),
    lastMusicSamplePlayPosition(0.0f),
    stopFadingOutSample(-1),
    samplesLoaded(false),
    looped(true)
{
    //empty
}
//------------------------------------------------------------------------------
/**
*/
nMusicTable::~nMusicTable()
{
    this->Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
nMusicTable::Reset()
{
    for (int i=0; i < this->musicSamples.Size(); i++)
    {
        if ( this->musicSamples[i].sample.isvalid() )
        {
            this->musicSamples[i].sample->Release();
        }
        n_delete( this->musicSamples[i].exitPoints );
    }
    for (int i=0; i < this->transitionSamples.Size(); i++)
    {
        if ( this->transitionSamples[i].sample.isvalid() )
        {
            this->transitionSamples[i].sample->Release();
        }
    }
    this->musicSamples.Reset();
    this->transitionSamples.Reset();
    this->transitions.Reset();
    this->moodInfo.Reset();
    
    
    StyleInfo * sInfo = 0;
    nString styleName;
    this->styleInfo.Begin();
    this->styleInfo.Next(styleName, sInfo);
    while ( sInfo )
    {
        this->styleInfo.Next(styleName, sInfo);
        this->styleInfo.Remove(styleName);
    }

    this->samplesLoaded = false;
}

//------------------------------------------------------------------------------
/**
    @brief Add a music sample
    @param fileName Resource sound file name without path

    If there was a sample already with that index, it is substituted
*/
void
nMusicTable::AddMusicSample(const char * fileName)
{
    nString filePath( "wc:export/music/" );
    filePath.Append( fileName );

    if ( this->GetMusicSampleIndex( fileName ) != -1 )
    {
        NLOG( musicLog, (0, "AddMusicSample: Sample '%s' already exists", filePath ) );
        return;
    }

    nString mangledFilePath = nFileServer2::Instance()->ManglePath(filePath);
    if ( !nFileServer2::Instance()->FileExists( mangledFilePath ) )
    {
        NLOG( musicLog, (0, "AddMusicSample: file '%s' doesn't exist", filePath.Get() ));
        return;
    }

    nSound3* sound = nAudioServer3::Instance()->NewSound();
    sound->SetFilename( mangledFilePath );
    sound->SetLooping( true );
    sound->SetAmbient( true );
    sound->SetStreaming( true );
    sound->SetCategory( nAudioServer3::Music );

    MusicSampleInfo& ms = this->musicSamples.At( this->musicSamples.Size() );
    ms.name = fileName;
    ms.sample.set( sound );
    ms.exitPoints = n_new( nArray<float> )(0, 16, 0.0f);
    ms.exitPointInterval = 1.0f;
}
//------------------------------------------------------------------------------
/**
    @brief Set interval of automatic fixed-interval exit points
    @param musicSample Sample to set interval to.
    @param interval The interval
*/
void
nMusicTable::SetMusicSampleExitPointsInterval(const char * sampleName, float interval)
{
    int musicSample = this->GetMusicSampleIndex( sampleName );
    if ( musicSample == -1 )
    {
        NLOG( musicLog, (0, "SetMusicSampleExitPointsInterval: Sample '%s' doesn't exist", sampleName ) );
        return;
    }

    this->musicSamples[ musicSample ].exitPointInterval = interval;
}

//------------------------------------------------------------------------------
/**
    @brief Add a user music exit point
    @param musicSample Sample to add the exit point
    @param time Exit point time counted from beggining of sample
*/
void
nMusicTable::AddMusicSampleExitPoint(const char * sampleName, float time)
{
    int musicSample = this->GetMusicSampleIndex( sampleName );
    if ( musicSample == -1 )
    {
        NLOG( musicLog, (0, "AddMusicSampleExitPoint: Sample '%s' doesn't exist", sampleName ) );
        return;
    }
    nArray<float>& ep = *this->musicSamples[ musicSample ].exitPoints;
    int i = 0;
    while( i < ep.Size() && ep[i] < time )
    {
        i++;
    }
    if ( ep.Size() == 0 || ( i < ep.Size() && abs( ep[i] - time ) < MIN_EXIT_POINT_SEP ) )
    {
        ep.Insert(i, time);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Add a transition sample
    @param pos Index of the transition sample
    @param fileName Resource sound file path

    If there was a sample already with that index, it is substituted
*/
void
nMusicTable::AddTransitionSample(const char * fileName)
{
    nString filePath = musicPath;
    filePath += fileName;

    if ( this->GetTransitionSampleIndex( fileName ) != -1 )
    {
        NLOG( musicLog, (0, "AddTransitionSample: Sample '%s' already exists", fileName ) );
        return;
    }

    if ( ! nFileServer2::Instance()->FileExists( filePath ) )
    {
        NLOG( musicLog, (0, "AddTransitionSample: file '%s' doesn't exist", fileName ) );
        return;
    }

    nSound3* sound = nAudioServer3::Instance()->NewSound();
    n_assert( sound );
    sound->SetFilename(filePath);
    sound->SetLooping( false );
    sound->SetAmbient( true );
    sound->SetStreaming( true );

    TransitionSampleInfo& ts = this->transitionSamples.At( this->transitionSamples.Size() );
    ts.name = fileName;
    ts.sample.set( sound );
}
//------------------------------------------------------------------------------
/**
    @brief Add a transition
    @param fromSample Initial music sample
    @param fromSample Destination music sample
    @param fromSample Transition sample used for transition ( can be -1 for no sample )
    @param fadeOutDuration Duration of fade-out of current sample
    @param fadeInDuration Duration of fade-in of incoming sample

    If transition sample is -1 the transition is a pure cross-fade and lasts max(fadeOutDuration, fadeInDuration)
*/
void
nMusicTable::AddTransition(const char * fromSampleName, const char * toSampleName, const char * transitionSampleName, float fadeOutDuration, float fadeInDuration)
{
    int fromSample = this->GetMusicSampleIndex( fromSampleName );
    int toSample = this->GetMusicSampleIndex( toSampleName );
    int transitionSample = -1;
    if ( *transitionSampleName )
    {
        transitionSample = this->GetTransitionSampleIndex( transitionSampleName );
        if ( transitionSample == -1 )
        {
            NLOG( musicLog, (0, "AddTransition (%s, %s, %s, %f, %f): transitionSample doesn't exist", fromSampleName, toSampleName, transitionSampleName, fadeInDuration, fadeOutDuration ) );
            return;
        }
    }

    fadeOutDuration = max( 0.0f, fadeOutDuration);
    fadeInDuration = max( 0.0f, fadeInDuration);

    if ( fromSample == -1 )
    {
        NLOG( musicLog, (0, "AddTransition (%s, %s, %s, %f, %f): fromSample doesn't exist", fromSampleName, toSampleName, transitionSampleName, fadeInDuration, fadeOutDuration ) );
        return;
    }
    if ( toSample == -1 )
    {
        NLOG( musicLog, (0, "AddTransition (%s, %s, %s, %f, %f): toSample doesn't exist", fromSampleName, toSampleName, transitionSampleName, fadeInDuration, fadeOutDuration ) );
        return;
    }

    int pos = 0;
    while ( pos < this->transitions.Size() && !( this->transitions[pos].fromSample == fromSample && this->transitions[pos].toSample == toSample ) )
    {
        pos++;
    }
    if ( pos < this->transitions.Size() )
    {
           pos = max( 0, this->transitions.Size()-1 );
    }

    TransitionInfo t;
    t.fromSample = fromSample;
    t.toSample = toSample;
    t.transitionSample = transitionSample;
    t.fadeOutDuration = fadeOutDuration;
    t.fadeInDuration = fadeInDuration;
    this->transitions.Insert( pos, t );

}
//------------------------------------------------------------------------------   
/**
    @brief Loads all music and transition samples
*/
bool
nMusicTable::LoadAllSamples()
{
    if ( this->samplesLoaded )
    {
        return true;
    }

    bool success = true;
    for (int i=0;i<musicSamples.Size();i++)
    {
        n_assert( musicSamples[i].sample );
        musicSamples[i].sample->SetNumTracks( 1 );
        success &= musicSamples[i].sample->Load();
    }
    for (int i=0;i<transitionSamples.Size();i++)
    {
        n_assert( transitionSamples[i].sample );
        transitionSamples[i].sample->SetNumTracks( 2 );
        success &= transitionSamples[i].sample->Load();
    }

    n_assert2( success, "Some music files failed to load, see resource log" );
    if ( success )
    {
        this->samplesLoaded = true;
    }
    return success;
}
//------------------------------------------------------------------------------   
/**
    @brief Update frame
    Makes the process needed at each frame, starting, stopping and setting volumes on samples during a transition
*/
void
nMusicTable::Update( float time )
{
    float fadeOutDur, fadeInDur;
    float v;

    switch ( this->playbackState )
    {
        case Stopped:    // No music is playing
            break;
        case Playing:    // Playing only music sample

            if ( this->nextTransition != -1 )
            {
                // Playing music sample and a transition is queued
                if ( time < this->transitionTime )
                {
                    // Update track elapsed time
                    this->trackElapsedT = time - this->trackBeginT;

                    // Still didn't reach transition begin
                    return;
                }

                fadeOutDur = this->transitions[ this->nextTransition ].fadeOutDuration;
                fadeInDur = this->transitions[ this->nextTransition ].fadeInDuration;

                // Passed transition beginning: transition begins here
                if ( this->nextTransitionSample != -1 )
                {
                    n_assert( this->transitionSamples[ this->nextTransitionSample ].sample );
                    if ( fadeOutDur > 0.0f )
                    {
                        // Starting fade out of current sample and fade in of transition sample
                        this->finishTime = this->transitionTime + fadeOutDur;
                        this->invFadeOutDuration = 1.0f / fadeOutDur;
                        this->fadeOutBegin = this->transitionTime;
                        this->playbackState = FadingOut;

                        this->transitionSamples[ this->nextTransitionSample ].sample->SetVolume( 1.0f );
                        this->transitionSamples[ this->nextTransitionSample ].sample->Update();
                        this->transitionSamples[ this->nextTransitionSample ].sample->Start();

                        NLOG( musicLog, (0, "Playing   part(%s) -> FadingOut tran(%s)", this->musicSamples[this->currentSample].name.Get(), this->transitionSamples[this->nextTransitionSample].name.Get() ) );
                    }
                    else
                    {
                        // Finished previous sample. Fading out previous sample to prevent clicks
                        this->stopFadingOutSample = this->currentSample;

                        // Starting transition sample
                        this->finishTime = this->transitionTime + transitionDuration - this->transitions[ this->nextTransition ].fadeInDuration;
                        this->fadeInBegin = this->finishTime;
                        this->playbackState = PlayingTransition;
                        this->transitionSamples[ this->nextTransitionSample ].sample->SetVolume( 1.0f );
                        this->transitionSamples[ this->nextTransitionSample ].sample->Update(); 
                        this->transitionSamples[ this->nextTransitionSample ].sample->Start();
                        NLOG( musicLog, (0, "Playing   part(%s) -> PlayTrans tran(%s)", this->musicSamples[this->currentSample].name.Get(), this->transitionSamples[this->nextTransitionSample].name.Get() ) );
                    }

                    this->OnTransitionStart();
                }
                else
                {
                    // Transition doesn't have transition sample
                    // Starting fade out of current sample and fade in of next sample
                    this->fadeOutBegin = this->transitionTime;
                    this->fadeInBegin = this->transitionTime;
                    fadeInDur = max( fadeOutDur, fadeInDur );
                    if ( fadeInDur > 0.0f )
                    {
                        // Can't do a transition from a sample to itself
                        n_assert( this->currentSample != this->nextSample );

                        this->invFadeInDuration = 1.0f / fadeInDur;
                        this->finishTime = this->transitionTime + max( fadeOutDur , fadeInDur );
                        this->playbackState = FadingIn;
                        n_assert( this->musicSamples[ this->nextSample ].sample );
                        this->musicSamples[ this->nextSample ].sample->SetVolume( 1.0f ); 
                        this->musicSamples[ this->nextSample ].sample->Update(); 

                        if ( this->playNextSampleAtRandomEntryPoint )
                        {
                            this->lastMusicSamplePlayPosition = this->GetRandomExitPoint(this->nextSample);
                            this->musicSamples[ this->nextSample ].sample->SetTrackPosition( this->musicSamples[ this->nextSample ].sample->GetLastBufferUsed(), this->lastMusicSamplePlayPosition );
                        }
                        else
                        {
                            this->playNextSampleAtRandomEntryPoint = true;
                            this->lastMusicSamplePlayPosition = 0.0f;
                        }

                        this->musicSamples[ this->nextSample ].sample->Start();

                        NLOG( musicLog, (0, "Playing   part(%s) -> FadingIn  part(%s)", this->musicSamples[this->currentSample].name.Get(), this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );

                        this->OnTransitionStart();
                    }
                    else
                    {
                        // Finished fading in

                        bool currentEqualsNext = false;
                        if ( this->currentSample == this->nextSample )
                        {
                            currentEqualsNext = true;
                        }

                        if ( ! currentEqualsNext )
                        {
                            n_assert( this->musicSamples[ this->currentSample ].sample );
                            this->musicSamples[ this->currentSample ].sample->Stop();
                        }
                        else
                        {
                            // Current is same same sample as next, so it loops
                            this->playNextSampleAtRandomEntryPoint = false;
                        }

                        // Next sample is now current sample
                        n_assert( this->musicSamples[ this->nextSample ].sample );
                        this->finishTime = this->transitionTime + this->musicSamples[ this->nextSample ].sample->GetDuration();
                        this->playbackState = Playing;
                        this->currentSample = this->nextSample;
            
                        this->musicSamples[ this->nextSample ].sample->SetVolume( 1.0f );
                        this->musicSamples[ this->nextSample ].sample->Update(); 


                        if ( this->playNextSampleAtRandomEntryPoint )
                        {
                            this->lastMusicSamplePlayPosition = this->GetRandomExitPoint(this->nextSample);

                            this->musicSamples[ this->nextSample ].sample->SetTrackPosition(
                                this->musicSamples[ this->nextSample ].sample->GetLastBufferUsed(), this->lastMusicSamplePlayPosition );

                            this->finishTime -= this->lastMusicSamplePlayPosition;
                            n_assert( this->finishTime > this->transitionTime );
                        }
                        else
                        {
                            this->playNextSampleAtRandomEntryPoint = true;
                        }
                        this->lastMusicSamplePlayPosition = 0.0f;

                        if ( ! currentEqualsNext )
                        {
                            this->musicSamples[ this->nextSample ].sample->Start();
                        }

                        NLOG( musicLog, (0, "Playing   part(%s) -> Playing   part(%s)", this->musicSamples[this->currentSample].name.Get(), this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );

                        this->nextSample = -1;
                        this->nextTransition = -1;
                        this->nextTransitionSample = -1;

                        this->OnTransitionStart();
                        this->OnTransitionEnd();
                    }
                }
            }
            else
            {
                // Playing music sample and no transition is queued
                if ( time < this->finishTime )
                {
                    // Update track elapsed time
                    this->trackElapsedT = time - this->trackBeginT;

                    return;
                }
                this->playbackState = Stopped;
                NLOG( musicLog, (0, "Playing    -> Stop") );

                this->OnTransitionEnd();
            }
        break;
        case FadingOut:  // Fading out a music sample and mixing with a transition part or next music sample
            if ( time < this->finishTime )
            {
                // Fade out of previous sample (and fade in of transition sample)
                v = ( time - this->fadeOutBegin ) * this->invFadeOutDuration;
                n_assert( this->musicSamples[ this->currentSample ].sample );
                this->musicSamples[ this->currentSample ].sample->SetVolume( 1.0f - v );
                this->musicSamples[ this->currentSample ].sample->Update(); 

                // Update track elapsed time
                this->trackElapsedT = time - this->trackBeginT;

                return;
            }

            // Finished fading out
            n_assert( this->musicSamples[ this->currentSample ].sample );
            this->musicSamples[ this->currentSample ].sample->Stop();

            time = transitionDuration - this->transitions[ this->nextTransition ].fadeOutDuration - this->transitions[ this->nextTransition ].fadeInDuration;

            if ( time <= 0.0f )
            {
                // If duration of fade in is 0, transition is done, begin playing next sample
                fadeInDur = this->transitions[ this->nextTransition ].fadeInDuration;
                if ( fadeInDur <= 0.0f )
                {
                    this->trackElapsedT = 0.0f;
                    this->trackBeginT = this->finishTime;
                    n_assert( this->musicSamples[ this->nextSample ].sample );
                    this->finishTime += this->musicSamples[ this->nextSample ].sample->GetDuration();
                    this->playbackState = Playing;
                    this->currentSample = this->nextSample;
                    this->nextSample = -1;
                    this->nextTransition = -1;
                    this->nextTransitionSample = -1;

                    this->musicSamples[ this->nextSample ].sample->SetVolume( 1.0f );
                    this->musicSamples[ this->nextSample ].sample->Update();

                    if ( this->playNextSampleAtRandomEntryPoint )
                    {
                        this->lastMusicSamplePlayPosition = this->GetRandomExitPoint(this->nextSample);
                        this->musicSamples[ this->nextSample ].sample->SetTrackPosition( this->musicSamples[ this->nextSample ].sample->GetLastBufferUsed(), this->lastMusicSamplePlayPosition );
                        this->finishTime -= this->lastMusicSamplePlayPosition;
                        n_assert( this->finishTime > this->trackBeginT );
                    }
                    else
                    {
                        this->playNextSampleAtRandomEntryPoint = true;
                    }
                    this->lastMusicSamplePlayPosition = 0.0f;

                    this->musicSamples[ this->nextSample ].sample->Start();

                    NLOG( musicLog, (0, "FadingOut part(%s) -> Playing   part(%s)", this->musicSamples[this->currentSample].name.Get(), this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );

                    this->OnTransitionStart();
                    this->OnTransitionEnd();
                }
                else
                {
                    // If duration of 'only transition' playing is 0, begin playing next music sample and enter fade-in state
                    this->playbackState = FadingIn;
                    this->fadeInBegin = this->finishTime;
                    this->finishTime += fadeInDur;
                    this->invFadeInDuration = 1.0f / fadeInDur;
                    n_assert( this->musicSamples[ this->nextSample ].sample );
                    this->musicSamples[ this->nextSample ].sample->SetVolume( 1.0f );
                    this->musicSamples[ this->nextSample ].sample->Update();

                    if ( this->playNextSampleAtRandomEntryPoint )
                    {
                        this->lastMusicSamplePlayPosition = this->GetRandomExitPoint(this->nextSample);
                        this->musicSamples[ this->nextSample ].sample->SetTrackPosition( this->musicSamples[ this->nextSample ].sample->GetLastBufferUsed(), this->lastMusicSamplePlayPosition );
                    }
                    else
                    {
                        this->playNextSampleAtRandomEntryPoint = true;
                        this->lastMusicSamplePlayPosition = 0.0f;
                    }

                    this->musicSamples[ this->nextSample ].sample->Start();

                    NLOG( musicLog, (0, "FadingOut part(%s) -> FadingIn  part(%s)", this->musicSamples[this->currentSample].name.Get(), this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );
                }
            }
            else
            {
                // enter in play only transition state
                this->finishTime += time;
                this->playbackState = PlayingTransition;
                n_printf("FADEOUT->PLAYTRANSITION\n");
                NLOG( musicLog, (0, "FadingOut part(%s) -> PlayTrans tran(%s)", this->musicSamples[this->currentSample].name.Get(), this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );
            }
            break;
        case PlayingTransition: // Playing only transition

            if ( time < this->finishTime )
            {
                // Update track elapsed time
                this->trackElapsedT = time - this->trackBeginT;

                // Fade out of music sample that just ended
                if ( this->stopFadingOutSample >= 0 )
                {
                    nSound3 * stoppingSample = this->musicSamples[ this->stopFadingOutSample ].sample;
                    n_assert( stoppingSample );
                    stoppingSample->SetVolume( stoppingSample->GetVolume() * 0.5f );
                    stoppingSample->Update();
                }

                return;
            }

            // Stop completely music sample that's being fading out
            if ( this->stopFadingOutSample >= 0 )
            {
                n_assert( this->musicSamples[ this->stopFadingOutSample ].sample );
                this->musicSamples[ this->stopFadingOutSample ].sample->Stop();
                this->stopFadingOutSample = -1;
            }

            fadeInDur = this->transitions[ this->nextTransition ].fadeInDuration;
            if ( fadeInDur > 0.0f )
            {
                // Starting fading in of next sample
                this->fadeInBegin = this->finishTime;
                this->finishTime += fadeInDur;
                this->invFadeInDuration = 1.0f / fadeInDur;
                this->playbackState = FadingIn;

                n_assert( this->musicSamples[ this->nextSample ].sample );
                this->musicSamples[ this->nextSample ].sample->SetVolume( 1.0f );
                this->musicSamples[ this->nextSample ].sample->Update();

                if ( this->playNextSampleAtRandomEntryPoint )
                {
                    this->lastMusicSamplePlayPosition = this->GetRandomExitPoint(this->nextSample);
                    this->musicSamples[ this->nextSample ].sample->SetTrackPosition( this->musicSamples[ this->nextSample ].sample->GetLastBufferUsed(), this->lastMusicSamplePlayPosition );
                }
                else
                {
                    this->playNextSampleAtRandomEntryPoint = true;
                    this->lastMusicSamplePlayPosition = 0.0f;
                }

                this->musicSamples[ this->nextSample ].sample->Start();

                NLOG( musicLog, (0, "PlayTrans tran(%s) -> FadingIn  part(%s)", this->musicSamples[this->currentSample].name.Get(), this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );
            }
            else
            {
                // No fading in of next sample, so transition has ended (play next sample)
                this->trackElapsedT = 0.0f;
                this->trackBeginT = this->finishTime;
                n_assert( this->musicSamples[ this->nextSample ].sample );
                this->finishTime += this->musicSamples[ this->nextSample ].sample->GetDuration();
                this->playbackState = Playing;
                this->currentSample = this->nextSample;

                // @todo Delete this line. Dont's stop transition samples, it's not needed.
                // this->transitionSamples[ this->nextTransitionSample ].sample->Stop();

                this->musicSamples[ this->nextSample ].sample->SetVolume( 1.0f );
                this->musicSamples[ this->nextSample ].sample->Update();

                if ( this->playNextSampleAtRandomEntryPoint )
                {
                    this->lastMusicSamplePlayPosition = this->GetRandomExitPoint(this->nextSample);
                    this->musicSamples[ this->nextSample ].sample->SetTrackPosition( this->musicSamples[ this->nextSample ].sample->GetLastBufferUsed(), this->lastMusicSamplePlayPosition );
                    this->finishTime -= this->lastMusicSamplePlayPosition;
                    n_assert( this->finishTime > this->trackBeginT );
                }
                else
                {
                    this->playNextSampleAtRandomEntryPoint = true;
                }
                this->lastMusicSamplePlayPosition = 0.0f;

                this->musicSamples[ this->nextSample ].sample->Start();

                this->nextSample = -1;
                this->nextTransition = -1;
                this->nextTransitionSample = -1;

                NLOG( musicLog, (0, "PlayTrans tran(?) -> Playing   part(%s)", this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );

                this->OnTransitionEnd();
            }

            break;
        case FadingIn:   // Fading out the music part or transition sample and fading in a new music sample.

            if ( time < this->finishTime )
            {
                // Fade in of next sample (and fade out of transition sample, or previous sample if there wasn't a transition sample)
                v = ( time - this->fadeInBegin ) * this->invFadeInDuration;
                if ( this->nextTransitionSample != -1 )
                {
                    // Fade out of transition sample
                    n_assert( this->transitionSamples[ this->nextTransitionSample ].sample );
                    this->transitionSamples[ this->nextTransitionSample ].sample->SetVolume( 1.0f - v );
                    this->transitionSamples[ this->nextTransitionSample ].sample->Update();
                }
                else
                {
                    // Fade out of previous sample
                    n_assert( this->musicSamples[ this->currentSample ].sample );
                    this->musicSamples[ this->currentSample ].sample->SetVolume( 1.0f - v );
                    this->musicSamples[ this->currentSample ].sample->Update();
                }
                // Small fade in of next sample to prevent clicks
                nSound3 * startingSample = this->musicSamples[ this->nextSample ].sample;
                n_assert( startingSample );
                startingSample->SetVolume( 1.0f - ( 1.0f - startingSample->GetVolume() ) * 0.5f );
                startingSample->Update();



                // Update track elapsed time
                this->trackElapsedT = time - this->trackBeginT;

                return;
            }

            if ( this->nextTransitionSample != -1 )
            {
                // Finished fade in, stop transition sample
                // @todo Delete this line. Dont's stop transition samples, it's not needed.
                //this->transitionSamples[ this->nextTransitionSample ].sample->Stop();
            }
            else
            {
                // Finished fade in, stop next sample
                n_assert( this->musicSamples[ this->currentSample ].sample );
                this->musicSamples[ this->currentSample ].sample->Stop();
            }

            // Next sample is now current sample
            this->trackElapsedT = 0.0f;
            this->trackBeginT = this->finishTime;
            //this->finishTime += this->musicSamples[ this->currentSample ].sample->GetDuration() - this->transitions[ this->nextTransition ].fadeInDuration - this->lastMusicSamplePlayPosition;
            float currentTime = ((float)nTimeServer::Instance()->GetTime());
            this->finishTime = currentTime + this->musicSamples[ this->nextSample ].sample->GetDuration() - this->transitions[ this->nextTransition ].fadeInDuration;
            this->playbackState = Playing;
            this->currentSample = this->nextSample;
            this->nextSample = -1;
            this->nextTransition = -1;
            this->nextTransitionSample = -1;

            NLOG( musicLog, (0, "FadingIn ? -> Playing   part(%s)", this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );

            this->OnTransitionEnd();

            break;
    }
}

//------------------------------------------------------------------------------   
/**
    @brief Play a music part, or queue a part for playing after the current one ends
    @param part The music part
    @ Param queue Queue the part at the end of the current one instead of playing it at the first exit point available
    This method plays a music part or queues a transition. There are three cases:
    1 - Music is stopped. The music sample starts with this call.
    2 - Music is playing but no other samples is queued for transition. The transition
        to the new sample is queued.
    3 - There is already a transition queued (so there is a sample currently playing, a 
        transition to another sample is waiting for its end) The new sample is stored as
        the next transition and will be queued when the first transition ends.
*/
bool
nMusicTable::PlayMusicPartByIndex( int part, bool queue)
{
    if ( part < 0 || part >= this->musicSamples.Size() )
    {
        return false;
    }

    // Load music if not loaded yet
    this->LoadAllSamples();

    float currentTime = ((float)nTimeServer::Instance()->GetTime());

    TransitionInfo* trans;
    if ( this->playbackState == Stopped )
    {
        // Music was stopped, play the sample
        int previousCurrentSample = this->currentSample;
        this->currentSample = part;
        this->nextTransition = -1;
        this->nextSample = -1;
        this->queuedSample = -1;
        this->playbackState = Playing;
        n_assert( this->musicSamples[ this->currentSample ].sample );
        this->finishTime = currentTime + this->musicSamples[ this->currentSample ].sample->GetDuration();
        this->transitionTime = this->finishTime;
        this->transitionDuration = 0.0f;
        this->trackElapsedT = 0.0f;
        this->trackBeginT = currentTime;

        this->musicSamples[ this->currentSample ].sample->SetVolume( 1.0f ); 
        this->musicSamples[ this->currentSample ].sample->Update(); 

        // Exception: if current sample is positive and equals the part to play, it means that's still playing looped, so don't play it again
        if ( previousCurrentSample != this->currentSample )
        {
            this->musicSamples[ this->currentSample ].sample->Start();
        }
        else if ( !this->looped )
        {
            this->StopMusic();
            return false;
        }

        this->playNextSampleAtRandomEntryPoint = true;

        NLOG( musicLog, (0, "Stop               -> Playing   part(%s)", this->musicSamples[this->currentSample].name.Get() ) );

        return true;
    }
    else
    {
        if ( this->nextTransition == -1 || this->playbackState == Playing )
        {
            // A sample is playing but no transition queued; or it is queued but currently we are not playing in the transition, so
            // queue the transition to the new sample.

            if ( this->currentSample == part )
            {
                // Tried to play the same sample that's playing
                return false;
            }
            int nextTr = this->GetTransition( this->currentSample, part );

            if ( nextTr < 0 )
            {
                NLOG( musicLog, (0, "Music error: No transition found from track %s to track %s", this->musicSamples[this->currentSample].name.Get(),
                                                                                                  this->musicSamples[part].name.Get() ) );
                this->StopMusic();
                return false;
            }
            n_assert( nextTr >= 0 );
            
            trans = &this->transitions[ nextTr ];
            int nextTrSam = trans->transitionSample;
            
            if ( ! queue && ( this->nextTransition == -1 && ( currentTime + trans->fadeOutDuration >= this->finishTime ) || 
                              this->nextTransition != -1 && ( currentTime + trans->fadeOutDuration >= this->transitionTime ) ) )
            {
                // Can't queue this sample because the fade out would pass the end of the sample (the transition would need to start earlier)
                return false;
            }

            float trDur = 0.0f;
            float fadeOutDur = 0.0f;
            if ( nextTrSam != -1 )
            {
                n_assert( this->transitionSamples[ nextTrSam ].sample );
                trDur = this->transitionSamples[ nextTrSam ].sample->GetDuration();
                fadeOutDur = trans->fadeOutDuration;
                if ( trDur - trans->fadeOutDuration - trans->fadeInDuration < 0.0f )
                {
                    // Crossfade duration can't be larger than transition duration
                    NLOG( musicLog, (0, "Music error: Crossfade duration can't be larger than transition duration (in transition %s -> %s, %s, %f, %f)",
                        this->musicSamples[this->currentSample].name.Get(), this->musicSamples[part].name.Get(), this->transitionSamples[trans->transitionSample].name.Get(),
                        trans->fadeOutDuration, trans->fadeInDuration ) );
                    this->StopMusic();
                    return false;
                }
            }
            else
            {
                trDur = max( trans->fadeOutDuration, trans->fadeInDuration );
                fadeOutDur = trDur;
            }

            // Get exit time (transition begin)
            float exitTime = 0.0f;
            if ( ! queue )
            {
                exitTime = this->GetNextExitPoint( this->currentSample, this->trackElapsedT);
            }
            else
            {
                n_assert( this->musicSamples[ trans->fromSample ].sample );
                exitTime = this->musicSamples[ trans->fromSample ].sample->GetDuration() - trans->fadeOutDuration; // -0.5f;
            }

            // Select first exit point from the queue time
            exitTime += this->trackBeginT;

            trans = &this->transitions[ nextTr ];
            this->nextTransition = nextTr;
            this->nextTransitionSample = nextTrSam;
            this->nextSample = trans->toSample;
            this->queuedSample = -1;
            this->transitionDuration = trDur;

            if ( this->nextTransition != -1 )
            {
                this->transitionTime = exitTime;
            }
            else
            {
                this->finishTime = exitTime;
            }

            if ( this->playbackState == Playing )
            {
                NLOG( musicLog, (0, "Playing   part(%d) -> Playing   part(%d)", this->musicSamples[this->currentSample].name.Get(), this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );
            }
            else
            {
                NLOG( musicLog, (0, "Playing   ?        -> Playing   part(%d)", this->nextSample == -1? "?" : this->musicSamples[this->nextSample].name.Get() ) );
            }

            return true;
        }
        else
        {
            // We are transitioning right now so queue the transition for later
            this->queuedSample = part;
            return false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Play a music part independently
*/
void
nMusicTable::PlayMusicStinger(const char * partName, float volume)
{
    this->PlayMusicStingerByIndex( this->GetTransitionSampleIndex( partName ), volume );
}

//------------------------------------------------------------------------------
/**
    @brief Play a music part independently
*/
void
nMusicTable::PlayMusicStingerByIndex(int partIndex, float volume)
{

    // Load music if not loaded yet
    this->LoadAllSamples();

    // clamp input volume
    volume = min(1.0f, max( 0.0f, volume ) );

    if ( partIndex != -1 )
    {
        n_assert( this->transitionSamples[ partIndex ].sample );
        this->transitionSamples[ partIndex ].sample->Stop();
        this->transitionSamples[ partIndex ].sample->SetVolume( volume );
        this->transitionSamples[ partIndex ].sample->Update();
        this->transitionSamples[ partIndex ].sample->Start();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get exit point next to the time given
*/
float
nMusicTable::GetNextExitPoint( int sample, float time )
{
    // Fixed step exit time
    MusicSampleInfo& ms = this->musicSamples[ sample ];
    float exitTime = time / ms.exitPointInterval;
    if ( time > 0.0f )
    {
        exitTime = float( int( exitTime ) + 1 ) * ms.exitPointInterval;
        n_assert( this->musicSamples[ sample ].sample );
        float duration = this->musicSamples[ sample ].sample->GetDuration();
        if ( exitTime > duration )
        {
            exitTime = duration;
        }
    }

    // User exit point
    int i = 0;
    while ( i < ms.exitPoints->Size() )
    {
        float t = (*ms.exitPoints)[i];
        if ( time < t && t < exitTime )
        {
            exitTime = t;
        }
        i++;
    }
    return exitTime;
}

//------------------------------------------------------------------------------
/**
    @brief Get a random fixed-step exit point from the sample
    @returns Time of a random exit point in seconds
*/
float
nMusicTable::GetRandomExitPoint( int sample )
{
    MusicSampleInfo& ms = this->musicSamples[ sample ];
    
    int exitPoint = int( ( ms.exitPoints->Size() + 1 ) * n_rand() );

    if ( exitPoint == 0)
    {
        return 0.0f;
    }
    else
    {
        return (*ms.exitPoints)[ min(0, max( ms.exitPoints->Size(), exitPoint - 1 ) ) ];
    }
}

//------------------------------------------------------------------------------
/**
    @brief Stop music immediatly and clear queued samples
*/
void
nMusicTable::StopMusic()
{

    for ( int i = 0; i < this->musicSamples.Size(); i++)
    {
        this->musicSamples[i].sample->Stop();
    }
    
    for ( int i = 0; i < this->transitionSamples.Size(); i++)
    {
        this->transitionSamples[i].sample->Stop();
    }

    this->playbackState = Stopped;
    this->currentSample = -1;
    this->stopFadingOutSample = -1;
    this->nextTransition = -1;
    this->currentMood = -1;
    this->nextMood = -1;

    NLOG( musicLog, (0, "Music stopped") );
}

//------------------------------------------------------------------------------
/**
    @brief Get first transition associated with start sample and dest sample, -1 if not found
*/
int
nMusicTable::GetTransition( int fromSample, int toSample )
{
    int i = 0;
    while ( i < this->transitions.Size() && ! ( this->transitions[ i ].fromSample == fromSample && this->transitions[ i ].toSample == toSample ) )
    {
        i++;
    }
    
    if ( i >= this->transitions.Size() )
    {
        return -1;
    }

    int j = i + 1;
    while ( j < this->transitions.Size() && ( this->transitions[ j ].fromSample == fromSample && this->transitions[ j ].toSample == toSample ) )
    {
        j++;
    }

    int k = (int) ( i + ( j - i ) * n_rand() ); 

    if ( k < this->transitions.Size() )
    {
        return k;
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    @brief Called when starting a transition
*/
void
nMusicTable::OnTransitionStart()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Called when finishing a transition or just after OnTransitionStart if the transition has duration 0
*/
void
nMusicTable::OnTransitionEnd()
{
    // Insert new transition if there was another queued sample
    if ( this->queuedSample != -1 )
    {
        this->PlayMusicPartByIndex( this->queuedSample, false );
        this->queuedSample = -1;
    }
    else
    {
        // Mood change?
        if ( this->nextMood != -1 )
        {
            this->currentMood = this->nextMood;
            this->nextMood = -1;
        }

        // Play part of current mood, queued at the end of current sample
        if ( this->currentMood != -1 )
        {
            int part = this->GetMoodRandomSample( this->currentMood );
            if ( part != -1 )
            {
                // Play a music part of the current mood
                this->PlayMusicPartByIndex( part, true );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Add a mood
*/
void
nMusicTable::AddMood(const char* name)
{
    int moodIndex = this->FindMood( name );
    if ( moodIndex >= 0 || *name==0 )
    {
        // Already exists
        NLOG( musicLog, (0, "AddMood (Warning): Mood '%s' already exists", name ) );
        return;
    }
    MoodInfo & mInfo = this->moodInfo.At( this->moodInfo.Size() );
    mInfo.name = name;
}

//------------------------------------------------------------------------------
/**
    @brief Add a mood's music sample
*/
void
nMusicTable::AddMoodMusicSample(const char* moodName, const char * musicSampleName)
{
    int moodIndex = this->FindMood( moodName );
    if ( moodIndex < 0 )
    {
        // Mood doesn't exist
        NLOG( musicLog, (0, "AddMoodMusicSample (Warning): Mood %s doesn't exist", moodName ) );
        return;
    }

    int musicSample = this->GetMusicSampleIndex( musicSampleName );
    if ( musicSample < 0 )
    {
        // Sample doesn't exist
        NLOG( musicLog, (0, "AddMoodMusicSample (Warning): Music sample %s doesn't exist", musicSampleName ) );
        return;
    }

    MoodInfo & mInfo = this->moodInfo.At( moodIndex );
    if ( mInfo.musicSamples.FindIndex( musicSample ) == -1 )
    {
        mInfo.musicSamples.Append( musicSample );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Add a style
*/
void
nMusicTable::AddStyle(const char* name)
{
    StyleInfo * style = this->styleInfo[name];
    if ( !style )
    {
        StyleInfo sInfo;
        sInfo.name = name;
        this->styleInfo.Add(name, &sInfo);
    }
    else
    {
        NLOG( musicLog, (0, "AddStyle (Warning): Style %s already exists", name ) );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Add a mood to a style
*/
void
nMusicTable::AddMoodToStyle(const char * moodName, const char * styleName, int intensity)
{
    int moodIndex = this->FindMood( moodName );
    if ( moodIndex < 0 )
    {
        NLOG( musicLog, (0, "AddMoodToStyle (Warning): Mood %s doesn't exist", moodName ) );
        return;
    }

    StyleInfo * sInfo = this->styleInfo[ styleName ];
    if ( sInfo )
    {
        this->moodInfo[ moodIndex ].style = styleName;
        this->moodInfo[ moodIndex ].intensity = intensity;
        intensity = max(0, min( intensity, sInfo->moods.Size() ) );
        if ( sInfo->moods.Size() < intensity )
        {
            NLOG( musicLog, (0, "AddMoodToStyle (Warning): Style %s hasn't got some mood definitions under intensity %d", styleName, intensity ) );
        }
        sInfo->moods.At( intensity ) = moodName;
    }
    else
    {
        NLOG( musicLog, (0, "AddMoodToStyle (Warning): Style %s doesn't exist", styleName ) );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set current style and intensity
*/
void
nMusicTable::SetCurrentStyle(const char *styleName, int intensity, bool loop)
{
    StyleInfo * sInfo = this->styleInfo[ styleName ];
    if ( sInfo )
    {
        if ( sInfo->moods.Size() == 0)
        {
            NLOG( musicLog, (0, "SetCurrentStyle (Error): Style '%s' doesn't has any mood", styleName ) );
            return;
        }
        int i = max(0, min( intensity, sInfo->moods.Size()-1 ) );
        NLOG( musicLog, (0, "Playing style '%s' with intensity %d", styleName, intensity ) );
        this->SetCurrentMood( sInfo->moods[ i ].Get(), loop );
    }
    else
    {
        NLOG( musicLog, (0, "SetCurrentStyle (Error): Style '%s' doesn't exist", styleName ) );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set current style (by index) and intensity
*/
void
nMusicTable::SetCurrentStyleByIndex(int styleIndex, int intensity, bool loop )
{
    const char * styleName = this->GetStyleName( styleIndex );
    if ( styleName )
    {
        this->SetCurrentStyle( styleName, intensity, loop );
    }
    else
    {
        NLOG( musicLog, (0, "SetCurrentStyle (Error): Style with index '%d' doesn't exist", styleIndex ) );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set current mood's intensity
*/
void
nMusicTable::SetCurrentStyleIntensity(int intensity, bool loop)
{
    if ( this->currentMood )
    {
        this->SetCurrentStyle( this->moodInfo[ this->currentMood ].style.Get(), intensity, loop );
    }
    else
    {
        NLOG( musicLog, (0, "SetCurrentStyleIntensity (Warning): No current style is set" ) );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set current mood
    @parameter mood Mood to set
*/
void
nMusicTable::SetCurrentMood(const char* name, bool loop)
{
    NLOG( musicLog, (0, "Playing mood '%s'", name ) );
    this->SetCurrentMoodByIndex( this->FindMood(name), loop );
}

//------------------------------------------------------------------------------
/**
    @brief Set current mood by index
    @parameter moodIndex Index of the mood to set
*/
void
nMusicTable::SetCurrentMoodByIndex( int moodIndex, bool loop )
{
    if ( moodIndex < 0 || moodIndex == this->currentMood )
    {
        // Mood doesn't exist or is already set
        return;
    }

    int part = this->GetMoodRandomSample( moodIndex );
    if ( part!= -1 )
    {
        if ( this->playbackState == Stopped )
        {
            // Set mood and play a music part
            this->currentMood = moodIndex;
            this->nextMood = -1;
            this->looped = loop;
            this->PlayMusicPartByIndex( part, false );
        }
        else
        {
            if ( this->currentMood != moodIndex )
            {
                // Change mood and play a music part
                this->playNextSampleAtRandomEntryPoint = false;
                this->currentMood = -1;
                this->nextMood = moodIndex;
                this->looped = loop;
                this->PlayMusicPartByIndex( part, false );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Get a mood by name
*/
int
nMusicTable::FindMood(const char* name) const
{
    for ( int i = 0; i < this->moodInfo.Size(); i++ )
    {
        if ( strcmp( this->moodInfo[i].name.Get(), name ) == 0 )
        {
            return i;
        }
    
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Get a mood's random music sample or -1 if mood has no samples
*/
int
nMusicTable::GetMoodRandomSample(int moodIndex)
{
    n_assert( moodIndex >= 0 && moodIndex < this->moodInfo.Size() );

    float r = n_rand();

    int numSamples = this->moodInfo[ moodIndex ].musicSamples.Size();
    if ( numSamples > 0 )
    {
        int randomSample = this->moodInfo[ moodIndex ].musicSamples[ int( r * numSamples ) ];

        // If it's equal to the last played, try another one
        while ( this->lastPlayedMusicSample == randomSample && this->moodInfo[ moodIndex ].musicSamples.Size() > 1)
        {
            r = n_rand();
            randomSample = this->moodInfo[ moodIndex ].musicSamples[ int( r * numSamples ) ];
        }
        this->lastPlayedMusicSample = randomSample;

        return randomSample;
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
    Get a sample index by name
*/
int
nMusicTable::GetMusicSampleIndex(const char * fileName) const
{
    for ( int i = 0; i < this->musicSamples.Size(); i ++ )
    {
        if ( this->musicSamples[ i ].name == fileName )
        {
            return i;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
    Get a transition  sample index by name
*/
int
nMusicTable::GetTransitionSampleIndex(const char * fileName) const
{
    for ( int i = 0; i < this->transitionSamples.Size(); i ++ )
    {
        if ( this->transitionSamples[ i ].name == fileName )
        {
            return i;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
    Get a style index by name
*/
int
nMusicTable::FindStyle( const char * styleName )
{

    StyleInfo * sInfo = this->styleInfo[ styleName ];
    if ( !sInfo )
    {
        return -1;
    }

    nString sName;
    this->styleInfo.Begin();
    this->styleInfo.Next( sName, sInfo );
    int index = 0;
    while ( sInfo && sName != styleName )
    {
        this->styleInfo.Next( sName, sInfo );
        index++;
    }
    if ( sInfo )
    {
        return index;
    }
    else
    {
        return -1;
    }
}

//-----------------------------------------------------------------------------
/**
    Get a style by index
*/
const char *
nMusicTable::GetStyleName( int styleIndex )
{
    nString sName;
    StyleInfo * sInfo;
    this->styleInfo.Begin();
    this->styleInfo.Next( sName, sInfo );
    while ( styleIndex > 0 && sInfo )
    {
        this->styleInfo.Next( sName, sInfo );
        styleIndex--;
    }
    if ( sInfo )
    {
        return sInfo->name.Get();
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
/**
    @brief Network command to play music part
    @param partName music part
    @param player Player entity, can be null meaning broadcast to all players

*/
void
nMusicTable::SendPlayMusicPart( const char* partName, nObject * player ) const
{
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        int playerId = 0;
        if ( player )
        {
            nEntityObject* playerEntity = static_cast< nEntityObject * > ( player );
            playerId = playerEntity->GetId();
        }
        n_assert( playerId == 0 || nEntityObjectServer::Instance()->GetEntityObject( playerId ) );

        int partIndex = this->GetMusicSampleIndex( partName );
#ifndef NGAME
        if ( partIndex < 0 )
        {
            nString msg( "Music sample '" + nString(partName) + "' not found." );
            NLOG( musicLog, (0, msg.Get() ));
        }
#endif
        nstream data;
        data.SetWrite( true );
        data.UpdateInt( playerId );
        data.UpdateInt( partIndex );

        nNetworkManager::Instance()->CallRPCAllServer(
            nRnsEntityManager::PLAY_MUSIC_PART,
            data.GetBufferSize(), data.GetBuffer() );
    }
}

//-----------------------------------------------------------------------------
/**
    Receive network command to play music part
*/
void
nMusicTable::ReceivePlayMusicPart( nstream* data )
{
    n_assert( data );

    if ( data )
    {
        // Do nothing if there isn't a local player
        nEntityObject* localPlayer( nRnsEntityManager::Instance()->GetLocalPlayer() );
        if ( !localPlayer )
        {
            return;
        }

        int playerId;
        data->UpdateInt( playerId );
        if ( !playerId || localPlayer->GetId() == nEntityObjectId(playerId) )
        {
            int partIndex;
            data->UpdateInt( partIndex );
            this->PlayMusicPartByIndex( partIndex, false );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    @brief Network Network command to stop music
    @param player Player entity, can be null meaning broadcast to all players
*/
void
nMusicTable::SendStopMusic( nObject * player ) const
{
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        int playerId = 0;
        if ( player )
        {
            nEntityObject* playerEntity = static_cast< nEntityObject * > ( player );
            playerId = playerEntity->GetId();
        }
        n_assert( playerId == 0 || nEntityObjectServer::Instance()->GetEntityObject( playerId ) );

        nstream data;
        data.SetWrite( true );
        data.UpdateInt( playerId );

        nNetworkManager::Instance()->CallRPCAllServer(
            nRnsEntityManager::STOP_MUSIC,
            data.GetBufferSize(), data.GetBuffer() );
    }
}

//-----------------------------------------------------------------------------
/**
    Receive network command to stop music
*/
void
nMusicTable::ReceiveStopMusic( nstream* data )
{
    n_assert( data );

    if ( data )
    {
        // Do nothing if there isn't a local player
        nEntityObject* localPlayer( nRnsEntityManager::Instance()->GetLocalPlayer() );
        if ( !localPlayer )
        {
            return;
        }

        int playerId;
        data->UpdateInt( playerId );
        if ( !playerId || localPlayer->GetId() == nEntityObjectId(playerId) )
        {
            this->StopMusic();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    @brief Network command to play music stinger
    @param partName music stinger part
    @param player Player entity, can be null meaning broadcast to all players
*/
void
nMusicTable::SendPlayMusicStinger( const char* partName, float volume, nObject * player ) const
{
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        // TODO: Musicians wants this. Ugly hack because cannot use default parameters in command property grid.
        if ( volume == 0.0f )
        {
            volume = 1.0f;
        }

        int playerId = 0;
        if ( player )
        {
            nEntityObject* playerEntity = static_cast< nEntityObject * > ( player );
            playerId = playerEntity->GetId();
        }
        n_assert( playerId == 0 || nEntityObjectServer::Instance()->GetEntityObject( playerId ) );

        int partIndex = this->GetTransitionSampleIndex( partName );
#ifndef NGAME
        if ( partIndex < 0 )
        {
            nString msg( "Music sample '" + nString(partName) + "' not found." );
            NLOG( musicLog, (0, msg.Get() ));
        }
#endif
        nstream data;
        data.SetWrite( true );
        data.UpdateInt( playerId );
        data.UpdateInt( partIndex );
        data.UpdateFloat( volume );

        nNetworkManager::Instance()->CallRPCAllServer(
            nRnsEntityManager::PLAY_MUSIC_STINGER,
            data.GetBufferSize(), data.GetBuffer() );
    }
}

//-----------------------------------------------------------------------------
/**
    Receive network command to play music part
*/
void
nMusicTable::ReceivePlayMusicStinger( nstream* data )
{
    n_assert( data );

    if ( data )
    {
        // Do nothing if there isn't a local player
        nEntityObject* localPlayer( nRnsEntityManager::Instance()->GetLocalPlayer() );
        if ( !localPlayer )
        {
            return;
        }

        int playerId;
        data->UpdateInt( playerId );
        if ( !playerId || localPlayer->GetId() == nEntityObjectId(playerId) )
        {
            int partIndex;
            float volume;
            data->UpdateInt( partIndex );
            data->UpdateFloat( volume );
            PlayMusicStingerByIndex( partIndex, volume );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    @brief Tell the client of a player to set some mood
    @param mood music mood
    @param player Player entity, can be null meaning broadcast to all players
*/
void
nMusicTable::SendMoodToPlay( const char* mood, nObject * player, bool loop ) const
{
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        n_assert( mood );

        int playerId = 0;
        if ( player )
        {
            nEntityObject* playerEntity = static_cast< nEntityObject * > ( player );
            playerId = playerEntity->GetId();
        }
        n_assert( playerId == 0 || nEntityObjectServer::Instance()->GetEntityObject( playerId ) );

        int moodId( this->FindMood(mood) );
#ifndef NGAME
        if ( moodId < 0 )
        {
            nString msg( "Music mood '" + nString(mood) + "' not found." );
            NLOG( musicLog, (0, msg.Get() ));
        }
#endif

        nstream data;
        data.SetWrite( true );
        data.UpdateInt( playerId );
        data.UpdateInt( moodId );
        data.UpdateBool( loop );

        nNetworkManager::Instance()->CallRPCAllServer(
            nRnsEntityManager::SET_MUSIC_MOOD,
            data.GetBufferSize(), data.GetBuffer() );
    }
}

//-----------------------------------------------------------------------------
/**
    Set the specified mood if I'm the owner client of the given player
*/
void
nMusicTable::ReceiveMoodToPlay( nstream* data )
{
    n_assert( data );

    if ( data )
    {
        // Do nothing if there isn't a local player
        nEntityObject* localPlayer( nRnsEntityManager::Instance()->GetLocalPlayer() );
        if ( !localPlayer )
        {
            return;
        }

        // Set the mood when the local player matches with the one given by the server
        int playerId;
        data->UpdateInt( playerId );
        if ( !playerId || localPlayer->GetId() == nEntityObjectId(playerId) )
        {
            int moodId;
            bool loop;
            data->UpdateInt( moodId );
            data->UpdateBool( loop );

            this->SetCurrentMoodByIndex( moodId, loop );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    @brief Network command to set a music style and intensity
    @param style style
    @param intensity intensity
    @param player Player entity, can be null meaning broadcast to all players
*/
void
nMusicTable::SendSetCurrentStyle( const char* style, int intensity, nObject * player, bool loop )
{
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        n_assert( style );

        int playerId = 0;
        if ( player )
        {
            nEntityObject* playerEntity = static_cast< nEntityObject * > ( player );
            playerId = playerEntity->GetId();
        }
        n_assert( playerId == 0 || nEntityObjectServer::Instance()->GetEntityObject( playerId ) );

        int styleId = this->FindStyle( style );

#ifndef NGAME
        if ( styleId < 0 )
        {
            nString msg( "Music style '" + nString(style) + "' not found." );
            NLOG( musicLog, (0, msg.Get() ));
        }
#endif

        nstream data;
        data.SetWrite( true );
        data.UpdateInt( playerId );
        data.UpdateInt( styleId );
        data.UpdateInt( intensity );
        data.UpdateBool( loop );

        nNetworkManager::Instance()->CallRPCAllServer(
            nRnsEntityManager::SET_MUSIC_STYLE,
            data.GetBufferSize(), data.GetBuffer() );
    }
}

//-----------------------------------------------------------------------------
/**
    Receive network command to set a music style and intensity
*/
void
nMusicTable::ReceiveSetCurrentStyle( nstream* data )
{
    n_assert( data );

    if ( data )
    {
        // Do nothing if there isn't a local player
        nEntityObject* localPlayer( nRnsEntityManager::Instance()->GetLocalPlayer() );
        if ( !localPlayer )
        {
            return;
        }

        // Set the mood when the local player matches with the one given by the server
        int playerId;
        data->UpdateInt( playerId );
        if ( !playerId || localPlayer->GetId() == nEntityObjectId( playerId ) )
        {
            int styleIndex;
            data->UpdateInt( styleIndex );
            int intensity;
            data->UpdateInt( intensity );
            bool loop;
            data->UpdateBool( loop );
            this->SetCurrentStyleByIndex( styleIndex, intensity, loop );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Get the number of music samples
*/
int
nMusicTable::GetNumberOfMusicSamples () const
{
    return this->musicSamples.Size();
}

//------------------------------------------------------------------------------
/**
    @brief Get info for a music sample by index
    @param index Global index of sample
    @return MusicSample info
*/
const nMusicTable::MusicSampleInfo&
nMusicTable::GetMusicSampleInfo( int index ) const
{
    n_assert( index >= 0 && index < this->GetNumberOfMusicSamples() );
    return this->musicSamples[ index ];
}

//------------------------------------------------------------------------------
/**
    Get the name of the music sample at the given index
*/
const nString&
nMusicTable::GetMusicSampleName( int index ) const
{
    n_assert( index >= 0 && index < this->GetNumberOfMusicSamples() );

    return this->GetMusicSampleInfo( index ).name;
}

//-----------------------------------------------------------------------------
/**
    Get the number of moods
*/
int
nMusicTable::GetNumberOfMoods () const
{
    return this->moodInfo.Size();
}

//------------------------------------------------------------------------------
/**
    @brief Get info for a mood by index
    @param index Global index of mood
    @return Mood info
*/
const nMusicTable::MoodInfo&
nMusicTable::GetMoodInfo( int index ) const
{
    n_assert( index >= 0 && index < this->GetNumberOfMoods() );
    return this->moodInfo[ index ];
}

//------------------------------------------------------------------------------
/**
    Get the name of the mood at the given index
*/
const nString&
nMusicTable::GetMoodName( int index ) const
{
    n_assert( index >= 0 && index < this->GetNumberOfMoods() );

    return this->GetMoodInfo( index ).name;
}

//-----------------------------------------------------------------------------
/**
    Get the number of styles
*/
int
nMusicTable::GetNumberOfStyles () const
{
    return const_cast<nMapTableTypes<StyleInfo>::NString&>(this->styleInfo).GetCount();
}
