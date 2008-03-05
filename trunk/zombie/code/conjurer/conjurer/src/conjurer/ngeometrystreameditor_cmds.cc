#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ngeometrystreameditor_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ngeometrystreameditor.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ngeometrystreameditor

    @cppclass
    nGeometryStreamEditor

    @superclass
    nroot

    @classinfo
    A meta-description of geometry streams for editor only.
*/
NSCRIPT_INITCMDS_BEGIN(nGeometryStreamEditor)
    NSCRIPT_ADDCMD('BSTR', void, BeginGeometries, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SGAT', void, SetGeometryAt, 4, (int, nString&, int, nString&), 0, ());
    NSCRIPT_ADDCMD('GGAT', void, GetGeometryAt, 1, (int), 3, (nString&, int&, nString&));
    NSCRIPT_ADDCMD('SFAT', void, SetFrequencyAt, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GFAT', int, GetFrequencyAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESTR', void, EndGeometries, 0, (), 0, ());
    NSCRIPT_ADDCMD('APPS', void, ApplyStream, 0, (), 0, ());
    NSCRIPT_ADDCMD('REMS', void, RemoveStream, 0, (), 0, ());
    NSCRIPT_ADDCMD('DELS', void, DeleteStream, 0, (), 0, ());
    NSCRIPT_ADDCMD('BLDS', void, BuildStream, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('SNGM', void, SetNumGeometries, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GNGM', int, GetNumGeometries, 0, (), 0, ());
    NSCRIPT_ADDCMD('SGA0', void, SetGeometryAt0, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA0', void, GetGeometryAt0, 0, (), 4, (nString&, int&, nString&, int&));
    NSCRIPT_ADDCMD('SGA1', void, SetGeometryAt1, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA1', void, GetGeometryAt1, 0, (), 4, (nString&, int&, nString&, int&));
    NSCRIPT_ADDCMD('SGA2', void, SetGeometryAt2, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA2', void, GetGeometryAt2, 0, (), 4, (nString&, int&, nString&, int&));
    NSCRIPT_ADDCMD('SGA3', void, SetGeometryAt3, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA3', void, GetGeometryAt3, 0, (), 4, (nString&, int&, nString&, int&));
    NSCRIPT_ADDCMD('SGA4', void, SetGeometryAt4, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA4', void, GetGeometryAt4, 0, (), 4, (nString&, int&, nString&, int&));
    NSCRIPT_ADDCMD('SGA5', void, SetGeometryAt5, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA5', void, GetGeometryAt5, 0, (), 4, (nString&, int&, nString&, int&));
    NSCRIPT_ADDCMD('SGA6', void, SetGeometryAt6, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA6', void, GetGeometryAt6, 0, (), 4, (nString&, int&, nString&, int&));
    NSCRIPT_ADDCMD('SGA7', void, SetGeometryAt7, 4, (nString&, int, nString&, int), 0, ());
    NSCRIPT_ADDCMD('GGA7', void, GetGeometryAt7, 0, (), 4, (nString&, int&, nString&, int&));
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
void
nGeometryStreamEditor::SetNumGeometries(int num)
{
    //update number of geometries, keeping the current ones first
    if (this->geometryArray.Size() != num)
    {
        nArray<GeometryEntry> tempArray(this->geometryArray);
        this->geometryArray.SetFixedSize(num);
        for (int index = 0; index < n_min(tempArray.Size(), num); ++index)
        {
            this->geometryArray.Set(index, tempArray.At(index));
        }
    }
}

int
nGeometryStreamEditor::GetNumGeometries() const
{
    return this->geometryArray.Size();
}

#define N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(index) \
void \
nGeometryStreamEditor::SetGeometryAt##index(nString& className, int level, nString& nodePath, int frequency) \
{ \
    if (index < this->GetNumGeometries()) \
    { \
        this->SetGeometryAt(index, className, level, nodePath); \
        this->SetFrequencyAt(index, frequency); \
    } \
}

#define N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(index) \
void \
nGeometryStreamEditor::GetGeometryAt##index(nString& className, int& level, nString& nodePath, int& frequency) \
{ \
    if (this->GetNumGeometries() > index) \
    { \
        this->GetGeometryAt(index, className, level, nodePath); \
        frequency = this->GetFrequencyAt(index); \
    } \
}

N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(0)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(0)
N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(1)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(1)
N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(2)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(2)
N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(3)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(3)
N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(4)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(4)
N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(5)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(5)
N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(6)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(6)
N_GEOMETRYSTREAMEDITOR_SETGEOMETRYAT(7)
N_GEOMETRYSTREAMEDITOR_GETGEOMETRYAT(7)

#undef DECLARE_SETGEOMETRYAT
#undef DECLARE_GETGEOMETRYAT

#endif

//------------------------------------------------------------------------------
/**
*/
bool
nGeometryStreamEditor::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- begingeometries ---
        cmd = ps->GetCmd(this, 'BSTR');
        cmd->In()->SetI(this->geometryArray.Size());
        ps->PutCmd(cmd);

        int index;
        for (index = 0; index < this->geometryArray.Size(); ++index)
        {
            GeometryEntry& geometry = this->geometryArray[index];

            // --- setgeometryat ---
            cmd = ps->GetCmd(this, 'SGAT');
            cmd->In()->SetI(index);
            cmd->In()->SetS(geometry.className.Get());
            cmd->In()->SetI(geometry.level);
            cmd->In()->SetS(geometry.materialName.Get());
            ps->PutCmd(cmd);

            // --- setfrequencyat ---
            cmd = ps->GetCmd(this, 'SFAT');
            cmd->In()->SetI(index);
            cmd->In()->SetI(geometry.frequency);
            ps->PutCmd(cmd);
        }

        // --- endgeometries ---
        cmd = ps->GetCmd(this, 'ESTR');
        ps->PutCmd(cmd);

        //... properties to identify which material attributes use and which ones ignore

        return true;
    }

    return false;
}
