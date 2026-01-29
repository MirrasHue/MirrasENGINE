local mirras = require 'MirrasENGINE'

local Player =
{
    speed = 10.0,
    health = 100,
    has_armor = true,
    name = "Mirras"
}

mirras.log_error("this is an error: " .. 10)

function Player: on_update(dt)
    transform = entity:get_transform()
    transform.position:add_x(0.1)
    cam = entity:get_camera()

    --print("speed:", self.speed)
end

return Player
