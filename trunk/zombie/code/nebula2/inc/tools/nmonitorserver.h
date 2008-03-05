#ifndef N_GRAPHSERVER_H
#define N_GRAPHSERVER_H
//------------------------------------------------------------------------------
/**
    @class nMonitorServer
    @ingroup NebulaGraphicsSystem
    @brief
    Onscreen graph server that represents a set of nWatched variables

    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ncmdprotonativecpp.h"

class nTimeServer;
class nGfxServer2;

//------------------------------------------------------------------------------
class nMonitorServer : public nRoot
{
public:

    /// constructor
    nMonitorServer();
    /// destructor
    virtual ~nMonitorServer();
    /// return instance pointer
    static nMonitorServer* Instance();
    /// per-frame trigger method
    void Trigger();
    /// render the graphs
    void Render();

    int AddGraph(const char *, float, float, vector4);
    void RemoveGraph(int);
    int GetNumGraphs();

    void SetVisualParameters(vector2, vector2,vector4);
    void SetHistoryPeriod(float);
    float GetHistoryPeriod();
    bool GetDisplayActivated();
    void SetDisplayActivated(bool);
    bool GetSamplingActivated();
    void SetSamplingActivated(bool);
    bool GetFullScreen();
    void SetFullScreen(bool);

    const char * GetGraphVarName(int);
    void SetGraphParameters(int, float, float, vector4);
    void GetGraphParameters(int, float&, float&, vector4&);
    bool GetGraphActive(int);
    void SetGraphActive(int, bool);
    float GetGraphMinValue(int);
    float GetGraphMaxValue(int);
    float GetGraphMeanValue(int);
    void SetCurrentGraph(int);
    int GetCurrentGraph();
    void GetCurrentGraphStats(float&, float&, float&, float&);
    void ResetAllHistory();

private:

    // Struct with info of a monitor
    struct GraphInfo
    {
        // Reference to env varible
        nAutoRef<nEnv> refEnv;
        // Graph color
        vector4 color;
        // Sample period in seconds. If 0, sampling is per-frame
        nTime samplePeriod;
        // Next sample time, if period is not 0
        nTime nextSampleTime;
        // Smoothing factor (0 = no smoothing, near 1 = max smoothing)
        float smooth;
        // max display value, for rescaling
        float rangeValue;
        // min,max,mean measured values
        float minValue, maxValue, meanValue;
        // total samples measured for calc. the mean
        int totalSamples;
        // accumulated samples
        double sampleAcc;
        // last value for smoothing
        float lastValue;
        // history
        nRingBuffer<vector2> history;
        // activation flag
        bool active;
    };

    static nMonitorServer* Singleton;
    nRef<nTimeServer>  refTimeServer;
    nRef<nGfxServer2>  refGfxServer;

    nArray<GraphInfo*> measures;

    // Visualization parameteres. Dimensions are normalized to device
    vector2 graphCornerNW;
    vector2 graphCornerSE;
    vector4 borderColor;

    // Graphs history period in seconds
    nTime historyPeriod;

    // Number of graphs currently sampled and displayed
    int numGraphsActive;

    // Global display activation flag
    bool displayActivated;

    // Global sampling activation flag
    bool samplingActivated;

    // Fullscreen flag
    bool displayFullScreen;

    // Slowest sample rate graph
    int slowestGraph;

    // longest history array size
    int longestHistory;

    // vertex array for drawing
    nArray<vector3> vertexArray;

    // Current graph - the only one that shows the scale
    int currentGraph;

    // String for label drawing
    nString labelString;
};

//------------------------------------------------------------------------------
/**
*/
inline
nMonitorServer*
nMonitorServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}
//------------------------------------------------------------------------------
#endif
