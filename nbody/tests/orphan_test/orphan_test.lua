
prng = DSFMT.create(argSeed)
nbody = 4096


dwarfMass = 16
dwarfRadius = 0.2
reverseTime = 4.0
evolveTime = 3.945


function makeHistogram()
   return HistogramParams.create()
end

function makePotential()
   local disk, halo, spherical
   disk = Disk.miyamotoNagai{
      mass        = 4.45865888e5,
      scaleLength = 6.5,
      scaleHeight = 0.26
   }

   halo = Halo.logarithmic{
      vhalo       = 73,
      scaleLength = 12.0,
      flattenZ    = 1.0
   }

   spherical = Spherical.spherical{
      mass  = 1.52954402e5,
      scale = 0.7
   }

   local function closure(position)
      return 5.0 / Vector.abs(position)
   end

   return Potential.create{
      disk      = disk,
      halo      = halo,
      spherical = spherical
   }
end

function makeContext()
   return NBodyCtx.create{
      timestep   = calculateTimestep(dwarfMass, dwarfRadius),
      timeEvolve = evolveTime,
      eps2       = calculateEps2(nbody, dwarfRadius),
      criterion  = "sw93",
      useQuad    = true,
      theta      = 1.0
   }
end

function makeBodies(ctx, potential)
   return predefinedModels.plummer{
      nbody       = nbody,
      prng        = prng,
      position    = lbrToCartesian(ctx, Vector.create(218, 53.5, 28.6)),
      velocity    = Vector.create(0, 0, 0),
      mass        = dwarfMass,
      scaleRadius = dwarfRadius,
      ignore      = false
   }
end

