//-----------------------------------------------------------------------------
//  nphymaterial_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphymaterial.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyMaterial, "nroot");

//-----------------------------------------------------------------------------

nPhyMaterial::tContainerMaterials nPhyMaterial::listMaterials( NumInitialMaterialsSpace, NumMaterialsGrowthPace );

surfacedata* nPhyMaterial::materialsTable(0);

int nPhyMaterial::MaterialsSize = 0;

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 28-Sep-2004   David Reyes   created
*/
nPhyMaterial::nPhyMaterial() :
    nameMaterial(""),
    id( -1 ),
    flags(0),
    velocityBouncing(0)
#ifndef NGAME
    ,hasToBeRemoved(false)
#endif
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 28-Sep-2004   David Reyes   created
*/
nPhyMaterial::~nPhyMaterial()
{
    if( materialsTable )
    {
        n_delete_array( materialsTable );
        materialsTable = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Updates the table of materials.

    history:
        - 28-Sep-2004   David Reyes   created
*/
void nPhyMaterial::Update()
{
    // updating id
    this->id = this->listMaterials.Size() + 1;

    listMaterials.Add( this->id, this );

    UpdateTableMaterialContacts();
}


//-----------------------------------------------------------------------------
/**
    Creates the table of material contacts.

    history:
        - 28-Sep-2004   David Reyes   created
        - 28-Oct-2005   David Reyes   made it public
*/
void nPhyMaterial::UpdateTableMaterialContacts()
{
    if( materialsTable )
    {
        n_delete( materialsTable );

        materialsTable = 0;
    }

    int NumberOfMaterials( listMaterials.Size() + 1 );

    /// creating space for the new materials table
    materialsTable = n_new_array( surfacedata,  NumberOfMaterials * NumberOfMaterials );

    n_assert2( materialsTable, "Failed to create the materials table" );

    surfacedata* material( materialsTable );

    /// building the materials table
    for( int y(0); y < NumberOfMaterials; ++y )
    {
        nPhyMaterial* ymaterial(0);
        
        if( y != NoValidID )
        {
            #ifndef __NEBULA_NO_ASSERT__
            bool result =
            #endif
                listMaterials.Find( y, ymaterial );

            n_assert2( result, "Data corruption" );
        }

        for( int x(0); x < NumberOfMaterials; ++x, ++material )
        {
            nPhyMaterial* xmaterial(0);        

            if( x != NoValidID )
            {
                #ifndef __NEBULA_NO_ASSERT__
                bool result = 
                #endif
                    listMaterials.Find( x, xmaterial );

                n_assert2( result, "Data corruption" );
            }

            if( x == NoValidID )
            {
                if( y == NoValidID )
                {
                    continue;
                }
                else
                {
                    ymaterial->Fill( *material );
                }
            }
            else
            {
                if( y == NoValidID )
                {
                    xmaterial->Fill( *material );
                }
                else
                {         
                    if( x == y )
                    {
                        xmaterial->Fill( *material );
                    }
                    else
                    {
                        *material = *ymaterial + *xmaterial;
                    }
                }
            }

        }
    }
    MaterialsSize = listMaterials.Size() + 1;
}

//-----------------------------------------------------------------------------
/**
    Fills a surface data information.

    @param data structure to be filled

    history:
        - 28-Sep-2004   David Reyes   created
*/
void nPhyMaterial::Fill( surfacedata& data ) const
{
    /// resets the data
    phyReset( data );

    /// check each contact type soported and fill the data
    for( int i(1); i < Num; ++i )
    {
        if( !IsType(typeContact(i)) )
        {
            continue;
        }

        switch( typeContact(i) )
        {
        case NormalFriction:
            phySetColoumbFrictionCoeficient( data, this->GetCoeficientNormalFriction() );
            continue;
        case Bounce:
            phySetBouncingCoeficient( data, this->GetCoeficientBounce() );
            continue;
        case SoftnessERP:
            phySetSoftnessERPCoeficient( data, this->GetCoeficientSoftnessERP() );
            continue;
        case SoftnessCFM:
            phySetSoftnessCFMCoeficient( data, this->GetCoeficientSoftnessCFM() );
            continue;
        case Slip:
            phySetDependentSlipCoeficient( data, this->GetCoeficientSlip() );
            continue;
        case PyramidFriction:
            phySetPyramidFrictionCoeficient( data, this->GetCoeficientPyramidFriction() );
            continue;
        case FrictionDirection:
            phyEnableFrictionDirection( data );
            continue;
        default:
            n_assert2_always( "Code corruption (stack overflow likely)" );
            continue;
        }
    }

    phySetBounceVelocity(data, this->GetMinVelocityBounce() );
}

//-----------------------------------------------------------------------------
/**
    Operator to mix two materials.

    @param material a material

    @return datasurface information

    history:
        - 29-Sep-2004   David Reyes   created
*/
const surfacedata nPhyMaterial::operator + ( const nPhyMaterial& material ) const
{
    surfacedata data;

    /// resets the data
    phyReset( data );

    phyreal coeficient(0);
    
    phyreal bounceVelocity(0);

    /// check each contact type soported and fill the data
    for( int i(1); i < Num; ++i )
    {
        coeficient = 0;

        typeContact tcontact = typeContact(i);

        switch( tcontact )
        {
        case FrictionDirection:
            if( IsType( tcontact ) )
            {
                phyEnableFrictionDirection( data );
            }
            else if( material.IsType(tcontact) )
            {
                phyEnableFrictionDirection( data );
            }
            continue;
        case NormalFriction:
            
            if( IsType(tcontact) )
            {
                phySetColoumbFrictionCoeficient( data, this->GetCoeficientNormalFriction() );
            }
            if( material.IsType(tcontact) )
            {
                phySetColoumbFrictionCoeficient( data, material.GetCoeficientNormalFriction(), !IsType(tcontact)  );
            }
            continue;
        case Bounce:
            
            if( IsType(tcontact) )
            {
                if( bounceVelocity == phyreal(0) )
                {
                    bounceVelocity = this->GetMinVelocityBounce();
                }
                else
                {
                    bounceVelocity += this->GetMinVelocityBounce() / phyreal(2);
                }

                coeficient = this->GetCoeficientBounce();
                if( material.IsType(tcontact) )
                {
                    if( bounceVelocity == phyreal(0) )
                    {
                        bounceVelocity = material.GetMinVelocityBounce();
                    }
                    else
                    {
                        bounceVelocity += material.GetMinVelocityBounce() / phyreal(2);
                    }
                    coeficient += material.GetCoeficientBounce();
                    coeficient /= phyreal(2);
                }
                phySetBouncingCoeficient( data, coeficient );
            }
            else
            {
                if( material.IsType(tcontact) )
                {
                    coeficient = material.GetCoeficientBounce();
                    if( bounceVelocity == phyreal(0) )
                    {
                        bounceVelocity = material.GetMinVelocityBounce();
                    }
                    else
                    {
                        bounceVelocity += material.GetMinVelocityBounce() / phyreal(2);
                    }
                   phySetBouncingCoeficient( data, coeficient );
                }
            }
            

            continue;
        case SoftnessERP:

            if( IsType(tcontact) )
            {
                coeficient = this->GetCoeficientSoftnessERP();
                if( material.IsType(tcontact) )
                {
                    coeficient += material.GetCoeficientSoftnessERP();
                    coeficient /= phyreal(2);
                }
            }
            else
            {
                if( material.IsType(tcontact) )
                {
                    coeficient = material.GetCoeficientSoftnessERP();
                }
            }
            phySetSoftnessERPCoeficient( data, coeficient );
            continue;

        case SoftnessCFM:
            if( IsType(tcontact) )
            {
                coeficient = this->GetCoeficientSoftnessCFM();
                if( material.IsType(tcontact) )
                {
                    coeficient += material.GetCoeficientSoftnessCFM();
                    coeficient /= phyreal(2);
                }
            }
            else
            {
                if( material.IsType(tcontact) )
                {
                    coeficient = material.GetCoeficientSoftnessCFM();
                }
            }
            phySetSoftnessCFMCoeficient( data, coeficient );
            continue;
        case Slip:
            if( IsType(tcontact) )
            {
                phySetDependentSlipCoeficient( data, this->GetCoeficientSlip() );
            }
            if( material.IsType(tcontact) )
            {
                phySetDependentSlipCoeficient( data, material.GetCoeficientSlip(), false );
            }
            continue;
        case PyramidFriction:
            if( IsType(tcontact) )
            {
                phySetPyramidFrictionCoeficient( data, this->GetCoeficientPyramidFriction() );
            }
            if( material.IsType(tcontact) )
            {
                phySetPyramidFrictionCoeficient( data, material.GetCoeficientPyramidFriction(), false );
            }
            continue;

        default:
            n_assert2_always(  "Code corruption (stack overflow likely)" );
            continue;
        }
    }

    phySetBounceVelocity( data, bounceVelocity );

    return data;
}

#if defined(_DEBUG) || !defined(NGAME)
//-----------------------------------------------------------------------------
/**
    DEBUG: checks that a material type is correct.

    @param type material type

    @return boolean

    history:
        - 29-Sep-2004   David Reyes   created
*/
bool nPhyMaterial::Validate( const int type )
{
    if( type < 0 )
        return false;
    if( type > listMaterials.Size() )
        return false;
    return true;
}

#endif
//-----------------------------------------------------------------------------
/**
    Returns a material type trough the id.

    @param id material id

    @return material

    history:
        - 29-Sep-2004   David Reyes   created
*/  
nPhyMaterial* nPhyMaterial::GetMaterial( idmaterial id )
{
    nPhyMaterial* material;

    if( listMaterials.Find( id, material ) )
        return material;

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Gets the material name.

    @return material name

    history:
        - 26-Sep-2005   David Reyes   created
*/
const nString& nPhyMaterial::GetMaterialName() const
{
    return this->nameMaterial;
}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Return if the object it's meant to be removed.

    @return true/false

    history:
        - 03-Nov-2005   David Reyes   created
*/
bool nPhyMaterial::GetToBeRemoved() const
{
    return this->hasToBeRemoved;
}

//-----------------------------------------------------------------------------
/**
    Set to be removed.

    @param has true/false

    history:
        - 03-Nov-2005   David Reyes   created
*/
void nPhyMaterial::SetToBeRemoved( bool has )
{
    if( has )
    {
        this->SetObjectDirty(true);
        this->HideInEditor();
    }
    else
    {
        this->UnHideInEditor();
    }
    this->hasToBeRemoved = has;
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
