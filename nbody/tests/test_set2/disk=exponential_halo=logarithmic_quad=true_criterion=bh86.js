{  //test results are using double
"nbody-parameters-file": {
    "nbody-context": {
        "headline" : "disk=exponential_halo=logarithmic_quad=true_criterion=bh86",
        "criterion" : "bh86",
        "use-quadrupole-corrections" : true,
        "accuracy-parameter" : 1.0,
        "seed" : 0,

        "potential" : {
            "disk" : {
                "exponential" : {
                    "mass" : 6.0E5,
                    "scale-length" : 4.0,
                    "scale-height" : 0.4
                }
            },

            "spherical" : {
                "sphere" : {
                    "mass" : 1.0E5,
                    "r0-scale" : 1.0
                }
            },

            "halo" : {
                "logarithmic" : {
                    "vhalo" : 155,
                    "scale-length" : 15.0,
                    "z-flattening" : 1.1,
                    "x-flattening" : 0.8,
                    "y-flattening" : 1.0,
                    "triaxial-angle" : 0.1
                }
            }
        },

        "dwarf-model": {
            "plummer" : {
                "mass" : 10,
                "nbody" : 100,
                "scale-radius" : 0.3,
                "time-orbit" : 4,
                "time-dwarf" : 3.945
            }
        }
    },

    "initial-conditions": {
        "useGalC" : false,
        "angle-use-radians" : false,
        "velocity" : [ -156, 79, 107 ],
        "position" : [ 28.6, 218.0, 53.5 ]
    },


}}
