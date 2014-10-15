arg = { ... }

assert(#arg == 3, "Expected 3 arguments")
assert(argSeed ~= nil, "Expected seed")

prng = DSFMT.create(argSeed)

evolveTime       = arg[1]

r0  = arg[2]

dwarfMass  = arg[3]

model1Bodies = 100000
totalBodies = model1Bodies

nbodyLikelihoodMethod = "EMD"
nbodyMinVersion = "1.32"

function makePotential()
   return  Potential.create{
      spherical = Spherical.spherical{ mass  = 1, scale = 0.7 },
      disk      = Disk.miyamotoNagai{ mass = 1, scaleLength = 6.5, scaleHeight = 0.26 },
      halo      = Halo.caustic{ vhalo = 73, scaleLength = 12.0 }
   }
end

encMass = plummerTimestepIntegral(r0, sqr(r0), dwarfMass, 1e-7)

function makeContext()
   return NBodyCtx.create{
      timeEvolve = evolveTime,
      timestep   = sqr(1/10.0) * sqrt((pi_4_3 * cube(r0)) / (encMass + dwarfMass)),
      eps2       = calculateEps2(totalBodies, r0),
      criterion  = "NewCriterion",
      useQuad    = true,
      theta      = 1.0
   }
end

-- Also required
function makeBodies(ctx, potential)
    local firstModel
    firstModel = predefinedModels.plummer{
        nbody       = model1Bodies,
        prng        = prng,
        position    = Vector.create(10,0,0),
        velocity    = Vector.create(0,100,0),
        mass        = dwarfMass,
        scaleRadius = r0,
        ignore      = false
    } 
return firstModel
end

function makeHistogram()
   return HistogramParams.create()
end


