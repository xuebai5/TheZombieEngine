-------------------------------------------
-- Script called to init the FSM library --
-------------------------------------------

-- Get a global reference to the world interface object used to access the world
-- from the script side. This helps to hide this query to designers and makes
-- scripts more readable and friendly.
world = lookup( '/sys/servers/worldinterface' )
