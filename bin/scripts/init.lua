set_map(8, 8, 1)
set_map(9, 9, 1)

has_ball = true

function on_tick(elapsed_time)
end

function on_receive(str)
end

function on_key_down(key)

  if key == 32 and has_ball then

    local pos = get_position()
    local rot = get_rotation()

    spawn(
    -- specify properties of object we're spawning
    {
      position_x = pos.x + 2 * math.cos(rot),
      position_y = pos.y + 2 * math.sin(rot),
      position_z = 0.35,
      velocity_x = 5 * math.cos(rot),
      velocity_y = 5 * math.sin(rot),
      velocity_z = 1,
      rotation = rot,
      friction = 1,
      height = 0.1,
      width = 0.1,
      color = 0x00ff00,
      bounces = -1,
      bouncy = 1,
      collision_radius = 0.5,
      explosion_size = 0,
    },

    -- executed when object collides with you
    function ()
      has_ball = true
    end)

    has_ball = false

  end

end

function on_key_up(key)
end
