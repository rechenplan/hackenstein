--[[

map_set(8, 8, 1)
map_get(8, 8) == 1
map_width()
map_height()

broadcast([ 
  sender = "bob"
  type = "spawn_object",
  blueprint = [
    explodes = true,
    position = [ x = 1, y = 2, z = 0.5 ],
    velocity = get_velocity(),
  ],
  on_collide = "do_damage",
  damage = 10,
])

function hurt_me(damage)
  health = health - damage
  if (health < 0)
    position = [ x = 1, y = 2, z = 0.5 ],
  end
end

function do_damage(attacker, damage)
  return function ()
    hurt_me(damage)
    print(attacker .. " hit you for " .. tostring(damage) .. " damage.")
  end
end

function on_receive(msg)
  if (msg.type == "spawn_object")
    if (msg.on_collide == "do_damage")
      spawn_object(msg.blueprint, do_damage(msg.sender, msg.damage))
    end
  end
end

functions:

broadcast(str)
spawn(blueprint_table, collision_function)
[] = get_map()
set_map(array2d)
[] = get_position()
set_position([ x = 0, y = 0, z = 0 ])
[] = get_velocity()
set_velocity([ x = 0, y = 0, z = 0 ])
phi = get_rotation()
set_rotation(phi)

]]--

function on_tick(elapsed_time)
end

function on_receive(str)
end

function on_key_down(key)
  if key == 32 then
    spawn(
    -- specify properties of object we're spawning
    {
      position_x = 1.5,
      position_y = 1.5,
      position_z = 0,
      height = 0.5,
      width = 0.5,
      collision_radius = 1.0,
      color = 0xff00ff
    },
    -- executed when object collides with you
    function ()
      print("collision (" .. tostring(key) .. ")")
    end)
  end
end

function on_key_up(key)
end
