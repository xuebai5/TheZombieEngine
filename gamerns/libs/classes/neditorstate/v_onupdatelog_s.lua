---------------------------------------------------------------------------------
--   application state on screen log
--
--   This procedure is called when a long task is working
--   Parameters: message and percentage of work done
---------------------------------------------------------------------------------
function ( self, message )

    local messageWindow = nebula.gui.editorRootWindow.MessageWindow

    if message ~= nil then
                
        -- Find prefix separator (:)
        local dotPos = string.find(message, ':')
        local prefixPos = string.find(message, ' ')
        if dotPos ~= nil and prefixPos ~= nil then
            local t = nebula.sys.servers.time:gettime()
            local prefix = string.sub(message, prefixPos + 1, dotPos )
            local dotPos = string.find(message, ':')
            local lastMessage = messageWindow:getlastmessage()

            -- Check if prefix is the same that last message's prefix
            if lastLogTime > 0 and lastMessage ~= nil and string.len(prefix) ~= 0 then
                local lastPrefixDotPos = string.find(lastMessage, ':')
                local lastPrefixPos = string.find(lastMessage, ' ')
                if lastPrefixDotPos ~= nil and lastPrefixPos ~= nil then
                    lastPrefix = string.sub(lastMessage, lastPrefixPos + 1, lastPrefixDotPos )
                    if lastPrefix == prefix then
    
                        -- Find completion %
                        local percentPos = string.find(message, '[0-9 ][0-9 ]%d%%')
                        local currentPercent = 0
                        if percentPos ~=nil then
                        
                            -- Calculate left time estimation
                            currentPercent = string.sub(message, percentPos, percentPos+2)

                            local timeDelta = t - lastLogTime 
                            elapsedLogTime = elapsedLogTime + timeDelta

                            local leftTime = ( 100 - currentPercent ) * ( elapsedLogTime / currentPercent )
                        
                            local days = math.floor( leftTime / (24.0 * 3600.0) )
                            leftTime = leftTime - days * 24.0 * 3600.0
                            local hours = math.floor( leftTime / 3600.0 )
                            leftTime = leftTime - hours * 3600.0
                            local minutes = math.floor( leftTime / 60.0 )
                            leftTime = leftTime - minutes * 60.0
                            leftTime = math.floor( leftTime )
    
                            -- Add it to message
                            local timeStr = string.format('(%02d:%02d:%02d left)', hours, minutes, leftTime)
                            message = message .. timeStr
                        else
                            estimatedMeanTime = 0
                            elapsedLogTime = 0
                        end
                    else
                        estimatedMeanTime = 0
                        elapsedLogTime = 0
                    end
                else
                    estimatedMeanTime = 0
                    elapsedLogTime = 0
                end
            else
                message = message..'              '
                estimatedMeanTime = 0
                elapsedLogTime = 0
            end
            lastLogTime = t
            
            -- Set last message with same prefix, or add message with new prefix
            messageWindow:setlastmessage(prefixPos, prefix, '%', message..'  ')            
            
            -- Set black background to prevent transparency when 'busy log'
--            if nebula.sys.servers.time:gettime() - nebula.sys.servers.time:getframetime() > 0.25 then
                --messageWindow:setdefaultbrush('black')
--            end
        end
    end
end
