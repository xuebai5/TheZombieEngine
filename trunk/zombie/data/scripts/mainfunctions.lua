--
-- Create a new node, select this a return the thunk
function selNew( class , name)
  new(class,name)
  sel(name)
  this = lookup('.')
  return this
end

--
-- equal to sel cmd, but the thunk 'this' is the actual node
--
function luaSel( name)
--  puts('luaSel ' .. name ..'\n')
 --  conls()
   local tmp = sel(name)
   this = lookup('.')
--  puts(this:getfullname() .. '\n\n' )
   return tmp
end

--
-- equal luapushcmd , but the thunk 'this' is the actualnode
--
function luaPushCwd (name)
   local tmp = pushcwd(name)
   this = lookup('.')
   return tmp
end

--
-- 
--
function luaPopCwd (name)
     local tmp = popcwd(name)
     this = lookup('.')
     return tmp
end



function nfindllog( tname)
    return {
       name = tname,
       setwarning = 
               function (self , level)  
                   nebula.sys.servers.log:setwarninglevel( self.name, level) 
               end,
       getwarning = function (self)
                     return nebula.sys.servers.log:getwarninglevel(self.name)                   
                end,
        setenable = function (self , val)
                    return nebula.sys.servers.log:setlogenable( self.name , val)
                end,
         getenable = function (self )
                     return nebula.sys.servers.log:getlogenable( self.name)
                 end,
         getdescription = function (self )
                     return nebula.sys.servers.log:setenable( self.name)
                 end,
          log = function (self)
                  return   nebula.sys.servers.log:log(  name, level, message)
                 end
    }
end

function ncreatelog( name, description)
    nebula.sys.servers.log:register( name , description)
    return nfindllog( name)
end

function nlog( name, level, message)
    nebula.sys.servers.log:log(  name, level, message)
end

function logls()
  local t = nebula.sys.servers.log:getlogclass()
  if ( t ~= nil ) then
    for index, value in t do
      if ( value ~= nil ) then
        puts( value )
      end
      puts( ' ' )
    end
  end
  puts( '\n' )
  return t
end



function nput( ... )
    local function putrec ( tab,   index,  t)
        local typestring = type(t)
        puts( tab .. tostring(index) .. ' : '   )
        
        if ( typestring == 'table'  ) then
            puts( typestring .. ' : \n')
                for i, value in t do
                    putrec( tab..'... ' , i , (value) )
                end
        elseif ( typestring == 'string'  ) then
            puts( typestring .. ' : ' .. t .. '\n')
        else  
            puts( tostring(t)  .. '\n')
        end
    end  --end putrec
    
    for index, value in arg do
        putrec( '... ', index , (value) )
    end
    
end
