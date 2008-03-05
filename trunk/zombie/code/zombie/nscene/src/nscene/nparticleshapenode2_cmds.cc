#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nparticleshapenode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nparticleshapenode2.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nParticleShapeNode2)
    NSCRIPT_ADDCMD('SINV', void, SetInvisible, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GINV', bool, GetInvisible, 0, (), 0, ());
    NSCRIPT_ADDCMD('SEMD', void, SetEmissionDuration, 1, (nTime), 0, ());
    NSCRIPT_ADDCMD('GEMD', nTime, GetEmissionDuration, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLOP', void, SetLoop, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GLOP', bool, GetLoop, 0, (), 0, ());
    NSCRIPT_ADDCMD('SACD', void, SetActivityDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GACD', float, GetActivityDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD('SROF', void, SetRenderOldestFirst, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GROF', bool, GetRenderOldestFirst, 0, (), 0, ());
    NSCRIPT_ADDCMD('SRMN', void, SetStartRotationMin, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GRMN', float, GetStartRotationMin, 0, (), 0, ());
    NSCRIPT_ADDCMD('SRMX', void, SetStartRotationMax, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GRMX', float, GetStartRotationMax, 0, (), 0, ());
    NSCRIPT_ADDCMD('SGRV', void, SetGravity, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GGRV', float, GetGravity, 0, (), 0, ());
    NSCRIPT_ADDCMD('SPST', void, SetParticleStretch, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GPST', float, GetParticleStretch, 0, (), 0, ());
    NSCRIPT_ADDCMD('STTX', void, SetTileTexture, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GTTX', int, GetTileTexture, 0, (), 0, ());
    NSCRIPT_ADDCMD('SSTS', void, SetStretchToStart, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GSTS', bool, GetStretchToStart, 0, (), 0, ());
    NSCRIPT_ADDCMD('SSCV', void, SetStretchUsingCurve, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GSCV', bool, GetStretchUsingCurve, 0, (), 0, ());
    NSCRIPT_ADDCMD('SPCT', void, SetPrecalcTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GPCT', float, GetPrecalcTime, 0, (), 0, ());
    NSCRIPT_ADDCMD('SSDT', void, SetStretchDetail, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GSDT', int, GetStretchDetail, 0, (), 0, ());
    NSCRIPT_ADDCMD('SVAF', void, SetViewAngleFade, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GVAF', bool, GetViewAngleFade, 0, (), 0, ());
    NSCRIPT_ADDCMD('STDL', void, SetStartDelay, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GTDL', float, GetStartDelay, 0, (), 0, ());
    NSCRIPT_ADDCMD('SCVA', void, SetEmissionFrequency, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVA', void, GetEmissionFrequency, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVB', void, SetParticleLifeTime, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVB', void, GetParticleLifeTime, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVC', void, SetParticleRGB, 6, (const vector3&, const vector3&, const vector3&, const vector3&, float, float), 0, ());
    NSCRIPT_ADDCMD('GCVC', void, GetParticleRGB, 0, (), 6, (vector3&, vector3&, vector3&, vector3&, float&, float&));
    NSCRIPT_ADDCMD('SCVD', void, SetParticleSpreadMin, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVD', void, GetParticleSpreadMin, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVE', void, SetParticleSpreadMax, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVE', void, GetParticleSpreadMax, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVF', void, SetParticleStartVelocity, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVF', void, GetParticleStartVelocity, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVH', void, SetParticleRotationVelocity, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVH', void, GetParticleRotationVelocity, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVJ', void, SetParticleSize, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVJ', void, GetParticleSize, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVL', void, SetParticleMass, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVL', void, GetParticleMass, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('STMM', void, SetTimeManipulator, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GTMM', void, GetTimeManipulator, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVM', void, SetParticleAlpha, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVM', void, GetParticleAlpha, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVN', void, SetParticleVelocityFactor, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVN', void, GetParticleVelocityFactor, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVQ', void, SetParticleAirResistance, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVQ', void, GetParticleAirResistance, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVV', void, SetParticleStretchVelocity, 9, (float, float, float, float, float, float, float, float, int), 0, ());
    NSCRIPT_ADDCMD('GCVV', void, GetParticleStretchVelocity, 0, (), 9, (float&, float&, float&, float&, float&, float&, float&, float&, int&));
    NSCRIPT_ADDCMD('SCVR', void, SetParticleVelocityRandomize, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GCVR', float, GetParticleVelocityRandomize, 0, (), 0, ());
    NSCRIPT_ADDCMD('SCVS', void, SetParticleRotationRandomize, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GCVS', float, GetParticleRotationRandomize, 0, (), 0, ());
    NSCRIPT_ADDCMD('SCVT', void, SetParticleSizeRandomize, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GCVT', float, GetParticleSizeRandomize, 0, (), 0, ());
    NSCRIPT_ADDCMD('SCVU', void, SetRandomRotDir, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GCVU', bool, GetRandomRotDir, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('CSFS', void, SetFuncSrcBlend, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CGFS', int, GetFuncSrcBlend, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSFD', void, SetFuncDestBlend, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CGFD', int, GetFuncDestBlend, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSPT', void, SetParticleTexture, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('CGPT', nString, GetParticleTexture, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSPM', void, SetParticleMesh, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('CGPM', nString, GetParticleMesh, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
void
P2GetCurveParameters(const nEnvelopeCurve& curve, float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    keyFrameValue0 = curve.keyFrameValues[0];
    keyFrameValue1 = curve.keyFrameValues[1];
    keyFrameValue2 = curve.keyFrameValues[2];
    keyFrameValue3 = curve.keyFrameValues[3];
    keyFramePos1 = curve.keyFramePos1;
    keyFramePos2 = curve.keyFramePos2;
    frequency = curve.frequency;
    amplitude = curve.amplitude;
    modulationFunc = curve.modulationFunc;
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetEmissionFrequency(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::EmissionFrequency, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetEmissionFrequency(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::EmissionFrequency);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleLifeTime(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleLifeTime, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleLifeTime(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleLifeTime);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleRGB(const vector3& keyFrameValue0, const vector3& keyFrameValue1, const vector3& keyFrameValue2, const vector3& keyFrameValue3, float keyFramePos1, float keyFramePos2)
{
    this->SetRGBCurve(nVector3EnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleRGB(vector3& keyFrameValue0, vector3& keyFrameValue1, vector3& keyFrameValue2, vector3& keyFrameValue3, float& keyFramePos1, float& keyFramePos2)
{
    const nVector3EnvelopeCurve& curve = this->GetRGBCurve();
    keyFrameValue0 = curve.keyFrameValues[0];
    keyFrameValue1 = curve.keyFrameValues[1];
    keyFrameValue2 = curve.keyFrameValues[2];
    keyFrameValue3 = curve.keyFrameValues[3];
    keyFramePos1 = curve.keyFramePos1;
    keyFramePos2 = curve.keyFramePos2;
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleSpreadMin(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleSpreadMin, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleSpreadMin(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleSpreadMin);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleSpreadMax(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleSpreadMax, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleSpreadMax(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleSpreadMax);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleStartVelocity(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleStartVelocity, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleStartVelocity(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleStartVelocity);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleRotationVelocity(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleRotationVelocity, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleRotationVelocity(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleRotationVelocity);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleSize(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleScale, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleSize(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleScale);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleAlpha(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleAlpha, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleAlpha(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleAlpha);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleVelocityFactor(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleVelocityFactor, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleVelocityFactor(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleVelocityFactor);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleMass(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleMass, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleMass(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleMass);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetTimeManipulator(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::TimeManipulator, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetTimeManipulator(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::TimeManipulator);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleAirResistance(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleAirResistance, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleAirResistance(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleAirResistance);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetParticleStretchVelocity(float keyFrameValue0, float keyFrameValue1, float keyFrameValue2, float keyFrameValue3, float keyFramePos1, float keyFramePos2, float frequency, float amplitude, int modulationFunc)
{
    this->SetCurve(nParticle2Emitter::ParticleStretchVelocity, nEnvelopeCurve(keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::GetParticleStretchVelocity(float& keyFrameValue0, float& keyFrameValue1, float& keyFrameValue2, float& keyFrameValue3, float& keyFramePos1, float& keyFramePos2, float& frequency, float& amplitude, int& modulationFunc)
{
    const nEnvelopeCurve& curve = this->GetCurve(nParticle2Emitter::ParticleStretchVelocity);
    P2GetCurveParameters(curve, keyFrameValue0, keyFrameValue1, keyFrameValue2, keyFrameValue3, keyFramePos1, keyFramePos2, frequency, amplitude, modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
P2SetCurveCmdInput(nCmd* cmd, const nEnvelopeCurve& curve)
{
    cmd->In()->SetF(curve.keyFrameValues[0]);
    cmd->In()->SetF(curve.keyFrameValues[1]);
    cmd->In()->SetF(curve.keyFrameValues[2]);
    cmd->In()->SetF(curve.keyFrameValues[3]);
    cmd->In()->SetF(curve.keyFramePos1);
    cmd->In()->SetF(curve.keyFramePos2);
    cmd->In()->SetF(curve.frequency);
    cmd->In()->SetF(curve.amplitude);
    cmd->In()->SetI(curve.modulationFunc);
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    set the path of diffMap texture
*/
void 
nParticleShapeNode2::SetParticleTexture(const char* path)
{
    nString newPath(path);
    if (!nFileServer2::Instance()->UnManglePath(newPath , "home"))
    {
        nFileServer2::Instance()->UnManglePath(newPath , "wctextures");
    }
    this->SetTexture(nShaderState::diffMap, newPath.Get());
}

//------------------------------------------------------------------------------
/**
    Returns the path of diffMap texture
*/
nString
nParticleShapeNode2::GetParticleTexture()
{
    return nFileServer2::Instance()->ManglePath( this->GetTexture(nShaderState::diffMap) );  
}

//------------------------------------------------------------------------------
/**
    set the path of mesh
    Each vertex of mesh is a spawn point
*/
void 
nParticleShapeNode2::SetParticleMesh(const char* path)
{
    nString newPath(path);
    if (newPath.CheckExtension( "nvx2") || newPath.CheckExtension("n3d2"))
    {
        if (!nFileServer2::Instance()->UnManglePath(newPath , "home"))
        {
            nFileServer2::Instance()->UnManglePath(newPath , "wc");
        }
        this->SetMesh(newPath.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Returns the path of mesh
*/
nString
nParticleShapeNode2::GetParticleMesh()
{
    return nFileServer2::Instance()->ManglePath(this->GetMesh());  
}

//------------------------------------------------------------------------------
/**
*/
int
nParticleShapeNode2::GetFuncSrcBlend()
{
    if (this->HasParam(nShaderState::funcSrcBlend))
    {
        return this->GetInt(nShaderState::funcSrcBlend);
    }
    return 5; /*SRCALPHA*/
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetFuncSrcBlend(int function)
{
    this->SetInt(nShaderState::funcSrcBlend, function);
}

//------------------------------------------------------------------------------
/**
*/
int
nParticleShapeNode2::GetFuncDestBlend()
{
    if (this->HasParam(nShaderState::funcDestBlend))
    {
        return this->GetInt(nShaderState::funcDestBlend);
    }
    return 6; /*INVSRCALPHA*/
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::SetFuncDestBlend(int function)
{
    this->SetInt(nShaderState::funcDestBlend, function);
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool
nParticleShapeNode2::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setemissionfrequency ---
        cmd = ps->GetCmd(this, 'SCVA');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::EmissionFrequency]);
        ps->PutCmd(cmd);

        //--- setparticlelifetime ---
        cmd = ps->GetCmd(this, 'SCVB');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleLifeTime]);
        ps->PutCmd(cmd);

        //--- set spread min ---
        cmd = ps->GetCmd(this, 'SCVD');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleSpreadMin]);
        ps->PutCmd(cmd);

        //--- set spread max ---
        cmd = ps->GetCmd(this, 'SCVE');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleSpreadMax]);
        ps->PutCmd(cmd);

        //--- set start velocity ---
        cmd = ps->GetCmd(this, 'SCVF');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleStartVelocity]);
        ps->PutCmd(cmd);

        //--- set rotation velocity ---
        cmd = ps->GetCmd(this, 'SCVH');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleRotationVelocity]);
        ps->PutCmd(cmd);

        //--- set scale ---
        cmd = ps->GetCmd(this, 'SCVJ');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleScale]);
        ps->PutCmd(cmd);

        //--- set alpha ---
        cmd = ps->GetCmd(this, 'SCVM');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleAlpha]);
        ps->PutCmd(cmd);

        //--- set mass ---
        cmd = ps->GetCmd(this, 'SCVL');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleMass]);
        ps->PutCmd(cmd);

        //--- set time manipulator ---
        cmd = ps->GetCmd(this, 'STMM');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::TimeManipulator]);
        ps->PutCmd(cmd);

        //--- set velocity factor ---
        cmd = ps->GetCmd(this, 'SCVN');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleVelocityFactor]);
        ps->PutCmd(cmd);

        //--- set air resistance ---
        cmd = ps->GetCmd(this, 'SCVQ');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleAirResistance]);
        ps->PutCmd(cmd);

        //--- set stretch velocity ---
        cmd = ps->GetCmd(this, 'SCVV');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleStretchVelocity]);
        ps->PutCmd(cmd);

        //--- setparticlergb ---
        cmd = ps->GetCmd(this, 'SCVC');
        const nVector3EnvelopeCurve& curve = this->rgbCurve;
        int idx;
        for (idx = 0; idx < 4; idx++)
        {
            cmd->In()->SetF(curve.keyFrameValues[idx].x);
            cmd->In()->SetF(curve.keyFrameValues[idx].y);
            cmd->In()->SetF(curve.keyFrameValues[idx].z);
        }
        cmd->In()->SetF(curve.keyFramePos1);
        cmd->In()->SetF(curve.keyFramePos2);
        ps->PutCmd(cmd);


        //--- setemissionduration ---
        cmd = ps->GetCmd(this, 'SEMD');
        cmd->In()->SetF((float) this->GetEmissionDuration());
        ps->PutCmd(cmd);

        //--- setloop ---
        cmd = ps->GetCmd(this, 'SLOP');
        cmd->In()->SetB(this->GetLoop());
        ps->PutCmd(cmd);

        //--- setactivitydistance ---
        cmd = ps->GetCmd(this, 'SACD');
        cmd->In()->SetF(this->GetActivityDistance());
        ps->PutCmd(cmd);

        //--- setrenderoldestfirst ---
        cmd = ps->GetCmd(this, 'SROF');
        cmd->In()->SetB(this->GetRenderOldestFirst());
        ps->PutCmd(cmd);

        //--- set rotation min ---
        cmd = ps->GetCmd(this, 'SRMN');
        cmd->In()->SetF((float) this->startRotationMin);
        ps->PutCmd(cmd);

        //--- set rotation max ---
        cmd = ps->GetCmd(this, 'SRMX');
        cmd->In()->SetF((float) this->startRotationMax);
        ps->PutCmd(cmd);

        //--- set gravity ---
        cmd = ps->GetCmd(this, 'SGRV');
        cmd->In()->SetF((float) this->gravity);
        ps->PutCmd(cmd);

        //--- set stretch ---
        cmd = ps->GetCmd(this, 'SPST');
        cmd->In()->SetF((float) this->particleStretch);
        ps->PutCmd(cmd);

        //--- set tile texture ---
        cmd = ps->GetCmd(this, 'STTX');
        cmd->In()->SetI((int) this->tileTexture);
        ps->PutCmd(cmd);

        //--- set stretch to start ---
        cmd = ps->GetCmd(this, 'SSTS');
        cmd->In()->SetB((bool) this->stretchToStart);
        ps->PutCmd(cmd);

        //--- set stretch using curve ---
        cmd = ps->GetCmd(this, 'SSCV');
        cmd->In()->SetB((bool) this->stretchUsingCurve);
        ps->PutCmd(cmd);

        //--- set velocity randomization  ---
        cmd = ps->GetCmd(this, 'SCVR');
        cmd->In()->SetF((float) this->particleVelocityRandomize);
        ps->PutCmd(cmd);

        //--- set rotation randomization ---
        cmd = ps->GetCmd(this, 'SCVS');
        cmd->In()->SetF((float) this->particleRotationRandomize);
        ps->PutCmd(cmd);

        //--- set size randomization ---
        cmd = ps->GetCmd(this, 'SCVT');
        cmd->In()->SetF((float) this->particleSizeRandomize);
        ps->PutCmd(cmd);

        //--- set precalculation time ---
        cmd = ps->GetCmd(this, 'SPCT');
        cmd->In()->SetF((float) this->precalcTime);
        ps->PutCmd(cmd);

        //--- set random rotation dir ---
        cmd = ps->GetCmd(this, 'SCVU');
        cmd->In()->SetB((bool) this->randomRotDir);
        ps->PutCmd(cmd);

        //--- set stretch detail ---
        cmd = ps->GetCmd(this, 'SSDT');
        cmd->In()->SetI((int) this->stretchDetail);
        ps->PutCmd(cmd);

        //--- set view angle fade ---
        cmd = ps->GetCmd(this, 'SVAF');
        cmd->In()->SetB((bool) this->viewAngleFade);
        ps->PutCmd(cmd);

        //--- set start delay ---
        cmd = ps->GetCmd(this, 'STDL');
        cmd->In()->SetF((float) this->startDelay);
        ps->PutCmd(cmd);

        return true;
    }

    return false;
}
