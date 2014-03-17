/*
 * Copyright 2005-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

parameter parameters[] = {
   {
      "10_meter_u_velocity_sfc.glob",
      -100,
      -1,
      1,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 2},

         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "10_meter_u_velocity_sfc.lam",
      -100,
      -1,
      1,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 165},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 2},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

/* 
   MOGREPS (origin = 1(mogreps-mo-eua))
   2014-01-10: minimum limit for 10_meter_u_velocity_sfc changed to <-100,5> because of 2014010715_00+0000
*/
   {
      "10_meter_u_velocity_sfc.lam.mogreps-mo-eua",
      -100,
      5,
      1,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 1, "mogreps-mo-eua"},

         {"paramId", GRIB_TYPE_LONG, 165},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 2},

         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "10_meter_v_velocity_sfc.glob",
      -100,
      -1,
      1,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 3},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "10_meter_v_velocity_sfc.lam",
      -100,
      -1,
      1,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 166},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 3},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

/* 
   MOGREPS (origin = 1(mogreps-mo-eua))  set up for 10_meter_u_velocity_sfc
   2014-01-08: minimum limit for 10_meter_v_velocity_sfc changed to <-100,10> because of one early January forecast..
*/
   {
      "10_meter_v_velocity_sfc.lam.mogreps-mo-eua",
      -100,
      10,
      1,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 1, "mogreps-mo-eua"},

         {"paramId", GRIB_TYPE_LONG, 166},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 3},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "convective_available_potential_energy_sfc.glob",
      0,
      10,
      0,
      17000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 7},
         {"parameterNumber", GRIB_TYPE_LONG, 6},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },

   {
      "convective_available_potential_energy_sfc.lam",
      0,
      10,
      0,
      17000,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 59},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 7},
         {"parameterNumber", GRIB_TYPE_LONG, 6},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },

   {
      "convective_inhibition_sfc.glob",
      -60000,
      0,
      -10,
      5,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 7},
         {"parameterNumber", GRIB_TYPE_LONG, 7},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },

   {
      "convective_inhibition_sfc.lam",
      -60000,
      0,
      -10,
      5,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 228001},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 7},
         {"parameterNumber", GRIB_TYPE_LONG, 7},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },
   
/* 
   eggr                  (centre=74, model=1) cin max ~ -30000
   aladinhuneps-omsz-eu  (origin=12) cin max ~ <-60000,1>
   aladinlaef-zamg-eu    (origin=)   cin max ~ <-...,1>
*/
/* 
   cosmodeeps-dwd-eu (origin=7)
*/

   {
      "convective_inhibition_sfc.lam.cosmodeeps-dwd-eu",
      0,
      10,
      0,
      2000,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 7, "cosmodeeps-dwd-eu"},

         {"paramId", GRIB_TYPE_LONG, 228001},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 7},
         {"parameterNumber", GRIB_TYPE_LONG, 7},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },

/* 
   cosmoleps-arpasimc-eu (origin=4)
*/

   {
      "convective_inhibition_sfc.lam.cosmoleps-arpasimc-eu",
      -0.5,
      10,
      0,
      2000,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 4, "cosmoleps-arpasimc-eu"},

         {"paramId", GRIB_TYPE_LONG, 228001},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 7},
         {"parameterNumber", GRIB_TYPE_LONG, 7},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },

   {
      "field_capacity_sfc",
      1e99,
      -1e99,
      99,
      -99,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 2},
         {"parameterCategory", GRIB_TYPE_LONG, 3},
         {"parameterNumber", GRIB_TYPE_LONG, 12},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_LONG, 2},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 106},
         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 106},
         {NULL, },
      },
      {&point_in_time, &given_thickness, &has_bitmap},
   },

   {
      "geopotential_height_pl",
      -810,
      20000,
      320,
      22000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 3},
         {"parameterNumber", GRIB_TYPE_LONG, 5},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 100},
         {NULL, },
      },
      {&point_in_time, &given_level, &pressure_level},
   },

   {
      "land_sea_mask_sfc.glob",
      0,
      0,
      1,
      1,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 2},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "land_sea_mask_sfc.lam",
      0,
      0,
      1,
      1,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 172},

         {"discipline", GRIB_TYPE_LONG, 2},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "land_sea_mask_sfc.lam.hirlam-dmi-eu",
      -0.001,
      0,
      1,
      1,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 11, "hirlam-dmi-eu"},

         {"paramId", GRIB_TYPE_LONG, 172},

         {"discipline", GRIB_TYPE_LONG, 2},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "mean_sea_level_pressure_sfc.glob",
      91000,
      103000,
      99000,
      107000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 3},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 101},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "mean_sea_level_pressure_sfc.lam",
      91000,
      103000,
      99000,
      107000,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 151},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 3},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 101},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

/* 
   MOGREPS (origin = 1(mogreps-mo-eua))
   2014-02-06:21UTC mean_sea_level_pressure_sfc maximum value 98852 is not in [99000,107000]
*/
   {
      "mean_sea_level_pressure_sfc.lam.mogreps-mo-eua",
      91000,
      103000,
      98000,
      107000,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 1, "mogreps-mo-eua"},

        {"paramId", GRIB_TYPE_LONG, 151},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 3},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 101},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "orography_sfc.glob",
      -1300,
      0,
      2000,
      8888,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 3},
         {"parameterNumber", GRIB_TYPE_LONG, 5},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "orography_sfc.lam",
      -1300,
      0,
      2000,
      8888,
      {
        {"model", GRIB_TYPE_STRING, 0, "lam"},

        {"paramId", GRIB_TYPE_LONG, 228002},

        {"discipline", GRIB_TYPE_LONG, 0},
        {"parameterCategory", GRIB_TYPE_LONG, 3},
        {"parameterNumber", GRIB_TYPE_LONG, 5},

        {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
        {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

        {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
        {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

/* 
   MOGREPS (origin = 1(mogreps-mo-eua))
   2014-01-08: limits for orog changed to <1000,8888>
*/
   {
      "orography_sfc.lam.mogreps-mo-eua",
      -1300,
      0,
      1000,
      8888,
      {
        {"model", GRIB_TYPE_STRING, 0, "lam"},
        {"suiteName", GRIB_TYPE_STRING, 1, "mogreps-mo-eua"},

        {"paramId", GRIB_TYPE_LONG, 228002},

        {"discipline", GRIB_TYPE_LONG, 0},
        {"parameterCategory", GRIB_TYPE_LONG, 3},
        {"parameterNumber", GRIB_TYPE_LONG, 5},

        {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
        {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

        {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
        {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },
/* 
   DMI-EPS (origin = 11(hirlam-dmi-eu))
   2014-01-15: limits for orog changed to <1500,8888>
*/
   {
      "orography_sfc.lam.hirlam-dmi-eu",
      -1300,
      0,
      1800,
      2500,
      {
        {"model", GRIB_TYPE_STRING, 0, "lam"},
        {"suiteName", GRIB_TYPE_STRING, 11, "hirlam-dmi-eu"},

        {"paramId", GRIB_TYPE_LONG, 228002},

        {"discipline", GRIB_TYPE_LONG, 0},
        {"parameterCategory", GRIB_TYPE_LONG, 3},
        {"parameterNumber", GRIB_TYPE_LONG, 5},

        {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
        {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

        {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
        {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
        {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "potential_temperature_pv",
      220,
      265,
      380,
      1200,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 2},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 109},
         {NULL, },
      },
      {&point_in_time, &given_level, &potential_vorticity_level},
   },

   {
      "potential_vorticity_pt",
      -0.005,
      -1e-6,
      1e-6,
      0.002,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 14},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 320},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 107},
         {NULL, },
      },
      {&point_in_time, &given_level, &potential_temperature_level},
   },

   {
      "skin_temperature_sfc",
      180,
      210,
      320,
      355,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 17},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "snow_depth_water_equivalent_sfc",
      -4e-19,
      0,
      12000,
      15000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 60},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "snow_fall_water_equivalent_sfc",
      -1e-9,
      -1e-9,
      3.5,
      3.5,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 53},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "soil_moisture_sfc",
      -1e-18,
      0,
      450,
      500,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 2},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 22},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_LONG, 2},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 106},
         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 106},
         {NULL, },
      },
      {&point_in_time, &given_thickness, &has_bitmap},
   },

   {
      "soil_temperature_sfc",
      200,
      230,
      310,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 2},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 2},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_LONG, 2},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 106},
         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 106},
         {NULL, },
      },
      {&point_in_time, &given_thickness},
   },

   {
      "specific_humidity_pl",
      -1e-2,
      1e-4,
      0.0001,
      0.050,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 0},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 100},
         {NULL, },
      },
      {&point_in_time, &given_level, &pressure_level},
   },

   {
      "sunshine_duration_sfc",
      0,
      0,
      3600.00000001,
      3600.00000001,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 6},
         {"parameterNumber", GRIB_TYPE_LONG, 24},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "surface_air_temperature_sfc.glob",
      180,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "surface_air_temperature_sfc.lam",
      180,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 167},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 2},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "surface_air_temperature_sfc.lam.mogreps-mo-eua",
      180,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 1, "mogreps-mo-eua"},

         {"paramId", GRIB_TYPE_LONG, 167},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 15},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

/* 
   cosmodeeps-dwd-eu (origin=7)
   2014-01-08: minimum limit for surface_air_temperature_sfc changed to <140,290> 
*/
   {
      "surface_air_temperature_sfc.lam.cosmodeeps-dwd-eu",
      140,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 7, "cosmodeeps-dwd-eu"},

         {"paramId", GRIB_TYPE_LONG, 167},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 2},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "surface_air_dew_point_temperature_sfc.glob",
      175,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 6},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "surface_air_dew_point_temperature_sfc.lam",
      175,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 168},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 6},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 2},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "surface_air_dew_point_temperature_sfc.lam.mogreps-mo-eua",
      175,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 1, "mogreps-mo-eua"},

         {"paramId", GRIB_TYPE_LONG, 168},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 6},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 15},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

/* 
   cosmodeeps-dwd-eu (origin=7)
   2014-01-08: minimum limit for surface_air_dew_point_temperature_sfc changed to <160,290>
*/
   {
      "surface_air_dew_point_temperature_sfc.lam.cosmodeeps-dwd-eu",
      160,
      290,
      270,
      350,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 7, "cosmodeeps-dwd-eu"},

         {"paramId", GRIB_TYPE_LONG, 168},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 6},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 2},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&point_in_time, &given_level},
   },

   {
      "surface_air_maximum_temperature_sfc",
      190,
      230,
      310,
      330,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 2},
         {NULL, },
      },
      {&six_hourly, &given_level},
   },

   {
      "surface_air_minimum_temperature_sfc",
      190,
      230,
      310,
      320,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 3},
         {NULL, },
      },
      {&six_hourly, &given_level},
   },


   {
      "surface_pressure_sfc",
      48000,
      50500,
      103000,
      110000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 3},
         {"parameterNumber", GRIB_TYPE_LONG, 0},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&point_in_time, &predefined_level},
   },

   {
      "temperature_pl",
      185,
      250,
      230,
      330,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 0},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 100},
         {NULL, },
      },
      {&point_in_time, &given_level, &pressure_level},
   },

   {
      "time_integrated_outgoing_long_wave_radiation_sfc",
      -1.5e+6,
      -1.5e+6,
      -300000,
      -300000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 5},
         {"parameterNumber", GRIB_TYPE_LONG, 5},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 8},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "time_integrated_surface_latent_heat_flux_sfc",
      -4e+6,
      -4e+6,
      700000,
      700000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 10},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "time_integrated_surface_net_solar_radiation_sfc",
      0,
      0,
      4e+6,
      4e+6,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 4},
         {"parameterNumber", GRIB_TYPE_LONG, 9},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "time_integrated_surface_net_thermal_radiation_sfc",
      -1.1e+6,
      -1.1e+6,
      700000,
      700000,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 5},
         {"parameterNumber", GRIB_TYPE_LONG, 5},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "time_integrated_surface_sensible_heat_flux_sfc",
      -3e+6,
      -3e+6,
      1.5e+6,
      1.5e+6,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 11},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "total_cloud_cover_sfc",
      0,
      1e-10,
      100,
      100.00001,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 6},
         {"parameterNumber", GRIB_TYPE_LONG, 1},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },

   {
      "total_column_water_sfc",
      0.1,
      0.21,
      75,
      150,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 51},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 8},
         {NULL, },
      },
      {&point_in_time, &predefined_thickness},
   },

/*
   hourly extreems for cumulated parameters!

   - TBD: the problem is that for tigge/tigge-lam there are cumulated parameters from step 0 and
          the limits for e.g. 0-3 period should be very different to ones for e.g. 0-120 even
          the value is divided by the period lenght in hours!
*/

   {
      "total_precipitation_sfc.glob",
      -0.05,
      0.1,
      0.,
      100.,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 52},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "total_precipitation_sfc.lam",
      -0.05,
      0.1,
      0.,
      100.,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 228228},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 52},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

/*
   cosmodeeps-dwd-eu (origin=7)
   2013-??-??: maximum changed to <0,133> because of cosmo-de boundary effects when coupled with IFS ..
 */
   {
      "total_precipitation_sfc.lam.cosmodeeps-dwd-eu",
      -0.05,
      0.1,
      0.,
      133.,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 7, "cosmodeeps-dwd-eu"},

         {"paramId", GRIB_TYPE_LONG, 228228},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 52},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

/*hourly extreems for cumulated parameters! */
   {
      "large_scale_precipitation_sfc.glob",
      -0.05,
      0.1,
      0.0,
      100.,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 54},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "large_scale_precipitation_sfc.lam",
      -0.05,
      0.1,
      0.0,
      100.,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 3062},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 1},
         {"parameterNumber", GRIB_TYPE_LONG, 54},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 1},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_STRING, 0, "missing"},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&from_start, &predefined_level},
   },

   {
      "u_velocity_pl",
      -120,
      -20,
      20,
      120,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 2},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 100},
         {NULL, },
      },
      {&point_in_time, &given_level, &pressure_level},
   },

   {
      "u_velocity_pv",
      -120,
      -30,
      70,
      120,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 2},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 109},
         {NULL, },
      },
      {&point_in_time, &given_level, &potential_vorticity_level},
   },

   {
      "v_velocity_pl",
      -120,
      -20,
      20,
      120,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 3},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 100},
         {NULL, },
      },
      {&point_in_time, &given_level, &pressure_level},
   },

   {
      "v_velocity_pv",
      -120,
      -50,
      55,
      120,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 3},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 109},
         {NULL, },
      },
      {&point_in_time, &given_level, &potential_vorticity_level},
   },

   {
      "wilting_point_sfc",
      1e99,
      -1e99,
      99,
      -99,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},
         {"discipline", GRIB_TYPE_LONG, 2},
         {"parameterCategory", GRIB_TYPE_LONG, 0},
         {"parameterNumber", GRIB_TYPE_LONG, 26},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_LONG, 1},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_LONG, 2},
         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 106},
         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 106},
         {NULL, },
      },
      {&point_in_time, &given_thickness, &has_bitmap},
   },

   {
      "maximum_wind_gust.glob",
      0,
      10,
      0,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "glob"},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 22},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 2},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {NULL, },
      },
      {&three_hourly, &given_level},
   },

   {
      "maximum_wind_gust.lam",
      0,
      10,
      0,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 228028},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 22},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 2},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&three_hourly, &given_level},
   },

   {
      "maximum_wind_gust.lam",
      0,
      10,
      0,
      100,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},

         {"paramId", GRIB_TYPE_LONG, 228028},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 22},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 2},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&three_hourly, &given_level},
   },

/* 
   MOGREPS (origin = 1(mogreps-mo-eua))
   2014-01-08 minimum limit for maximum_wind_gust changed to <0,12> because of mogreps data from 2014010203_03+0033
   2014-01-10 maximum limit for maximum_wind_gust changed to <0,200> because of mogreps data from 2014010403_06+0033
   2014-01-10 maximum limit for maximum_wind_gust changed to <0,333> because of mogreps data from 2014010821_11+0033
   2014-01-27 minimum limit for maximum_wind_gust changed to <0,15> because of mogreps data from 2014012521_09+0018
   2014-02-20 maximum limit for maximum_wind_gust changed to <0,500> because of mogreps data from 2014022009_02+0021
*/
   {
      "maximum_wind_gust.lam.mogreps-mo-eua",
      0,
      15,
      0,
      500,
      {
         {"model", GRIB_TYPE_STRING, 0, "lam"},
         {"suiteName", GRIB_TYPE_STRING, 1, "mogreps-mo-eua"},

         {"paramId", GRIB_TYPE_LONG, 228028},

         {"discipline", GRIB_TYPE_LONG, 0},
         {"parameterCategory", GRIB_TYPE_LONG, 2},
         {"parameterNumber", GRIB_TYPE_LONG, 22},

         {"typeOfStatisticalProcessing", GRIB_TYPE_LONG, 2},

         {"typeOfFirstFixedSurface", GRIB_TYPE_LONG, 103},
         {"scaleFactorOfFirstFixedSurface", GRIB_TYPE_LONG, 0},
         {"scaledValueOfFirstFixedSurface", GRIB_TYPE_LONG, 10},

         {"typeOfSecondFixedSurface", GRIB_TYPE_LONG, 255},
         {"scaleFactorOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {"scaledValueOfSecondFixedSurface", GRIB_TYPE_STRING, 0, "missing"},
         {NULL, },
      },
      {&three_hourly, &given_level},
   },

};
