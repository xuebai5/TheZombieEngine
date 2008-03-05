//------------------------------------------------------------------------------
//  nparticle2emitter_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "particle/nparticle2emitter.h"
#include "particle/nparticle2.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
/**
*/
nParticle2Emitter::nParticle2Emitter():
    loop(true),
    activityDistance(100.0f),
    emissionDuration(10.0),
    startTime(-1.0),
    lastEmission(0.0),
    lastEmissionVertex(0),
    isOpen(false),
    meshGroupIndex(0),
    remainingTime(0),
    isSetUp(false),
    renderOldestFirst(false),
    tileTexture(1),
    stretchToStart(false),
    stretchUsingCurve(false),
    particles(0),
    particleVelocityRandomize(0),
    particleRotationRandomize(0),
    particleStretch(0),
    particleSizeRandomize(0),
    randomRotDir(false),
    frameWasRendered(true),
    hasLooped(false),
    invisibleTime(0),
    isSleeping(false),
    precalcTime(0),
    viewAngleFade(false),
    stretchDetail(1),
    dynMesh(),
    wind(),
    pStaticCurve(0),
    gravity(0),
    startRotationMin(0),
    startRotationMax(0),
    startDelay(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nParticle2Emitter::~nParticle2Emitter()
{
//    if (this->isOpen) n_delete_array(this->particles);
    if (0 != this->particles)
    {
        n_delete_array(this->particles);
    }
    this->particles = 0;
}

//------------------------------------------------------------------------------
/**
    Update existing particles, remove if expired
*/
void
nParticle2Emitter::CalculateStep(float fdTime)
{
    if (this->particleCount == 0)
    {
        return;
    }

    // because the time can be negative, we need an absolute value to advance the lifetime of the particle
    float fdAbsTime = fabsf(fdTime);

    vector3 windVek = vector3(this->wind.x, this->wind.y, this->wind.z) * this->wind.w;
    vector3 acc;
    vector3 freeacc;
    vector3 boundMin = this->particles[0].position;
    vector3 boundMax = this->particles[0].position;

    nParticle2* partSrc = this->particles;
    nParticle2* part = this->particles;
    int p;
    for (p = 0; p < this->particleCount; ++p)
    {
        *part = *partSrc;
        part->lifeTime += (float) fdAbsTime;
        float relParticleAge = part->lifeTime * part->oneDivMaxLifeTime;
        int curveIndex = n_iclamp((int) (relParticleAge * (float) PARTICLE_TIME_DETAIL), 0, PARTICLE_TIME_DETAIL - 1);
        float* curCurves = &this->pStaticCurve[curveIndex * CurveTypeCount];

        if ((relParticleAge < 1.0f) && (relParticleAge >= 0.0f))
        {
            acc = windVek * curCurves[ParticleAirResistance];
            acc.y += this->gravity;
            acc *= curCurves[ParticleMass];

            // calc new values
            part->acc = acc;
            part->position += part->velocity * (fdTime * curCurves[ParticleVelocityFactor]);
            part->velocity += acc * fdTime;
            part->rotation += curCurves[ParticleRotationVelocity] * part->rotationVariation * fdTime;

            // update boundary values
            if (part->position.x < boundMin.x) boundMin.x = part->position.x;
            if (part->position.x > boundMax.x) boundMax.x = part->position.x;
            if (part->position.y < boundMin.y) boundMin.y = part->position.y;
            if (part->position.y > boundMax.y) boundMax.y = part->position.y;
            if (part->position.z < boundMin.z) boundMin.z = part->position.z;
            if (part->position.z > boundMax.z) boundMax.z = part->position.z;

            part++;
        }
        else
        {
            // particle's lifetime is over
            // part does not get advanced
            this->particleCount--;
            p--;
        }

        partSrc++;
    }

    // apply boundary changes
    this->box.extend(boundMin);
    this->box.extend(boundMax);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticle2Emitter::Trigger(nTime triggerTime)
{
    if (!this->isSetUp)
    {
        return;
    }
   
    nTime curTime = triggerTime;
    float triggerTimePitch = 0.0f;
    int triggerSteps = 1;

    bool firstRun = false;
    if (!this->isOpen)
    {
        this->Open();
        firstRun = true;
        n_assert(this->isOpen);
        if (!this->isOpen)
        {
            return;
        }
    }

    if (firstRun || (curTime < this->lastEmission))
    {
        // Time reset

        // ok, the emitter is run for the first time or there has been a time reset. 
        // eventually, we need to calculate some steps in advance

        if (this->precalcTime != 0.0f)
        {
            triggerTimePitch = 0.1f; // calculate 1/10s steps
            triggerSteps = (int) (this->precalcTime / triggerTimePitch) + 1;
            curTime -= (triggerSteps - 1) * triggerTimePitch;
        }

        this->particleCount = 0;
        this->startTime = curTime;
        this->lastEmission = curTime;
        this->remainingTime = 0;
    }

    if (!this->refMesh.isvalid()) // The mesh is no valid
    {
        return;
    }

    nMesh2* meshPtr = this->refMesh.get_unsafe();

    int trigCnt;
    for (trigCnt = 0; trigCnt < triggerSteps; ++trigCnt)
    {
        // calculate timestep
        nTime deltaTime = curTime - this->lastEmission;

        // fdTime is manipulated by TimeManipulator
        float fdTime = (float) deltaTime;

        // apply Time - Manipulator on deltaTime
        float relAge = (float) ((curTime - this->startTime - this->startDelay) / this->emissionDuration);
        int curveIndex = n_iclamp((int) (relAge * PARTICLE_TIME_DETAIL), 0, PARTICLE_TIME_DETAIL - 1);
        fdTime *= this->pStaticCurve[curveIndex * CurveTypeCount + TimeManipulator];

        float fdAbsTime = fabsf(fdTime);

        // check for activity distance
        const matrix44& viewer = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
	    vector3 emitterViewer = viewer.pos_component() - this->matrix.pos_component();

        // invisibility- and out-of-range-check:
        // check only if this is not initial triggering
        if ((triggerSteps == 1) && 
            // viewer out of range
            ((emitterViewer.len() >= this->activityDistance) ||
                // skip if invisible, looping and has looped at least once
                ((!this->frameWasRendered) && (this->loop) && (this->hasLooped))))
        {
            // adjust starttime by missed timedelta
            this->startTime += deltaTime;
            this->lastEmission += deltaTime;
            if (!this->frameWasRendered)
            {
                this->invisibleTime += (float) deltaTime;
            }
            else
            {
                this->invisibleTime = 0.0f;
            }

            // go to sleep after beeing invisible for too long
            if ((this->loop) && (this->hasLooped) && (!this->isSleeping) && (this->invisibleTime > 3.0f))
            {
                this->isSleeping = true;
                if (0 != this->particles)
                {
                    n_delete_array(this->particles);
                }

                this->particles = 0;
                this->particleCount = 0;
            }

            return;
        }

        this->frameWasRendered = false;
        this->invisibleTime = 0.0f;

        // calculation goes here...
        this->CalculateStep(fdTime);

        // emit new particles if we are inside the emissiontimeframe
        if ((curTime >= this->startTime) && (lastEmission < this->startTime + this->startDelay + this->emissionDuration))
        {
            if (curTime >= this->startTime + this->startDelay)
            {
                float relAge = (float)((curTime - this->startTime - this->startDelay) / this->emissionDuration);
                int curveIndex = n_iclamp((int)(relAge * PARTICLE_TIME_DETAIL), 0, PARTICLE_TIME_DETAIL - 1);
                float* curCurves = &this->pStaticCurve[curveIndex * CurveTypeCount];

                float *emitterVertices = meshPtr->LockVertices();
                int vertexWidth = meshPtr->GetVertexWidth();
                ushort* srcIndices = meshPtr->LockIndices();
                const nMeshGroup& meshGroup = meshPtr->Group(this->meshGroupIndex);
		        int firstIndex  = meshGroup.GetFirstIndex();
		        int meshIndexSize = meshGroup.GetNumIndices();

                float curEmissionFrequency = curCurves[EmissionFrequency];

                // for correct emission we perform 1s/freq - steps
                nTime timeToDo = fdAbsTime + this->remainingTime;
                if (0 < curEmissionFrequency)
                {
                    nTime emitTimePitch = 1.0 / curEmissionFrequency;

                    while (timeToDo >= emitTimePitch)
                    {
                        nTime particleEmissionLifeTime = timeToDo;
                        nTime oneDivLifeTime = 1.0f;
                        if (0 != curCurves[ParticleLifeTime])
                        {
                            oneDivLifeTime = 1.0f / curCurves[ParticleLifeTime];
                        }

                        if (this->particleCount < this->maxParticleCount)
                        {
                            // creation goes here...
                            nParticle2* newParticle = &this->particles[this->particleCount];
                            this->particleCount++;

                            int verticeNr = firstIndex + (rand() * (meshIndexSize-1)) / RAND_MAX;
                            int curIndex = srcIndices[verticeNr] * vertexWidth;
                            vector3 position = this->matrix * vector3(emitterVertices[curIndex+0],
                                                                      emitterVertices[curIndex+1],
                                                                      emitterVertices[curIndex+2]);

                            matrix33 m33 = matrix33(this->matrix.M11, this->matrix.M12, this->matrix.M13,
                                                    this->matrix.M21, this->matrix.M22, this->matrix.M23, 
                                                    this->matrix.M31, this->matrix.M32, this->matrix.M33);

                            vector3 normal = m33 * vector3(emitterVertices[curIndex+3], 
                                                           emitterVertices[curIndex+4],
                                                           emitterVertices[curIndex+5]);

                            // find orthogonal vectors to spread normal vector
                            vector3 ortho1, norm;
                            ortho1 = normal.findortho();
                            ortho1.norm();
                            norm = normal;
                            float spreadMin = curCurves[ParticleSpreadMin];
                            float spreadMax = curCurves[ParticleSpreadMax];
                            if (spreadMin>spreadMax)
                            {
                                spreadMin = spreadMax;
                            }
                            float spread = (((float)rand()) / ((float)RAND_MAX)) * (spreadMax - spreadMin) + spreadMin;
                            float rotRandom = ((float)rand()) / ((float)RAND_MAX) * 360.0f;
                            normal.rotate(ortho1, n_deg2rad(spread));
                            normal.rotate(norm, n_deg2rad(rotRandom));

                            float velocityVariation = 1.0f - ((float)rand()) / ((float)RAND_MAX) * this->particleVelocityRandomize;
                            float startVelocity = curCurves[ParticleStartVelocity] * velocityVariation;
                            
                            // apply texture tiling
                            // uvmax and uvmin are arranged a bit strange, because they need to be flipped 
                            // horizontally and be rotated
                            if (this->tileTexture < 1)
                            {
                                this->tileTexture = 1;
                            }
                            vector2 uvStep = vector2(0.0f, 1.0f / (float) this->tileTexture);
                            int tileNr = (rand() * this->tileTexture / (RAND_MAX+1));
                            newParticle->uvmin = vector2(1.0f, 0.0f) + uvStep * (float) tileNr;
                            newParticle->uvmax = vector2(0.0f, newParticle->uvmin.y) + uvStep;

                            newParticle->lifeTime = (float) particleEmissionLifeTime;
                            newParticle->oneDivMaxLifeTime = (float) oneDivLifeTime;
                            newParticle->position = position;
                            newParticle->rotation = ((float) rand()) / ((float)RAND_MAX) * (this->startRotationMax - this->startRotationMin) + this->startRotationMin;

                            // calculate velocity variation
                            newParticle->rotationVariation = 1.0f - ((float)rand()) / ((float)RAND_MAX) * this->particleRotationRandomize;
                            if (this->randomRotDir && (((float)rand()) / ((float)RAND_MAX) < 0.5f))
                            {
                                newParticle->rotationVariation = -newParticle->rotationVariation;
                            }
                            newParticle->velocity = normal * startVelocity;
                            newParticle->startPos = newParticle->position;
                            newParticle->sizeVariation = 1.0f - ((float)rand()) / ((float)RAND_MAX) * this->particleSizeRandomize;

                            // add velocity * lifetime
                            newParticle->position += newParticle->velocity * newParticle->lifeTime;
                        }

                        timeToDo -= emitTimePitch;
                    }
                }

                this->remainingTime = timeToDo;
  		        meshPtr->UnlockVertices();
		        meshPtr->UnlockIndices();
            }

            //n_printf("now %d/%d particles\n", this->particleCount, this->maxParticleCount);
        }
        else
        {
            if (this->loop)
            {
                this->startTime = curTime;
                this->remainingTime = 0;
                this->hasLooped = true;
            }
        }

        this->lastEmission = curTime;
        
        curTime += triggerTimePitch;
    }
}

//------------------------------------------------------------------------------
/**
    Render as pure quad
*/
int
nParticle2Emitter::RenderPure(float* dstVertices, int maxVertices)
{
//    int curIndex  = 0;
    int curVertex = 0;
    tParticleVertex myVertex;

    nParticle2* particle = this->particles;
    int particlePitch = 1;
    if (this->renderOldestFirst)
    {
        // reverse iterating order
        particlePitch = -1;
        particle = &this->particles[particleCount -1];
    }

    tParticleVertex* destPtr = (tParticleVertex*) dstVertices;

    int curveIndex;
    float* curCurves;

    const matrix44& viewer = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
    myVertex.vel = viewer.x_component();

    int p;
    for (p = 0; p < particleCount; ++p)
    {
        // life-time-check is not needed, it is assured that the relative age is >=0 and <1
        curveIndex = n_iclamp((int) ((particle->lifeTime * particle->oneDivMaxLifeTime) * (float) PARTICLE_TIME_DETAIL), 0, PARTICLE_TIME_DETAIL - 1);
        curCurves = &this->pStaticCurve[curveIndex * CurveTypeCount];

        // components common to particle vertices
        myVertex.pos = particle->position;
        myVertex.scale = curCurves[ParticleScale] * particle->sizeVariation;
        myVertex.color = curCurves[StaticRGBCurve];
        myVertex.alpha = curCurves[ParticleAlpha];

        // components per vertex
        myVertex.u = particle->uvmax.x;
        myVertex.v = particle->uvmin.y;
        myVertex.rotation = particle->rotation + PI/4.0f + PI/2.0f;
        destPtr[0] = myVertex;
        destPtr[3] = myVertex;

        myVertex.u = particle->uvmax.x;
        myVertex.v = particle->uvmax.y;
        myVertex.rotation += PI/2.0;
        destPtr[1] = myVertex;

        myVertex.u = particle->uvmin.x;
        myVertex.v = particle->uvmax.y;
        myVertex.rotation += PI/2.0;
        destPtr[2] = myVertex;
        destPtr[4] = myVertex;

        myVertex.u = particle->uvmin.x;
        myVertex.v = particle->uvmin.y;
        myVertex.rotation += PI/2.0;
        destPtr[5] = myVertex;

        destPtr += 6;

        curVertex += 6;
        if (curVertex > maxVertices-6)
        {
            this->dynMesh.Swap(curVertex, dstVertices);
            destPtr = (tParticleVertex*) dstVertices;
            curVertex = 0;
        }

        particle += particlePitch;
    }

    return curVertex;
}

//------------------------------------------------------------------------------
/**
    Render stretched
*/
int
nParticle2Emitter::RenderStretched(float* dstVertices,int maxVertices)
{
    int curVertex = 0;
    tParticleVertex myVertex;

    int particlePitch = 1;
    int particleOffset = 0;
    if (this->renderOldestFirst)
    {
        // reverse iterating order
        particlePitch = -1;
        particleOffset = particleCount -1;
    }

    float viewFadeOut = 0.0f;
    if (this->viewAngleFade)
    {
        viewFadeOut = 256.0f;
    }

    tParticleVertex* destPtr = (tParticleVertex*) dstVertices;

    // ok, let's stretch
    vector3 stretchPos;
    int p;
    for (p = 0; p < particleCount; ++p)
    {
        nParticle2* particle = &particles[particleOffset];

        // lifetime-check is not needed, it is assured that the relative age is >=0 and <1
        float relParticleAge = particle->lifeTime * particle->oneDivMaxLifeTime;
        int curveIndex = n_iclamp((int) (relParticleAge * (float) PARTICLE_TIME_DETAIL), 0, PARTICLE_TIME_DETAIL - 1);
        float* curCurves = &this->pStaticCurve[curveIndex * CurveTypeCount];

        if (this->stretchToStart)
        {
            // stretch to the start position, ignore velocity
            stretchPos = particle->startPos;
        }
        else if (this->stretchUsingCurve)
        {
            // get velocity for direction, magnitude from curve
            // uses particleStretch to modulate the effect
            float stretchTime = n_min(this->particleStretch, particle->lifeTime);
            vector3 velocity(particle->velocity);
            velocity.norm();
            stretchPos = particle->position - (velocity * curCurves[ParticleStretchVelocity] * stretchTime);
        }
        else
        {
            float stretchTime = n_min(this->particleStretch, particle->lifeTime);
            stretchPos = particle->position - (particle->velocity - particle->acc * (stretchTime * 0.5f)) * (stretchTime * curCurves[ParticleVelocityFactor]);
        }

        float alpha = curCurves[ParticleAlpha] + viewFadeOut;

        // components common to particle vertices
        myVertex.pos = particle->position;
        myVertex.scale = curCurves[ParticleScale] * particle->sizeVariation;
        myVertex.color = curCurves[StaticRGBCurve];
        myVertex.vel = particle->position - stretchPos;

        // components per vertex
        myVertex.u = particle->uvmax.x;
        myVertex.v = particle->uvmin.y;
        myVertex.rotation = PI/4.0;
        myVertex.alpha = alpha;
        destPtr[0] = myVertex;
        destPtr[3] = myVertex;

        myVertex.u = particle->uvmax.x;
        myVertex.v = particle->uvmax.y;
        myVertex.rotation += PI/2.0;
        myVertex.pos = stretchPos;
        destPtr[1] = myVertex;

        myVertex.u = particle->uvmin.x;
        myVertex.v = particle->uvmax.y;
        myVertex.rotation += PI/2.0;
        destPtr[2] = myVertex;
        destPtr[4] = myVertex;

        myVertex.u = particle->uvmin.x;
        myVertex.v = particle->uvmin.y;
        myVertex.rotation += PI/2.0f;
        myVertex.pos = particle->position;
        destPtr[5] = myVertex;
        
        curVertex += 6;
        destPtr += 6;

        if (curVertex > maxVertices-6)
        {
            this->dynMesh.Swap(curVertex, dstVertices);
            curVertex = 0;
            destPtr = (tParticleVertex*)dstVertices;
        }

        particleOffset += particlePitch;
    }

    return curVertex;
}

//------------------------------------------------------------------------------
/**
    Render stretched and smooth
*/
int
nParticle2Emitter::RenderStretchedSmooth(float* dstVertices, int maxVertices)
{
    int curVertex = 0;
    tParticleVertex myVertex;

    int particlePitch = 1;
    int particleOffset = 0;
    if (this->renderOldestFirst)
    {
        // reverse iterating order
        particlePitch = -1;
        particleOffset = particleCount -1;
    }

    float oneDivStretchDetail = 1.0f / (float) this->stretchDetail;

    // ok, let's stretch
    vector3 stretchPos;
    vector3 velPitch;
    vector3 velPitchHalf;

    // set coded flag for viewangle fading
    float viewFadeOut = 0.0f;
    if (this->viewAngleFade)
    {
        viewFadeOut = 256.0f;
    }

    tParticleVertex* destPtr = (tParticleVertex*) dstVertices;

    int p;
    for (p = 0; p < this->particleCount; ++p)
    {
        nParticle2* particle = &particles[particleOffset];
        float relParticleAge = particle->lifeTime * particle->oneDivMaxLifeTime;
        int curveIndex = n_iclamp((int) (relParticleAge * (float) PARTICLE_TIME_DETAIL), 0, PARTICLE_TIME_DETAIL - 1);
        float* curCurves = &this->pStaticCurve[curveIndex * CurveTypeCount];

        // calculate stretch steps
        float stretchTime = this->particleStretch;
        if (stretchTime > particle->lifeTime)
        {
            stretchTime = particle->lifeTime;
        }
        float stretchStep = -(stretchTime * oneDivStretchDetail);
        velPitch = particle->acc * stretchStep;
        velPitchHalf = velPitch * 0.5f;
        float stretchStepVel = stretchStep * curCurves[ParticleVelocityFactor];

        float scale = curCurves[ParticleScale] * particle->sizeVariation;
        float vPitch = (particle->uvmax.y - particle->uvmin.y) * oneDivStretchDetail;

        myVertex.color = curCurves[StaticRGBCurve];
        myVertex.v = particle->uvmin.y;
        myVertex.pos = particle->position;
        myVertex.vel = particle->velocity;

        myVertex.alpha = curCurves[ParticleAlpha] + viewFadeOut;
        myVertex.scale = scale;

        int d;
        for (d = 0; d<this->stretchDetail; d++)
        {
            // life-time-check is not needed, it is assured that the relative age is >=0 and <1
            myVertex.u = particle->uvmin.x;
            myVertex.rotation = 3.0f * PI/2.0f;
            destPtr[0] = myVertex;
            destPtr[3] = myVertex;

            myVertex.rotation = PI / 2.0f;
            myVertex.u = particle->uvmax.x;
            destPtr[5] = myVertex;

            myVertex.rotation = 3.0f * PI/2.0f;
            myVertex.u = particle->uvmin.x;
            myVertex.v += vPitch;
            myVertex.pos += (myVertex.vel + velPitchHalf) * stretchStepVel;
            myVertex.vel += velPitch;
            destPtr[1] = myVertex;

            myVertex.rotation = PI / 2.0f;
            myVertex.u = particle->uvmax.x;
            destPtr[2] = myVertex;
            destPtr[4] = myVertex;

            destPtr += 6;
            curVertex += 6;
            if (curVertex > maxVertices-6)
            {
                this->dynMesh.Swap(curVertex, dstVertices);
                destPtr = (tParticleVertex*) dstVertices;
                curVertex = 0;
            }
        }

        particleOffset += particlePitch;
    }

    return curVertex;
}

//------------------------------------------------------------------------------
/**
    Render the Particles
*/
void
nParticle2Emitter::Render(nTime curTime)
{
    nParticleServer2* particleServer = nParticleServer2::Instance();

    if (!particleServer->IsEnabled())
    {
        return;
    }

    if (!this->isSetUp || !this->isOpen)
    {
        return;
    }

    if (this->isSleeping) // do we have to wakeup ?
    {
        this->isSleeping = false;

        // reallocate particles
        this->particles = n_new_array(nParticle2, this->maxParticleCount);
        n_assert(0 != this->particles);

        this->frameWasRendered = true;

        // trigger with a little difference, so that the emitter will reset
        this->Trigger(curTime - 0.001f); 

        // ok, we're up-to-date again
    }

    if (!this->dynMesh.IsValid())
    {
        this->dynMesh.Initialize(nGfxServer2::TriangleList,
                                 nMesh2::Coord | nMesh2::Normal | nMesh2::Uv0 | nMesh2::Color,
                                 nMesh2::WriteOnly | nMesh2::NeedsVertexShader, false);
        n_assert(this->dynMesh.IsValid());
    }

    float* dstVertices = 0;
    int maxVertices = 0;
    int remVertices = 0;
    this->dynMesh.Begin(dstVertices, maxVertices);

    if ((this->particleStretch == 0.0f) && (!this->stretchToStart))
    {
        remVertices = RenderPure(dstVertices, maxVertices);
    }
    else 
    {
        if (this->stretchToStart || (this->stretchDetail == 1))
        {
            remVertices = RenderStretched(dstVertices, maxVertices);
        }
        else
        {
            remVertices = RenderStretchedSmooth(dstVertices, maxVertices);
        }
    }

    // Draw
    this->dynMesh.End(remVertices);
    this->frameWasRendered = true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nParticle2Emitter::AreResourcesValid()
{
//    return this->refEmitterMesh.isvalid();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nParticle2Emitter::Open()
{
    n_assert2(this->pStaticCurve, "The particle system has no static curve!");
    if (!this->pStaticCurve)
    {
        return;
    }

    // calculate maximum number of particles
    float maxFreq = 0;
    float maxLife = 0;
    int i;
    for (i = 0; i < PARTICLE_TIME_DETAIL; i++)
    {
        if (this->pStaticCurve[i * CurveTypeCount + EmissionFrequency] > maxFreq)
        {
            maxFreq = this->pStaticCurve[i * CurveTypeCount + EmissionFrequency];
        }
        if (this->pStaticCurve[i * CurveTypeCount + ParticleLifeTime] > maxLife)
        {
            maxLife = this->pStaticCurve[i * CurveTypeCount + ParticleLifeTime];
        }
    }
    this->maxParticleCount = (int) (maxFreq * maxLife);

    // allocate array
    this->particles = n_new_array(nParticle2, this->maxParticleCount);
    n_assert(0 != this->particles);

    // reset particles
    this->particleCount = 0;
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void    
nParticle2Emitter::CurvesChanged()
{
    if (this->particles != 0)
    {
        // we need to rearrange the particlearray, because the curves have changed
        float maxFreq = 0;
        float maxLife = 0;
        int i;
        for (i = 0; i < PARTICLE_TIME_DETAIL; ++i)
        {
            if (this->pStaticCurve[i * CurveTypeCount+EmissionFrequency] > maxFreq)
            {
                maxFreq = this->pStaticCurve[i * CurveTypeCount+EmissionFrequency];
            }
            if (this->pStaticCurve[i * CurveTypeCount+ParticleLifeTime] > maxLife)
            {
                maxLife = this->pStaticCurve[i * CurveTypeCount + ParticleLifeTime];
            }
        }

        int newMaxParticleCount = (int) (maxFreq * maxLife);

        // allocate array
        nParticle2* newPtr = n_new_array(nParticle2, newMaxParticleCount);
        n_assert(0 != newPtr);

        int partsToCopy = this->particleCount;
        if (partsToCopy > newMaxParticleCount)
        {
            partsToCopy = newMaxParticleCount;
        }

        memcpy(newPtr, this->particles, partsToCopy * sizeof(nParticle2));

        // delete old array
        n_delete_array(this->particles);

        // set new values
        this->particleCount = partsToCopy;
        this->particles = newPtr;
        this->maxParticleCount = newMaxParticleCount;
    }
}
