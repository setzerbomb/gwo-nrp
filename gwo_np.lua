math.randomseed((math.tan(os.time())) * ((os.clock() * 1000) + (os.time() / 1000)))

function shallowcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in pairs(orig) do
            copy[orig_key] = orig_value
        end
    else
        copy = orig
    end
    return copy
end

function fibonacci(n)
  local function inner(m, a, b)
    if m == 0 then
      return a
    end
    return inner(m-1, b, a+b)
  end
  return inner(n, 0, 1)
end

function quickSort(array, p, r)
    p = p or 1
    r = r or #array
    if p < r then
        q = partition(array, p, r)
        quickSort(array, p, q - 1)
        quickSort(array, q + 1, r)
    end
end

function partition(array, p, r)
    local x = array[r].fitness
    local i = p - 1
    for j = p, r - 1 do
        if array[j].fitness <= x then
            i = i + 1
            local temp = array[i]
            array[i] = array[j]
            array[j] = temp
        end
    end
    local temp = array[i + 1]
    array[i + 1] = array[r]
    array[r] = temp
    return i + 1
end

local fitness_client_satisfaction = function (max_story_points, problem)
  return function (position)
    local fitness_value = 0.0
    local total_story_points = 0.0
    for i = 1,#position do
      if (position[i] > 1) then
        total_story_points = total_story_points + problem[i].story_points
        fitness_value = fitness_value + problem[i].satisfaction
      end
    end

    if total_story_points > max_story_points  then
      return math.huge
    else
      return 1/fitness_value
    end
  end
end

local wolf = function (fitness, dim, minx, maxx)
  local self = {}

  self.position = {}

  for i = 1,dim do
    self.position[i] = ((maxx - minx) * math.random() + minx)
  end

  self.fitness = fitness(self.position)

  return self
end

local gwo = function(fitness, max_iter, n, dim, minx, maxx)

  local population = {}

  for i = 1, n do
    population[i] = wolf(fitness, dim, minx, maxx)
  end

  quickSort(population, 1, #population)

  local alpha_wolf, beta_wolf, gamma_wolf = shallowcopy(population[1]), shallowcopy(population[2]), shallowcopy(population[3])

  local explore = false

  local iter = 0
  local alpha_const = 2
  local previous_alpha = 1
  local explore_counter = 1
  local max_iter_multiplier = 1.0
  while (iter < (max_iter * max_iter_multiplier) and (max_iter_multiplier < 2)) do

    if (iter % 10 == 0 and iter > 1) then

      print(string.format("Iter = " .. iter .. " best fitness = %.6f", alpha_wolf.fitness))
      if (string.format("%.6f",alpha_wolf.fitness) ==  previous_alpha) then
        explore = true
        explore_counter = explore_counter + 1
      else
        explore = false
        explore_counter = 1
        max_iter_multiplier = max_iter_multiplier + 0.025
      end
    end

    local a = (function()
        if (explore == false) then
          return alpha_const *(1 - iter/max_iter);
        else
          return alpha_const *(1 + iter/max_iter) * (math.random()*explore_counter);
        end;
      end)()
    print(a)

    for i = 1, n do
      local X1 = {}
      local X2 = {}
      local X3 = {}
      local nextPos = {}

      local A1, A2, A3 = a * (alpha_const * math.random() - 1), a * (
              alpha_const * math.random() - 1), a * (alpha_const * math.random() - 1)
      local C1, C2, C3 = alpha_const * math.random(), alpha_const*math.random(), alpha_const*math.random()

      for j = 1,dim do

        X1[j] = alpha_wolf.position[j] - A1 * math.abs(
        C1 * alpha_wolf.position[j] - population[i].position[j])

        X2[j] = beta_wolf.position[j] - A2 * math.abs(
        C2 *  beta_wolf.position[j] - population[i].position[j])

        X3[j] = gamma_wolf.position[j] - A3 * math.abs(
        C3 * gamma_wolf.position[j] - population[i].position[j])

        nextPos[j] = X1[j] + X2[j] + X3[j]
      end

      for k  = 1,dim do
        nextPos[k] = nextPos[k] / dim
      end

      local nextFitness = fitness(nextPos)

      if nextFitness < population[i].fitness then
          population[i].position = nextPos
          population[i].fitness = nextFitness
      end

    end
    quickSort(population, 1, #population)

    previous_alpha = string.format("%.6f",alpha_wolf.fitness)

    alpha_wolf, beta_wolf, gamma_wolf = shallowcopy(population[1]), shallowcopy(population[2]), shallowcopy(population[3])

    iter = iter + 1
  end

  return alpha_wolf.position
end

--[[
local problem = {}
for i = 1, math.random(1,10) * 5 do
  problem[i] = {}
  problem[i].story_points = fibonacci(math.random(1,10))
  problem[i].satisfaction = math.random(1,10) * 10
end
]]--

local problem = {
    {story_points = 3,satisfaction = 20},
    {story_points = 5,satisfaction = 50},
    {story_points = 2,satisfaction = 40},
    {story_points = 13,satisfaction = 100},
    {story_points = 5, satisfaction = 40},
    {story_points = 8, satisfaction = 10},
    {story_points = 5,satisfaction = 20},
    {story_points = 5,satisfaction = 40},
    {story_points = 3,satisfaction = 30},
    {story_points = 3,satisfaction = 10}
}

print("\nBegin grey wolf optimization on NRP client satisfaction\n")
local dim = #problem
local fitness = fitness_client_satisfaction(20, problem)

print("Goal is to minimize  " .. dim .. " variables")

local num_particles = 75
local max_iter = math.pow(#problem,2)

print("Setting num_particles = " .. num_particles)
print("Setting max_iter    = " .. (max_iter))
print("\nStarting GWO algorithm\n")

local best_position = gwo(fitness, max_iter, num_particles, dim, 0.0, 2.0)

print("\nBest solution found:")

io.write("[")
local total_sp = 0
local total_satisfaction = 0
local after_first = false
for i = 1,dim do
  if (best_position[i] > 1) then
    if (after_first) then
      io.write(", ")
    else
      after_first = true
    end
    io.write('F'..i)

    total_sp = total_sp + problem[i].story_points
    total_satisfaction = total_satisfaction + problem[i].satisfaction
  end
end
print("]")

print("Total story points: " .. total_sp)
print("Total satisfaction: " .. total_satisfaction)

local err = fitness(best_position)

print(string.format("Fitness of best solution = %.6f", err))