#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpdeterminering.cc
//------------------------------------------------------------------------------
#include "nGPGotoNextWaypoint/nGPGotoNextWaypoint.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "nwaypointserver/nwaypointserver.h"
#include "ncaimovengine/ncaimovengine.h"
#include "ncwaypointpath/ncwaypointpath.h"
#include "nastar/nastar.h"

#include "kernel/nlogclass.h"

nNebulaScriptClass(nGPGotoNextWaypoint, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPGotoNextWaypoint)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPGotoNextWaypoint::nGPGotoNextWaypoint():
livingEntity ( 0 ),
path ( 0 ),
movEngine ( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPGotoNextWaypoint::~nGPGotoNextWaypoint()
{
    this->End();
}



//------------------------------------------------------------------------------
/**
*/
bool
nGPGotoNextWaypoint::Init (nEntityObject * entity)
{
    bool valid = entity != 0;   // Entity must exists

    if ( valid )
    {        
        this->livingEntity = entity->GetComponent<ncGameplayLiving>();
        this->path = this->livingEntity->GetPathId();
        this->movEngine = entity->GetComponent <ncAIMovEngine>();

        valid =  ( this->livingEntity != 0 ) && 
				 ( nWayPointServer::Instance()->CheckPathExists(this->path) ) && 
				 (this->movEngine != 0);
        n_assert( this->livingEntity != 0 && this->movEngine != 0 );
    }

    if ( valid )
    {
        this->entity = entity;        
		if ( nWayPointServer::Instance()->GetNumWayPoints(this->path) > 0 )
		{
			nextWayPoint = this->livingEntity->GetLastWayPoint();
			if ( nextWayPoint > nWayPointServer::Instance()->GetNumWayPoints(this->path) - 1)
			{
				// Restart path
				nextWayPoint = 0;
				this->livingEntity->SetLastWayPoint(0);
			}
			// Get next waypoint and move entity to it
			WayPoint* wayPoint = nWayPointServer::Instance()->GetWayPoint(this->path, nextWayPoint); 
			n_assert2 ( wayPoint, "Waypoint not registered in waypoint server" );		
			if ( wayPoint )
			{
				vector3 waypointPosition = wayPoint->GetPosition();

                if ( this->livingEntity->IsProne() )
                {
                    // If move mode is walk and entity is prone, set prone mode
                    this->movEngine->SetMovMode(ncAIMovEngine::MOV_WALKPRONE);
                }
                else
                {
                    this->movEngine->SetMovMode(ncAIMovEngine::MOV_WALK);
                }
                
                this->movEngine->SetMovDirection(ncAIMovEngine::MD_FORWARD);
                valid = this->movEngine->MoveTo( waypointPosition );
			}    
			else
			{
				NLOG( resource, 
					( 0, "ngpgotonextwaypoint, Waypoint %d not registered in waypoint server", nextWayPoint ) );
				// Maybe next waypoint is valid...
				this->livingEntity->SetLastWayPoint(this->livingEntity->GetLastWayPoint() + 1);			
				valid = false;
				
			}
		}
		else
		{
			valid = false;
		}
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPGotoNextWaypoint::IsDone() const
{
    n_assert(this->init);
    bool done = false;

    if (this->entity)
    {
        if ( !this->movEngine->IsMoving() )
        {
            this->livingEntity->SetLastWayPoint(this->livingEntity->GetLastWayPoint() + 1);
            done = true;
        }
    }        

    return done;
}

//------------------------------------------------------------------------------
/**
*/
/*void
nGPGotoNextWaypoint::End()
{
    n_assert( this->entity );
    if ( this->entity )
    {
        ncAIMovEngine* engine = 0;
        engine = this->entity->GetComponent <ncAIMovEngine>();
        n_assert( engine );
        if ( engine )
        {
            engine->Stop();
        }
    }
}*/

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
