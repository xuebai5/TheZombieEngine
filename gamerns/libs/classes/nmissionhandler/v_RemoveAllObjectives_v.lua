-- RemoveAllObjectives
--
-- Set the state of all currently assigned or completed objectives to removed
--
-- /p self The mission handler
function ( self )
    for i = 0, self:getobjectivesnumber() - 1 do
        state = self:getobjectivestatestringbyindex( i )
        if state == 'assigned' or state == 'completed' then
            self:setobjectivestatestringbyindex( i, 'removed' )
        end
    end
end
