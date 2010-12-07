{
"nbody-parameters-file": {
    "nbody-context": {
        "headline" : "orphan model 4",
        "criterion" : "sw93",
        "use-quadrupole-corrections" : true,
        "accuracy-parameter" : 1.0,
        "seed" : 0,

        "time-orbit" : 4,
        "time-evolve" : 3.945,

        "potential" : {
            "disk" : {
                "type" : "miyamoto-nagai",
                "mass" : 4.45865888E5,
                "scale-length" : 6.5,
                "scale-height" : 0.26
            },

            "spherical" : {
                "type" : "sphere",
                "mass" : 1.52954402E5,
                "r0-scale" : 0.7
            },

            "halo" : {
                "type" : "logarithmic",
                "vhalo" : 114,
                "scale-length" : 12.0,
                "z-flattening" : 1.0,
            }
        },

        "dwarf-model": [
            {
                "type" : "plummer",
                "mass" : 16,
                "nbody" : 1000,
                "scale-radius" : 0.2,

                "initial-conditions": {
                    "use-galactic-coordinates" : false,
                    "angle-use-radians" : false,
                    "velocity" : [ -179, 106, 109 ],
                    "position" : [ 218, 53.5, 28.9 ]
                }
            }
        ]
    },

    "histogram" : { }

}}

