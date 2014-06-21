#!/usr/bin/ksh

p4 edit trend2.def
cat trend1.def | sed 's/Trend1/Trend2/g' > trend2.def
p4 edit trend3.def
cat trend1.def | sed 's/Trend1/Trend3/g' > trend3.def
p4 edit trend4.def
cat trend1.def | sed 's/Trend1/Trend4/g' > trend4.def

p4 edit horizontalVisibility_Trend2.def
cat horizontalVisibility_Trend1.def | sed 's/Trend1/Trend2/g' > horizontalVisibility_Trend2.def
p4 edit horizontalVisibility_Trend3.def
cat horizontalVisibility_Trend1.def | sed 's/Trend1/Trend3/g' > horizontalVisibility_Trend3.def
p4 edit horizontalVisibility_Trend4.def
cat horizontalVisibility_Trend1.def | sed 's/Trend1/Trend4/g' > horizontalVisibility_Trend4.def

p4 edit present_weather_Trend2.def
cat present_weather_Trend1.def | sed 's/Trend1/Trend2/g' > present_weather_Trend2.def
p4 edit present_weather_Trend3.def
cat present_weather_Trend1.def | sed 's/Trend1/Trend3/g' > present_weather_Trend3.def
p4 edit present_weather_Trend4.def
cat present_weather_Trend1.def | sed 's/Trend1/Trend4/g' > present_weather_Trend4.def

p4 edit wind_Trend2.def
cat wind_Trend1.def | sed 's/Trend1/Trend2/g' > wind_Trend2.def
p4 edit wind_Trend3.def
cat wind_Trend1.def | sed 's/Trend1/Trend3/g' > wind_Trend3.def
p4 edit wind_Trend4.def
cat wind_Trend1.def | sed 's/Trend1/Trend4/g' > wind_Trend4.def

p4 edit clouds_Trend2.def
cat clouds_Trend1.def | sed 's/Trend1/Trend2/g' > clouds_Trend2.def
p4 edit clouds_Trend3.def
cat clouds_Trend1.def | sed 's/Trend1/Trend3/g' > clouds_Trend3.def
p4 edit clouds_Trend4.def
cat clouds_Trend1.def | sed 's/Trend1/Trend4/g' > clouds_Trend4.def
