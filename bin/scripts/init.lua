function on_load(me, map)
  map_set(map, 8, 8, 1)
end

function on_collide(player, object)
end

function on_key_down(key)
  broadcast(string.char(key))
end

function on_key_up(key)
end

function on_join(player)
end

function on_leave(player)
end

function on_receive(message)
end
