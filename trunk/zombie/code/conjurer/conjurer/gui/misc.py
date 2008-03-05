##\file misc.py
##\brief All miscellanious stuff that you don't know where to place

import pynebula

import app
import servers


# MiscBehaviour class
class MiscBehaviour:
    def __init__(self):
        pynebula.pyBindSignal( app.get_object_state(),
            'singleentityplaced', self, 'onentityplaced', 0 )
    
    def onentityplaced(self):
        """Do some post process for some entities after their placed"""
        pass
##        obj_state = app.get_object_state()
##        entity = obj_state.getentityplaced()
##        
##        # Make entities with a FSM emit an event by default
##        if entity.hascomponent('ncFSM'):
##            event = 'squad_member'
##            if entity.hascomponent('ncGameplayScavenger'):
##                event = 'scavenger'
##            elif entity.hascomponent('ncGameplayStrider'):
##                event = 'strider'
##            ts = servers.get_trigger_server()
##            event_type = ts.geteventtransientid( event )
##            entity_id = entity.getid()
##            ts.registerareaevent( event_type, entity_id, entity_id, 0, 0 )
##        
##        # Make agent entities perceive some extra events by default
##        if entity.hascomponent('ncAgentTrigger'):
##            if entity.hascomponent('ncGameplayLiving') and not entity.hascomponent('ncGameplayScavenger') \
##                                    and not entity.hascomponent('ncGameplayStrider'):
##                entity.setperceivableevent('scavenger', True)   
##                entity.setperceivableevent('strider', True)
