{  //test results are using double
"nbody-parameters-file": {
    "nbody-context": {
        "headline" : "disk=miyamoto-nagai_halo=NFW_quad=true_criterion=sw93",
        "criterion" : "sw93",
        "use-quadrupole-corrections" : true,
        "accuracy-parameter" : 1.0,
        "seed" : 0,

        "potential" : {
            "disk" : {
                "miyamoto-nagai" : {
                    "mass" : 4.45865888E5,
                    "scale-length" : 6.5,
                    "scale-height" : 0.26
                }
            },

            "spherical" : {
                "sphere" : {
                    "mass" : 1.52954402E5,
                    "r0-scale" : 0.7
                }
            },

            "halo" : {
                "nfw" : {
                    "vhalo" : 155,
                    "scale-length" : 22.25
                }
            }
        },

        "dwarf-model": {
            "plummer" : {
            "mass" : 41.81540899570283,
            "nbody" : 32767,
            "scale-radius" : 0.3724421237882427,
            "time-orbit" : 1.552466585983782,
            "time-dwarf" : 1.5006893857942978
            }
        }
    },

    "initial-conditions": {
        "useGalC" : false,
        "angle-use-radians" : false,
        "velocity" : [ -178, 106, 108 ],
        "position" : [ 218, 53.5, 28.9 ]
    },


}}

