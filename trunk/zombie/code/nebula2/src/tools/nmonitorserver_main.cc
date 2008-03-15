#include "precompiled/pchntoollib.h"
//------------------------------------------------------------------------------
//  nmonitorserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "tools/nmonitorserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"

#include "gfx2/nlineserver.h"

nNebulaScriptClass(nMonitorServer, "nroot");
nMonitorServer* nMonitorServer::Singleton = 0;

//------------------------------------------------------------------------------
nMonitorServer::nMonitorServer():
    slowestGraph(0),
    numGraphsActive(0),
    displayActivated(false),
    samplingActivated(false),
    displayFullScreen(false),
    historyPeriod(5.0f),
    longestHistory(0),
    borderColor(0.0f, 0.2f, 1.0f,1.0f),
    graphCornerNW(-0.8f, 0.8f),
    graphCornerSE(-0.1f, 0.2f),
    currentGraph(-1)
{
    n_assert(0 == Singleton);
    Singleton = this;

    this->refTimeServer = nTimeServer::Instance();
    this->refGfxServer = nGfxServer2::Instance();
    this->vertexArray.Reallocate( 100, 50 );
    this->vertexArray.At( 99 );
}
//------------------------------------------------------------------------------
nMonitorServer::~nMonitorServer()
{
    n_assert(Singleton);
    Singleton = 0;

    for (int i = 0; i < measures.Size();i++)
    {
        measures[i]->refEnv.set(0);
        n_delete( measures[i] );
    }
}

//------------------------------------------------------------------------------
void
nMonitorServer::Trigger()
{
    if ( !this->samplingActivated || this->numGraphsActive == 0 )
    {
        return;
    }

    nTime t = this->refTimeServer->GetTime();

    int processed = 0;
    for (int i=0; i < measures.Size() && processed < this->numGraphsActive; i++)
    {
        GraphInfo* graph = measures[i];
        if ( graph->active )
        {
            if ( graph->samplePeriod == 0.0f || graph->nextSampleTime <= t )
            {
                nEnv* env = graph->refEnv;
                float val;

                // Get variable sample
                switch ( env->GetType() )
                {
                    case nArg::Int:
                    val = (float)env->GetI();
                    break;

                    case nArg::Float:
                    val = env->GetF();
                    break;

                    default:
                    val = 0.0f;
                }

                if ( graph->history.IsEmpty() )
                {
                    // Initialize first sample value for smoothing
                    graph->lastValue = val;
                }

                // Calc. min, max, mean
                if ( val > graph->maxValue )
                {
                    graph->maxValue = val;
                }
                if ( val < graph->minValue )
                {
                    graph->minValue = val;
                }
                graph->totalSamples++;

                graph->sampleAcc += val;
                graph->meanValue = static_cast<float> (graph->sampleAcc / graph->totalSamples);

/*
                @todo this long expression was to save accuracy in the long term
                graph->meanValue = ( graph->meanValue * ( float(graph->totalSamples) - 1.0f ) + val ) / float(graph->totalSamples) ;
                graph->meanValue = ( graph->meanValue / float(graph->totalSamples) ) * ( ( float(graph->totalSamples) - 1.0f ) / float(graph->totalSamples) ) + val / float(graph->totalSamples);
*/
                // Smooth value
                val = val * (1.0f - graph->smooth) + graph->lastValue * graph->smooth;
                graph->lastValue = val;

                // Add sample to history
                if ( graph->history.IsFull() )
                {
                    graph->history.DeleteTail();
                }
                vector2* sample = graph->history.Add();
                sample->y = static_cast<float> (t);

                // Rescale current sample
                float valNorm = val * min(100.0f, 1.0f / graph->rangeValue);
                sample->x = valNorm;

                // If sample was a new absolute max,
                if ( val > graph->rangeValue )
                {
                    // Rescale all samples 
                    float rel = min(100.0f, graph->rangeValue / val);
                    for (vector2* s = graph->history.GetHead(); s != 0; s = graph->history.GetPrev( s ))
                    {
                        s->x = s->x * rel;
                    }
                    graph->rangeValue = val;
                }

                // Calc. next sample time
                if ( graph->samplePeriod != 0.0f )
                {
                    graph->nextSampleTime += graph->samplePeriod;
                }

                processed++;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
nMonitorServer::Render()
{
    if ( ! this->displayActivated )
    {
        return;
    }

    nLineHandler lh;
    lh.SetShader("shaders:line_grid.fx");
    lh.SetPrimitiveType( nGfxServer2::LineStrip );
    lh.SetVertexComponents( nMesh2::Coord | nMesh2::Color );
    matrix44 m;
    this->refGfxServer->SetTransform(nGfxServer2::Model, m);
    this->refGfxServer->SetTransform(nGfxServer2::View, m);
    this->refGfxServer->SetTransform(nGfxServer2::Projection, m);

    vector2 pos0, pos1;
    if ( displayFullScreen )
    {
        pos0 = vector2(-0.99f, -0.99f);
        pos1 = vector2( 0.99f,  0.99f);
    }
    else
    {
        pos0 = this->graphCornerNW;
        pos1 = this->graphCornerSE;
    }

    vector2 extents = pos1 - pos0;

    if ( this->numGraphsActive > 0 )
    {
        /*
            Draw scale of the first graph
        */
        int n = 10; // number of divisions fixed to 10. This expression makes them somewhat dynamic-> max(2, int(abs(extents.y) * 20));
        float dy = - extents.y / ( n-1 );
        GraphInfo* curGraph = measures[this->currentGraph];
        float v = curGraph->minValue;
        float dv = ( curGraph->maxValue - curGraph->minValue ) / n;
        vector2 labelPos( pos0.x - extents.x * 0.25f, pos1.y );
        vector4 scaleColor(curGraph->color);
        scaleColor.w = 0.5;

        for (int i=0; i < n; i++)
        {
            this->vertexArray[0] = vector3(pos0.x - extents.x * 0.01f, labelPos.y, 0.0f);
            this->vertexArray[1] = vector3(pos1.x, labelPos.y, 0.0f);
            lh.BeginLines( m );
            lh.DrawLines3d( &this->vertexArray[0], 0, 2, scaleColor);
            lh.EndLines();

            this->labelString.SetFloat( v );
            nGfxServer2::Instance()->Text(this->labelString.Get(), curGraph->color, labelPos.x, -labelPos.y - dy * 0.5f);

            v += dv;
            labelPos.y += dy;
        }
    }

    /*
        Draw border
    */
    this->vertexArray[0] = vector3(pos0.x, pos0.y, 0.0f);
    this->vertexArray[1] = vector3(pos1.x, pos0.y, 0.0f);
    this->vertexArray[2] = vector3(pos1.x, pos1.y, 0.0f);
    this->vertexArray[3] = vector3(pos0.x, pos1.y, 0.0f);
    this->vertexArray[4] = this->vertexArray[0];

    /*
    @todo drawing lines 2d doesn't work
    nArray<vector2> v2d;
    for (int i=0;i<this->vertexArray.Size();i++)
    {
        vector3 p( this->vertexArray[i]);
        v2d.Append(vector2(p.x,p.y));
    }
    nGfxServer2::Instance()->BeginLines();
    nGfxServer2::Instance()->DrawLines2d( &v2d[0], 5, this->borderColor);
    nGfxServer2::Instance()->EndLines();
    */

    lh.BeginLines( m );
    lh.DrawLines3d( &this->vertexArray[0], 0, 5, this->borderColor);
    lh.EndLines();

    /*
        Draw graphs
    */
    nTime t = this->refTimeServer->GetTime();
    nTime t0 = t - this->historyPeriod;
    int n = this->vertexArray.Size();
    for (int g = 0; g < measures.Size(); g++)
    {
        GraphInfo& graph = *measures[g];
        if ( ! graph.active )
        {
            continue;
        }

        int i = 0;
        vector3 curPos;
        curPos.z = 0.0f;
        vector2* cur = graph.history.GetHead();
        //MBATLE float incX = extents.x  * static_cast<float>(graph.samplePeriod) / static_cast<float>(this->historyPeriod);
        while ( cur && i < n && cur->y > t0)
        {
            curPos.x = pos1.x - extents.x * (static_cast<float>(t) - cur->y) / static_cast<float>(this->historyPeriod);
            curPos.y = pos1.y - extents.y * cur->x;
            this->vertexArray[i++] = curPos;
            cur = graph.history.GetPrev( cur );
        }
        if ( i > 1 )
        {
            lh.BeginLines( m );
            lh.DrawLines3d( &this->vertexArray[0], 0, i, graph.color);
            lh.EndLines();
        }
    }
}

//------------------------------------------------------------------------------
int
nMonitorServer::AddGraph(const char * name, float samplePeriod, float smooth, vector4 color)
{
    nString pathname("/sys/var/");
    pathname += name;

    int n = measures.Size();
    GraphInfo* info = n_new(GraphInfo);
    measures.Append(info);
    
    info->refEnv = pathname.Get();
    if ( !info->refEnv.isvalid() )
    {
        measures.Erase( n );
        return -1;
    }
    nArg::Type type = info->refEnv->GetType();
    if ( type != nArg::Int && type != nArg::Float )
    {
        measures.Erase( n );
        return -1;
    }
    info->rangeValue = 0.0f;
    info->lastValue = 0.0f;
    info->totalSamples = 0;
    info->sampleAcc = 0.0;
    info->meanValue = 0.0f;
    info->minValue = 0.0f;
    info->maxValue = 0.0f;
    this->numGraphsActive++;

    info->active = true;

    this->SetGraphParameters( n, samplePeriod, smooth, color);

    if ( measures.Size() == 1 )
    {
        this->currentGraph = 0;
    }
    return n;
}

//------------------------------------------------------------------------------
void
nMonitorServer::RemoveGraph(int i)
{
    if ( 0<=i && i < measures.Size() )
    {
        if ( measures[i]->active )
        {
            numGraphsActive--;
        }
        measures.Erase( i );

        if ( this->currentGraph >= measures.Size() )
        {
            this->currentGraph--;
        }
        if ( measures.Size() == 0 )
        {
            this->currentGraph = -1;
        }
    }
}

//------------------------------------------------------------------------------
int
nMonitorServer::GetNumGraphs()
{
    return measures.Size();
}

//------------------------------------------------------------------------------
void
nMonitorServer::SetVisualParameters(vector2 graphCornerNW, vector2 graphCornerSE, vector4 color)
{
    this->graphCornerNW = graphCornerNW;
    this->graphCornerSE = graphCornerSE;
    this->borderColor = color;
}

//------------------------------------------------------------------------------
void
nMonitorServer::SetGraphParameters(int i, float samplePeriod, float smooth, vector4 color)
{
    n_assert( 0<=i && i < measures.Size() );
    GraphInfo& info = *measures.At( i );

    // A period under a millisecond is supposed to be per-frame sampling
    if ( samplePeriod <= 0.001f )
    {
        samplePeriod = 0.0f;
    }
    info.samplePeriod = samplePeriod;
    info.smooth = smooth;
    info.color = color;

    int size = 0;
    if ( info.samplePeriod != 0.0f )
    {
        size = min( int( this->historyPeriod / samplePeriod ), 1000000);
        info.nextSampleTime = this->refTimeServer->GetTime() + info.samplePeriod;
    }
    else
    {
        // If per-frame sampling, calc. history buffer size with frame time
        //MBATLE nTime ft = this->refTimeServer->GetFrameTime();
        size = min( int( this->historyPeriod * 1000.0f ), 1000000);
    }
    info.history.~nRingBuffer();
    info.history.Initialize( size );

    // Calc. vertex array size
    if ( this->longestHistory < size )
    {
        this->longestHistory = size;
        this->vertexArray.Reallocate( this->longestHistory, this->longestHistory / 2 );
        this->vertexArray.At( this->longestHistory - 1 );
    }

    // Calc. slowest graph
    this->slowestGraph = 0;
    int n = this->measures.Size();
    for (int j = 1; j < n; j++)
    {
        if ( this->measures[this->slowestGraph]->samplePeriod < this->measures[j]->samplePeriod )
        {
            this->slowestGraph = j;
        }
    }
}

//------------------------------------------------------------------------------
void
nMonitorServer::GetGraphParameters(int i, float& samplePeriod, float& smooth, vector4& color)
{
    n_assert( 0<=i && i < measures.Size() );

    GraphInfo* info = measures.At( i );
    color = info->color;
    samplePeriod = static_cast<float> (info->samplePeriod);
    smooth = info->smooth;
}

//------------------------------------------------------------------------------
/**
    @brief get variable name referenced by a graph
*/
const char *
nMonitorServer::GetGraphVarName(int i)
{
    n_assert( 0<=i && i < measures.Size() );

    return measures[i]->refEnv->GetName();
}

//------------------------------------------------------------------------------
/**
    @brief Set activation flag of a graph (nothing is sampled or displayed)
*/
bool
nMonitorServer::GetGraphActive(int i)
{
    n_assert( 0<=i && i < measures.Size() );
    return measures[i]->active;
}

//------------------------------------------------------------------------------
/**
    @brief Get global display  flag of the server 
*/
bool
nMonitorServer::GetDisplayActivated()
{
    return this->displayActivated;
}

//------------------------------------------------------------------------------
/**
    @brief Set global display flag of the server 
*/
void
nMonitorServer::SetDisplayActivated(bool activated)
{
    this->displayActivated = activated;
    this->ResetAllHistory();
}

//------------------------------------------------------------------------------
/**
    @brief Get global sampling flag of the server
*/
bool
nMonitorServer::GetSamplingActivated()
{
    return this->samplingActivated;
}

//------------------------------------------------------------------------------
/**
    @brief Set global sampling flag of the server 
*/
void
nMonitorServer::SetSamplingActivated(bool activated)
{
    this->samplingActivated = activated;
    this->ResetAllHistory();
}

//------------------------------------------------------------------------------
/**
    @brief Get full screen flag
*/
bool
nMonitorServer::GetFullScreen()
{
    return this->displayFullScreen;
}

//------------------------------------------------------------------------------
/**
    @brief Set full screen flag
*/
void
nMonitorServer::SetFullScreen(bool activated)
{
    this->displayFullScreen = activated;
}

//------------------------------------------------------------------------------
/**
    @brief Set active flag for a graph (not active graphs are not sampled or displayed)
*/
void
nMonitorServer::SetGraphActive(int i, bool active)
{
    n_assert( 0<=i && i < measures.Size() );

    if ( measures[i]->active != active )
    {
        if ( active )
        {
            this->numGraphsActive++;
        }
        else
        {
            this->numGraphsActive--;
        }
    }
    measures[i]->active = active;
}

//------------------------------------------------------------------------------
/**
    @brief Get common history time ( the time length of all graphs)
*/
float
nMonitorServer::GetHistoryPeriod()
{
    return static_cast<float> (this->historyPeriod);
}

//------------------------------------------------------------------------------
/**
    @brief Set common history time ( the time length of all graphs)
*/
void
nMonitorServer::SetHistoryPeriod(float t)
{
    n_assert( t > 0.5 && t < 100.0f);
    this->historyPeriod = t;
}
//------------------------------------------------------------------------------
/**
    @brief Reset all captured sample related data of all graphs
*/
void
nMonitorServer::ResetAllHistory()
{
    for (int g = 0; g < this->measures.Size(); g++)
    {
        GraphInfo& graph = *this->measures[g];
        graph.rangeValue = 0.0f;
        graph.minValue = 0.0f;
        graph.maxValue = 0.0f;
        graph.meanValue = 0.0f;
        graph.lastValue = 0.0f;
        graph.totalSamples = 0;
        graph.sampleAcc = 0.0;
        graph.history.Clear();
    }
}

//------------------------------------------------------------------------------
float
nMonitorServer::GetGraphMinValue(int g)
{
    n_assert( 0 <= g && g < measures.Size() );
    return measures[g]->minValue;
}

//------------------------------------------------------------------------------
float
nMonitorServer::GetGraphMaxValue(int g)
{
    n_assert( 0 <= g && g < measures.Size() );
    return measures[g]->maxValue;
}

//------------------------------------------------------------------------------
float
nMonitorServer::GetGraphMeanValue(int g)
{
    n_assert( 0 <= g && g < measures.Size() );
    return measures[g]->meanValue;
}

//------------------------------------------------------------------------------
/**
    @brief Set the graph shown with scale
*/
void
nMonitorServer::SetCurrentGraph(int g)
{
    n_assert( 0 <= g && g < measures.Size() );
    this->currentGraph = g;
}

//------------------------------------------------------------------------------
/**
    @brief Get the graph shown with scale
*/
int
nMonitorServer::GetCurrentGraph()
{
    return this->currentGraph;
}

//------------------------------------------------------------------------------
/**
    @brief Get the statistics of current graph (the one shown on top)
*/
void
nMonitorServer::GetCurrentGraphStats(float& minVal, float& maxVal, float& meanVal, float& lastVal)
{
    n_assert(0 <= this->currentGraph && this->currentGraph < measures.Size());
    //MBATLE if ( 0 <= this->currentGraph && this->currentGraph < measures.Size() );
    GraphInfo& info = *measures[this->currentGraph];
    minVal = info.minValue;
    maxVal = info.maxValue;
    meanVal = info.meanValue;
    lastVal = info.lastValue;
}
