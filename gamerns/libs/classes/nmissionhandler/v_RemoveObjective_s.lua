-- RemoveObjective
--
-- Set the state of an objective to removed
--
-- /p self The mission handler
-- /p objective_name The name that references the objective whose state is going to be changed
function ( self, objective_name )
    self:setobjectivestate( objective_name, 'removed' )
end
